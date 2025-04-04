#include "iniparserp.h"
#include "restyle.h"
#include "SchemaUtils.h"
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
#include <variant>
#include <winnt.h>

// Uncomment to enable preprocessor support, which is currently incomplete.
//#define ENABLE_PREPROCESSOR

// Temporarily enable CIniParser to create its own instance of CSymbolManager while the code structure
// is still being determined:
#define TEMP_CREATE_OWN_SYMBOL_MANAGER

namespace IniParser
{

using namespace Restyle;

bool IsSymbolTypePredefined(ESymbolType eSymType)
{
    return eSymType >= ESymbolType::FirstPredefined && eSymType <= ESymbolType::LastPredefined;
}

bool IsSymbolTypeManual(ESymbolType eSymType)
{
    return eSymType >= ESymbolType::FirstManual && eSymType <= ESymbolType::LastManual;
}

struct PredefinedToManualSymbolTypeMap
{
    ESymbolType ePredefined;
    ESymbolType eManual;
};

static PredefinedToManualSymbolTypeMap g_rgSymTypePredefinedToManualMap[] = {
    { ESymbolType::Part, ESymbolType::ManualPart },
    { ESymbolType::State, ESymbolType::ManualState },
    { ESymbolType::PropertyKey, ESymbolType::ManualPropertyKey },
};

static ESymbolType GetManualTypeCorrespondingToPredefined(ESymbolType ePredefined)
{
    return GetXTypeCorrespondingToY<
        ESymbolType, PredefinedToManualSymbolTypeMap, decltype(g_rgSymTypePredefinedToManualMap), g_rgSymTypePredefinedToManualMap,
        &PredefinedToManualSymbolTypeMap::ePredefined, &PredefinedToManualSymbolTypeMap::eManual>(ePredefined);
}

static ESymbolType GetPredefinedTypeCorrespondingToManual(ESymbolType eManual)
{
    return GetXTypeCorrespondingToY<ESymbolType, PredefinedToManualSymbolTypeMap, decltype(g_rgSymTypePredefinedToManualMap), g_rgSymTypePredefinedToManualMap,
        &PredefinedToManualSymbolTypeMap::eManual, &PredefinedToManualSymbolTypeMap::ePredefined>(eManual);
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
    size_t cbsz = (wcslen(sz) + sizeof(L'\0')) * sizeof(WCHAR);
    
    for (const LPCWSTR &szExisting : *this)
    {
        if (AsciiStrCmpI(szExisting, sz) == 0)
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
    ValueResult<Symbol *> AddSymbol(LPCWSTR szSymName, ESymbolType eSymType);
    ValueResult<Symbol *> AddManualSymbol(int iVal, ESymbolType eSymType, OPTIONAL int iType = 0);
    LPCWSTR GetGlobalSymbolName(LPCWSTR szSymName, OUT OPTIONAL int *piSchemaOffset = nullptr);
    Symbol *FindSymbolPointer(LPCWSTR szSymName);
    bool HasSymbol(LPCWSTR szSymName);
};

ValueResult<Symbol *> CSymbolManager::AddSymbol(LPCWSTR szSymName, ESymbolType eSymType)
{
    assert(IsSymbolTypePredefined(eSymType));

    if (Symbol *p = FindSymbolPointer(szSymName))
    {
        return p;
    }
    
    int iSchemaOffset = -1;
    LPCWSTR szSafeSymName = GetGlobalSymbolName(szSymName, &iSchemaOffset);
    
    if (!szSafeSymName)
    {
        // Probably out of memory...
        return E_FAIL;
    }
    
    Symbol sym {};

    sym.szName = szSafeSymName;
    sym.eSymType = eSymType;
    sym.iSchemaOffset = iSchemaOffset;
    
    _rgSymbols.push_back(sym);
    return &_rgSymbols[_rgSymbols.size() - 1];
}

ValueResult<Symbol *> CSymbolManager::AddManualSymbol(int iVal, ESymbolType eSymType, OPTIONAL int iType)
{
    assert(IsSymbolTypeManual(eSymType));

    // Try to find a duplicate symbol in the array to optimise memory usage:
    for (Symbol &s : _rgSymbols)
    {
        if (s.eSymType == eSymType && s.iName == iVal)
        {
            return &s;
        }
    }

    Symbol sym {};

    sym.iName = iVal;
    sym.eSymType = eSymType;
    sym.iPrimType = iType;

    _rgSymbols.push_back(sym);
    return &_rgSymbols[_rgSymbols.size() - 1];
}

LPCWSTR CSymbolManager::GetGlobalSymbolName(LPCWSTR szSymName, OUT OPTIONAL int *piSchemaOffset)
{
    LPCWSTR pszResult = nullptr;

    const TMPROPINFO *pPropInfo = SearchSchema(ESchemaSearchQuery::SearchWholeSchema, szSymName);
    
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

Symbol *CSymbolManager::FindSymbolPointer(LPCWSTR szSymName)
{
    for (Symbol &s : _rgSymbols)
    {
        if (AsciiStrCmpI(s.szName, szSymName) == 0)
        {
            return &s;
        }
    }

    return nullptr;
}

bool CSymbolManager::HasSymbol(LPCWSTR szSymName)
{
    return FindSymbolPointer(szSymName) != nullptr;
}

/**
 * Stores unique names used within a parsing context, especially class names.
 */
class CValueArena : public CTBaseArena<CValueArena, BYTE[kLargestValueTypeSize], 256>
{
    // Precached values:
    const BoolValue _valBoolFalse = { TMT_BOOL, sizeof(BOOL), FALSE };
    const BoolValue _valBoolTrue  = { TMT_BOOL, sizeof(BOOL), TRUE };

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

    template <typename TValue, auto TValue:: *pValue>
    ValueResult<const TValue *> CreateTValue(auto nVal)
    {
        CEnsureArenaPointerChanged ensurePointerChanged(this);

        HRESULT hr = ResizeIfNecessary(sizeof(TValue));
        if (FAILED(hr))
        {
            return hr;
        }

        TValue *pResult = new (_pvCur) TValue();
        pResult->*pValue = nVal;
        _pvCur += sizeof(Value<>) + pResult->cbSize;
        return pResult;
    }

    template <typename TValue, auto TValue :: *pValue>
    ValueResult<const TValue *> CreateTListValue(auto rgnVal, UINT cVals)
    {
        CEnsureArenaPointerChanged ensurePointerChanged(this);

        size_t cb = sizeof(int) * cVals;

        HRESULT hr = ResizeIfNecessary(sizeof(TValue) + cb);
        if (FAILED(hr))
        {
            return hr;
        }

        TValue *pResult = new (_pvCur) TValue();
        pResult->cbSize = cb;
        if (!memcpy((void *)&(pResult->*pValue)[0], rgnVal, cb))
        {
            return E_OUTOFMEMORY;
        }

        _pvCur += sizeof(Value<>) + pResult->cbSize;
        return pResult;
    }

public:
    ValueResult<const IntValue *> CreateIntValue(int iVal);
    ValueResult<const EnumValue *> CreateEnumValue(int iVal);
    ValueResult<const FloatValue *> CreateFloatValue(float flVal);
    ValueResult<const SizeValue *> CreateSizeValue(int iVal);
    ValueResult<const BoolValue *> CreateBoolValue(BOOL fVal);
    ValueResult<const RectValue *> CreateRectValue(RECT rcVal);
    ValueResult<const MarginsValue *> CreateMarginsValue(MARGINS marVal);
    ValueResult<const PositionValue *> CreatePositionValue(POINT ptVal);
    ValueResult<const ColorValue *> CreateColorValue(COLORREF crVal);
    ValueResult<const IntListValue *> CreateIntListValue(int *rgiVal, UINT cVals);
    ValueResult<const FloatListValue *> CreateFloatListValue(float *rgflVal, UINT cVals);
    ValueResult<const StringValue *> CreateStringValue(LPCWSTR szVal);
    ValueResult<const AnimationSetValue *> CreateAnimationSetValue(std::vector<IniAssociation> rgAssociations);
};

#define CREATE_T_VALUE(TmType, specializedName) CreateTValue<Value<TmType>, &Value<TmType>::specializedName>
#define CREATE_T_LIST_VALUE(TmType, specializedName) CreateTListValue<Value<TmType>, &Value<TmType>::specializedName>

ValueResult<const IntValue *> CValueArena::CreateIntValue(int iVal)
{
    return CREATE_T_VALUE(TMT_INT, iVal)(iVal);
}

ValueResult<const EnumValue *> CValueArena::CreateEnumValue(int iVal)
{
    return CREATE_T_VALUE(TMT_ENUM, iVal)(iVal);
}

ValueResult<const FloatValue *> CValueArena::CreateFloatValue(float flVal)
{
    return CREATE_T_VALUE(TMT_FLOAT, flVal)(flVal);
}

ValueResult<const SizeValue *> CValueArena::CreateSizeValue(int iVal)
{
    return CREATE_T_VALUE(TMT_SIZE, iVal)(iVal);
}

ValueResult<const BoolValue *> CValueArena::CreateBoolValue(BOOL fVal)
{
    return fVal ? &_valBoolTrue : &_valBoolFalse;
}

ValueResult<const RectValue *> CValueArena::CreateRectValue(RECT rcVal)
{
    return CREATE_T_VALUE(TMT_RECT, rcVal)(rcVal);
}

ValueResult<const MarginsValue *> CValueArena::CreateMarginsValue(MARGINS marVal)
{
    return CREATE_T_VALUE(TMT_MARGINS, marVal)(marVal);
}

ValueResult<const ColorValue *> CValueArena::CreateColorValue(COLORREF crVal)
{
    return CREATE_T_VALUE(TMT_COLOR, crVal)(crVal);
}

ValueResult<const PositionValue *> CValueArena::CreatePositionValue(POINT ptVal)
{
    return CREATE_T_VALUE(TMT_POSITION, ptVal)(ptVal);
}

ValueResult<const IntListValue *> CValueArena::CreateIntListValue(int *rgiVal, UINT cVals)
{
    return CREATE_T_LIST_VALUE(TMT_INTLIST, rgiVal)(rgiVal, cVals);
}

ValueResult<const FloatListValue *> CValueArena::CreateFloatListValue(float *rgflVal, UINT cVals)
{
    return CREATE_T_LIST_VALUE(TMT_FLOATLIST, rgflVal)(rgflVal, cVals);
}

ValueResult<const StringValue *> CValueArena::CreateStringValue(LPCWSTR szVal)
{
    CEnsureArenaPointerChanged ensurePointerChanged(this);

    size_t cch = wcslen(szVal);
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
        return E_OUTOFMEMORY;
    }

    _pvCur += sizeof(Value<>) + pResult->cbSize;
    return pResult;
}

ValueResult<const AnimationSetValue *> CValueArena::CreateAnimationSetValue(std::vector<IniAssociation> rgAssociations)
{
    CEnsureArenaPointerChanged ensurePointerChanged(this);

    size_t targetSize = sizeof(IniAssociation) + rgAssociations.size();

    HRESULT hr = ResizeIfNecessary(sizeof(AnimationSetValue) + targetSize);
    if (FAILED(hr))
    {
        return hr;
    }

    AnimationSetValue *pResult = new (_pvCur) AnimationSetValue();
    pResult->cbSize = targetSize;
    if (!memcpy(pResult->rgAssociations, rgAssociations.data(), targetSize))
    {
        return E_OUTOFMEMORY;
    }

    _pvCur += sizeof(Value<>) + pResult->cbSize;
    return pResult;
}

class CIniParser
{
    CScanner _scanner;
    CSymbolManager *_pSymbolManager;
    CValueArena valueArena;
    std::vector<IniAssociation> _associations;

    // Precached symbols:
    Symbol *_pSymNullBaseClass = nullptr;
    
    IniSection _iniSectionCur;
    
    EParseMode _eMode;
    bool _fParsingIncludeChild = false;

    HRESULT Initialize();
    
    //--------------------------------------------------------------------------------------------------------------
    // Error handling
    
    ParseError _parseError;
    HRESULT SourceError(EParseErrorCode eCode, LPCWSTR szCustomMessage = nullptr, ...);

    //--------------------------------------------------------------------------------------------------------------

    std::wstring ReadNextWord();

    HRESULT ParseNextSectionHeader();
    HRESULT ParseNextAssociation();
    ValueResult<const std::wstring> ParseNextClassName();

    struct ParseManualSymbolResult
    {
        int iSymbolName = 0;
        int iType = 0;
    };

    /**
     * Used for intermediate parsing of symbols.
     */
    class CSymbolComponent
    {
        std::variant<std::monostate, std::wstring, ParseManualSymbolResult> _component;

    public:
        inline CSymbolComponent()
        {
        }

        inline CSymbolComponent(std::wstring strComponent)
            : _component(strComponent)
        {
        }

        inline CSymbolComponent(LPCWSTR szComponent)
            : CSymbolComponent(std::wstring(szComponent))
        {
        }

        inline CSymbolComponent(ParseManualSymbolResult manualComponent)
            : _component(manualComponent)
        {
        }

        inline CSymbolComponent &operator =(std::wstring strComponent)
        {
            _component = strComponent;
            return *this;
        }

        inline CSymbolComponent &operator =(LPCWSTR szComponent)
        {
            _component = std::wstring(szComponent);
            return *this;
        }

        inline CSymbolComponent &operator =(ParseManualSymbolResult manualComponent)
        {
            _component = manualComponent;
            return *this;
        }

        bool IsSet()
        {
            return !std::holds_alternative<std::monostate>(_component);
        }

        bool IsManual()
        {
            return std::holds_alternative<ParseManualSymbolResult>(_component);
        }

        ValueResult<std::wstring> GetString()
        {
            if (std::wstring *pStr = std::get_if<std::wstring>(&_component))
            {
                return *pStr;
            }

            return E_FAIL;
        }

        bool IsNonEmptyString()
        {
            ValueResult<std::wstring> str = GetString();

            if (str.Succeeded())
            {
                return !str.Unwrap().empty();
            }

            return false;
        }

        ValueResult<ParseManualSymbolResult> GetParseManualSymbolResult()
        {
            if (ParseManualSymbolResult *pStr = std::get_if<ParseManualSymbolResult>(&_component))
            {
                return *pStr;
            }

            return E_FAIL;
        }

        ValueResult<Symbol *> CreateAndAddSymbol(CSymbolManager *pSymbolManager, ESymbolType eSymType)
        {
            if (std::wstring *pStr = std::get_if<std::wstring>(&_component))
            {
                return pSymbolManager->AddSymbol(pStr->c_str(), GetPredefinedTypeCorrespondingToManual(eSymType));
            }
            else if (ParseManualSymbolResult *pManual = std::get_if<ParseManualSymbolResult>(&_component))
            {
                return pSymbolManager->AddManualSymbol(pManual->iSymbolName, GetManualTypeCorrespondingToPredefined(eSymType), pManual->iType);
            }
            else
            {
                // If we don't contain any information (i.e. if our value was never changed by the parser),
                // then we will always return a successful ValueResult containing a null pointer. Null
                // pointers to Symbol objects are regarded by all parsing code pertaining to them, so this
                // is fine.
                return nullptr;
            }
        }
    };

    ValueResult<const ParseManualSymbolResult> ParseNextManualSymbolSegment(ESymbolType eExpectType, bool fParsedAsterisk = true);
    ValueResult<const int> ParseSizeInfoUnits(int iNum, LPCWSTR pszDefaultUnits);

    ValueResult<const IntValue *> ParseIntValue();
    ValueResult<const FloatValue *> ParseFloatValue();
    ValueResult<const BoolValue *> ParseBoolValue();
    ValueResult<const SizeValue *> ParseSizeValue();
    ValueResult<const EnumValue *> ParseEnumValue(CSymbolComponent &rcmProperty);

    ValueResult<const IntListValue *> ParseIntListValue(UINT uLimit = 0);
    ValueResult<const FloatListValue *> ParseFloatListValue(UINT uLimit = 0);
    ValueResult<const RectValue *> ParseRectValue();
    ValueResult<const MarginsValue *> ParseMarginsValue();
    ValueResult<const PositionValue *> ParsePositionValue();
    ValueResult<const ColorValue *> ParseColorValue();
    
#ifdef ENABLE_PREPROCESSOR
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
#endif

    template <typename ItemType>
    friend ValueResult<std::vector<ItemType> > ParseListValue(CIniParser *pParser, UINT uLimit);

    template <typename T>
    friend ValueResult<std::vector<T> > ValidateStrictLengthListValue(CIniParser *pParser, int iLength, LPCWSTR pszFailMsg);
    
public:
    CIniParser(LPCWSTR szText, DWORD cchText);
    CIniParser(std::wstring text);
    
    // static CIniParser CreateIncludeIniParser(CIniParser *pParent, CSimpleFile *pChildFile);
    
    HRESULT Parse();

    ParseError GetParseError();
    
    bool ReadName(IN OUT LPWSTR szId, DWORD cchId);
    
    HRESULT Test();
};

CIniParser::CIniParser(LPCWSTR szText, DWORD cchText)
    : _scanner(szText, cchText)
{
    Initialize();
}

CIniParser::CIniParser(std::wstring text)
    : _scanner(text)
{
    Initialize();
}

HRESULT CIniParser::Initialize()
{
    // TODO: The symbol manager needs to be loaded into the INI parser from somewhere.
    // Where do we get it from? Do we create it ourselves and just relinquish control
    // once we don't need it anymore?
#ifdef TEMP_CREATE_OWN_SYMBOL_MANAGER
    // This leaks, as we do not have an explicit destructor.
    _pSymbolManager = new CSymbolManager();
#endif

    // TODO: How do we elegantly make the null base class symbol work out?
    // It's a weird edge case.
    _pSymNullBaseClass = _pSymbolManager->AddSymbol(nullptr, ESymbolType::BaseClass);

    return S_OK;
}

HRESULT CIniParser::SourceError(EParseErrorCode eCode, LPCWSTR szCustomMessage, ...)
{
    _parseError.eCode = eCode;
    _parseError.iLine = _scanner.GetLineNum();
    
    if (szCustomMessage)
    {
        va_list args;
        va_start(args, szCustomMessage);

        WCHAR szFormatted[1024];
        swprintf_s(szFormatted, szCustomMessage, args);
        
        va_end(args);

        _parseError.strMessage = szFormatted;
    }
    else
    {
        switch (eCode)
        {
            case EParseErrorCode::ExpectedCharacter:
                _parseError.strMessage = L"Expected character";
                break;
            case EParseErrorCode::ExpectedNumber:
                _parseError.strMessage = L"Expected number";
                break;
            case EParseErrorCode::ExpectedSymbol:
                _parseError.strMessage = L"Expected symbol";
                break;
        }
    }

    return HRESULT_FROM_WIN32(ERROR_UNKNOWN_PROPERTY);
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
#ifdef ENABLE_PREPROCESSOR
            case EParseMode::Preprocessor:
            {
                hr = ParseNextCPreprocessor();
                break;
            }
#endif
            
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

ParseError CIniParser::GetParseError()
{
    return _parseError;
}

/**
 * Parses manual symbol segments.
 * 
 * Manual symbols follow the format:
 *     *Identifer#N<Type>
 * 
 * The * character is often used for detection by other parser functions, and thus is usually
 * skipped by this function.
 * 
 * The components must be of the following types:
 *     * = A required prefix character to denote manual symbols.
 *     Identifier = One of case-insensitive built-in symbols "Part", "State", or interchangable
 *                  "Prop"/"Property". This is a required component.
 *     # = An optional character that may precede the number for aesthetic purposes.
 *     N = An integer number literal, which may be written in decimal or hexadecimal. For more
 *         information, see the general documentation on parsing integer numbers. This is a
 *         required component.
 *     Type = An optionally-parsed type annotation, required for property types and illegal
 *            for all other types. When used, the value must be a valid type symbol or enum
 *            name as specified in the schema.
 * 
 * Examples of major valid constructions include:
 *     *Part#1
 *     *State0xBADF00D
 *     *Property#1<Int>
 */
auto CIniParser::ParseNextManualSymbolSegment(ESymbolType eExpectType, bool fParsedAsterisk) -> ValueResult<const ParseManualSymbolResult>
{
    assert(IsSymbolTypeManual(eExpectType));

    if (!fParsedAsterisk)
    {
        if (!_scanner.GetChar(L'*'))
        {
            return SourceError(EParseErrorCode::ExpectedCharacter, L"Expected '*'");
        }
    }

    std::wstring strIdentifier = ReadNextWord();

    if (strIdentifier.empty())
    {
        return SourceError(EParseErrorCode::ExpectedSymbol, L"Expected a class identifer, got nothing");
    }

    if (eExpectType == ESymbolType::ManualPart && !AsciiStrCmpI(strIdentifier.c_str(), L"Part"))
    {
        return SourceError(EParseErrorCode::ExpectedSymbol, L"Unexpected identifier '%s', expected 'Part'.", strIdentifier.c_str());
    }
    else if (eExpectType == ESymbolType::ManualState && !AsciiStrCmpI(strIdentifier.c_str(), L"State"))
    {
        return SourceError(EParseErrorCode::ExpectedSymbol, L"Unexpected identifier '%s', expected 'State'.", strIdentifier.c_str());
    }
    else if (eExpectType == ESymbolType::ManualPropertyKey &&
        !AsciiStrCmpI(strIdentifier.c_str(), L"Prop") ||
        !AsciiStrCmpI(strIdentifier.c_str(), L"Property"))
    {
        return SourceError(EParseErrorCode::ExpectedSymbol, L"Unexpected identifier '%s', expected 'Prop' or 'Property'.", strIdentifier.c_str());
    }

    // Skip the optional "#" if it is there.
    _scanner.GetChar(L'#');

    int iValue;
    if (!_scanner.GetNumber(&iValue))
    {
        return SourceError(EParseErrorCode::ExpectedNumber);
    }

    int iType = 0; // 0 = Unset.

    // If we're parsing a property key type, then we always require a type name which is in the
    // schema. Since the schema is static data, we can validate that a specified name is legal
    // during this parsing pass.
    if (eExpectType == ESymbolType::ManualPropertyKey)
    {
        if (_scanner.GetChar(L'<'))
        {
            return SourceError(EParseErrorCode::ExpectedCharacter, L"Expected '<'");
        }

        std::wstring strType = ReadNextWord();

        if (strType.empty())
        {
            return SourceError(EParseErrorCode::ExpectedSymbol, L"Expected a type name");
        }

        // Find the type of the property from the schema:
        const TMPROPINFO *pPrimitiveTypeInfo = SearchSchema(ESchemaSearchQuery::PrimitiveProperty, strType.c_str());
        iType = pPrimitiveTypeInfo ? pPrimitiveTypeInfo->sEnumVal : 0;

        // Fail: the above loop fell through without setting a value.
        if (iType == 0)
        {
            return SourceError(EParseErrorCode::ExpectedSymbol, L"Unknown type name '%s' specified", strType.c_str());
        }

        switch (iType)
        {
            case TMT_ENUM:
            case TMT_HBITMAP:
            case TMT_DISKSTREAM:
            case TMT_STREAM:
            case TMT_BITMAPREF:
                return SourceError(
                    EParseErrorCode::UnexpectedSymbol, 
                    L"Properties may not be special type '%s', as values thereof are impossible to represent",
                    strType.c_str()
                );
        }

        if (_scanner.GetChar(L'>'))
        {
            return SourceError(EParseErrorCode::ExpectedCharacter, L"Expected '>'");
        }
    }

    return ParseManualSymbolResult { iValue, iType };
}

/*
 * Parses size info units.
 * 
 * Size info units are suffixes that may optionally follow numbers and specify the
 * unit of that number.
 * 
 * They may be of any of the following values:
 *     "pixels", "px" = Pixel units. The "px" synonym is a Restyle feature.
 *     "twips"        = 1/20's of a typographical point.
 *     "points", "pt" = Typographical points. The "pt" synonym is a Restyle feature.
 * 
 * As with any other symbol, spacing is optional. In our opinion, it is preferred to
 * include a space when writing the full name of a unit (i.e. "pixels") and to leave
 * the space out when writing the abbreviated name of a unit (i.e. "px").
 * 
 * Examples of major valid constructions include:
 *     20px
 *     43 pixels
 *     10 twips
 *     14pt
 *     12 points
 */
ValueResult<const int> CIniParser::ParseSizeInfoUnits(int iNum, LPCWSTR pszDefaultUnits)
{
    std::wstring strUnits = ReadNextWord();

    if (strUnits.empty())
    {
        strUnits = pszDefaultUnits;
    }

    LPCWSTR pszUnits = strUnits.c_str();

    if (AsciiStrCmpI(pszUnits, L"pixels") == 0 || AsciiStrCmpI(pszUnits, L"px") == 0)
    {
        // Fallthrough; we are already in pixels.
    }
    else if (AsciiStrCmpI(pszUnits, L"twips") == 0)
    {
        iNum *= -MulDiv(iNum, 96, 72 * 20);
    }
    else if (AsciiStrCmpI(pszUnits, L"points") == 0 || AsciiStrCmpI(pszUnits, L"pt") == 0)
    {
        iNum *= -MulDiv(iNum, 96, 72);
    }
    else
    {
        return SourceError(UnexpectedSymbol, L"Unexpected unit size '%s'", pszUnits);
    }

    return iNum;
}

/**
 * Parses class names.
 * 
 * Class names follow the format as ordained by Microsoft:
 *     AppName::ClassName
 * 
 * where the only required component is the ClassName.
 * 
 * The components must be of the following types:
 *     AppName = A class name fragment. This is a standard alphanumeric string.
 *     :: = An optionally-parsed character sequence which is required when an
 *          AppName component is provided.
 *     ClassName = A class name fragment. This is a standard alphanumeric string.
 * 
 * Examples of major valid constructions include:
 *     Button
 *     Start::Button
 */
ValueResult<const std::wstring> CIniParser::ParseNextClassName()
{
    std::wstring strFinalClass;
    std::wstring strFirst = strFinalClass = ReadNextWord();

    if (strFirst.empty())
    {
        return SourceError(EParseErrorCode::ExpectedSymbol, L"Expected a word");
    }

    // If we're going to follow with a "::" sequence, then we must not allow spaces
    // to surround it. While Restyle makes a distinction between the main class name
    // and the semantic base class, the visual styles engine itself does not care.
    // They are considered one unit, and the full name with the "::" sequence is
    // considered to be one single qualified name.
    bool fNextCharIsSpace = _scanner.IsSpace(_scanner.Read());

    if (_scanner.GetKeyword(L"::"))
    {
        if (fNextCharIsSpace)
        {
            return SourceError(EParseErrorCode::UnexpectedSpace);
        }

        strFinalClass += L"::";

        std::wstring strBaseClass = ReadNextWord();

        if (strBaseClass.empty())
        {
            return SourceError(EParseErrorCode::ExpectedSymbol, L"Expected a word");
        }

        strFinalClass += strBaseClass;
    }

    // We'll check again to dispatch appropriate meaningful parse errors:
    fNextCharIsSpace = _scanner.IsSpace(_scanner.Read());

    if (_scanner.GetKeyword(L"::"))
    {
        if (fNextCharIsSpace)
        {
            return SourceError(EParseErrorCode::UnexpectedSymbol, L"Unexpected '::' (you might have meant ':' instead)");
        }
        else
        {
            return SourceError(EParseErrorCode::UnexpectedSymbol, L"Unexpected '::'. Class names may not exceed a maximum of 2 levels of an app name and a base class.");
        }
    }

    return strFinalClass;
}

/**
 * Parses section headers.
 * 
 * Section headers follow the format:
 *     [Class.Part(State) : BaseClass]
 * 
 * where the only required component is the Class.
 * 
 * The components must be of the following types:
 *     Class = A class name. If this name does not already exist, it will be registered.
 *     . = An optionally-parsed separation token, required if the Part component is
 *         specified.
 *     Part = A qualified part name associated with the class, or a custom class name
 *            of the formats "*PartN" or "*Part#N" (where N is a 32-bit signed integer
 *            number), or unspecified.
 *     ( = An optionally-parsed separation token, required if the State component is
 *         specified.
 *     State = A qualified state name associated with the class or part, or a custom
 *             part name of the format "*StateN" or "*State#N" (where N is a 32-bit
 *             signed integer number), or unspecified.
 *     ) = An optionally-parsed separation token, required if the State component is
 *         specified.
 *     : = An optionally-parsed separation token, required if the BaseClass component is
 *         specified.
 *     BaseClass = A qualified class name, the special keyword "*None", the integer
 *                 number 0, or unspecified.
 * 
 * All qualified types must be referenced elsewhere in the theme codebase. Nonexistent
 * names will cause a parse error.
 * 
 * State component names may correlate with either the symbol for the Part component or
 * the Class component. If the Part was specified, then the State is associated
 * with it, otherwise it is associated with the Class. In other words, it binds to
 * the last specified component.
 * 
 * The BaseClass component may not co-exist with a Part and/or a State. The
 * backed component, the inheriting base class that is compiled into the BCMAP in the
 * final msstyles, is usually implied from the semantic base class in the Class component. The
 * BaseClass component may be used for complex inheritance or otherwise
 * overriding the implicit base class. In order to force the use of no base class, the
 * case-insensitive special symbol "*None" may be used instead of a qualified class
 * name, or alternatively the integer number "0" may be used for the same purpose.
 * 
 * Examples of major valid constructions include:
 *     [Class]
 *     [Class(State)]
 *         whereby the State name must correlate with the ClassName.
 *     [Class.Part(State)]
 *         whereby the State name must correlate with the Part name
 *     [Class : BaseClass]
 *     [LVPopup::Toolbar : *None]
 *     [LVPopup::Toolbar : 0]
 */
HRESULT CIniParser::ParseNextSectionHeader()
{
    if (!_scanner.GetChar(L'['))
    {
        return SourceError(EParseErrorCode::ExpectedCharacter, L"Expected '['");
    }

    CSymbolComponent cmClass, cmState, cmPart, cmBaseClass;
    bool fEmptyBaseClass = false;

    // The next word should always be the class name. This is the only required part
    // of the section header.
    PROPAGATE_ERROR_IF_FAILED(cmClass = ParseNextClassName());

    // If we're going to follow with a "::" sequence, then we must not allow spaces
    // to surround it. While Restyle makes a distinction between the semantic base
    // class and the main class name, the visual styles engine itself does not care.
    // They are considered one unit, and the full name with the "::" sequence is
    // considered to be one single qualified name.
    bool fNextCharIsSpace = _scanner.IsSpace(_scanner.Read());

    // We loop so we can easily report error messages for out-of-order (i.e. accidentally
    // malformed) text. The statements within the loop should be in the correct strict
    // order, separated within an else-if chain.
    while (!_scanner.EndOfFile())
    {
        // If we follow with a "." character, then we're parsing a part specifier.
        if (_scanner.GetChar(L'.'))
        {
            // Ensure that we're in the right position:
            // TODO: add cases

            if (_scanner.GetChar(L'*'))
            {
                PROPAGATE_ERROR_IF_FAILED(cmPart = ParseNextManualSymbolSegment(ESymbolType::ManualPart));
            }
            else
            {
                cmPart = ReadNextWord();

                if (!cmPart.IsNonEmptyString())
                {
                    return SourceError(EParseErrorCode::ExpectedSymbol);
                }
            }
        }
        // If we follow with a "(" character, then we're parsing a state specifier.
        else if (_scanner.GetChar(L'('))
        {
            // Ensure that we're in the right position:
            // TODO: add cases

            if (_scanner.GetChar(L'*'))
            {
                PROPAGATE_ERROR_IF_FAILED(cmState = ParseNextManualSymbolSegment(ESymbolType::ManualState));
            }
            else
            {
                cmState = ReadNextWord();

                if (!cmState.IsNonEmptyString())
                {
                    return SourceError(EParseErrorCode::ExpectedSymbol);
                }
            }

            if (!_scanner.GetChar(L')'))
            {
                return SourceError(EParseErrorCode::ExpectedCharacter, L"Expected ')'");
            }
        }
        // If we follow with a ":" character, then we're parsing a manual inheriting
        // class name specifier.
        else if (_scanner.GetChar(L':'))
        {
            // Ensure that we're in the right position:
            // TODO: add cases

            int i;
            if (_scanner.GetKeyword(L"*None") || (_scanner.GetNumber(&i) && i == 0))
            {
                fEmptyBaseClass = true;
            }
            else
            {
                PROPAGATE_ERROR_IF_FAILED(cmBaseClass = ParseNextClassName());

                if (cmBaseClass.GetString().Unwrap().empty())
                {
                    return SourceError(EParseErrorCode::ExpectedSymbol);
                }
            }
        }
        // If we follow with a "]" character, then we will terminate the whole sequence
        // and verify that we have some form of valid output.
        else if (_scanner.GetChar(L']'))
        {
            break;
        }
        // Otherwise, we have a completely unexpected sequence, and so we'll just return
        // a parse error.
        else
        {
            return SourceError(EParseErrorCode::UnexpectedSequence);
        }
    }

    //
    // Create symbols for each of the components:
    //

    Symbol *pSymClass = nullptr, *pSymPart = nullptr, *pSymState = nullptr, *pSymBaseClass = nullptr;

    pSymClass = cmClass.CreateAndAddSymbol(_pSymbolManager, ESymbolType::Class);
    pSymPart = cmPart.CreateAndAddSymbol(_pSymbolManager, ESymbolType::Part);
    pSymState = cmPart.CreateAndAddSymbol(_pSymbolManager, ESymbolType::State);

    if (cmBaseClass.IsNonEmptyString())
    {
        pSymBaseClass = cmBaseClass.CreateAndAddSymbol(_pSymbolManager, ESymbolType::BaseClass);
    }
    else if (fEmptyBaseClass)
    {
        pSymBaseClass = _pSymNullBaseClass;
    }

    //
    // Create the internal representation of the INI section and set it as the currently-active one:
    //

    _iniSectionCur = {};

    _iniSectionCur.pSymClass = pSymClass;
    _iniSectionCur.pSymPart = pSymPart;
    _iniSectionCur.pSymState = pSymState;
    _iniSectionCur.pSymBaseClass = pSymBaseClass;

    //
    // Update the current state of the parser:
    //

    if (pSymBaseClass)
    {
        // If we parsed an inheritance definition, then we do not allow properties under any
        // circumstances.
        _eMode = EParseMode::SectionHeader;
    }
    else
    {
        // Otherwise, we expect associations to follow.
        _eMode = EParseMode::Assoc;
    }

    return S_OK;
}

/**
 * Parses an association.
 * 
 * Associations follow the format:
 *     Property = Value
 * 
 * The components must be of the following types:
 *     Property = A qualified property name as defined in the Theme Manager schema, or a custom
 *                property of the formats "*Property#N<Type>", "*Prop#N<Type>" where N is a
                  32-bit signed integer number, and Type is a qualified primitive type name
 *                or enum name as defined in the Theme Manager schema. In either case, this
 *                component has a primitive type, either implicitly retrieved from the schema, or
 *                manually specified in the case of custom properties.
 *     `=` = A required separation character delineating the Property and Value components.
 *     Value = A value matching the format outlined for the primitive type of the Property
 *             component. For more information, see the documentation for the parsers for each of
 *             the value types.
 * 
 * Examples of major valid constructions include:
 *     Transparent = True
 *     ImageCount = 8
 *     ImageFile1 = "ButtonBackground.png"
 *     ContentMargins = 5, 5, 5, 5
 *     *Property#9000<Float> = 3.2
 */
HRESULT CIniParser::ParseNextAssociation()
{
    CSymbolComponent cmProperty;

    const TMPROPINFO *pPropInfo = nullptr;

    if (_scanner.GetChar(L'*'))
    {
        PROPAGATE_ERROR_IF_FAILED(cmProperty = ParseNextManualSymbolSegment(ESymbolType::ManualPropertyKey));
    }
    else
    {
        // Always expect a string name.
        cmProperty = ReadNextWord();

        if (!cmProperty.IsNonEmptyString())
        {
            return SourceError(EParseErrorCode::ExpectedSymbol);
        }

        if (cmProperty.GetString().Failed())
        {
            // TODO: InvalidSymbol seems weird. Perhaps a different code should be introduced for this type.
            return SourceError(EParseErrorCode::InvalidSymbol, L"Failed to parse property name.");
        }

        // Ensure that the name is statically valid from schema
        pPropInfo = SearchSchema(ESchemaSearchQuery::SearchWholeSchema, cmProperty.GetString().Unwrap().c_str());
        if (!pPropInfo)
        {
            return SourceError(EParseErrorCode::InvalidSymbol, L"Invalid symbol name '%s'", cmProperty.GetString().Unwrap().c_str());
        }
    }

    assert(pPropInfo != nullptr);

    if (!_scanner.GetChar(L'='))
    {
        return SourceError(EParseErrorCode::ExpectedCharacter, L"Expected '='");
    }

    //
    // Determine the type of the value:
    //

    int iPrimVal;

    if (!cmProperty.IsManual())
    {
        iPrimVal = pPropInfo->bPrimVal;
    }
    else
    {
        iPrimVal = cmProperty.GetParseManualSymbolResult().Unwrap().iType;
    }

    //
    // Parse the value depending on the type:
    //

    Value<> *pValue = nullptr;

    switch (iPrimVal)
    {
        case TMT_INT:
        {
            PROPAGATE_ERROR_IF_FAILED(pValue = (Value<> *)ParseIntValue());
            break;
        }

        case TMT_BOOL:
        {
            PROPAGATE_ERROR_IF_FAILED(pValue = (Value<> *)ParseBoolValue());
            break;
        }

        case TMT_SIZE:
        {
            PROPAGATE_ERROR_IF_FAILED(pValue = (Value<> *)ParseSizeValue());
            break;
        }

        case TMT_ENUM:
        {
            PROPAGATE_ERROR_IF_FAILED(pValue = (Value<> *)ParseEnumValue(cmProperty));
            break;
        }

        case TMT_RECT:
        {
            PROPAGATE_ERROR_IF_FAILED(pValue = (Value<> *)ParseRectValue());
            break;
        }

        case TMT_MARGINS:
        {
            PROPAGATE_ERROR_IF_FAILED(pValue = (Value<> *)ParseMarginsValue());
            break;
        }

        case TMT_POSITION:
        {
            PROPAGATE_ERROR_IF_FAILED(pValue = (Value<> *)ParsePositionValue());
            break;
        }

        case TMT_COLOR:
        {
            PROPAGATE_ERROR_IF_FAILED(pValue = (Value<> *)ParseColorValue());
            break;
        }

        case TMT_INTLIST:
        {
            PROPAGATE_ERROR_IF_FAILED(pValue = (Value<> *)ParseIntListValue());
            break;
        }

        case TMT_FLOATLIST:
        {
            PROPAGATE_ERROR_IF_FAILED(pValue = (Value<> *)ParseFloatListValue());
            break;
        }
    }

    //
    // Create symbols for each of the components:
    //

    Symbol *pSymProperty = cmProperty.CreateAndAddSymbol(_pSymbolManager, ESymbolType::PropertyKey);
    assert(pSymProperty);

    //
    // Create the internal representation of the property and add it to the list:
    //

    IniAssociation assoc;
    assoc.section = _iniSectionCur;
    assoc.pKeySymbol = pSymProperty;
    assert(pValue);
    assoc.pVal = pValue;

    return S_OK;
}

/*
 * Parses an integer value.
 *
 * Integer values follow the following format:
 *      {Sign}{Number}
 * 
 * The components must be of the following types:
 *      Sign = An optional signage symbol, which may not be followed by whitespace. It may
 *             be either a "+" or "-" symbol. "+" will have no effect on the number, and "-"
 *             will invert the number.
 *      Number = A number parsed from a decimal number sequence using the digits 0 through 9
 *               or a hexadecimal number sequenced, prefixed with the case insensitive prefix
 *               "0x", using the case-insensitive digits 0 through F.
 * 
 * Examples of major valid constructions include:
 *      0
 *      123
 *      +100
 *      -456
 *      2147483647
 *      0xBADBEEF
 */
ValueResult<const IntValue *> CIniParser::ParseIntValue()
{
    int iResult;
    if (_scanner.GetNumber(&iResult))
    {
        return valueArena.CreateIntValue(iResult);
    }
    
    return SourceError(EParseErrorCode::ExpectedNumber, L"An integer value must be a valid integer number");
}

/*
 * Parses a floating-point number value.
 *
 * Floating-point number values follow the following format:
 *      {Sign}{Number}.{Decimal}e{Exponent}
 *
 * The components must be of the following types:
 *      Sign = An optional signage symbol, which may not be followed by whitespace. It may
 *             be either a "+" or "-" symbol. "+" will have no effect on the number, and "-"
 *             will invert the number.
 *      Number = A number parsed from a decimal number sequence using the digits 0 through 9,
 *               or the built-in case-insensitive keyword "NaN", or the built-in case-insensitive
 *               keyword "Infinity".
 *      . = An optionally-parsed character which separates the Number and Decimal components.
 *      Decimal = An optionally-parsed number parsed from a decimal number sequence using the
 *                digits 0 through 9.
 *      e = An optionally-parsed character which separates the previous Number or Decimal component
 *          and the Exponent component.
 *      Exponent = An optionally-parsed number parsed from a decimal number sequence using the
 *                 digits 0 through 9.
 *
 * Examples of major valid constructions include:
 *      0
 *      123
 *      +100
 *      -123.456
 *      1.234e10
 *      -0.0
 *      NaN
 *      -Infinity
 */
ValueResult<const FloatValue *> CIniParser::ParseFloatValue()
{
    float flResult;
    if (_scanner.GetFloatNumber(&flResult))
    {
        return valueArena.CreateFloatValue(flResult);
    }

    return SourceError(EParseErrorCode::ExpectedNumber, L"A floating-point number value must be a valid floating-point number.");
}

/**
 * Parses a boolean value.
 * 
 * Boolean values follow the following format:
 *      TrueOrFalse
 * 
 * The component must be of the following type:
 *      TrueOrFalse = One of the built-in keywords case-insensitive keywords: "True" or "False".
 * 
 * Examples of major valid constructions include:
 *      True
 *      False
 */
ValueResult<const BoolValue *> CIniParser::ParseBoolValue()
{
    if (_scanner.GetKeyword(L"True"))
    {
        return valueArena.CreateBoolValue(TRUE);
    }
    else if (_scanner.GetKeyword(L"False"))
    {
        return valueArena.CreateBoolValue(FALSE);
    }

    return SourceError(EParseErrorCode::ExpectedSymbol, L"A boolean value must be either true or false");
}

/**
 * Parses a size value.
 * 
 * Size values follow the following format:
 *     {IntegerNumber}{Unit}
 * 
 * The components must be of the following types:
 *     IntegerNumber = A valid integer number. See the documentation on parsing integer numbers
 *                     for more information.
 *     Unit = One of the valid size info units, as described in the documentation for
 *            ParseSizeInfoUnits. The default unit is "pixels".
 */
ValueResult<const SizeValue *> CIniParser::ParseSizeValue()
{
    int iResult;
    if (_scanner.GetNumber(&iResult))
    {
        // Try to read the explicit unit if it exists.:
        ValueResult<const int> vrUnit = ParseSizeInfoUnits(iResult, L"pixels");

        // If something went wrong while trying to read the explicit unit, then
        // propagate the error. This case does not encompass the lack of a unit,
        // since units are always optional. This case will be hit, i.e., if the
        // specified manual unit is illegal.
        PROPAGATE_ERROR_IF_FAILED(vrUnit);

        return valueArena.CreateSizeValue(vrUnit.Unwrap());
    }

    return SourceError(EParseErrorCode::ExpectedNumber, L"An size value must be a valid integer number");
}

/**
 * Parses an enum value.
 * 
 * Enum values follow the following format:
 *      EnumValue
 * 
 * The component must be of the following type:
 *      EnumValue = A qualified enum value defined in the Theme Manager schema that corresponds
 *                  to the Property component of the parsing context.
 * 
 * Examples of major valid constructions include, in the case of the use within associations:
 *      ImageLayout = Vertical
 *      BgType = ImageFile
 *      SizingType = TrueSize
 */
ValueResult<const EnumValue *> CIniParser::ParseEnumValue(CSymbolComponent &rcmProperty)
{
    SearchSchemaParams searchParams = { 0 };
    searchParams.cbSize = sizeof(searchParams);
    searchParams.eSearchQuery = ESchemaSearchQuery::Enum;

    if (rcmProperty.IsManual() || rcmProperty.IsNonEmptyString())
    {
        return E_INVALIDARG;
    }

    searchParams.szName = rcmProperty.GetString().Unwrap().c_str();

    // TODO: Implement target OS version manager.
    searchParams.eSupportedOs = ESupportedOS::All;

    const TMPROPINFO *pEnumIndex = SearchSchema(&searchParams);

    if (!pEnumIndex)
    {
        return SourceError(EParseErrorCode::TypeDoesNotExist, L"Failed to find enum definition for %s", searchParams.szName);
    }

    // Read the next word to figure out the enum value.
    std::wstring strEnumVal = ReadNextWord();

    if (strEnumVal.empty())
    {
        return SourceError(EParseErrorCode::ExpectedSymbol, L"Expected an enum value name");
    }

    // Iterate the enum properties until we find a match.
    const TMPROPINFO *pCurVal = pEnumIndex;
    bool fFoundVal = false;
    while (pCurVal = SearchSchema(ESchemaSearchQuery::Enum, pCurVal, TMT_ENUM))
    {
        if (AsciiStrCmpI(pCurVal->pszName, strEnumVal.c_str()) == 0)
        {
            // We found the value, so break.
            fFoundVal = true;
            break;
        }
    }

    if (!fFoundVal)
    {
        return SourceError(EParseErrorCode::UnexpectedSymbol, L"'%s' is not a valid value for the enum type '%s'", 
            strEnumVal.c_str(), searchParams.szName
        );
    }

    return valueArena.CreateEnumValue(pCurVal->sEnumVal);
}

template <typename ItemType>
    requires std::is_same_v<ItemType, int> || 
             std::is_same_v<ItemType, float>
static ValueResult<std::vector<ItemType> > ParseListValue(CIniParser *pParser, UINT uLimit)
{
    bool (CScanner:: *pfnGetNextValue)(OUT ItemType *);

    if constexpr (std::is_same_v<ItemType, int>)
         pfnGetNextValue = &CScanner::GetNumber;
    else if constexpr (std::is_same_v<ItemType, float>)
         pfnGetNextValue = &CScanner::GetFloatNumber;

    // Lists are particularly hard to parse, because they can span multiple lines and have two
    // "equally valid" characters for item separation. The number-scanning methods in CScanner
    // already regard parsing the next separation. We only allow multi-line parsing if the line
    // ends in a comma.
    bool fLastCharWasComma = false;
    int iCurrentItem = 0;
    std::vector<ItemType> vecItems;
    do
    {
        bool fIsFirstItemOfSecondaryLine = false;

        if (uLimit > 0 && iCurrentItem > uLimit - 1)
        {
            return pParser->SourceError(
                ItemCountMismatch,
                L"The maximum number of items that this list can hold is %d.",
                uLimit
            );
        }

        // Parse the next value:
        ItemType next{};
        if (!(pParser->_scanner.*pfnGetNextValue)(&next))
        {
            return pParser->SourceError(
                EParseErrorCode::ExpectedNumber,
                L"Expected number at item #%d of list%s",
                iCurrentItem,
                (fIsFirstItemOfSecondaryLine
                ? L". If you did not mean for this line to be parsed as a list, remove the comma from "
                L"the end of the list on the previous line."
                : L"")
            );
        }

        // When we call SkipSpaces here, we want to avoid skipping to the next line so that we can perform our
        // own special checks for such an operation.
        pParser->_scanner.SkipSpaces(false);
        fLastCharWasComma = pParser->_scanner.GetChar(L',');
        pParser->_scanner.SkipSpaces(false);

        // Since this flag isn't needed for the current iteration anymore, this is a good place to clear it.
        fIsFirstItemOfSecondaryLine = false;

        if (pParser->_scanner.Read() == 0) // 0 indicates end of line
        {
            if (!fLastCharWasComma)
            {
                // Even if there is data that the user intended to be relevant to us, the syntax is
                // malformed and it's none of our business now.
                break;
            }

            // Call SkipSpaces again and progress to the next line.
            pParser->_scanner.SkipSpaces(true);
            fIsFirstItemOfSecondaryLine = true;
        }

        vecItems.push_back(next);
        iCurrentItem++;
    }
    while (!pParser->_scanner.EndOfFile());

    return vecItems;
}

/*
 * Parse an integer list value.
 * 
 * Integer list values follow the following format, where the body may repeat indefinitely:
 *     {{IntegerNumber}{Comma}{WhiteSpace}}...
 * 
 * Lists of numbers may not end in a comma, as line-final commas are regarded by the parser to mark
 * continuation of a list onto the next line.
 * 
 * As such, it is legal to write:
 *     *Property#1<IntList> = 1, 2, 3, 4,
 *                            5, 6, 7, 8
 * 
 * But illegal to write:
 *     *Property#1<IntList> = 1 2 3 4
 *                            5 6 7 8
 * 
 * ...due to syntactic ambiguity.
 * 
 * The components must be of the following types:
 *     IntegerNumber = A valid integer number. See the documentation on parsing integer numbers
 *                     for more information.
 *     Comma = A "," character. This explicitly marks that there is another list entry to come.
 *     WhiteSpace = One or more whitespace characters. This implicitly marks that there is another
 *                  list entry to come.
 * 
 * Examples of major valid constructions include:
 *     1, 2, 3, 4, 5
 *     6 7 8 9 10
 */
ValueResult<const IntListValue *> CIniParser::ParseIntListValue(UINT uLimit)
{
    ValueResult<std::vector<int> > vrVecItems = ParseListValue<int>(this, uLimit);
    PROPAGATE_ERROR_IF_FAILED(vrVecItems);
    return valueArena.CreateIntListValue(vrVecItems.Unwrap().data(), vrVecItems.Unwrap().size());
}

/*
 * Parse an floating-point number list value.
 *
 * Floating-point number list values follow the following format, where the body may repeat indefinitely:
 *     {{FloatingPointNumber}{Comma}{WhiteSpace}}...
 *
 * Lists of numbers may not end in a comma, as line-final commas are regarded by the parser to mark
 * continuation of a list onto the next line.
 *
 * As such, it is legal to write:
 *     *Property#1<FloatList> = 1.0, 1.1, 1.2, 1.3,
 *                              1.4, 1.5, 1.6, 1.7
 *
 * But illegal to write:
 *     *Property#1<FloatList> = 1.0 1.1 1.2 1.3
 *                              1.4 1.5 1.6 1.7
 *
 * ...due to syntactic ambiguity.
 *
 * The components must be of the following types:
 *     FloatingPointNumber = A valid floating-point number. See the documentation on parsing
 *                           floating-point numbers for more information.
 *     Comma = A "," character. This explicitly marks that there is another list entry to come.
 *     WhiteSpace = One or more whitespace characters. This implicitly marks that there is another
 *                  list entry to come.
 *
 * Examples of major valid constructions include:
 *     1.0, 2.5, 3.2, 4.6, 5.93, NaN, Infinity
 *     6.12 7.34 8.43 -Infinity 9.288 10.499 NaN
 */
ValueResult<const FloatListValue *> CIniParser::ParseFloatListValue(UINT uLimit)
{
    ValueResult<std::vector<float> > vrVecItems = ParseListValue<float>(this, uLimit);
    PROPAGATE_ERROR_IF_FAILED(vrVecItems);
    return valueArena.CreateFloatListValue(vrVecItems.Unwrap().data(), vrVecItems.Unwrap().size());
}

template <typename T>
static ValueResult<std::vector<T> > ValidateStrictLengthListValue(CIniParser *pParser, int iLength, LPCWSTR pszFailMsg)
{
    ValueResult<std::vector<int> > vrVecItems = ParseListValue<int>(pParser, iLength);

    // If the error is not a parse error or if it is a non-item-count-mismatch parse error,
    // then propagate the error if so. We do this to override the error mismatch for our
    // strict count vector.
    if (vrVecItems.GetResult() == HRESULT_FROM_WIN32(ERROR_UNKNOWN_PROPERTY) || pParser->_parseError.eCode == ItemCountMismatch)
    {
        return pParser->SourceError(ItemCountMismatch, pszFailMsg);
    }
    else if (vrVecItems.Failed())
    {
        PROPAGATE_ERROR_IF_FAILED(vrVecItems);
    }

    std::vector<int> &vec = vrVecItems.Unwrap();

    // ParseListValue will validate the upper boundary, but not the lower boundary.
    // As such, we valid the lower boundary here:
    if (vec.size() != iLength)
    {
        return pParser->SourceError(ItemCountMismatch, pszFailMsg);
    }

    return vrVecItems;
}

/*
 * Parses a rectangle value.
 * 
 * Rectangle values follow the following format:
 *     {Left}{,}{Top}{,}{Right}{,}{Bottom}
 * 
 * Rectangle values are lists parsed using the same rules as the IntList type. However,
 * they must consist of exactly 4 value components, rather than having an indefinite
 * number like values of the IntList type can freely have.
 * 
 * The components must be of the following types:
 *     , = A comma or whitespace separation character. These follow the general list
 *         type separation rules.
 *     Left = An integer number.
 *     Top = An integer number.
 *     Right = An integer number.
 *     Bottom = An integer number.
 * 
 * Examples of major valid constructions include:
 *     0, 0, 20, 20
 *     0 0 20 20
 */
ValueResult<const RectValue *> CIniParser::ParseRectValue()
{
    constexpr LPCWSTR pszInvalidCountErrorMsg = L"Rect values must have exactly 4 specified values.";

    ValueResult<std::vector<int> > vrVecItems = ValidateStrictLengthListValue<int>(this, 4, pszInvalidCountErrorMsg);
    PROPAGATE_ERROR_IF_FAILED(vrVecItems);
    auto &vec = vrVecItems.Unwrap();

    // The parsed parts follow the order: left, top, right, bottom
    RECT rc;
    rc.left = vec[0];
    rc.top = vec[1];
    rc.right = vec[2];
    rc.bottom = vec[3];

    return valueArena.CreateRectValue(rc);
}

/*
 * Parses a margins value.
 *
 * Margin values follow the following format:
 *     {LeftWidth}{,}{RightWidth}{,}{TopHeight}{,}{BottomHeight}
 *
 * Margin values are lists parsed using the same rules as the IntList type. However,
 * they must consist of exactly 4 value components, rather than having an indefinite
 * number like values of the IntList type can freely have.
 *
 * The components must be of the following types:
 *     , = A comma or whitespace separation character. These follow the general list
 *         type separation rules.
 *     LeftWidth = An integer number.
 *     RightWidth = An integer number.
 *     TopHeight = An integer number.
 *     BottomHeight = An integer number.
 *
 * Examples of major valid constructions include:
 *     5, 5, 5, 5
 *     5 5 5 5
 */
ValueResult<const MarginsValue *> CIniParser::ParseMarginsValue()
{
    constexpr LPCWSTR pszInvalidCountErrorMsg = L"Margin values must have exactly 4 specified values.";

    ValueResult<std::vector<int> > vrVecItems = ValidateStrictLengthListValue<int>(this, 4, pszInvalidCountErrorMsg);
    PROPAGATE_ERROR_IF_FAILED(vrVecItems);
    auto &vec = vrVecItems.Unwrap();

    // The parsed parts follow the order: left width, right width, top height, bottom height
    MARGINS mar;
    mar.cxLeftWidth = vec[0];
    mar.cxRightWidth = vec[1];
    mar.cyTopHeight = vec[2];
    mar.cyBottomHeight = vec[3];

    return valueArena.CreateMarginsValue(mar);
}

/*
 * Parses a position value.
 *
 * Position values follow the following format:
 *     {X}{,}{Y}
 *
 * Position values are lists parsed using the same rules as the IntList type. However,
 * they must consist of exactly 2 value components, rather than having an indefinite
 * number like values of the IntList type can freely have.
 *
 * The components must be of the following types:
 *     , = A comma or whitespace separation character. These follow the general list
 *         type separation rules.
 *     X = An integer number.
 *     Y = An integer number.
 *
 * Examples of major valid constructions include:
 *     32, 32
 *     32 32
 */
ValueResult<const PositionValue *> CIniParser::ParsePositionValue()
{
    constexpr LPCWSTR pszInvalidCountErrorMsg = L"Margin values must have exactly 2 specified values.";

    ValueResult<std::vector<int> > vrVecItems = ValidateStrictLengthListValue<int>(this, 2, pszInvalidCountErrorMsg);
    PROPAGATE_ERROR_IF_FAILED(vrVecItems);
    auto &vec = vrVecItems.Unwrap();

    // The parsed parts follow the order: x, y
    POINT pt;
    pt.x = vec[0];
    pt.y = vec[1];

    return valueArena.CreatePositionValue(pt);
}

/*
 * Parses a color value.
 *
 * Color values follow the following formats:
 *     {Red}{,}{Green}{,}{Blue}
 *     #HexColorDefinition
 *
 * Color values are lists parsed using the same rules as the IntList type. However,
 * they must consist of exactly 3 value components, rather than having an indefinite
 * number like values of the IntList type can freely have.
 *
 * For the first format, the components must be of the following types:
 *     , = A comma or whitespace separation character. These follow the general list
 *         type separation rules.
 *     Red = An integer number.
 *     Green = An integer number.
 *     Blue = An integer number.
 * 
 * Restyle provides a secondary format reminiscent of color value definitions in CSS.
 * For this format, the components must be of the following types:
 *     # = A required prefix character to denote that this format is requested instead
 *         of the list-based format.
 *     HexColorDefinition = A 3-digit or 6-digit hexadecimal number sequence specifying
 *                          the red, green, and blue color parts. In the case of 3-digit
 *                          sequences, each digit is doubled and each hexadecimal number
 *                          is reparsed thereafter.
 *
 * Examples of major valid constructions include:
 *     255, 255, 255
 *     255 255 255
 *     #FFFFFF
 */
ValueResult<const ColorValue *> CIniParser::ParseColorValue()
{
    constexpr LPCWSTR pszInvalidCountErrorMsg = L"Color values must have exactly 3 specified values.";

    // XXX(isabella): The parser in XP does not reject lists with more than 3 values. The excess
    // values are just ignored. This is good for Restyle, but it would be cool to implement an
    // option to suppress this validation for the ease of porting XP custom themes which may be
    // malformed (by our standards).

    ValueResult<std::vector<int> > vrVecItems = ValidateStrictLengthListValue<int>(this, 3, pszInvalidCountErrorMsg);
    PROPAGATE_ERROR_IF_FAILED(vrVecItems);
    auto &vec = vrVecItems.Unwrap();

    // The parsed parts follow the order: red, green, blue
    return valueArena.CreateColorValue(RGB(vec[0], vec[1], vec[2]));
}

#ifdef ENABLE_PREPROCESSOR
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
#endif

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