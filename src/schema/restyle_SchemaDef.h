//#pragma once

#include "RestyleBase.h"
#include "SupportedOS.h"
#include "SchemaPriv.h" // TODO: restructure?

//---------------------------------------------------------------------------
#ifndef SCHEMA_STRINGS           // FIRST PASS of this header file
//---------------------------------------------------------------------------

#ifndef SCHEMADEF_H
#define SCHEMADEF_H

namespace Restyle
{   
    struct TMPROPINFO
    {
        LPCWSTR pszName;
        SHORT sEnumVal;
        BYTE bPrimVal;
#if DEBUG
        LPCWSTR szProgrammaticName;
        int iValidationVal;
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
#define BEGIN_TM_PART_STATES_FOR_OS(name, supportedOS) enum { // TODO: how to do this?

#define TM_PROP(val, prefix, name, prefCap, primval) prefix##_##name = val, 
#define TM_ENUM(val, prefix, name, prefCap)          prefix##_##name = val,
#define TM_PART(val, prefix, name, prefCap)          prefix##_##name = val, 
#define TM_STATE(val, prefix, name, prefCap)         prefix##_##name = val, 
#define TM_COMMENT(comment)

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
#undef BEGIN_TM_PART_STATES_FOR_OS
#undef TM_PROP
#undef TM_PART
#undef TM_STATE
#undef TM_ENUM
#undef TM_COMMENT
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
        { L#name L"STATES", TMT_ENUMDEF, TMT_ENUMDEF, ESupportedOS::All },
    #define BEGIN_TM_PART_STATES_FOR_OS(name, supportedOS)    \
        { L#name L"STATES", TMT_ENUMDEF, TMT_ENUMDEF, supportedOS },

    #define TM_PROP(val, prefix, name, prefCap, primval) \
        { L##prefCap, prefix##_##name, TMT_##primval },
    #define TM_PART(val, prefix, name, prefCap) \
        { L##prefCap, prefix##_##name, TMT_ENUMVAL },
    #define TM_STATE(val, prefix, name, prefCap) \
        { L##prefCap, prefix##_##name, TMT_ENUMVAL, },
    #define TM_ENUM(val, prefix, name, prefCap) \
        { L##prefCap, prefix##_##name, TMT_ENUMVAL  },
    #define TM_COMMENT(comment) \
        { L##comment, 0, TMT_COMMENT },
#else
    #define BEGIN_TM_CLASS_PARTS(name)    \
            { L#name L"PARTS", TMT_ENUMDEF, TMT_ENUMDEF, L#name L"PARTS", 0, ESupportedOS::All },
    #define BEGIN_TM_CLASS_PARTS_FOR_OS(name, supportedOS)    \
            { L#name L"PARTS", TMT_ENUMDEF, TMT_ENUMDEF, L#name L"PARTS", 0, supportedOS },
    #define BEGIN_TM_PART_STATES(name)    \
            { L#name L"STATES", TMT_ENUMDEF, TMT_ENUMDEF, L#name L"STATES", 0, ESupportedOS::All },
    #define BEGIN_TM_PART_STATES_FOR_OS(name, supportedOS)    \
            { L#name L"STATES", TMT_ENUMDEF, TMT_ENUMDEF, L#name L"STATES", 0, supportedOS },

    #define TM_PROP(val, prefix, name, prefCap, primval) \
            { L##prefCap, prefix##_##name, TMT_##primval, L#name, val },
    #define TM_PART(val, prefix, name, prefCap) \
            { L##prefCap, prefix##_##name, TMT_ENUMVAL, L#name, val },
    #define TM_STATE(val, prefix, name, prefCap) \
            { L##prefCap, prefix##_##name, TMT_ENUMVAL, L#name, val },
    #define TM_ENUM(val, prefix, name, prefCap) \
            { L##prefCap, prefix##_##name, TMT_ENUMVAL, L#name, val  },
    #define TM_COMMENT(comment) \
            { L##comment, 0, TMT_COMMENT, L##comment, 0 },
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
