#pragma once

#include "restyle.h"

enum TA_TRANSFORM_TYPE_EXTENSIONS
{
    TATT_TRANSLATE_3D = 258,
    TATT_SCALE_3D = 259,
    TATT_ROTATE_3D = 260,
};

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