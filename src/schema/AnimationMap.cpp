#include "AnimationMap.h"
#include <uxtheme.h>

// Defines for the animation map:
AnimationSymbolMap g_rgAnimSymbols[] = {
    { L"CubicBezier", TTFT_CUBIC_BEZIER },
    { L"Undefined", TTFT_UNDEFINED },
};

AnimationSymbolMap g_rgAnimProperties[] = {
    { L"Flags", TAP_FLAGS },
    { L"TransformCount", TAP_TRANSFORMCOUNT },
    { L"StaggerDelay", TAP_STAGGERDELAY },
    { L"StaggerDelayCap", TAP_STAGGERDELAYCAP },
    { L"StaggerDelayFactor", TAP_STAGGERDELAYFACTOR },
    { L"ZOrder", TAP_ZORDER },
};

AnimationSymbolMap g_rgAnimTransformTypes[] = {
    { L"Translate2D", TATT_TRANSLATE_2D },
    { L"Scale2D", TATT_SCALE_2D },
    { L"Opacity", TATT_OPACITY },
    { L"Clip", TATT_CLIP },
    { L"Rotate2D", TATT_ROTATE_2D },
    { L"Skew2D", TATT_SKEW_2D },
    { L"Translate3D", TATT_TRANSLATE_3D },
    { L"Scale3D", TATT_SCALE_3D },
    { L"Rotate3D", TATT_ROTATE_3D },
    { L"Skew3D", TATT_SKEW_3D },
};

AnimationSymbolMap g_rgTransformFlag[] = {
    { L"None", TATF_NONE },
    { L"TargetValues_User", TATF_TARGETVALUES_USER },
    { L"HasInitialValues", TATF_HASINITIALVALUES },
    { L"HasOriginValues", TATF_HASORIGINVALUES },
};