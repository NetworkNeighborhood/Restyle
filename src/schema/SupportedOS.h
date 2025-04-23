#pragma once
#include "Restyle.h"

namespace Restyle
{
    enum class ESupportedOS : unsigned
    {
        NotSet     = 0,
        WinVista   = (1 << 0),
        Win7       = (1 << 1),
        Win8       = (1 << 2),
        Win81      = (1 << 3),
        Win10_1507 = (1 << 4),
        Win10_1607 = (1 << 5),
        Win11_22H2 = (1 << 6),

        All = WinVista | Win7 | Win8 | Win81 | Win10_1507 | Win10_1607 | Win11_22H2
    };

#define MAP_OS(os) { L ## #os, ESupportedOS::os },
    static const struct
    {
        LPCWSTR pszString;
        ESupportedOS eVal;
    } kSupportedOSStringMap[] = {
        MAP_OS(WinVista)
        MAP_OS(Win7)
        MAP_OS(Win8)
        MAP_OS(Win81)
        MAP_OS(Win10_1507)
        MAP_OS(Win10_1607)
        MAP_OS(Win11_22H2)
    };
#undef MAP_OS

    inline ESupportedOS operator|(ESupportedOS a, ESupportedOS b)
    {
        return (ESupportedOS)((int)a | (int)b);
    }

    inline bool operator&(ESupportedOS a, ESupportedOS b)
    {
        return (bool)((int)a & (int)b);
    }

    inline ESupportedOS StringToSupportedOS(LPCWSTR pszString)
    {
        if (!pszString)
            return ESupportedOS::NotSet;

        for (const auto &mapping : kSupportedOSStringMap)
        {
            if (0 == _wcsicmp(pszString, mapping.pszString))
                return mapping.eVal;
        }

        return ESupportedOS::NotSet;
    }

    inline LPCWSTR SupportedOSToString(ESupportedOS eSupportedOS)
    {
        for (const auto &mapping : kSupportedOSStringMap)
        {
            if (mapping.eVal == eSupportedOS)
                return mapping.pszString;
        }
        return nullptr;
    }
} // namespace Restyle