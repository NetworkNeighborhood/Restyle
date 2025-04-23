#include "Decompiler.h"

Restyle::ESupportedOS g_eSupportedOS = Restyle::ESupportedOS::NotSet;
WCHAR                 g_szOutFolder[MAX_PATH];
std::wstring          g_spszCurrentVariant;

struct ThemeINILine
{
	std::wstring spszKey;
	std::wstring spszValue;
	std::wstring spszComment;
};

typedef std::map<std::wstring, std::vector<ThemeINILine>> ThemeINIFile;

/* Multiple types are glorified int lists, so this is its own function. */
std::wstring ParseIntListValue(int *rgInts, size_t cInts)
{
	std::wstring result = L"";
	for (size_t i = 0; i < cInts; i++)
	{
		result += std::to_wstring(rgInts[i]);
		if (i != (cInts - 1))
			result += L' ';
	}
	return result;
}

HRESULT ValueToString(const VSRECORD *lpRecord, std::wstring &spszValue)
{
#define MINIMUM_SIZE(size) \
	if (lpRecord->cbData < size) \
	{ \
		Log(L"FATAL: Expected data size of at least %d, got %d.\n", ELogLevel::Fatal, size, lpRecord->cbData); \
		return E_FAIL; \
	}

#define EXPECTED_SIZE(size) \
	if (lpRecord->cbData != size) \
	{ \
		Log(L"FATAL: Expected data size of %d, got %d.\n", ELogLevel::Fatal, size, lpRecord->cbData); \
		return E_FAIL; \
	} 

	void *lpData = (void *)(lpRecord + 1);
	spszValue.clear();

	switch (lpRecord->lType)
	{
		case Restyle::TMT_BOOL:
			EXPECTED_SIZE(sizeof(BOOL));
			spszValue = *(BOOL *)lpData ? L"True" : L"False";
			return S_OK;

		case Restyle::TMT_INT:
		case Restyle::TMT_SIZE:
			EXPECTED_SIZE(sizeof(int));
			spszValue = std::to_wstring(*(int *)lpData);
			return S_OK;

		case Restyle::TMT_FLOAT:
			EXPECTED_SIZE(sizeof(float));
			spszValue = std::to_wstring(*(float *)lpData);
			return S_OK;

		case Restyle::TMT_ENUM:
		{
			EXPECTED_SIZE(sizeof(int));
			long lValue = *(int *)lpData;

			const Restyle::TMPROPINFO *pEnumValInfo = Restyle::FindEnumValueInfo(lpRecord->lSymbolVal, lValue, g_eSupportedOS);
			if (!pEnumValInfo)
			{
				Log(L"FATAL: Failed to find enum value name for enum %d, value %d.\n", ELogLevel::Fatal, lpRecord->lSymbolVal, lValue);
				return E_FAIL;
			}

			spszValue = pEnumValInfo->pszName;
			return S_OK;
		}
		
		case Restyle::TMT_STRING:
			// Documentation properties use stringtables
			if (lpRecord->uResID)
			{
				LPWSTR pszTemp = nullptr;
				int cch = LoadStringW(g_hThemeModule, lpRecord->uResID, (LPWSTR)&pszTemp, 0) + 1;
				if (cch <= 0)
				{
					Log(L"FATAL: Failed to load string resource %d from the theme.\n", ELogLevel::Fatal, lpRecord->uResID);
					return E_FAIL;
				}

				LPWSTR pszBuffer = new WCHAR[cch];
				if (!pszBuffer)
				{
					Log(L"FATAL: Failed to allocate memory for string.\n", ELogLevel::Fatal);
					return E_OUTOFMEMORY;
				}

				LoadStringW(g_hThemeModule, lpRecord->uResID, pszBuffer, cch);
				spszValue = pszBuffer;
				delete[] pszBuffer;
				return S_OK;
			}
			else
			{
				spszValue = (LPCWSTR)lpData;
				return S_OK;
			}

		case Restyle::TMT_RECT:
		case Restyle::TMT_MARGINS:
			static_assert(sizeof(RECT) == sizeof(MARGINS));
			EXPECTED_SIZE(sizeof(RECT));
			spszValue = ParseIntListValue((int *)lpData, 4);
			return S_OK;

		case Restyle::TMT_INTLIST:
		{
			MINIMUM_SIZE(sizeof(int));
			INTLIST *lpIntList = (INTLIST *)lpData;
			// Revalidate:
			EXPECTED_SIZE(sizeof(int) + (sizeof(int) * lpIntList->iValueCount));

			spszValue = ParseIntListValue(lpIntList->iValues, lpIntList->iValueCount);
			return S_OK;
		}

		case Restyle::TMT_FLOATLIST:
		{
			MINIMUM_SIZE(sizeof(float));
			FLOATLIST *lpFloatList = (FLOATLIST *)lpData;
			// Revalidate:
			EXPECTED_SIZE(sizeof(int) + (sizeof(int) * lpFloatList->iValueCount));

			for (size_t i = 0; i < lpFloatList->iValueCount; i++)
			{
				spszValue += std::to_wstring(lpFloatList->flValues[i]);
				if (i != (lpFloatList->iValueCount - 1))
					spszValue += L' ';
			};
			return S_OK;
		}

		case Restyle::TMT_POSITION:
		{
			EXPECTED_SIZE(sizeof(POINT));
			spszValue = ParseIntListValue((int *)lpData, 2);
			return S_OK;
		}

		case Restyle::TMT_COLOR:
		{
			EXPECTED_SIZE(sizeof(COLORREF));
			COLORREF crColor = *(COLORREF *)lpData;
			spszValue += std::to_wstring(GetRValue(crColor));
			spszValue += ' ';
			spszValue += std::to_wstring(GetGValue(crColor));
			spszValue += ' ';
			spszValue += std::to_wstring(GetBValue(crColor));
			return S_OK;
		}

		case Restyle::TMT_FONT:
		{
			if (!lpRecord->uResID)
			{
				Log(L"FATAL: Font property missing resource ID.\n", ELogLevel::Fatal);
				return E_FAIL;
			}
			// No idea. UXTheme does this.
			MINIMUM_SIZE(0x5C);

			// UXTheme uses a fixed 128-char buffer for this, so we'll do the same.
			WCHAR szBuffer[128];
			LoadStringW(g_hThemeModule, lpRecord->uResID, szBuffer, 128);

			spszValue = szBuffer;
			return S_OK;
		}

		// FUCK this type. They made a primitive type for an enum...
		case Restyle::TMT_HCCOLOR:
		{
			EXPECTED_SIZE(sizeof(int));
			int iVal = *(int *)lpData;
			const Restyle::TMPROPINFO *pValInfo =
				Restyle::FindEnumValueInfo(L"HIGHCONTRASTCOLOR", iVal, g_eSupportedOS);
			if (!pValInfo)
			{
				Log(L"FATAL: Invalid high contrast color type %d\n", ELogLevel::Fatal, iVal);
				return E_FAIL;
			}

			spszValue = pValInfo->pszName;
			return S_OK;
		}

		default:
			Log(L"FATAL: Type %d does not exist or cannot be represented as a string\n", ELogLevel::Fatal, lpRecord->lType);
			return E_FAIL;
	}

#undef EXPECTED_SIZE
#undef MINIMUM_SIZE
}

std::wstring GetSectionName(int iClass, int iPart, int iState)
{
	std::wstring spszResult;
	LPCWSTR lpClassName = BinParser::NameOfClass(iClass);
	spszResult += lpClassName;
	
	if (iPart || iState)
	{
		std::wstring spszSearchName = GetClassSearchName(lpClassName);
		std::wstring spszStateSearchName = spszSearchName;

		if (iPart)
		{
			spszResult += L'.';

			std::wstring spszEnumName = spszSearchName;
			spszEnumName += L"PARTS";

			const Restyle::TMPROPINFO *pPropInfo =
				Restyle::FindEnumValueInfo(spszEnumName.c_str(), iPart, g_eSupportedOS);

			if (pPropInfo)
			{
				spszResult += pPropInfo->pszName;
				spszStateSearchName = pPropInfo->pszName;
			}
			else
			{
				Log(
					L"WARNING: Unknown part name for class '%s', part %d. Defaulting to *Part%d.\n",
					ELogLevel::Warning,
					spszSearchName.c_str(),
					iPart,
					iPart
				);
				spszResult += L"*Part";
				spszResult += std::to_wstring(iPart);
			}
		}

		if (iState)
		{
			spszResult += L'(';

			spszStateSearchName += L"STATES";

			const Restyle::TMPROPINFO *pPropInfo =
				Restyle::FindEnumValueInfo(spszStateSearchName.c_str(), iState, g_eSupportedOS);

			if (pPropInfo)
			{
				spszResult += pPropInfo->pszName;
			}
			else
			{
				Log(
					L"WARNING: Unknown state name for class '%s', part %d, state %d. Defaulting to *State%d.\n",
					ELogLevel::Warning,
					spszSearchName.c_str(),
					iPart,
					iState,
					iState
				);
				spszResult += L"*State";
				spszResult += std::to_wstring(iState);
			}

			spszResult += L')';
		}
	}
	return spszResult;
}

void EnsureClassBaseSection(int iClass, ThemeINIFile &file)
{
	/* These classes in RMAP don't need a base section. */
	static LPCWSTR kNoBaseSectionClasses[] = {
		L"colorvariant",
		L"sizevariant",
	};

	std::wstring spszSectionName = GetSectionName(iClass, 0, 0);
	for (LPCWSTR pszNoBase : kNoBaseSectionClasses)
	{
		if (spszSectionName == pszNoBase)
			return;
	}

	// Already initialized
	if (file.count(spszSectionName))
		return;

	file[spszSectionName] = {};

	DWORD dwBaseClass = BinParser::GetBaseClass(iClass);
	if (dwBaseClass != (DWORD)-1)
	{
		LPCWSTR lpBaseClassName = BinParser::NameOfClass(dwBaseClass);
		ThemeINILine line;
		line.spszKey = L"Base";
		line.spszValue = lpBaseClassName;
		file[spszSectionName].push_back(line);
	}
}

bool ParseRecordToThemeINIFile(const VSRECORD *lpRecord, void *lpParam)
{
	ThemeINIFile &file = *(ThemeINIFile *)lpParam;
	EnsureClassBaseSection(lpRecord->iClass, file);

	std::wstring spszHeaderName = GetSectionName(
		lpRecord->iClass,
		lpRecord->iPart,
		lpRecord->iState
	);

	if (!file.count(spszHeaderName))
		file[spszHeaderName] = {};

	Restyle::SearchSchemaParams params = { 0 };
	params.cbSize = sizeof(params);
	params.eSearchQuery = Restyle::ESchemaSearchQuery::Property;
	params.sEnumVal = lpRecord->lSymbolVal;
	const Restyle::TMPROPINFO *pPropInfo = Restyle::SearchSchema(&params);
#if DEBUG
	if (!pPropInfo)
		__debugbreak();
#endif

	std::wstring spszValue;
	// Special handler to dump images
	if (lpRecord->lType == Restyle::TMT_FILENAME
	|| lpRecord->lType == Restyle::TMT_DISKSTREAM)
	{

	}
	else
	{
		if (FAILED(ValueToString(lpRecord, spszValue)))
			return false;
	}

	LPCWSTR pszComment = Restyle::GetCommentForProperty(pPropInfo);

	ThemeINILine line;
	line.spszKey = pPropInfo->pszName;
	line.spszValue = spszValue;
	if (pszComment)
		line.spszComment = pszComment;
	file[spszHeaderName].push_back(line);

	return true;
}

bool DumpThemeINIFile(ThemeINIFile &outFile, LPCWSTR pszOutPath)
{
	std::wstring output;

	output +=
		L"; Generated by Restyle\r\n"
		L"; https://github.com/NetworkNeighborhood/Restyle\r\n"
		L";\r\n"
		L"; --------------------------------------------\r\n"
		L"; file: ";
	output += pszOutPath;
	output += L"\r\n; --------------------------------------------";

	ThemeINIFile::iterator it;
	for (it = outFile.begin(); it != outFile.end(); it++)
	{
		output += L"\r\n\r\n[";
		output += it->first;
		output += L"]";

		for (const ThemeINILine &line : it->second)
		{
			if (!line.spszComment.empty())
			{
				output += L"\r\n; ";
				std::wstring spszComment = line.spszComment;
				size_t pos = spszComment.find(L"\r\n");
				while (pos != std::wstring::npos)
				{
					spszComment.replace(pos, 2, L"\r\n; ");
					pos = spszComment.find(L"\r\n", pos + 4);
				}
				output += spszComment;
			}

			output += L"\n";
			output += line.spszKey;
			output += L" = ";
			output += line.spszValue;
		}
	}

	WCHAR szFullOutPath[MAX_PATH];
	wcscpy_s(szFullOutPath, g_szOutFolder);
	PathCchAppend(szFullOutPath, MAX_PATH, pszOutPath);
	CSimpleFile file;
	if (FAILED(file.Create(szFullOutPath)))
		return false;
	if (FAILED(file.WriteString(output.c_str(), output.size())))
		return false;
	return true;
}

bool DecompileTheme(LPCWSTR pszOutFolder, Restyle::ESupportedOS eSupportedOS)
{
	if (!BinParser::ParseClassMap() || !BinParser::ParseBaseClassMap() || !BinParser::ParseVariantMap())
		return false;

	g_eSupportedOS = eSupportedOS;
	wcscpy_s(g_szOutFolder, pszOutFolder);

	ThemeINIFile rmap;
	if (!BinParser::ParseRecordResource(L"VARIANT", L"NORMAL", &rmap, ParseRecordToThemeINIFile))
		return false;

	if (!DumpThemeINIFile(rmap, L"themes.ini"))
		return false;

	rmap;
	return true;
}