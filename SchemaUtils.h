#pragma once
#include "restyle.h"

namespace Restyle
{

enum class ESchemaSearchQuery
{
    SearchWholeSchema,
    Property,
    Enum,
    Parts,
    States,
};

/**
 * Type of a schema enum.
 */
enum class EEnumType
{
    /**
     * A standard enum type.
     */
    Enum,

    /**
     * A class parts enum.
     */
    Parts,

    /**
     * A class states or parts states enum.
     */
    States,

    /**
     * An invalid value.
     */
    Invalid,
};

bool IsValidPropInfoPtr(const TMPROPINFO *pPropInfo);
unsigned int GetPropInfoIndex(const TMPROPINFO *pPropInfo);

HRESULT InitializeSchemaUtils();


/**
 * Denotes that a primitive type value should generally be ignored for parsing purposes.
 */
bool IsMetadataType(BYTE bPrimVal);
LPCWSTR GetCommentForProperty(const TMPROPINFO *pPropInfo);

EEnumType GetEnumType(const TMPROPINFO *pEnumInfo);
EEnumType GetEnumTypeFromName(LPCWSTR szEnumName);

const TMPROPINFO *FindEnumDefForEnumVal(const TMPROPINFO *pEnumValInfo);

const TMPROPINFO *FindEnumValueInfo(SHORT sEnum, SHORT sEnumVal, ESupportedOS eSupportedOs = ESupportedOS::NotSet);
const TMPROPINFO *FindEnumValueInfo(LPCWSTR szEnumName, SHORT sEnumVal, ESupportedOS eSupportedOs = ESupportedOS::NotSet);
const TMPROPINFO *FindEnumValueInfo(const TMPROPINFO *pPropInfo, SHORT sEnumVal, ESupportedOS eSupportedOs = ESupportedOS::NotSet);

struct SearchSchemaParams
{
    size_t cbSize;
    ESchemaSearchQuery eSearchQuery;
    const TMPROPINFO *pPropInfoFrom;
    LPCWSTR szName;
    BYTE bType;
    SHORT sEnumVal;
    ESupportedOS eSupportedOs;
};

// Thunks:
const TMPROPINFO *SearchSchema(ESchemaSearchQuery eSearchQuery, BYTE bType, ESupportedOS eSupportedOs = ESupportedOS::NotSet);
const TMPROPINFO *SearchSchema(ESchemaSearchQuery eSearchQuery, const TMPROPINFO *pPropInfoFrom, BYTE bType, ESupportedOS eSupportedOs = ESupportedOS::NotSet);
const TMPROPINFO *SearchSchema(ESchemaSearchQuery eSearchQuery, LPCWSTR szName, ESupportedOS eSupportedOs = ESupportedOS::NotSet);
const TMPROPINFO *SearchSchema(ESchemaSearchQuery eSearchQuery, LPCWSTR szName, BYTE bType, ESupportedOS eSupportedOs = ESupportedOS::NotSet);
const TMPROPINFO *SearchSchema(ESchemaSearchQuery eSearchQuery, const TMPROPINFO *pPropInfoFrom, LPCWSTR szName, BYTE bType = 0, ESupportedOS eSupportedOs = ESupportedOS::NotSet);
const TMPROPINFO *SearchSchema(SearchSchemaParams pParams);

// Main implementation:
const TMPROPINFO *SearchSchema(SearchSchemaParams *pParams);


}