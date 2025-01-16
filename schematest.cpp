#include "restyle.h"
#ifdef DEBUG

#include "schematest.h"
#include "restyle_TmSchema.h"
#define SCHEMA_STRINGS
#include "restyle_TmSchema.h"

using namespace Restyle;

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

// TODO: Move. The parser needs this too.
#define __ascii_towlower(c)  ( (((c) >= L'A') && ((c) <= L'Z')) ? ((c) - L'A' + L'a') : (c) )
int AsciiStrCmpI(const WCHAR *dst, const WCHAR *src)
{
	WCHAR a, b;

	if (!dst)
	{
		return src ? -1 : 0;
	}
	else if (!src)
	{
		return 1;
	}

	do
	{
		a = __ascii_towlower(*dst);
		b = __ascii_towlower(*src);
		dst++;
		src++;
	}
	while (a && (a == b));

	return (int)(a - b);
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
		wprintf(L" - Value: %d\n", pPropInfo->sEnumVal);
        wprintf(L" - Primitive type: %s\n", GetPrimValueName(pPropInfo->bPrimVal));
		wprintf(L" - Supported OS: ");

		bool fPrintedSupportedOs = false;
		if (pPropInfo->supportedOS == ESupportedOS::All)
		{
			wprintf(L"All");
            fPrintedSupportedOs = true;
		}
        else if (pPropInfo->supportedOS == ESupportedOS::NotSet)
        {
            wprintf(L"(N/A)");
            fPrintedSupportedOs = true;
        }
		else
		{
			if ((int)pPropInfo->supportedOS & (int)ESupportedOS::Win11)
			{
				if (fPrintedSupportedOs)
				{
					wprintf(L", ");
				}
				wprintf(L"Windows 11");
				fPrintedSupportedOs = true;
			}
			if ((int)pPropInfo->supportedOS & (int)ESupportedOS::Win10)
			{
				if (fPrintedSupportedOs)
				{
					wprintf(L", ");
				}
				wprintf(L"Windows 10");
				fPrintedSupportedOs = true;
			}
			if ((int)pPropInfo->supportedOS & (int)ESupportedOS::Win81)
			{
				if (fPrintedSupportedOs)
				{
					wprintf(L", ");
				}
				wprintf(L"Windows 8.1");
				fPrintedSupportedOs = true;
			}
			if ((int)pPropInfo->supportedOS & (int)ESupportedOS::Win8)
			{
				if (fPrintedSupportedOs)
				{
					wprintf(L", ");
				}
				wprintf(L"Windows 8");
				fPrintedSupportedOs = true;
			}
			if ((int)pPropInfo->supportedOS & (int)ESupportedOS::Win7)
			{
				if (fPrintedSupportedOs)
				{
					wprintf(L", ");
				}
				wprintf(L"Windows 7");
				fPrintedSupportedOs = true;
			}
			if ((int)pPropInfo->supportedOS & (int)ESupportedOS::WinVista)
			{
				if (fPrintedSupportedOs)
				{
					wprintf(L", ");
				}
				wprintf(L"Windows Vista");
				fPrintedSupportedOs = true;
			}
		}
        
        if (!fPrintedSupportedOs)
        {
            wprintf(L"Unknown supported OS value.");
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
		bool fAnyMismatch = false;

		for (unsigned i = 0; i < pSchemaInfo->iPropCount; i++)
		{
			const TMPROPINFO *pPropInfo = pSchemaInfo->pPropTable + i;
			if (pPropInfo->szPreferredCapitalization && AsciiStrCmpI(pPropInfo->pszName, pPropInfo->szPreferredCapitalization) != 0)
			{
				wprintf(L"Mismatch for pair { \"%s\", \"%s\" }\n", pPropInfo->pszName, pPropInfo->szPreferredCapitalization);
				fAnyMismatch = true;
			}
		}

		if (!fAnyMismatch)
		{
			wprintf(L"Validated table contents successfully without any error detected.\n");
		}
		else
		{
			wprintf(L"Errors present in table.\n");
		}
	}
}
#endif