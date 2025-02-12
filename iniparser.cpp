#include "iniparserp.h"
#include "restyle.h"
#include "util.h"
#include "file.h"
#include "scanner.h"
#include <iterator>
#include <stringapiset.h>
#include <strsafe.h>
#include <winerror.h>
#include <winnls.h>
#include <string>
#include <vector>
#include <winnt.h>

namespace IniParser
{

bool IsSymbolTypePredefined(ESymbolType eSymType)
{
    return eSymType >= ESymbolType::FirstPredefined && eSymType <= ESymbolType::LastPredefined;
}

bool IsSymbolTypeManual(ESymbolType eSymType)
{
    return eSymType >= ESymbolType::FirstManual && eSymType <= ESymbolType::LastManual;
}

#define BASEARENATEMPLATE \
    template <class Self, typename BaseType, int kInitialArrayCount, int kGrowArrayCount>

#define BASEARENATYPE \
    CTBaseArena<Self, BaseType, kInitialArrayCount, kGrowArrayCount>

BASEARENATEMPLATE
class CTBaseArena
{
protected:
    static constexpr size_t kInitialSize = kInitialArrayCount * sizeof(BaseType);
    static constexpr size_t kGrowSize = kGrowArrayCount * sizeof(BaseType);
    
    bool _fInitialized = false;
    BYTE *_pvData = nullptr;
    BYTE *_pvCur = nullptr;
    DWORD _dwSize = 0;
    Self *_pNext = nullptr;
    
public:
    ~CTBaseArena();
    HRESULT EnsureInitialized();
    HRESULT Initialize();
    HRESULT Resize(DWORD dwNumberOfBytes);
    HRESULT ResizeIfNecessary(DWORD cbRequested);
};

#define BASEARENAIMPL(T)                                                      \
    BASEARENATEMPLATE                                                         \
    T BASEARENATYPE


BASEARENATEMPLATE
BASEARENATYPE::~CTBaseArena()
{
    CTBaseArena *pCur = this;
        
    // First pass: reverse the pointers in the linked list to point backwards.
    while (pCur->_pNext)
    {
        CTBaseArena *pTemp = pCur->_pNext;
        std::swap(pCur->_pNext, pCur->_pNext->_pNext);
        pCur = pTemp;
    }
    
    // Second pass: free all branches.
    while (pCur)
    {
        if (pCur->_pvData)
        {
            delete pCur->_pvData;
        }
        
        pCur = pCur->_pNext;
    }
}

BASEARENAIMPL(HRESULT)::EnsureInitialized()
{
    if (!_fInitialized)
    {
        return Initialize();
    }
    
    return S_OK;
}

BASEARENAIMPL(HRESULT)::Initialize()
{
    if ((_pvData = new BYTE[kInitialSize]))
    {
        ZeroMemory(_pvData, kInitialSize);
        _dwSize = kInitialSize;
        _pvCur = _pvData;
        _fInitialized = true;
        return S_OK;
    }
    
    return E_OUTOFMEMORY;
}

BASEARENAIMPL(HRESULT)::ResizeIfNecessary(DWORD cbRequested)
{
    if ((size_t)_pvCur + cbRequested - (size_t)_pvData > _dwSize)
    {
        if (FAILED(Resize((kGrowSize + cbRequested) & 0xFFFFFFF8)))
        {
            // Out of memory:
            return E_OUTOFMEMORY;
        }
    }
    
    return S_OK;
}

BASEARENAIMPL(HRESULT)::Resize(DWORD dwNumberOfBytes)
{
    // We are always ourself, so there's absolutely no case where the type contract is
    // broken. This is simply required because the base class is different, and our
    // this variable is of the base class rather than a child class.
    Self *pCur = static_cast<Self *>(this);
    
    while (pCur->_pNext) pCur = pCur->_pNext;
    
    pCur->_pNext = new Self();
    
    if (!pCur->_pNext)
    {
        return E_OUTOFMEMORY;
    }
    
    HRESULT hr = pCur->_pNext->Initialize();
    
    if (FAILED(hr))
    {
        return hr;
    }
    
    return S_OK;
}

/**
 * Stores unique names used within a parsing context, especially class names.
 */
class CNameArena : public CTBaseArena<CNameArena, WCHAR, 1024, 64>
{
public:
    struct Iterator
    {
        using iterator_category = std::forward_iterator_tag;
        using difference_type = std::ptrdiff_t;
        using value_type = LPCWSTR;
        using pointer = value_type;
        using reference = const value_type &;
        
        // Beginning (real) iterator constructor
        Iterator(CNameArena *nameArena)
            : _pNameArena(nameArena)
            , _p((pointer)nameArena->_pvData)
        {
        }
        
        // Ending (sentinel) iterator constructor
        Iterator()
            : _pNameArena(nullptr)
            , _p(nullptr)
        {
        }
        
        reference operator*() const
        {
            return _p;
        }
        
        pointer operator->() const
        {
            return _p;
        }
        
        Iterator &operator++()
        {
            GetNextString();
            return *this;
        }
        
        Iterator &operator++(int)
        {
            GetNextString();
            return *this;
        }
        
        friend bool operator==(const Iterator &a, const Iterator &b)
        {
            // The end iterator is always a sentinel object, with its pointer and CNameArena
            // pointers both being nullptr.
            assert(a._p == nullptr || b._p == nullptr);
            
            const Iterator *pRealIterator = a._p != nullptr ? &a : &b;
            
            LPCWSTR pEndOfData = pRealIterator->GetEndOfData();
            
            if (pRealIterator->_p > pEndOfData || pRealIterator->_fIsEnd)
            {
                // i == iterator.end()
                return true;
            }
            
            return false;
        }
        
        friend bool operator!=(const Iterator &a, const Iterator &b)
        {
            return !(operator==(a, b));
        }
        
    private:
        void UseNextArena()
        {
            _pNameArena = _pNameArena->_pNext;
            _p = (pointer)_pNameArena->_pvData;
        }
    
        pointer GetEndOfData() const
        {
            return (LPCWSTR)((size_t)_pNameArena->_pvData + _pNameArena->_dwSize);
        }
    
        pointer GetNextString()
        {
            do
            {
                while (++_p < GetEndOfData() && _p != L'\0');
                LPCWSTR szResult = ++_p;
                
                if (_pNameArena->_pNext)
                {
                    UseNextArena();
                    continue;
                }
                else if (*szResult == '\0')
                {
                    _fIsEnd = true;
                }
                
                return szResult;
            }
            while (1);
        }
    
        CNameArena *_pNameArena;
        bool _fIsEnd = false;
        pointer _p;
    };
    
    // C++ iterator concept
    Iterator begin()
    {
        return Iterator(this);
    }
    
    // C++ iterator concept
    Iterator end()
    {
        return Iterator();
    }

    LPCWSTR Add(LPCWSTR sz);
};

LPCWSTR CNameArena::Add(LPCWSTR sz)
{
    DWORD cbsz = (wcslen(sz) + sizeof('\0')) * sizeof(WCHAR);
    
    for (const LPCWSTR &szExisting : *this)
    {
        if (AsciiStrCmpI(szExisting, sz))
        {
            // Avoid inserting duplicate items:
            return szExisting;
        }
    }
    
    if (FAILED(ResizeIfNecessary(cbsz)))
    {
        return nullptr;
    }
    
    memcpy(_pvCur, sz, cbsz);
    return (LPCWSTR)_pvCur++;
}

/**
 * Manages symbols.
 */
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
    sym.eSymType = eSymType;
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

/**
 * Stores unique names used within a parsing context, especially class names.
 */
class CValueArena : public CTBaseArena<CValueArena, RectValue, 256, 64>
{
public:
    IntValue *CreateIntValue(int iVal);
    BoolValue *CreateBoolValue(BOOL fVal);
    StringValue *CreateStringValue(LPCWSTR szVal);
};

IntValue *CValueArena::CreateIntValue(int iVal)
{
    if (FAILED(ResizeIfNecessary(sizeof(IntValue))))
    {
        return nullptr;
    }
    
    IntValue *pResult = new (_pszCur) IntValue();
    pResult->iVal = iVal;
    return pResult;
}

BoolValue *CValueArena::CreateBoolValue(BOOL fVal)
{
    if (FAILED(ResizeIfNecessary(sizeof(BoolValue))))
    {
        return nullptr;
    }
    
    BoolValue *pResult = new (_pszCur) BoolValue();
    pResult->fVal = fVal;
    return pResult;
}

StringValue *CValueArena::CreateStringValue(LPCWSTR szVal)
{
    DWORD cch = wcslen(szVal);
    size_t targetSize = cch * sizeof(WCHAR) + sizeof(L'\0');
    
    if (FAILED(ResizeIfNecessary(sizeof(StringValue) + targetSize)))
    {
        return nullptr;
    }
    
    StringValue *pResult = new (_pszCur) StringValue();
    pResult->cbSize = targetSize;
    if (!memcpy((void *)&pResult->szVal[0], szVal, targetSize))
    {
        return nullptr;
    }
    
    return pResult;
}

class CIniParser
{
    CScanner _scanner;
    CSymbolManager *_pSymbolManager;
    std::vector<IniAssociation> _associations;
    
    IniSection _iniSectionCur;
    
    EParseMode _eMode;
    bool _fParsingIncludeChild = false;
    
    std::wstring ReadNextWord();
    
    HRESULT ParseNextCPreprocessor();
    
    HRESULT ParseCPreprocessorInclude();
    HRESULT ParseCPreprocessorIf();
    HRESULT ParseCPreprocessorElif();
    HRESULT ParseCPreprocessorElse();
    HRESULT ParseCPreprocessorEndif();
    
    struct QuotePairMap
    {
        LPCWSTR szOpeningQuote;
        LPCWSTR szClosingQuote;
    };
    
    static constexpr QuotePairMap s_rgQuotePairMap[] = {
        { L"\"", L"\"" },
        { L"'", L"'" },
    };
    
    static constexpr struct
    {
        LPCWSTR szCommand;
        HRESULT (CIniParser:: *pfnCallback)();
    } s_rgPreprocessorCommandMap[] = {
        { L"include", &CIniParser::ParseCPreprocessorInclude },
        { L"if", &CIniParser::ParseCPreprocessorIf },
        { L"elif", &CIniParser::ParseCPreprocessorElif },
        { L"else", &CIniParser::ParseCPreprocessorElse },
        { L"endif", &CIniParser::ParseCPreprocessorEndif },
    };
    
public:
    CIniParser(LPCWSTR szText, DWORD cchText);
    CIniParser(std::wstring text);
    
    // static CIniParser CreateIncludeIniParser(CIniParser *pParent, CSimpleFile *pChildFile);
    
    HRESULT Parse();
    
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

std::wstring CIniParser::ReadNextWord()
{
    std::wstring result;
    
    while (_scanner.IsNameChar(false))
    {
        result.push_back(_scanner.ReadNext());
    }
    
    return result;
}

HRESULT CIniParser::Parse()
{
    HRESULT hr;
    
    while (!_scanner.EndOfFile() && _eMode != EParseMode::BreakParseLoop)
    {
        switch (_eMode)
        {
            case EParseMode::Preprocessor:
            {
                hr = ParseNextCPreprocessor();
                break;
            }
        }
    }
    
    return S_OK;
}

HRESULT CIniParser::ParseNextCPreprocessor()
{
    std::wstring strNextWord = ReadNextWord();
    HRESULT hr = E_FAIL;
    
    if (strNextWord.empty())
    {
        return E_FAIL;
    }
    
    bool fHandled = false;
    
    for (auto &cmd : s_rgPreprocessorCommandMap)
    {
        if (wcscmp(strNextWord.c_str(), cmd.szCommand) == 0)
        {
            fHandled = true;
            hr = (this->*cmd.pfnCallback)();
        }
    }
    
    if (!fHandled)
    {
        // Invalid keyword.
        return E_FAIL;
    }
    
    return hr;
}

HRESULT CIniParser::ParseCPreprocessorInclude()
{
    // Include is a tricky one. This will be written later.
    return S_OK;
}

// // static
// CIniParser CIniParser::CreateIncludeIniParser(CIniParser *pParent, CSimpleFile *pChildFile)
// {
//     CIniParser iniParser(pChildFile, pParent->);
// }

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