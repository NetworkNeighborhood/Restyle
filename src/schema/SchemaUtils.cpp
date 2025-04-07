#include "SchemaUtils.h"
#include "Util.h"

namespace Restyle
{

struct Range
{
    int iBegin;
    int iEnd;
};

// Primitive values are stored within a BYTE, which can only range up to 255. The lower 200 values are
// used to store private types, such as those used for internal parsing logic.
Range g_rangePrimitiveTypes = { 200, 255 };

const TMSCHEMAINFO *g_pSchemaInfo = nullptr;

static inline bool InRange(int iNum, Range range)
{
    return iNum >= range.iBegin && iNum <= range.iEnd;
}

bool IsValidPropInfoPtr(const TMPROPINFO *pPropInfo)
{
    return pPropInfo >= g_pSchemaInfo->pPropTable && pPropInfo < g_pSchemaInfo->pPropTable + g_pSchemaInfo->iPropCount;
}

unsigned int GetPropInfoIndex(const TMPROPINFO *pPropInfo)
{
    assert(IsValidPropInfoPtr(pPropInfo));
    return (size_t)(pPropInfo - g_pSchemaInfo->pPropTable);
}

static void UpdateRuntimeRangeIfInStaticRange(const TMPROPINFO *pPropInfo, int i, Range *pStaticRange, Range *pRuntimeRange)
{
    if (InRange(pPropInfo->sEnumVal, *pStaticRange))
    {
        if (pRuntimeRange->iBegin == -1)
        {
            pRuntimeRange->iBegin = i;
        }
        else
        {
            pRuntimeRange->iEnd = i;
        }
    }
}

bool IsMetadataType(BYTE bPrimVal)
{
    return bPrimVal == TMT_COMMENT;
}

EEnumType GetEnumTypeFromName(LPCWSTR szEnumName)
{
    if (StrEndsWithW(szEnumName, L"PARTS"))
    {
        return EEnumType::Parts;
    }
    else if (StrEndsWithW(szEnumName, L"STATES"))
    {
        return EEnumType::States;
    }
    
    return EEnumType::Enum;
}

EEnumType GetEnumType(const TMPROPINFO *pEnumInfo)
{
    // Verify that this is a valid enum definition:
    if (pEnumInfo->bPrimVal != TMT_ENUMDEF)
    {
        return EEnumType::Invalid;
    }

    return GetEnumTypeFromName(pEnumInfo->pszName);
}

LPCWSTR GetCommentForProperty(const TMPROPINFO *pPropInfo)
{
    UINT iIndex = GetPropInfoIndex(pPropInfo);

    if (iIndex > 0)
    {
        if (g_pSchemaInfo->pPropTable[iIndex - 1].bPrimVal == TMT_COMMENT)
        {
            return g_pSchemaInfo->pPropTable[iIndex - 1].pszName;
        }
    }

    return nullptr;
}

const TMPROPINFO *FindEnumDefForEnumVal(const TMPROPINFO *pEnumValInfo)
{
    if (!pEnumValInfo)
    {
        return nullptr;
    }

    if (pEnumValInfo->bPrimVal != TMT_ENUMVAL)
    {
        return nullptr;
    }

    for (TMPROPINFO *pPropInfo = (TMPROPINFO *)pEnumValInfo; pPropInfo >= &g_pSchemaInfo->pPropTable[0]; pPropInfo--)
    {
        if (pPropInfo->bPrimVal == TMT_ENUMDEF)
        {
            return pPropInfo;
        }
        else if (pPropInfo->bPrimVal == TMT_ENUMVAL || IsMetadataType(pPropInfo->bPrimVal))
        {
            continue;
        }
        else
        {
            assert(0, "Schema property table is invalidly formed.");
            return nullptr;
        }
    }

    return nullptr;
}

const TMPROPINFO *FindEnumValueInfo(const TMPROPINFO *pPropInfo, SHORT sEnumVal, ESupportedOS eSupportedOs)
{
    for (TMPROPINFO *pPropCur = (TMPROPINFO *)pPropInfo + 1; IsValidPropInfoPtr(pPropCur); pPropCur++)
    {
        if (pPropCur->bPrimVal != TMT_ENUMVAL)
            // We've almost certainly moved on to a different enum/other definition
            // if this is the case.
            break;

        if (pPropCur->sEnumVal == sEnumVal)
            return pPropCur;
    }

    return nullptr;
}

const TMPROPINFO *FindEnumValueInfo(LPCWSTR szEnumName, SHORT sEnumVal, ESupportedOS eSupportedOs)
{
    // Find the property info of the requested enum name.
    const TMPROPINFO *pEnumDef = SearchSchema(ESchemaSearchQuery::Enum, szEnumName, TMT_ENUMDEF, eSupportedOs);

    if (pEnumDef)
    {
        assert(AsciiStrCmpI(szEnumName, pEnumDef->pszName) == 0);
        return FindEnumValueInfo(pEnumDef, sEnumVal, eSupportedOs);
    }

    return nullptr;
}

const TMPROPINFO *FindEnumValueInfo(SHORT sEnum, SHORT sEnumVal, ESupportedOS eSupportedOs)
{
    // Find the enum property index from the property table.
    SearchSchemaParams params = { 0 };
    params.cbSize = sizeof(params);
    params.eSearchQuery = ESchemaSearchQuery::Property;
    params.bType = TMT_ENUM;
    params.sEnumVal = sEnum;
    params.eSupportedOs = eSupportedOs;

    const TMPROPINFO *pEnumDef = SearchSchema(&params);

    if (pEnumDef)
    {
#if DEBUG // Validate the response name is the same as the static name for this property. TODO: Use this to avoid work?
        std::unique_ptr<WCHAR[]> pszPropName = std::make_unique<WCHAR[]>(512);
        GetPropName(sEnum, TMT_ENUM, pszPropName);
        assert(AsciiStrCmpI(pszPropName.get(), pEnumDef->pszName) == 0);
#endif

        return FindEnumValueInfo(pEnumDef->pszName, sEnumVal, eSupportedOs);
    }

    return nullptr;
}

const TMPROPINFO *SearchSchema(ESchemaSearchQuery eSearchQuery, BYTE bType, ESupportedOS eSupportedOs)
{
    return SearchSchema(eSearchQuery, nullptr, nullptr, bType, eSupportedOs);
}

const TMPROPINFO *SearchSchema(ESchemaSearchQuery eSearchQuery, const TMPROPINFO *pPropInfoFrom, BYTE bType, ESupportedOS eSupportedOs)
{
    return SearchSchema(eSearchQuery, pPropInfoFrom, nullptr, bType, eSupportedOs);
}

const TMPROPINFO *SearchSchema(ESchemaSearchQuery eSearchQuery, LPCWSTR szName, ESupportedOS eSupportedOs)
{
    return SearchSchema(eSearchQuery, nullptr, szName, 0, eSupportedOs);
}

const TMPROPINFO *SearchSchema(ESchemaSearchQuery eSearchQuery, LPCWSTR szName, BYTE bType, ESupportedOS eSupportedOs)
{
    return SearchSchema(eSearchQuery, nullptr, szName, bType, eSupportedOs);
}

const TMPROPINFO *SearchSchema(ESchemaSearchQuery eSearchQuery, const TMPROPINFO *pPropInfoFrom, LPCWSTR szName, BYTE bType, ESupportedOS eSupportedOs)
{
    SearchSchemaParams params = { 0 };
    params.cbSize = sizeof(params);
    params.eSearchQuery = eSearchQuery;
    params.pPropInfoFrom = pPropInfoFrom;
    params.szName = szName;
    params.bType = bType;
    params.eSupportedOs = eSupportedOs;

    return SearchSchema(&params);
}

const TMPROPINFO *SearchSchema(SearchSchemaParams pParams) { return SearchSchema(&pParams); }

const TMPROPINFO *SearchSchema(SearchSchemaParams *pParams)
{
    using Q = ESchemaSearchQuery;

    if (!pParams || pParams->cbSize < sizeof(SearchSchemaParams))
    {
#if DEBUG
        if (pParams && pParams->cbSize == 0)
            assert(0, "You forgot to set cbSize.");
        else if (pParams)
            assert(0, "You used the wrong size somewhere.");
        else
            assert(0, "You passed a null pointer.");
#endif
        return nullptr;
    }

    const ESchemaSearchQuery eSearchQuery = pParams->eSearchQuery;
    const ESupportedOS eSupportedOs = pParams->eSupportedOs;

    UINT iIndex = pParams->pPropInfoFrom ? GetPropInfoIndex(pParams->pPropInfoFrom) + 1 : 0;
    
    if (pParams->pPropInfoFrom)
        assert(&g_pSchemaInfo->pPropTable[iIndex - 1] == pParams->pPropInfoFrom);

    for (UINT i = iIndex; i < g_pSchemaInfo->iPropCount; i++)
    {
        const TMPROPINFO *pPropInfo = &g_pSchemaInfo->pPropTable[i];

        bool fConditionSucceeded = true;

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Conditions like this are common, so I made this macro.
#define SEARCH_SCHEMA_CONDITION(conditionA, conditionB) \
    if ((conditionA) && (conditionB))                   \
        continue;

#define SEARCH_SCHEMA_BREAK_CONDITION(conditionA, conditionB) \
    if ((conditionA) && (conditionB))                        \
        break;

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        // We want to ignore all metadata types.
        if (IsMetadataType(pPropInfo->bPrimVal))
        {
            continue;
        }

        SEARCH_SCHEMA_BREAK_CONDITION(eSearchQuery == Q::NextEnum || eSearchQuery == Q::NextPart || eSearchQuery == Q::NextState,
            pPropInfo->bPrimVal != TMT_ENUMDEF &&
            pPropInfo->bPrimVal != TMT_ENUMVAL
        );

        // XXX (isabella): Queries will be used for further optimisation in the future.
        SEARCH_SCHEMA_CONDITION(eSearchQuery == Q::Enum || eSearchQuery == Q::Parts || eSearchQuery == Q::States, 
            pPropInfo->bPrimVal != TMT_ENUMDEF &&
            pPropInfo->bPrimVal != TMT_ENUMVAL
        );
        SEARCH_SCHEMA_CONDITION(eSearchQuery == Q::Property,
            !InRange(pPropInfo->bPrimVal, g_rangePrimitiveTypes)
        );
        SEARCH_SCHEMA_CONDITION(eSearchQuery == Q::PrimitiveProperty,
            !InRange(pPropInfo->sEnumVal, g_rangePrimitiveTypes)
        );

        SEARCH_SCHEMA_CONDITION(pParams->sEnumVal, pPropInfo->sEnumVal != pParams->sEnumVal);

        SEARCH_SCHEMA_CONDITION(pParams->bType, pPropInfo->bPrimVal != pParams->bType);

        SEARCH_SCHEMA_CONDITION(eSupportedOs != ESupportedOS::NotSet, 
            pPropInfo->supportedOS & eSupportedOs &&
            pPropInfo->supportedOS != ESupportedOS::NotSet
        );

        SEARCH_SCHEMA_CONDITION(pParams->szName, AsciiStrCmpI(pPropInfo->pszName, pParams->szName) != 0);

        if (fConditionSucceeded)
        {
            return pPropInfo;
        }
    }

    return nullptr;
}

HRESULT InitializeSchemaUtils()
{
    HRESULT hr = S_OK;

    g_pSchemaInfo = GetSchemaInfo();

    return hr;
}

}