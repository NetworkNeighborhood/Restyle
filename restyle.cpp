#include "restyle.h"

WCHAR g_szThemeFilePath[MAX_PATH] = { 0 };
HMODULE g_hThemeModule = NULL;

std::vector<std::wstring> g_classMap;
std::vector<BASECLASS> g_baseClassMap;
std::vector<VSVARIANT> g_variantMap;

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
		L"    /pschema: Prints restyle schema debug information."
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
}

int wmain(int argc, wchar_t *argv[])
{
	// Header
	wprintf(L"restyle %u.%u.%u\n", VER_MAJOR, VER_MINOR, VER_REVISION);
	wprintf(L"Built %s %s\n\n", __WDATE__, __WTIME__);

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
		if (argc < 3 || !LoadThemeModule(argv[2]) || !ParseClassMap())
			return 1;

		wprintf(L" ID -> Class name\n");
		for (int i = 0; i < g_classMap.size(); i++)
		{
			wprintf(L"% 3u -> '%s'\n", i, NameOfClass(i));
		}
	}
	else if (IsArg(argv[1], "pbcmap"))
	{
		if (argc < 3 || !LoadThemeModule(argv[2]) || !ParseClassMap() || !ParseBaseClassMap())
			return 1;

		for (int i = 0; i < g_baseClassMap.size(); i++)
		{
			BASECLASS &bc = g_baseClassMap.at(i);
			wprintf(
				L"Base class:    '%s' (%u)\n"
				L"Derived class: '%s' (% u)\n",
				NameOfClass(bc.dwBaseId),
				bc.dwBaseId,
				NameOfClass(bc.dwDerivedId),
				bc.dwDerivedId
			);
			if (i != (g_baseClassMap.size() - 1))
				wprintf(L"------------------------------\n");
		}
	}
	else if (IsArg(argv[1], "pvmap"))
	{
		if (argc < 3 || !LoadThemeModule(argv[2]) || !ParseVariantMap())
			return 1;

		for (int i = 0; i < g_variantMap.size(); i++)
		{
			VSVARIANT &var = g_variantMap.at(i);
			wprintf(
				L"Resource name: %s\n"
				L"Size name:     %s\n"
				L"Color name:    %s\n",
				var.resourceName.c_str(),
				var.sizeName.c_str(),
				var.colorName.c_str()
			);
			if (i != (g_variantMap.size() - 1))
				wprintf(L"------------------------------\n");
		}
	}
	else if (IsArg(argv[1], "precord"))
	{
		if (argc < 5 || !LoadThemeModule(argv[2]) || !ParseRecordResource(argv[3], argv[4]))
			return 1;
	}
#ifdef DEBUG
	else if (IsArg(argv[1], "pschema"))
	{
		bool fPrintEntry = false;
		unsigned uEntryId = 0;

		if (argc == 3)
		{
			fPrintEntry = true;
			if (!swscanf(argv[2], L"%i", &uEntryId))
			{
				fwprintf(stderr, L"Fatal: Failed to parse integer argument for item ID.");
				return 0;
			}
		}
		else if (argc > 3)
		{
			fwprintf(stderr, L"Fatal: Too many arguments");
			return 0;
		}

		extern void TestSchema(bool fPrintEntry, unsigned uEntryId);
		TestSchema(fPrintEntry, uEntryId);
		return 1;
	}
#endif
	else
	{
		fwprintf(stderr, L"Fatal: Unrecognized action '%s'\n", argv[1]);
		fwprintf(stderr, L"Try `restyle /?` to see a list of actions\n");
		return 1;
	}

#undef IsArg

	return 0;
}