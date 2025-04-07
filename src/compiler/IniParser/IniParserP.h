#include "IniParser.h"

#include <variant>
#include "Util.h"
#include "Scanner.h"
#include "compiler/Symbol.h"
#include "compiler/SymbolManager.h"
#include "compiler/Value.h"
#include "compiler/ValueArena.h"

// Uncomment to enable testing:
#define ENABLE_INIPARSER_TEST 1

// Uncomment to enable preprocessor support, which is currently incomplete.
//#define ENABLE_PREPROCESSOR

namespace IniParser
{

template <typename EType, typename EMap, typename EMapArray, const EMapArray &pMap, EType EMap::*x, EType EMap::*y>
constexpr EType GetXTypeCorrespondingToY(EType eX)
{
    for (auto &i : pMap)
    {
        if (i.*x == eX)
        {
            return i.*y;
        }
    }

    return eX;
}

/**
 * Evaluates an expression and propagates the error result of a ValueResult if it failed.
 *
 * The expression must end in a ValueResult or this macro will fail to produce valid
 * code. Additionally, this macro must only be in functions returning a ValueResult or
 * a HRESULT.
 */
#define PROPAGATE_ERROR_IF_FAILED(errorExpr)                                             \
    do                                                                                   \
    {                                                                                    \
        HRESULT __hr;                                                                    \
                                                                                         \
        /* Evaluate the expression and get its value into the __result. */               \
        errorExpr.ResultInto(&__hr).Unwrap();                                            \
                                                                                         \
        if (FAILED(__hr))                                                                \
        {                                                                                \
            /* Propagate error. */                                                       \
            return __hr;                                                                 \
        }                                                                                \
    }                                                                                    \
    while ((void)0, 0)

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

enum class EParseMode
{
    FigureItOut,
    Assoc,
    SectionHeader,
    Preprocessor,
};

class CIniParser : public IIniParser
{
    CScanner _scanner;
    CSymbolManager *_pSymbolManager;
    CValueArena valueArena;

    // TODO(isabella): std::vector is reallocated every time that it needs to expand (i.e. due to a push_back). Move
    // to an arena which preserves its location in memory or store offsets rather than pointers.
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

    HRESULT FigureOutNextParseMode();
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

        ValueResult<std::wstring *> GetString()
        {
            if (std::wstring *pStr = std::get_if<std::wstring>(&_component))
            {
                return pStr;
            }

            return E_FAIL;
        }

        bool IsNonEmptyString()
        {
            ValueResult<std::wstring *> str = GetString();

            if (str.Succeeded())
            {
                return !str.Unwrap()->empty();
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
        HRESULT(CIniParser:: *pfnCallback)();
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
    CIniParser(std::wstring &text);

    // static CIniParser CreateIncludeIniParser(CIniParser *pParent, CSimpleFile *pChildFile);

    HRESULT Parse() override;
    ParseError GetParseError() override;

#if ENABLE_INIPARSER_TEST
    // Ignore the warning in Visual Studio. These are implemented in IniParserTest.cpp.
    HRESULT TestScanner();
    HRESULT TestParser();
#endif
};

}