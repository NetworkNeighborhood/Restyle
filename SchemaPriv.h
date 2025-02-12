#pragma once

#include "restylebase.h"

namespace Restyle
{

enum TmSchemaPriv
{
    TMT_DIBDATA = 2, // Exposed in vmsys32.h, but not tmschema.h
    TMT_GLYPHDIBDATA = 8, // Exposed in vmsys32.h, but not tmschema.h
    TMT_ENUMDEF, // Referenced in schemadef.h, but not public.
    TMT_ENUMVAL, // Referenced in schemadef.h, but not public.
    TMT_ANIMATIONSET, // Unique; used for special parsing purposes by restyle.
};

typedef BYTE PRIMVAL;

}