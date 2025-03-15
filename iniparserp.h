#include "iniparser.h"
#include "util.h"

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

#define MAP_TM_TO_NATIVE_TYPE(TmType, NativeType) template <> struct TmToNativeTypeMap<TmType> { using Type = NativeType; }
#define SPECIALIZE_VALUE_FOR(TmType) template <> struct Value<TmType> : ValueBase<TmType>
#define SPECIALIZE_EASY_VALUE_FOR(TmType, Name)                                          \
    SPECIALIZE_VALUE_FOR(TmType)                                                         \
    {                                                                                    \
        TmToNativeTypeMap<TmType>::Type Name;                                            \
    };

template <int iPrimTypeVal = 0, typename NativeType = TmToNativeTypeMap<iPrimTypeVal>::Type>
struct ValueBase
{
    using StoredType = NativeType;

    int iPrimType = iPrimTypeVal;
    size_t cbSize = sizeof(NativeType);
};

// Template specialisation just for pointers to ValueBase/Value.
template <>
struct ValueBase<0, void>
{
    using StoredType = void;

    int iPrimType;
    size_t cbSize;

#pragma warning(suppress: 26495)
    consteval ValueBase()
    {
        static_assert(
            "Cannot construct a typeless Value. If you did mean to instantiate, add a type parameter."
            "Otherwise, double check your pointer syntax."
        );
    }
};

// Maps for Theme Manager primitive value types to C++ primitive types:
template <int> struct TmToNativeTypeMap; // <int> so we can do anything arbitrary
MAP_TM_TO_NATIVE_TYPE(Restyle::TMT_ENUM, int);
MAP_TM_TO_NATIVE_TYPE(Restyle::TMT_INT, int);
MAP_TM_TO_NATIVE_TYPE(Restyle::TMT_SIZE, int);
MAP_TM_TO_NATIVE_TYPE(Restyle::TMT_BOOL, BOOL);
MAP_TM_TO_NATIVE_TYPE(Restyle::TMT_RECT, RECT);
MAP_TM_TO_NATIVE_TYPE(Restyle::TMT_MARGINS, MARGINS);
MAP_TM_TO_NATIVE_TYPE(Restyle::TMT_STRING, WCHAR *);

// Private (or Restyle-specific) Theme Manager primitives:
struct IniAssociation;
MAP_TM_TO_NATIVE_TYPE(Restyle::TMT_ANIMATIONSET, IniAssociation *);

// Other special types:
MAP_TM_TO_NATIVE_TYPE(0, void);

template <int iPrimTypeVal = 0, typename NativeType = TmToNativeTypeMap<iPrimTypeVal>::Type>
struct Value : ValueBase<iPrimTypeVal, NativeType>
{
};

// Template specialisation just for pointers to Value.
template <>
struct Value<0, void> : ValueBase<0, void>
{
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
    Value<> *pVal;
};

SPECIALIZE_EASY_VALUE_FOR(Restyle::TMT_ENUM, iVal);
SPECIALIZE_EASY_VALUE_FOR(Restyle::TMT_INT, iVal);
SPECIALIZE_EASY_VALUE_FOR(Restyle::TMT_SIZE, iVal);
SPECIALIZE_EASY_VALUE_FOR(Restyle::TMT_BOOL, fVal);
SPECIALIZE_EASY_VALUE_FOR(Restyle::TMT_RECT, rcVal);
SPECIALIZE_EASY_VALUE_FOR(Restyle::TMT_MARGINS, marVal);

SPECIALIZE_VALUE_FOR(Restyle::TMT_STRING)
{
    WCHAR szVal[];
};

SPECIALIZE_VALUE_FOR(Restyle::TMT_ANIMATIONSET)
{
    UINT cAnimations;
    IniAssociation rgAssociations[];
};

using EnumValue = Value<Restyle::TMT_ENUM>;
using IntValue = Value<Restyle::TMT_INT>;
using SizeValue = Value<Restyle::TMT_SIZE>;
using BoolValue = Value<Restyle::TMT_BOOL>;
using RectValue = Value<Restyle::TMT_RECT>;
using MarginsValue = Value<Restyle::TMT_MARGINS>;
using StringValue = Value<Restyle::TMT_STRING>;
using AnimationSetValue = Value<Restyle::TMT_ANIMATIONSET>;

template <typename T>
static constexpr T static_max(T a, T b) {
    return a < b ? b : a;
}

template <typename T, typename... Ts>
static constexpr T static_max(T a, Ts... bs) {
    return static_max(a, static_max(bs...));
}

template <typename... Ts>
constexpr size_t max_sizeof() {
    return static_max(sizeof(Ts)...);
};

constexpr size_t kLargestValueTypeSize = max_sizeof<
    EnumValue,
    IntValue,
    SizeValue,
    BoolValue,
    RectValue,
    MarginsValue,
    StringValue,
    AnimationSetValue
>();

}