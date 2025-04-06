#pragma once
#include "restyle.h"

#include "util.h"
#include "Symbol.h"

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
MAP_TM_TO_NATIVE_TYPE(Restyle::TMT_FLOAT, float);
MAP_TM_TO_NATIVE_TYPE(Restyle::TMT_SIZE, int);
MAP_TM_TO_NATIVE_TYPE(Restyle::TMT_BOOL, BOOL);
MAP_TM_TO_NATIVE_TYPE(Restyle::TMT_RECT, RECT);
MAP_TM_TO_NATIVE_TYPE(Restyle::TMT_MARGINS, MARGINS);
MAP_TM_TO_NATIVE_TYPE(Restyle::TMT_POSITION, POINT);
MAP_TM_TO_NATIVE_TYPE(Restyle::TMT_COLOR, COLORREF);
MAP_TM_TO_NATIVE_TYPE(Restyle::TMT_INTLIST, int *);
MAP_TM_TO_NATIVE_TYPE(Restyle::TMT_FLOATLIST, float *);
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
SPECIALIZE_EASY_VALUE_FOR(Restyle::TMT_FLOAT, flVal);
SPECIALIZE_EASY_VALUE_FOR(Restyle::TMT_SIZE, iVal);
SPECIALIZE_EASY_VALUE_FOR(Restyle::TMT_BOOL, fVal);
SPECIALIZE_EASY_VALUE_FOR(Restyle::TMT_RECT, rcVal);
SPECIALIZE_EASY_VALUE_FOR(Restyle::TMT_POSITION, ptVal);
SPECIALIZE_EASY_VALUE_FOR(Restyle::TMT_COLOR, crVal);
SPECIALIZE_EASY_VALUE_FOR(Restyle::TMT_MARGINS, marVal);

SPECIALIZE_VALUE_FOR(Restyle::TMT_INTLIST)
{
    UINT c;
    int rgiVal[];
};

SPECIALIZE_VALUE_FOR(Restyle::TMT_FLOATLIST)
{
    UINT c;
    float rgflVal[];
};

SPECIALIZE_VALUE_FOR(Restyle::TMT_STRING)
{
    UINT cch;
    WCHAR szVal[];
};

SPECIALIZE_VALUE_FOR(Restyle::TMT_ANIMATIONSET)
{
    UINT cAnimations;
    IniAssociation rgAssociations[];
};

using EnumValue = Value<Restyle::TMT_ENUM>;
using IntValue = Value<Restyle::TMT_INT>;
using FloatValue = Value<Restyle::TMT_FLOAT>;
using SizeValue = Value<Restyle::TMT_SIZE>;
using BoolValue = Value<Restyle::TMT_BOOL>;
using RectValue = Value<Restyle::TMT_RECT>;
using MarginsValue = Value<Restyle::TMT_MARGINS>;
using PositionValue = Value<Restyle::TMT_POSITION>;
using ColorValue = Value<Restyle::TMT_COLOR>;
using StringValue = Value<Restyle::TMT_STRING>;
using IntListValue = Value<Restyle::TMT_INTLIST>;
using FloatListValue = Value<Restyle::TMT_FLOATLIST>;
using AnimationSetValue = Value<Restyle::TMT_ANIMATIONSET>;

constexpr size_t kLargestValueTypeSize = max_sizeof<
    EnumValue,
    IntValue,
    SizeValue,
    BoolValue,
    RectValue,
    PositionValue,
    MarginsValue,
    ColorValue,
    StringValue,
    IntListValue,
    FloatListValue,
    AnimationSetValue
>();