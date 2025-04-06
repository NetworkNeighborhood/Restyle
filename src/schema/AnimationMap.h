#pragma once

#include "Restyle.h"

typedef enum TA_TRANSFORM_TYPE_PRIV
{
    TATT_ROTATE_2D = 256,
    TATT_SKEW_2D = 257,
    TATT_TRANSLATE_3D = 258,
    TATT_SCALE_3D = 259,
    TATT_ROTATE_3D = 260,
    TATT_SKEW_3D = 261
} TA_TRANSFORM_TYPE_PRIV;

typedef enum TA_PROPERTY_PRIV
{
    TAP_BACKGROUNDPARTID = 256,
    TAP_TUNINGLEVEL = 257,
    TAP_PERSPECTIVE = 258
} TA_PROPERTY_PRIV;

enum class AnimationSymbol : int
{
    TimingFunction,
    Animation,
};

struct AnimationSymbolMap
{
    LPCWSTR szName;
    int eSymbol;
};

extern AnimationSymbolMap g_rgAnimSymbols[];