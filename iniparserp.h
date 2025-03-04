#include "iniparser.h"

namespace IniParser
{

enum class EParseMode
{
    Assoc,
    SectionHeader,
    Preprocessor,
    BreakParseLoop,
};

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

/**
 * A parse symbol.
 */
struct Symbol
{
    /**
     * The type of the symbol.
     *
     * There are two notable types of symbols: predefined (those which use a string name which
     * Restyle defines for identifying a type) and manual (those defining their own internal
     * value).
     *
     * The use of predefined symbols is preferred by Restyle. Manual symbols exist to give
     * theme authors flexibility.
     */
    ESymbolType eSymType;
    
    // Name of property.
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
    
    // Property information.
    // Sharing memory because these are mutually-exclusive members:
    union
    {
        /**
         * Offset in the schema table information about the symbol can be found, 
         * in the case of predefined symbols.
         */
        int iSchemaOffset;
        
        /**
         * The primitive type of the property, in the case of the ManualPropertyKey type.
         */
        int iPrimType;
    };
};

template <typename T = int>
struct ValueResult
{
    HRESULT hr = E_FAIL;
    T value {};
    
    inline FORCEINLINE bool Succeeded() const
    {
        return SUCCEEDED(hr);
    }
    
    inline FORCEINLINE bool Failed() const
    {
        return FAILED(hr);
    }
    
    inline FORCEINLINE HRESULT GetResult() const
    {
        return hr;
    }
    
    inline FORCEINLINE T &Unwrap()
    {
        return value;
    }

    inline FORCEINLINE ValueResult &ResultInto(HRESULT *p)
    {
        *p = hr;
        return *this;
    }
    
    inline FORCEINLINE operator HRESULT() const
    {
        return hr;
    }
    
    inline FORCEINLINE operator T() const
    {
        return value;
    }
    
    inline ValueResult(HRESULT hr)
        : hr(hr)
    {
        assert(FAILED(hr));
    }
    
    inline ValueResult(T value)
        : hr(S_OK)
        , value(value)
    {
    }
    
    inline ValueResult(HRESULT hr, T value)
        : hr(hr)
        , value(value)
    {
    }
};

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

template <typename NativeType = void, int iPrimTypeVal = 0>
struct ValueBase
{
    int iPrimType = iPrimTypeVal;
    size_t cbSize = sizeof(NativeType);
};

struct IniSection
{
    Symbol *pSymClass;
    Symbol *pSymPart;
    Symbol *pSymState;
    Symbol *pSymBaseClass;
};

struct IniAssociation
{
    IniSection section;
    Symbol *pKeySymbol;
    ValueBase<> *pVal;
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

struct RectValue : public ValueBase<RECT, Restyle::TMT_RECT>
{
    RECT rcVal;
};

struct MarginsValue : public ValueBase<MARGINS, Restyle::TMT_MARGINS>
{
    MARGINS marVal;
};

struct StringValue : public ValueBase<WCHAR, Restyle::TMT_STRING>
{
    WCHAR szVal[];
};

struct AnimationSetValue : public ValueBase<IniAssociation, Restyle::TMT_ANIMATIONSET>
{
    UINT cAnimations;
    IniAssociation rgAssociations[];
};

}