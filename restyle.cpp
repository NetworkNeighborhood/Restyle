#include "restyle.h"
#include "schematest.h"
#include "binparser.h"
#include "util.h"

WCHAR g_szThemeFilePath[MAX_PATH] = { 0 };
HMODULE g_hThemeModule = NULL;

void PrintUsage(void)
{
	wprintf(
		L"Usage: restyle <action> [file/folder] [options]\n"
		L"\n"
		L"Actions:\n"
		L"    /?, /help: Display this help.\n"
		L"    /c: Compile a theme folder into a .MSSTYLES theme.\n"
		L"    /d: Decomiple a .MSSTYLES file into a theme folder.\n"
		L"    /pcmap: Print the CMAP (class map) of a compiled .MSSTYLES theme.\n"
		L"    /pbcmap: Print the BCMAP (base class map) of a compiled .MSSTYLES theme.\n"
		L"    /pvmap: Print the VMAP (variant map) of a compiled .MSSTYLES theme.\n"
		L"    /precord: Print a record resource (VARIANT, AMAP, RMAP) from a compiled .MSSTYLES theme.\n"
		L"        Usage: restyle /precord <file> <resource type> <resource name>\n"
#if DEBUG
		L"\n"
		L"Debug actions:\n"
		L"    /pschema: Prints restyle schema debug information.\n"
		L"        Usage: restyle /pschema <number>\n"
		L"               restyle /pschema /validate (Validates schema capitalizations.)\n"
#endif
		L"\n"
		L"Options:\n"
		L"    /out: Output file or folder of the /c and /d actions.\n"
	);
}

bool LoadThemeModule(LPWSTR pszPath)
{
	if (!pszPath || !*pszPath)
	{
		fwprintf(stderr, L"Fatal: Empty file path\n");
		return false;
	}

	if (g_hThemeModule)
	{
		FreeLibrary(g_hThemeModule);
		g_hThemeModule = NULL;
	}

	wcscpy_s(g_szThemeFilePath, pszPath);
	g_hThemeModule = LoadLibraryExW(g_szThemeFilePath, NULL, LOAD_LIBRARY_AS_DATAFILE);
	if (!g_hThemeModule)
	{
		fwprintf(stderr, L"Fatal: Failed to load theme file");
		return false;
	}

	return true;
}

int wmain(int argc, wchar_t *argv[])
{
	// Header
	Log(
		L"restyle %u.%u.%u"
#if DEBUG
		L" (DEBUG BUILD)"
#endif
		L"\n",
		VER_MAJOR, VER_MINOR, VER_REVISION
	);
	fwprintf(stderr, L"Built %s %s\n\n", __WDATE__, __WTIME__);

#define IsArg(str, name)   0 == _wcsicmp(str, L"/" name)

	if (argc <= 1 || IsArg(argv[1], "?") || IsArg(argv[1], "help"))
	{
		PrintUsage();
		return 0;
	}
	else if (IsArg(argv[1], "c"))
	{
		fwprintf(stderr, L"Fatal: Unimplemented action '/c'\n");
		return 1;
	}
	else if (IsArg(argv[1], "d"))
	{
		fwprintf(stderr, L"Fatal: Unimplemented action '/d'\n");
		return 1;
	}
	else if (IsArg(argv[1], "pcmap"))
	{
		if (argc < 3 || !LoadThemeModule(argv[2]) || !BinParser::ParseClassMap())
			return 1;

		wprintf(L" ID -> Class name\n");
		for (int i = 0; i < BinParser::classMap.size(); i++)
		{
			wprintf(L"% 3u -> '%s'\n", i, BinParser::NameOfClass(i));
		}
	}
	else if (IsArg(argv[1], "pbcmap"))
	{
		if (argc < 3 || !LoadThemeModule(argv[2]) || !BinParser::ParseClassMap() || !BinParser::ParseBaseClassMap())
			return 1;

		for (int i = 0; i < BinParser::baseClassMap.size(); i++)
		{
			BASECLASS &bc = BinParser::baseClassMap.at(i);
			wprintf(
				L"Base class:    '%s' (%u)\n"
				L"Derived class: '%s' (%u)\n",
				BinParser::NameOfClass(bc.dwBaseId),
				bc.dwBaseId,
				BinParser::NameOfClass(bc.dwDerivedId),
				bc.dwDerivedId
			);
			if (i != (BinParser::baseClassMap.size() - 1))
				wprintf(L"------------------------------\n");
		}
	}
	else if (IsArg(argv[1], "pvmap"))
	{
		if (argc < 3 || !LoadThemeModule(argv[2]) || !BinParser::ParseVariantMap())
			return 1;

		for (int i = 0; i < BinParser::variantMap.size(); i++)
		{
			VSVARIANT &var = BinParser::variantMap.at(i);
			wprintf(
				L"Resource name: %s\n"
				L"Size name:     %s\n"
				L"Color name:    %s\n",
				var.resourceName.c_str(),
				var.sizeName.c_str(),
				var.colorName.c_str()
			);
			if (i != (BinParser::variantMap.size() - 1))
				wprintf(L"------------------------------\n");
		}
	}
	else if (IsArg(argv[1], "precord"))
	{
		auto callback = [](const VSRECORD *lpRecord) -> bool
		{
			LPCWSTR szSymbolVal = GetSymbolValueName(lpRecord->lSymbolVal);
			LPCWSTR szType = GetPrimValueName(lpRecord->lType);
			LPCWSTR szClassName = BinParser::NameOfClass(lpRecord->iClass);
			LPCWSTR szPartName = GetPartName(szClassName, lpRecord->iPart);
			wprintf(
				L"========================================\n"
				L"lSymbolVal: %d (%s)\n"
				L"lType: %d (%s)\n"
				L"iClass: %d (%s)\n"
				L"iPart: %d (%s)\n"
				L"iState: %d\n"
				L"uResID: %u\n"
				L"lReserved: %d\n"
				L"cbData: %d\n",
				lpRecord->lSymbolVal, szSymbolVal ? szSymbolVal : L"Unknown symbol",
				lpRecord->lType, szType,
				lpRecord->iClass, szClassName ? szClassName : L"Invalid class name",
				lpRecord->iPart, szPartName,
				lpRecord->iState,
				lpRecord->uResID,
				lpRecord->lReserved,
				lpRecord->cbData
			);

			WCHAR szValueString[1024];
			switch (BinParser::GetRecordValueString(lpRecord, szValueString, 1024))
			{
				case EParseResult::Success:
					wprintf(L"Value: %s\n", szValueString);
					break;
				case EParseResult::Fail:
					wprintf(L"Value: !ERROR! FAILED TO GET VALUE AS STRING\n");
					break;
				case EParseResult::UnknownType:
					wprintf(L"Value: !ERROR! UNKNOWN VALUE TYPE\nRaw data: %s\n", szValueString);
					break;
			}
			return true;
		};

		if (argc < 5
		|| !LoadThemeModule(argv[2])
		|| !BinParser::ParseClassMap() // Need class names for debug output
		|| !BinParser::ParseRecordResource(argv[3], argv[4], callback))
			return 1;
	}
#if DEBUG
	else if (IsArg(argv[1], "pschema"))
	{
		ESchemaTestMode eTestMode = ESchemaTestMode::PrintHelpMessage;
		unsigned uEntryId = 0;

		if (argc == 3)
		{
			eTestMode = ESchemaTestMode::PrintEntryInfo;
			if (IsArg(argv[2], "validate"))
			{
				eTestMode = ESchemaTestMode::ValidateSymbols;
			}
			else if (!swscanf(argv[2], L"%i", &uEntryId))
			{
				Log(L"Fatal: Failed to parse integer argument for item ID.", ELogLevel::Fatal);
				return 0;
			}
		}
		else if (argc > 3)
		{
			Log(L"Fatal: Too many arguments", ELogLevel::Fatal);
			return 0;
		}

		TestSchema(eTestMode, uEntryId);
		return 1;
	}
#endif
	else
	{
		Log(L"Fatal: Unrecognized action '%s'\n", ELogLevel::Fatal, argv[1]);
		Log(L"Try `restyle /?` to see a list of actions\n");
		return 1;
	}

#undef IsArg

	return 0;
}