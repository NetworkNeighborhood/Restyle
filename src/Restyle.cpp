#include "Restyle.h"
#include "schema/SchemaTest.h"
#include "schema/SchemaUtils.h"
#include "decompiler/BinParser.h"
#include "decompiler/Decompiler.h"
#include "Util.h"
#include "File.h"
#include "compiler/IniParser/IniParser.h"
#include "BuildDate.h"

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
		L"    /pfile: Prints a file.\n"
		L"        Usage: restyle /pfile C:\\path\\to\\file.ini\n"
		L"    /testparser: Tests the parser.\n"
		L"        Usage: restyle /ptestparser C:\\path\\to\\file.ini\n"
#endif
		L"\n"
		L"Options:\n"
		L"    /out: Output file or folder of the /c and /d actions.\n"
		L"    /os: Windows version to use part and state names from for decompilation.\n"
		// Keep up to date with ESupportedOS enum in schema/SupportedOS.h
		L"         Can be one of the following:\n"
		L"             WinVista\n"
		L"             Win7\n"
		L"             Win8\n"
		L"             Win81\n"
		L"             Win10_1507\n"
		L"             Win10_1607\n"
		L"             Win10_1703\n"
		L"             Win11_22H2\n"
	);
}

bool LoadThemeModule(LPWSTR pszPath)
{
	if (!pszPath || !*pszPath)
	{
		Log(L"FATAL: Empty file path\n", ELogLevel::Fatal);
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
		Log(L"FATAL: Failed to load theme file\n", ELogLevel::Fatal);
		return false;
	}

	return true;
}

int wmain(int argc, wchar_t *argv[])
{
	// Header
	Log(
		L"Restyle %u.%u.%u"
#if DEBUG
		L" (DEBUG BUILD)"
#endif
		L"\n",
		VER_MAJOR, VER_MINOR, VER_REVISION
	);
	Log(L"%s\n\n", GetBuildDate());

// Inform the user to use native if they're running under emulation on ARM64.
// We don't do this for x86-32 builds on x86-64 as those still run as native code.
#if _M_X64 || _M_IX86
	// This function only exists since 1709, which is too recent for me to be comfortable
	// statically linking against.
	HMODULE hKernel32 = LoadLibraryW(L"kernel32.dll");
	if (hKernel32)
	{
		BOOL (WINAPI *pfnIsWow64Process2)(HANDLE hProcess, USHORT *pProcessMachine, USHORT *pNativeMachine) = nullptr;
		pfnIsWow64Process2 = (decltype(pfnIsWow64Process2))GetProcAddress(hKernel32, "IsWow64Process2");
		if (pfnIsWow64Process2)
		{
			USHORT uProcessMachine = 0;
			USHORT uNativeMachine = 0;
			if (pfnIsWow64Process2(GetCurrentProcess(), &uProcessMachine, &uNativeMachine))
			{
				if (uNativeMachine == IMAGE_FILE_MACHINE_ARM64)
				{
#if _WIN64
					Log(L"\nYou are using an x64 version of Restyle.\n", ELogLevel::Warning);
#else
					Log(L"\nYou are using an x86 version of Restyle.\n", ELogLevel::Warning);
#endif
					Log(L"For faster performance on your ARM CPU, we recommend that you download the native ARM64 version of Restyle.\n\n", ELogLevel::Warning);
				}
			}
		}

		FreeLibrary(hKernel32);
	}
#endif

	if (FAILED(Restyle::InitializeSchemaUtils()))
	{
		Log(L"Error: Failed to initialize schema utils.", ELogLevel::Fatal);
	}

#define IsArg(str, name)       (0 == _wcsicmp(str, L"/" name))
#define IsValueArg(str, name)  (0 == _wcsnicmp(str, L"/" name, sizeof(name)))
#define GetArgValue(str, name) (str + sizeof("/" name))

	if (argc <= 1 || IsArg(argv[1], "?") || IsArg(argv[1], "help"))
	{
		PrintUsage();
		return 0;
	}
	else if (IsArg(argv[1], "c"))
	{
		Log(L"FATAL: Unimplemented action '/c'\n", ELogLevel::Fatal);
		return 1;
	}
	else if (IsArg(argv[1], "d"))
	{
		if (argc < 3)
		{
			Log(L"FATAL: Too few arguments\n", ELogLevel::Fatal);
			return 1;
		}

		Restyle::ESupportedOS eSupportedOS = Restyle::ESupportedOS::NotSet;
		WCHAR szOutPath[MAX_PATH] = { 0 };

		if (!LoadThemeModule(argv[2]))
			return 1;

		for (int i = 3; i < argc; i++)
		{
			if (IsValueArg(argv[i], "os"))
			{
				LPCWSTR pszOS = GetArgValue(argv[i], "os");
				eSupportedOS = Restyle::StringToSupportedOS(pszOS);
				if (eSupportedOS == Restyle::ESupportedOS::NotSet)
				{
					Log(L"FATAL: Unrecognized OS name '%s'\n", ELogLevel::Fatal, pszOS);
					return 1;
				}
			}
			else if (IsValueArg(argv[i], "out"))
			{
				(void)_wfullpath(szOutPath, GetArgValue(argv[i], "out"), MAX_PATH);

				DWORD dwAttrib = GetFileAttributesW(szOutPath);
				if (dwAttrib == INVALID_FILE_ATTRIBUTES || !(dwAttrib & FILE_ATTRIBUTE_DIRECTORY))
				{
					if (!CreateDirectoryW(szOutPath, nullptr))
					{
						Log(L"FATAL: Failed to create directory '%s'.\n", ELogLevel::Fatal, szOutPath);
						return 1;
					}
				}

				if (!PathIsDirectoryEmptyW(szOutPath) && !EmptyDirectory(szOutPath))
				{
					Log(L"FATAL: Could not empty directory '%s'.\n", ELogLevel::Fatal, szOutPath);
					return 1;
				}
			}
		}

		if (!szOutPath[0])
		{
			Log(L"FATAL: No out directory defined.\n", ELogLevel::Fatal);
			return 1;
		}

		if (eSupportedOS == Restyle::ESupportedOS::NotSet)
		{
			eSupportedOS = Restyle::ESupportedOS::Win10_1703;
			Log(L"WARNING: No OS defined, defaulting to Win10_1703.\n", ELogLevel::Warning);
		}

		if (!DecompileTheme(szOutPath, eSupportedOS))
			return 1;
		return 0;
	}
	else if (IsArg(argv[1], "pcmap"))
	{
		if (argc < 3)
		{
			Log(L"FATAL: Too few arguments\n", ELogLevel::Fatal);
			return 1;
		}

		if (!LoadThemeModule(argv[2]) || !BinParser::ParseClassMap())
			return 1;

		wprintf(L" ID -> Class name\n");
		for (int i = 0; i < BinParser::classMap.size(); i++)
		{
			wprintf(L"% 3u -> '%s'\n", i, BinParser::NameOfClass(i));
		}
	}
	else if (IsArg(argv[1], "pbcmap"))
	{
		if (argc < 3)
		{
			Log(L"FATAL: Too few arguments\n", ELogLevel::Fatal);
			return 1;
		}

		if (!LoadThemeModule(argv[2]) || !BinParser::ParseClassMap() || !BinParser::ParseBaseClassMap())
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
		if (argc < 3)
		{
			Log(L"FATAL: Too few arguments\n", ELogLevel::Fatal);
			return 1;
		}

		if (!LoadThemeModule(argv[2]) || !BinParser::ParseVariantMap())
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
		auto callback = [](const VSRECORD *lpRecord, void *lpParam) -> bool
		{
			std::unique_ptr<WCHAR[]> pszPropName, pszPartName, pszStateName;
			EParseResult result = GetPropName(lpRecord->lSymbolVal, lpRecord->lType, pszPropName);
			RETURN_IF_PARSE_FAILED(result, false);
			LPCWSTR pszType = GetSymbolValueName(lpRecord->lType);
			LPCWSTR pszClassName = BinParser::NameOfClass(lpRecord->iClass);
			result = GetPartAndStateName(pszClassName, pszPartName, pszStateName, lpRecord->iPart, lpRecord->iState);
			RETURN_IF_PARSE_FAILED(result, false);
			wprintf(
				L"========================================\n"
				L"lSymbolVal: %d (%s)\n"
				L"lType: %d (%s)\n"
				L"iClass: %d (%s)\n"
				L"iPart: %d (%s)\n"
				L"iState: %d (%s)\n"
				L"uResID: %u\n"
				L"lReserved: %d\n"
				L"cbData: %d\n",
				lpRecord->lSymbolVal, pszPropName.get(),
				lpRecord->lType, pszType ? pszType : L"Unknown type",
				lpRecord->iClass, pszClassName ? pszClassName : L"Invalid class name",
				lpRecord->iPart, (lpRecord->iPart == 0) ? L"Common" : pszPartName.get(),
				lpRecord->iState, (lpRecord->iState == 0) ? L"Common" : pszStateName.get(),
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
		|| !BinParser::ParseRecordResource(argv[3], argv[4], nullptr, callback))
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
				Log(L"FATAL: Failed to parse integer argument for item ID.", ELogLevel::Fatal);
				return 0;
			}
		}
		else if (argc > 3)
		{
			Log(L"FATAL: Too many arguments", ELogLevel::Fatal);
			return 0;
		}

		TestSchema(eTestMode, uEntryId);
		return 1;
	}
	else if (IsArg(argv[1], "pfile"))
	{
		if (argc != 3)
		{
			Log(L"FATAL: Invalid number of arguments.", ELogLevel::Fatal);
			return 0;
		}

		LPCWSTR szPath = argv[2];

		CSimpleFile file;
		HRESULT hr = file.Open(szPath);

		if (FAILED(hr))
		{
			Log(L"FATAL: Failed to open file.", ELogLevel::Fatal);
			return 0;
		}

		DWORD dwFileSizeHigh = 0;
		DWORD dwFileSize = file.GetFileSize(&dwFileSizeHigh);

		if (dwFileSize == 0)
		{
			Log(L"FATAL: Failed to get file size.", ELogLevel::Fatal);
			return 0;
		}

		LPWSTR szString = new WCHAR[dwFileSize + sizeof('\0')];

		DWORD cbRead = 0;
		hr = file.Read((void *)szString, dwFileSize * sizeof(WCHAR) + sizeof(L'\0'), &cbRead);

		if (FAILED(hr))
		{
			delete[] szString;
			Log(L"FATAL: Failed to read file.", ELogLevel::Fatal);
			return 0;
		}

		Log(L"%s", szString, ELogLevel::Info);

		if (szString)
		{
			delete[] szString;
		}
	}
	else if (IsArg(argv[1], "testparser"))
	{
		if (argc != 2)
		{
			Log(L"FATAL: Invalid number of arguments.", ELogLevel::Fatal);
			return 0;
		}

		//LPCWSTR szPath = argv[2];

		IniParser::ParseIniFile(nullptr);
		system("pause");
	}
#endif
	else
	{
		Log(L"FATAL: Unrecognized action '%s'\n", ELogLevel::Fatal, argv[1]);
		Log(L"Try `restyle /?` to see a list of actions\n");
		return 1;
	}

	return 0;
}