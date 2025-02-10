#include "iniparser.h"
#include "restyle.h"
#include "util.h"
#include "file.h"
#include "scanner.h"
#include "schemapriv.h"
#include <stringapiset.h>
#include <strsafe.h>
#include <winerror.h>
#include <winnls.h>
#include <string>
#include <vector>
#include <winnt.h>

namespace IniParser
{

enum class ESymbolType
{
    Type,
    Class,
    Part,
    State,
    BaseClass,
    PropertyKey,
    EnumValue,
};

struct Symbol
{
    LPCWSTR szName;
    ESymbolType eType;
    int iSchemaOffset;
};

class CNameArena
{
    static constexpr size_t kInitialSize = 1024 * sizeof(WCHAR);
    static constexpr size_t kGrowSize = 64 * sizeof(WCHAR);
    
    bool _fInitialized = false;
    BYTE *_pszData = nullptr;
    BYTE *_pszCur = nullptr;
    DWORD _dwSize = 0;
    
public:
    ~CNameArena();

    HRESULT EnsureInitialized();
    HRESULT Initialize();
    LPCWSTR Add(LPCWSTR sz);
    HRESULT Resize(DWORD dwNumberOfBytes);
};

CNameArena::~CNameArena()
{
    if (_pszData)
    {
        free(_pszData);
    }
}

HRESULT CNameArena::EnsureInitialized()
{
    if (!_fInitialized)
    {
        return Initialize();
    }
    
    return S_OK;
}

HRESULT CNameArena::Initialize()
{
    if ((_pszData = (BYTE *)malloc(kInitialSize)))
    {
        _dwSize = kInitialSize;
        _pszCur = _pszData;
        _fInitialized = true;
        return S_OK;
    }
    
    return E_OUTOFMEMORY;
}

LPCWSTR CNameArena::Add(LPCWSTR sz)
{
    DWORD cbsz = (wcslen(sz) + sizeof('\0')) * sizeof(WCHAR);
    
    if ((size_t)_pszCur + cbsz - (size_t)_pszData > _dwSize)
    {
        if (FAILED(Resize((kGrowSize + cbsz) & 0xFFFFFFF8)))
        {
            // Out of memory:
            return nullptr;
        }
    }
    
    memcpy(_pszCur, sz, cbsz);
    return (LPCWSTR)_pszCur++;
}

HRESULT CNameArena::Resize(DWORD dwNumberOfBytes)
{
    if ((_pszData = (BYTE *)realloc(_pszData, _dwSize + dwNumberOfBytes)))
    {
        _dwSize += dwNumberOfBytes;
        return S_OK;
    }
    
    return E_OUTOFMEMORY;
}

class CSymbolManager
{
    std::vector<Symbol> _rgSymbols;
    CNameArena _nameArena;
    
public:
    HRESULT AddSymbol(LPCWSTR szSymName, ESymbolType eSymType);
    LPCWSTR GetGlobalSymbolName(LPCWSTR szSymName, OUT OPTIONAL int *piSchemaOffset = nullptr);
    bool HasSymbol(LPCWSTR szSymName);
};

HRESULT CSymbolManager::AddSymbol(LPCWSTR szSymName, ESymbolType eSymType)
{
    if (HasSymbol(szSymName))
    {
        return S_OK;
    }
    
    int iSchemaOffset = -1;
    LPCWSTR szSafeSymName = GetGlobalSymbolName(szSymName, &iSchemaOffset);
    
    if (!szSafeSymName)
    {
        // Probably out of memory...
        return E_FAIL;
    }
    
    Symbol sym;

    sym.szName = szSafeSymName;
    sym.eType = eSymType;
    sym.iSchemaOffset = iSchemaOffset;
    
    _rgSymbols.push_back(sym);
    return S_OK;
}

LPCWSTR CSymbolManager::GetGlobalSymbolName(LPCWSTR szSymName, OUT OPTIONAL int *piSchemaOffset)
{
    LPCWSTR pszResult = nullptr;
    const Restyle::TMSCHEMAINFO *pSchemaInfo = Restyle::GetSchemaInfo();
    
    // This is an existing name stored statically in the module in the schema:
    for (int i = 0; i < pSchemaInfo->iPropCount; i++)
    {
        const Restyle::TMPROPINFO *pPropInfo = &pSchemaInfo->pPropTable[i];
        
        if (AsciiStrCmpI(pPropInfo->pszName, szSymName))
        {
            pszResult = pPropInfo->pszName;
            break;
        }
    }
    
    // This is a unique name which only presents itself inside this INI file. This case
    // includes all of the class names. In this case, we'll copy the names over to our 
    // name arena so that we can preserve them even after the INI file is freed from memory.
    if (!pszResult)
    {
        _nameArena.EnsureInitialized();
        pszResult = _nameArena.Add(szSymName);
    }
    
    return pszResult;
}

bool CSymbolManager::HasSymbol(LPCWSTR szSymName)
{
    for (Symbol &s : _rgSymbols)
    {
        if (AsciiStrCmpI(s.szName, szSymName) == 0)
        {
            return true;
        }
    }
    
    return false;
}

enum class EParseMode
{
    Assoc,
    SectionHeader,
    Preprocessor,
};

class CFileAttributedElement
{
    int _iFileOffset;
    
public:
    inline int GetFileOffset()
    {
        return _iFileOffset;
    }
};

enum class EParseNodeType
{
    Null = 0,
    IniAssociation,
    IniHeader,
    // Animation sets are a special type of block node.
    AnimationSet,
    Block,
};

class CParseNode : public CFileAttributedElement
{
    std::vector<CParseNode> _rgChildren;
    
protected:
    EParseNodeType eType;
    
public:
    int GetChildCount()
    {
        return _rgChildren.size();
    }
    
    std::vector<CParseNode> &GetChildren()
    {
        return _rgChildren;
    }
    
    CParseNode *GetChild(int iOffset)
    {
        return &_rgChildren[iOffset];
    }
};

struct IniAssociation : public CParseNode
{
    Symbol *pKeySymbol;
    std::wstring strVal;
    
    IniAssociation()
    {
        eType = EParseNodeType::IniAssociation;
    }
};

struct IniHeader : public CParseNode
{
    Symbol *pClassNameSymbol;
    Symbol *pPartNameSymbol;
    Symbol *pStateNameSymbol;
    Symbol *pBaseClassNameSymbol;
    std::vector<IniAssociation> rgAssociations;

    IniHeader()
    {
        eType = EParseNodeType::IniHeader;
    }
};

struct AnimationSet : public CParseNode
{
    Symbol *pAnimationTypeSymbol;
};

class CIniParser
{
    CScanner _scanner;
    EParseMode _eMode;
    
public:
    CIniParser(LPCWSTR szText, DWORD cchText);
    CIniParser(std::wstring text);
    bool ReadName(IN OUT LPWSTR szId, DWORD cchId);
    
    HRESULT Test();
};

CIniParser::CIniParser(LPCWSTR szText, DWORD cchText)
    : _scanner(szText, cchText)
{
}

CIniParser::CIniParser(std::wstring text)
    : _scanner(text)
{
}

// Expects test file:
#if 0
[Fawk]
Hello = 1

[aaaa::bbbb.fuck(aaaaaa) : hello]
aaaaaaaaa = sijdisjfid

[animation.Fawk(dd)]
Transforms = {
    Translate2D {
        Duration = 20
    }
}
#endif
HRESULT CIniParser::Test()
{
    if (!_scanner.GetChar(L'['))
    {
        Log(L"First character bad. Parser probably getting bad input u_u. Check debugger.", ELogLevel::Fatal);
        return E_FAIL;
    }
    
    if (!_scanner.GetKeyword(L"Fawk"))
    {
        Log(L"Expected \"Fawk\"", ELogLevel::Fatal);
        return E_FAIL;
    }
    
    if (!_scanner.GetChar(L']'))
    {
        Log(L"Expected \"]\"", ELogLevel::Fatal);
        return E_FAIL;
    }
    
    if (!_scanner.GetKeyword(L"Hello"))
    {
        Log(L"Expected \"Hello\"", ELogLevel::Fatal);
        return E_FAIL;
    }
    
    if (!_scanner.GetChar(L'='))
    {
        Log(L"Expected \"=\"", ELogLevel::Fatal);
        return E_FAIL;
    }
    
    int iNumber = 0;
    if (!_scanner.GetNumber(&iNumber) || iNumber != 1)
    {
        Log(L"Expected number 1, got %d", iNumber, ELogLevel::Fatal);
        return E_FAIL;
    }
    
    Log(L"Succeeded INI parsing test with no problems.", ELogLevel::Info);
    return S_OK;
}

bool CIniParser::ReadName(IN OUT LPWSTR szId, DWORD cchId)
{
    if (cchId == 0)
    {
        return false;
    }
    
    _scanner.SkipSpaces();
    
    WCHAR *szRead = szId;
    
    while (_scanner.IsNameChar(false) && --cchId)
    {
        *szRead++ = _scanner.ReadNext();
    }
    *szRead = 0; // Add null terminator
    
    if (szRead == szId)
    {
        // If we hit this case, then we didn't end up reading anything.
        return false;
    }
    
    if (_scanner.IsNameChar(false))
    {
        // Buffer too small
        return false;
    }
    
    return true;
}


HRESULT ParseIniFile(LPCWSTR szPath)
{
    CSimpleFile file;
    HRESULT hr = file.Open(szPath);

    if (FAILED(hr))
    {
        Log(L"FATAL: Failed to open file.", ELogLevel::Fatal);
        return 0;
    }

    DWORD dwFileSizeHigh = 0;
    DWORD dwFileSize = file.GetFileSize(&dwFileSizeHigh);

    if (dwFileSize == 0)
    {
        Log(L"FATAL: Failed to get file size.", ELogLevel::Fatal);
        return 0;
    }

    LPWSTR szString = new WCHAR[dwFileSize + sizeof('\n')];

    DWORD cbRead = 0;
    hr = file.Read((void *)szString, dwFileSize * sizeof(WCHAR) + sizeof(L'\n'), &cbRead);

    if (FAILED(hr))
    {
        delete[] szString;
        Log(L"FATAL: Failed to read file.", ELogLevel::Fatal);
        return 0;
    }
    
    CIniParser iniParser(szString, dwFileSize + sizeof('\n'));
    iniParser.Test();
    
    // TODO: Add INI parsing test code here.

    if (szString)
    {
        delete[] szString;
    }
    
    return S_OK;
}

}