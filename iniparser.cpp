#include "iniparserp.h"
#include "restyle.h"
#include "util.h"
#include "file.h"
#include "scanner.h"
#include "arena.h"
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

/**
 * Stores unique names used within a parsing context, especially class names.
 */
class CNameArena
#define BASECLASS CTBaseArena<CNameArena, const WCHAR, 1024>
    : public BASECLASS
{    
    using Super = BASECLASS;
    
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
                while (++_p < GetEndOfData() && *_p != L'\0');
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

    ValueResult<LPCWSTR> Add(LPCWSTR sz);
};

ValueResult<LPCWSTR> CNameArena::Add(LPCWSTR sz)
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
    
    HRESULT hr = Super::Add(sz, cbsz);
    if (FAILED(hr))
    {
        return hr;
    }
    
    return (LPCWSTR)_pvCur;
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
class CValueArena : public CTBaseArena<CValueArena, RectValue, 64>
{
    /**
     * This class is used as an RAII wrapper to ensure that the methods of this
     * function actually update the offset.
     * 
     * In release builds, this class will be erased by the optimising compiler.
     */
    class CEnsureArenaPointerChanged
    {
#if DEBUG
        CValueArena *_pParent;
        BYTE *_pvOriginal;

    public:
        inline [[nodiscard]] CEnsureArenaPointerChanged(CValueArena *pParent)
            : _pParent(pParent)
            , _pvOriginal(pParent->_pvCur)
        {
        }

        inline ~CEnsureArenaPointerChanged()
        {
            assert(_pParent->_pvCur != _pvOriginal);
        }
#else
        inline FORCEINLINE CEnsureArenaPointerChanged(CValueArena *pParent)
        {
        }
#endif
    };

public:
    ValueResult<IntValue *> CreateIntValue(int iVal);
    ValueResult<BoolValue *> CreateBoolValue(BOOL fVal);
    ValueResult<StringValue *> CreateStringValue(LPCWSTR szVal);
};

ValueResult<IntValue *> CValueArena::CreateIntValue(int iVal)
{
    CEnsureArenaPointerChanged ensurePointerChanged(this);

    HRESULT hr = ResizeIfNecessary(sizeof(IntValue));
    if (FAILED(hr))
    {
        return hr;
    }
    
    IntValue *pResult = new (_pvCur) IntValue();
    pResult->iVal = iVal;
    _pvCur += pResult->cbSize;
    return pResult;
}

ValueResult<BoolValue *> CValueArena::CreateBoolValue(BOOL fVal)
{
    CEnsureArenaPointerChanged ensurePointerChanged(this);

    HRESULT hr = ResizeIfNecessary(sizeof(BoolValue));
    if (FAILED(hr))
    {
        return hr;
    }
    
    BoolValue *pResult = new (_pvCur) BoolValue();
    pResult->fVal = fVal;
    _pvCur += pResult->cbSize;
    return pResult;
}

ValueResult<StringValue *> CValueArena::CreateStringValue(LPCWSTR szVal)
{
    CEnsureArenaPointerChanged ensurePointerChanged(this);

    DWORD cch = wcslen(szVal);
    size_t targetSize = cch * sizeof(WCHAR) + sizeof(L'\0');
    
    HRESULT hr = ResizeIfNecessary(sizeof(StringValue) + targetSize);
    if (FAILED(hr))
    {
        return hr;
    }
    
    StringValue *pResult = new (_pvCur) StringValue();
    pResult->cbSize = targetSize;
    if (!memcpy((void *)&pResult->szVal[0], szVal, targetSize))
    {
        return E_FAIL;
    }

    _pvCur += pResult->cbSize;
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
    
    HRESULT ParseNextSectionHeader();
    HRESULT ParseNextAssociation();
    HRESULT ParseNextCPreprocessor();
    
    HRESULT ParseCPreprocessorInclude();
    HRESULT ParseCPreprocessorIf();
    HRESULT ParseCPreprocessorElif();
    HRESULT ParseCPreprocessorElse();
    HRESULT ParseCPreprocessorEndif();
    
    static constexpr struct
    {
        // Must be the length of the longest word in the map.
        WCHAR szCommand[sizeof(L"include")];
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
            
            case EParseMode::SectionHeader:
            {
                hr = ParseNextSectionHeader();
                break;
            }
            
            case EParseMode::Assoc:
            {
                hr = ParseNextAssociation();
                break;
            }
            
            case EParseMode::BreakParseLoop:
            {
                // This is useless because it's handled in the above while loop, however Intellisense
                // won't shut up.
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