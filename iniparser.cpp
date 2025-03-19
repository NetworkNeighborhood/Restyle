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

public:
    ValueResult<const IntValue *> CreateIntValue(int iVal);
    ValueResult<const FloatValue *> CreateFloatValue(float flVal);
    ValueResult<const SizeValue *> CreateSizeValue(int iVal);
    ValueResult<const BoolValue *> CreateBoolValue(BOOL fVal);
    ValueResult<const RectValue *> CreateRectValue(RECT rcVal);
    ValueResult<const MarginsValue *> CreateMarginsValue(MARGINS marVal);
    ValueResult<const StringValue *> CreateStringValue(LPCWSTR szVal);
    ValueResult<const AnimationSetValue *> CreateAnimationSetValue(std::vector<IniAssociation> rgAssociations);
};

#define CREATE_T_VALUE(TmType, specializedName) CreateTValue<Value<TmType>, &Value<TmType>::specializedName>

ValueResult<const IntValue *> CValueArena::CreateIntValue(int iVal)
{
    return CREATE_T_VALUE(TMT_INT, iVal)(iVal);
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

    ValueResult<const IntValue *> ParseIntValue();
    ValueResult<const FloatValue *> ParseFloatValue();
    ValueResult<const BoolValue *> ParseBoolValue();
    ValueResult<const EnumValue *> ParseEnumValue(CSymbolComponent &rcmProperty);
    
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

        case TMT_ENUM:
        {
            PROPAGATE_ERROR_IF_FAILED(pValue = (Value<> *)ParseEnumValue(cmProperty));
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
    // TODO: Expand SchemaUtils to make this nicer to do.
    return E_NOTIMPL;
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