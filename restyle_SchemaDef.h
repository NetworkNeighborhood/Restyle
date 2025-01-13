#include "restyle.h"

//---------------------------------------------------------------------------
#ifndef SCHEMA_STRINGS           // FIRST PASS of this header file
//---------------------------------------------------------------------------

#ifndef SCHEMADEF_H

namespace Restyle
{
    enum class ESupportedOS : unsigned
    {
        NotSet = 0,
        WinVista = 1,
        Win7 = 2,
        Win8 = 4,
        Win81 = 8,
        Win10 = 16,
        Win11 = 32,
        
        All = WinVista | Win7 | Win8 | Win81 | Win10 | Win11
    };
    
    struct TMPROPINFO
    {
        LPCWSTR pszName;
        SHORT sEnumVal;
        BYTE bPrimVal;
        LPCWSTR szCapitalizedName = nullptr;
        ESupportedOS supportedOS = ESupportedOS::NotSet;
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
#define BEGIN_TM_PART_STATES(name)          enum name##STATES { name##StateFiller0,

#define TM_PROP(val, prefix, name, primval) prefix##_##name = val, 
#define TM_ENUM(val, prefix, name, prefCap)          prefix##_##name = val,
#define TM_PART(val, prefix, name)          prefix##_##name = val, 
#define TM_STATE(val, prefix, name)         prefix##_##name = val, 

#define END_TM_CLASS_PARTS()                };
#define END_TM_PART_STATES()                };
#define END_TM_PROPS()                      };
#define END_TM_ENUM()                       };
#define END_TM_SCHEMA(name)

#define SYMBOL_H
#endif // SYMBOL_H

//---------------------------------------------------------------------------
#else                   // SECOND PASS of this header file
//---------------------------------------------------------------------------

#undef BEGIN_TM_SCHEMA
#undef BEGIN_TM_PROPS
#undef BEGIN_TM_ENUM
#undef BEGIN_TM_CLASS_PARTS
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
#define BEGIN_TM_ENUM(name, prefCap) { L#name, TMT_ENUMDEF, TMT_ENUMDEF, L#prefCap },
#define BEGIN_TM_CLASS_PARTS(name)    \
    { L#name L"PARTS", TMT_ENUMDEF, TMT_ENUMDEF, nullptr, ESupportedOS::All },
#define BEGIN_TM_CLASS_PARTS_FOR_OS(name, supportedOS)    \
    { L#name L"PARTS", TMT_ENUMDEF, TMT_ENUMDEF, nullptr, ESupportedOS::##supportedOS },
#define BEGIN_TM_PART_STATES(name)    \
    { L#name L"STATES", TMT_ENUMDEF, TMT_ENUMDEF, nullptr },
    
    
#define TM_PROP(val, prefix, name, prefCap, primval) \
    { L#name, prefix##_##name, TMT_##primval, prefCap },
#define TM_PART(val, prefix, name, prefCap) \
    { L#name, prefix##_##name, TMT_ENUMVAL, prefCap },
#define TM_STATE(val, prefix, name, prefCap) \
    { L#name, prefix##_##name, TMT_ENUMVAL, prefCap },
#define TM_ENUM(val, prefix, name, prefCap) \
    { L#name, prefix##_##name, TMT_ENUMDEF, prefCap  },



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
#endif // SCHEMADEF_H
//---------------------------------------------------------------------------