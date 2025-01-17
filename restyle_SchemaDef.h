//#pragma once

#include "restyle.h"

#include "schemapriv.h" // TODO: restructure?

//---------------------------------------------------------------------------
#ifndef SCHEMA_STRINGS           // FIRST PASS of this header file
//---------------------------------------------------------------------------

#ifndef SCHEMADEF_H
#define SCHEMADEF_H

namespace Restyle
{
    enum class ESupportedOS : unsigned
    {
        NotSet = 0,
        WinVista = 1,
        Win7 = 2,
        Win8 = 4,
        Win81 = 8,
        Win10Th1 = 16,
        Win10Th2 = 32,
        Win10_1607 = 64,
        Win10_1703 = 128,
        Win10_1709 = 256,
        Win10_1803 = 512,
        Win10_1809 = 1024,
        Win10_1903 = 2048,
        Win10_1909 = 4096,
        Win10_2004 = 8192,
        Win11_21H2 = 16384,
        Win11_22H2 = 32768,
        
        All = WinVista | Win7 | Win8 | Win81 | Win10Th1 | Win10Th2 | Win10_1607 | Win10_1703
            | Win10_1709 | Win10_1803 | Win10_1809 | Win10_1903 | Win10_1909 | Win10_2004
            | Win11_21H2 | Win11_22H2
    };
    
    inline ESupportedOS operator|(ESupportedOS a, ESupportedOS b)
    {
        return (ESupportedOS)((int)a | (int)b);
    }

    inline bool operator&(ESupportedOS a, ESupportedOS b)
    {
        return (bool)((int)a & (int)b);
    }
    
    struct TMPROPINFO
    {
        LPCWSTR pszName;
        SHORT sEnumVal;
        BYTE bPrimVal;
#ifdef DEBUG
        LPCWSTR szPreferredCapitalization;
#endif
        ESupportedOS supportedOS;
    };
    
    struct TMSCHEMAINFO
    {
        DWORD dwSize;
        int iPropCount;
        const struct TMPROPINFO *pPropTable;
    };
}

#define BEGIN_TM_SCHEMA(name)               
#define BEGIN_TM_PROPS()                    enum PropValues { DummyProp = 49,
#define BEGIN_TM_ENUM(name, prefCap)                 enum name {
#define BEGIN_TM_CLASS_PARTS(name)          enum name##PARTS { name##PartFiller0,
#define BEGIN_TM_CLASS_PARTS_FOR_OS(name, supportedOS) enum { // TODO: how to do this?
#define BEGIN_TM_PART_STATES(name)          enum name##STATES { name##StateFiller0,

#define TM_PROP(val, prefix, name, prefCap, primval) prefix##_##name = val, 
#define TM_ENUM(val, prefix, name, prefCap)          prefix##_##name = val,
#define TM_PART(val, prefix, name, prefCap)          prefix##_##name = val, 
#define TM_STATE(val, prefix, name, prefCap)         prefix##_##name = val, 

#define END_TM_CLASS_PARTS()                };
#define END_TM_PART_STATES()                };
#define END_TM_PROPS()                      };
#define END_TM_ENUM()                       };
#define END_TM_SCHEMA(name)

#endif // SCHEMADEF_H

//---------------------------------------------------------------------------
#else                   // SECOND PASS of this header file
//---------------------------------------------------------------------------

#undef BEGIN_TM_SCHEMA
#undef BEGIN_TM_PROPS
#undef BEGIN_TM_ENUM
#undef BEGIN_TM_CLASS_PARTS
#undef BEGIN_TM_CLASS_PARTS_FOR_OS
#undef BEGIN_TM_PART_STATES
#undef TM_PROP
#undef TM_PART
#undef TM_STATE
#undef TM_ENUM
#undef END_TM_CLASS_PARTS
#undef END_TM_PART_STATES
#undef END_TM_PROPS
#undef END_TM_ENUM
#undef END_TM_SCHEMA

//---------------------------------------------------------------------------

#define BEGIN_TM_SCHEMA(name)  static const TMPROPINFO name[] = {
#define BEGIN_TM_PROPS()

#if !DEBUG
    #define BEGIN_TM_ENUM(name, prefCap) { L##prefCap, TMT_ENUMDEF, TMT_ENUMDEF },
#else
    #define BEGIN_TM_ENUM(name, prefCap) { L#name, TMT_ENUMDEF, TMT_ENUMDEF, L##prefCap },
#endif

#if !DEBUG
    #define BEGIN_TM_CLASS_PARTS(name)    \
        { L#name L"PARTS", TMT_ENUMDEF, TMT_ENUMDEF, ESupportedOS::All },
    #define BEGIN_TM_CLASS_PARTS_FOR_OS(name, supportedOS)    \
        { L#name L"PARTS", TMT_ENUMDEF, TMT_ENUMDEF, supportedOS },
    #define BEGIN_TM_PART_STATES(name)    \
        { L#name L"STATES", TMT_ENUMDEF, TMT_ENUMDEF },

    #define TM_PROP(val, prefix, name, prefCap, primval) \
        { L##prefCap, prefix##_##name, TMT_##primval },
    #define TM_PART(val, prefix, name, prefCap) \
        { L##prefCap, prefix##_##name, TMT_ENUMVAL },
    #define TM_STATE(val, prefix, name, prefCap) \
        { L##prefCap, prefix##_##name, TMT_ENUMVAL, },
    #define TM_ENUM(val, prefix, name, prefCap) \
        { L##prefCap, prefix##_##name, TMT_ENUMDEF  },
#else
    #define BEGIN_TM_CLASS_PARTS(name)    \
            { L#name L"PARTS", TMT_ENUMDEF, TMT_ENUMDEF, nullptr, ESupportedOS::All },
    #define BEGIN_TM_CLASS_PARTS_FOR_OS(name, supportedOS)    \
            { L#name L"PARTS", TMT_ENUMDEF, TMT_ENUMDEF, nullptr, supportedOS },
    #define BEGIN_TM_PART_STATES(name)    \
            { L#name L"STATES", TMT_ENUMDEF, TMT_ENUMDEF },

    #define TM_PROP(val, prefix, name, prefCap, primval) \
            { L#name, prefix##_##name, TMT_##primval, L##prefCap },
    #define TM_PART(val, prefix, name, prefCap) \
            { L#name, prefix##_##name, TMT_ENUMVAL, L##prefCap },
    #define TM_STATE(val, prefix, name, prefCap) \
            { L#name, prefix##_##name, TMT_ENUMVAL, L##prefCap },
    #define TM_ENUM(val, prefix, name, prefCap) \
            { L#name, prefix##_##name, TMT_ENUMDEF, L##prefCap  },
#endif



#define END_TM_CLASS_PARTS() 
#define END_TM_PART_STATES() 
#define END_TM_PROPS() 
#define END_TM_ENUM()
#define END_TM_SCHEMA(name)                };         \
    static const TMSCHEMAINFO *GetSchemaInfo()        \
    {                                                 \
        static TMSCHEMAINFO si = {sizeof(si)};        \
        si.iPropCount = sizeof(name)/sizeof(name[0]); \
        si.pPropTable = name;                         \
                                                      \
        return &si;                                   \
    }
    
//---------------------------------------------------------------------------
#endif // SCHEMA_STRINGS
//---------------------------------------------------------------------------