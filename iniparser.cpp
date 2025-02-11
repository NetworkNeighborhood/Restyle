#include "iniparser.h"
#include "restyle.h"
#include "util.h"
#include "file.h"
#include "scanner.h"
#include "schemapriv.h"
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

enum class ESymbolType
{
    // -- Predefined types --
    FirstPredefined,
    Type = FirstPredefined,
    Class,
    Part,
    State,
    BaseClass,
    PropertyKey,
    EnumValue,
    LastPredefined = EnumValue,
    
    // -- Manual types --
    FirstManual,
    ManualPart = FirstManual,  // *Part#1
    ManualState,               // *State#1
    ManualPropertyKey,         // *Prop#1<Type>
    LastManual = ManualPropertyKey,
};

bool IsSymbolTypePredefined(ESymbolType eSymType)
{
    return eSymType >= ESymbolType::FirstPredefined && eSymType <= ESymbolType::LastPredefined;
}

bool IsSymbolTypeManual(ESymbolType eSymType)
{
    return eSymType >= ESymbolType::FirstManual && eSymType <= ESymbolType::LastManual;
}

/**
 * A parse symbol.
 */
struct Symbol
{
    /**
     * The type of the symbol.
     *
     * There are two notable types of symbols: predefined (those which use a string name which
     * restyle defines for identifying a type) and manual (those defining their own internal
     * value).
     *
     * The use of predefined symbols is preferred by restyle. Manual symbols exist to give
     * theme authors flexibility.
     */
    ESymbolType eSymType;
    
    // Sharing memory because these are mutually-exclusive members:
    union
    {
        /**
         * Pointer to the name of the symbol in the name arena or schema, in the case of
         * predefined symbols.
         */
        LPCWSTR szName;
        
        /**
         * Integer value for the name of the symbol, in the case of manual symbols.
         */
        INT_PTR iName;
    };
    

    /**
    * Offset in the schema table information about the symbol can be found, 
    * in the case of predefined symbols.
    */
    int iSchemaOffset;
        
    /**
    * The primitive type of the property, in the case of PropertyKey and ManualPropertyKey types.
    */
    int iPrimType;
};

/**
 * Stores unique names used within a parsing context, especially class names.
 */
class CNameArena
{
    static constexpr size_t kInitialSize = 1024 * sizeof(WCHAR);
    static constexpr size_t kGrowSize = 64 * sizeof(WCHAR);
    
    bool _fInitialized = false;
    BYTE *_pszData = nullptr;
    BYTE *_pszCur = nullptr;
    DWORD _dwSize = 0;
    
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
            , _p((LPCWSTR)nameArena->_pszData)
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
        pointer GetEndOfData() const
        {
            return (LPCWSTR)((size_t)_pNameArena->_pszData + _pNameArena->_dwSize);
        }
    
        pointer GetNextString()
        {
            while (++_p < GetEndOfData() && _p != L'\0');
            LPCWSTR szResult = ++_p;
            
            if (*szResult == '\0')
            {
                _fIsEnd = true;
            }
            
            return szResult;
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
    
    for (const LPCWSTR &szExisting : *this)
    {
        if (AsciiStrCmpI(szExisting, sz))
        {
            // Avoid inserting duplicate items:
            return szExisting;
        }
    }
    
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

enum class EParseMode
{
    Assoc,
    SectionHeader,
    Preprocessor,
    BreakParseLoop,
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
    IniSection,
    // Block node types:
    AnimationTransformSet,
};

class CParseNode : public CFileAttributedElement
{
    std::vector<CParseNode> _rgChildren;
    
protected:
    EParseNodeType _eType;
    
public:
    EParseNodeType GetType()
    {
        return _eType;
    }

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

template <typename NativeType, int iPrimTypeVal>
struct ValueBase
{
    int iPrimType = iPrimTypeVal;
    int cbSize = sizeof(NativeType);
};

struct IntValue : public ValueBase<int, Restyle::TMT_INT>
{
    int iVal;
};

struct SizeValue : public ValueBase<int, Restyle::TMT_SIZE>
{
    int iVal;
};

struct BoolValue : public ValueBase<BOOL, Restyle::TMT_BOOL>
{
    BOOL fVal;
};

struct RectVal : public ValueBase<RECT, Restyle::TMT_RECT>
{
    RECT rcVal;
};

struct MarginsVal : public ValueBase<MARGINS, Restyle::TMT_MARGINS>
{
    MARGINS marVal;
};

struct StringVal : public ValueBase<WCHAR, Restyle::TMT_STRING>
{
    WCHAR szVal[];
};

/**
 * Stores unique names used within a parsing context, especially class names.
 */
class CValueArena
{
    static constexpr size_t kInitialSize = 1024 * sizeof(WCHAR);
    static constexpr size_t kGrowSize = 64 * sizeof(WCHAR);

    bool _fInitialized = false;
    BYTE *_pszData = nullptr;
    BYTE *_pszCur = nullptr;
    DWORD _dwSize = 0;
    
public:
    ~CValueArena();

    HRESULT EnsureInitialized();
    HRESULT Initialize();
    HRESULT ResizeIfNecessary(DWORD cbRequested);
    IntValue *CreateIntValue(int iVal);
    HRESULT Resize(DWORD dwNumberOfBytes);
};

CValueArena::~CValueArena()
{
    if (_pszData)
    {
        free(_pszData);
    }
}

HRESULT CValueArena::EnsureInitialized()
{
    if (!_fInitialized)
    {
        return Initialize();
    }
    
    return S_OK;
}

HRESULT CValueArena::Initialize()
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

LPCWSTR CValueArena::Add(CValue &)
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
    
    
    
    memcpy(_pszCur, sz, cbsz);
    return (LPCWSTR)_pszCur++;
}

HRESULT CValueArena::ResizeIfNecessary(DWORD cbRequested)
{
    if ((size_t)_pszCur + cbRequested - (size_t)_pszData > _dwSize)
    {
        if (FAILED(Resize((kGrowSize + cbRequested) & 0xFFFFFFF8)))
        {
            // Out of memory:
            return E_OUTOFMEMORY;
        }
    }
    
    return S_OK;
}

HRESULT CValueArena::Resize(DWORD dwNumberOfBytes)
{
    if ((_pszData = (BYTE *)realloc(_pszData, _dwSize + dwNumberOfBytes)))
    {
        _dwSize += dwNumberOfBytes;
        return S_OK;
    }
    
    return E_OUTOFMEMORY;
}

struct IniAssociation : public CParseNode
{
    Symbol *pKeySymbol;
    std::wstring strVal;
    
    IniAssociation()
    {
        _eType = EParseNodeType::IniAssociation;
    }
};

struct IniSection : public CParseNode
{
    Symbol *pClassNameSymbol;
    Symbol *pPartNameSymbol;
    Symbol *pStateNameSymbol;
    Symbol *pBaseClassNameSymbol;
    std::vector<IniAssociation> rgAssociations;
    
    IniSection()
    {
        _eType = EParseNodeType::IniSection;
    }
};

struct AnimationTransformSet : public CParseNode
{
    Symbol *pAnimationTypeSymbol;
    
    AnimationTransformSet()
    {
        _eType = EParseNodeType::AnimationTransformSet;
    }
};

class CIniParser
{
    CScanner _scanner;
    CSymbolManager *_pSymbolManager;
    std::vector<CParseNode> _nodeTree;
    
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
    
    static constexpr QuotePairMap s_rgIniQuotePairMap[] = {
        { L"\"", L"\"" },
        { L"'", L"'" },
    };
    
    static constexpr QuotePairMap s_rgPreprocessorQuotePairMap[] = {
        { L"\"", L"\"" },
        { L"'", L"'" },
        { L"<", L">" }, // Used by the preprocessor
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