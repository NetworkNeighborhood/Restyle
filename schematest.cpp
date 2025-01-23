#include "SchemaPriv.h"
#include "restyle.h"
#if DEBUG

#include "util.h"
#include "schematest.h"

using namespace Restyle;

struct
{
	ESupportedOS eSupportedOsId;
	LPCWSTR szOsName;
} g_supportedOsMap[] = {
	{ ESupportedOS::WinVista,   L"Windows Vista" },
	{ ESupportedOS::Win7,       L"Windows 7" },
	{ ESupportedOS::Win8,       L"Windows 8" },
	{ ESupportedOS::Win81,      L"Windows 8.1" },
	{ ESupportedOS::Win10_1607, L"Windows 10 (1607)" },
	{ ESupportedOS::Win11_22H2, L"Windows 11 (22H2)" },
};

enum ValidationFlags : UINT
{
	VF_NONE = 0,
	VF_CAPITALIZATION_MISMATCH = 1,
	VF_BAD_ORDER = 2,
};

static LPCWSTR GetPrimValueName(BYTE bPrimVal)
{
	switch (bPrimVal)
	{
		case TMT_ENUM: return L"Enum";
		case TMT_ENUMDEF: return L"EnumDef";
		case TMT_ENUMVAL: return L"EnumVal";
        case TMT_STRING: return L"String";
        case TMT_INT: return L"Int";
        case TMT_BOOL: return L"Bool";
        case TMT_COLOR: return L"Color";
        case TMT_MARGINS: return L"Margins";
        case TMT_FILENAME: return L"FileName";
        case TMT_SIZE: return L"Size";
        case TMT_POSITION: return L"Position";
        case TMT_RECT: return L"Rect";
        case TMT_FONT: return L"Font";
        case TMT_INTLIST: return L"IntList";
	}

	return L"Unknown primitive type";
}

static bool StrEndsWithW(LPCWSTR a, LPCWSTR b)
{
	if (wcslen(a) >= wcslen(b))
	{
		if (wcscmp(a + wcslen(a) - wcslen(b), b) == 0)
		{
			return true;
		}
	}
	
	return false;
}

void TestSchema(ESchemaTestMode eMode, unsigned uEntryId)
{
	const TMSCHEMAINFO *pSchemaInfo = GetSchemaInfo();

	if (eMode == ESchemaTestMode::PrintHelpMessage)
	{
		wprintf(L"Schema information:\n");
		wprintf(L" - Size of TMSCHEMAINFO structure in bytes: %d\n", pSchemaInfo->dwSize);
		wprintf(L" - Number of entries: %d\n", pSchemaInfo->iPropCount);
		wprintf(L" - Pointer to data: %p\n", pSchemaInfo->pPropTable);
		wprintf(L"\n");
		wprintf(L"To see information about an individual item, run with /pschema <id>.\n");
	}
	else if (eMode == ESchemaTestMode::PrintEntryInfo && uEntryId <= pSchemaInfo->iPropCount)
	{
		const TMPROPINFO *pPropInfo = pSchemaInfo->pPropTable + uEntryId;

		wprintf(L"Information for entry #%d\n", uEntryId);
		wprintf(L" - Name of entry: %s\n", pPropInfo->pszName);
		wprintf(L" - Programmatic name base: %s\n", pPropInfo->szProgrammaticName ? pPropInfo->szProgrammaticName : L"(N/A)");
		wprintf(L" - Value: %d\n", pPropInfo->sEnumVal);
        wprintf(L" - Primitive type: %s\n", GetPrimValueName(pPropInfo->bPrimVal));
		wprintf(L" - Supported OS: "); // intentional no \n

		bool fPrintedSupportedOs = false;
		if (pPropInfo->supportedOS == ESupportedOS::All)
		{
			wprintf(L"All"); // intentional no \n
            fPrintedSupportedOs = true;
		}
		else if (pPropInfo->supportedOS == ESupportedOS::NotSet)
        {
            wprintf(L"(N/A)"); // intentional no \n
            fPrintedSupportedOs = true;
        }
		else
		{
			for (int i = 0; i < ARRAYSIZE(g_supportedOsMap); i++)
			{
				if (pPropInfo->supportedOS & g_supportedOsMap[i].eSupportedOsId)
				{
					if (fPrintedSupportedOs)
					{
						wprintf(L", "); // intentional no \n
					}
					wprintf(g_supportedOsMap[i].szOsName);
					fPrintedSupportedOs = true;
				}
			}
		}
        
        if (!fPrintedSupportedOs)
        {
            wprintf(L"Unknown supported OS value."); // intentional no \n
        }

		wprintf(L"\n");
	}
	else if (eMode == ESchemaTestMode::PrintEntryInfo)
	{
		fwprintf(stderr, L"Invalid entry name.\n");
	}
	else if (eMode == ESchemaTestMode::ValidateSymbols)
	{
		wprintf(L"Validating schema symbols...\n");
		UINT uValidationFlags = VF_NONE;
		int nErrors = 0;
		
		// Enum validation shit:
		int iLastVisitedEnumValue = 0;
		bool fSetLastVistedEnumVal = false;
		LPCWSTR szLastEnumName = nullptr; // used for printing

		for (unsigned i = 0; i < pSchemaInfo->iPropCount; i++)
		{			
			const TMPROPINFO *pPropInfo = pSchemaInfo->pPropTable + i;
			
			// We're visiting a new enum, so reset our enum parsing values:
			if (pPropInfo->bPrimVal == TMT_ENUMDEF)
			{
				iLastVisitedEnumValue = 0;
				fSetLastVistedEnumVal = false;
				szLastEnumName = pPropInfo->pszName;
			}
			
			// Make sure our preferred capitalization names don't differ from the programmatic identifiers:
			if (pPropInfo->szProgrammaticName && AsciiStrCmpI(pPropInfo->pszName, pPropInfo->szProgrammaticName) != 0)
			{
				wprintf(L"Mismatch for pair { \"%s\", \"%s\" }\n", pPropInfo->pszName, pPropInfo->szProgrammaticName);
				uValidationFlags |= VF_CAPITALIZATION_MISMATCH;
				nErrors++;
			}
			
			// Make sure enum values are sequential:
			if (pPropInfo->bPrimVal == TMT_ENUMVAL && fSetLastVistedEnumVal && pPropInfo->iValidationVal != iLastVisitedEnumValue + 1)
			{
				wprintf(L"Item #%d does not follow sequence in enum definition \"%s\"\n", pPropInfo->iValidationVal, szLastEnumName);
				uValidationFlags |= VF_BAD_ORDER;
				nErrors++;
			}
			
			// Make sure enum values align with the programmatic enum:
			if (pPropInfo->bPrimVal == TMT_ENUMVAL && pPropInfo->sEnumVal != pPropInfo->iValidationVal)
			{
				wprintf(L"Item #%d deviates from programmatic sequence (expected index %d) in enum definition \"%s\"\n", pPropInfo->iValidationVal, pPropInfo->sEnumVal, szLastEnumName);
				uValidationFlags |= VF_BAD_ORDER;
				nErrors++;
			}
			
			// Other enum value validation:
			if (pPropInfo->bPrimVal == TMT_ENUMVAL)
			{
				// No matter what, we want to increment the last visited enum value since
				// we're done validating that.
				iLastVisitedEnumValue = pPropInfo->iValidationVal;
				fSetLastVistedEnumVal = true;
				
				bool fIsPartsStates = false;
				if (
					szLastEnumName &&
					StrEndsWithW(szLastEnumName, L"PARTS") ||
					StrEndsWithW(szLastEnumName, L"STATES")
				)
				{
					fIsPartsStates = true;
				}
				
				if (fIsPartsStates && pPropInfo->iValidationVal == 0)
				{
					wprintf(L"Illegal use of reserved common properties index 0 in parts/states definition \"%s\"\n", szLastEnumName);
					uValidationFlags |= VF_BAD_ORDER;
					nErrors++;
				}
			}
		}

		if (uValidationFlags == VF_NONE)
		{
			wprintf(L"Validated table contents successfully without any errors detected.\n");
		}
		else
		{
			if (uValidationFlags & VF_CAPITALIZATION_MISMATCH)
			{
				wprintf(L"Capitalization mismatch error(s) present in table.\n");
			}
			if (uValidationFlags & VF_BAD_ORDER)
			{
				wprintf(L"Bad order error(s) present in table.\n");
			}
			
			wprintf(L"%d error(s) present in table.\n", nErrors);
		}
	}
}
#endif