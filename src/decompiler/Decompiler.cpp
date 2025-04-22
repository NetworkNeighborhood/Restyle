#include "Decompiler.h"

struct ThemeINILine
{
	std::wstring spszKey;
	std::wstring spszValue;
	std::wstring spszComment;
};

typedef std::map<std::wstring, std::vector<ThemeINILine>> ThemeINIFile;

std::wstring GetBaseClassName(const std::wstring &className)
{
	LPCWSTR lpStr = wcsrstr(className.c_str(), L"::");
	if (lpStr)
		return std::wstring(lpStr + 2);
	else
		return className;
}

std::wstring GetSectionName(const VSRECORD *lpRecord)
{
	std::wstring spszResult;
	LPCWSTR lpClassName = BinParser::NameOfClass(lpRecord->iClass);
	spszResult += lpClassName;
	if (lpRecord->iPart == 0 && lpRecord->iState == 0)
	{
		DWORD dwBaseClass = BinParser::GetBaseClass(lpRecord->iClass);
		LPCWSTR lpBaseClassName = nullptr;
		if (dwBaseClass != (DWORD)-1)
			lpBaseClassName = BinParser::NameOfClass(dwBaseClass);
		bool fExpectedBaseClass = (nullptr != wcsstr(lpClassName, L"::"));
;		// Expected implicit base class but got none. We explicitly derive from
		// none in this edge case.
		if (dwBaseClass == (DWORD)-1 && fExpectedBaseClass)
		{
			spszResult += L" : *None";
		}
		// Check if we can rely on implicit base class.
		else if (dwBaseClass != (DWORD)-1 && fExpectedBaseClass)
		{
			std::wstring spszExpectedBaseClass = GetBaseClassName(lpClassName);
			if (spszExpectedBaseClass != lpBaseClassName)
			{
				spszResult += L" : ";
				spszResult += lpBaseClassName;
			}
		}
		else if (dwBaseClass != (DWORD)-1)
		{
			spszResult += L" : ";
			spszResult += lpBaseClassName;
		}
	}
	else
	{
		std::wstring spszSearchName = GetBaseClassName(lpClassName);

		if (lpRecord->iPart)
		{
			//spszResult += L'.';

			//Restyle::SearchSchemaParams params = {};
			//params.cbSize = sizeof(params);
			//params.eSearchQuery = Restyle::ESchemaSearchQuery::Parts;
			//params.szName = spszSearchName.c_str();
			//params.bType = lpRecord->lType;
			//params.sEnumVal = lpRecord->iPart;
			//const Restyle::TMPROPINFO *pPropInfo = Restyle::SearchSchema(&params);
			//if (pPropInfo)
			//{
			//	spszResult += pPropInfo->pszName;
			//}
			//else
			//{
			//	spszResult += L"*Part";
			//	spszResult += lpRecord->iPart;
			//}
		}
	}
	return spszResult;
}

bool ParseRecordToThemeINIFile(const VSRECORD *lpRecord, void *lpParam)
{
	ThemeINIFile *lpFile = (ThemeINIFile *)lpParam;

	return true;
}

bool DecompileTheme(LPCWSTR pszOutFolder)
{
	if (!BinParser::ParseClassMap() || !BinParser::ParseBaseClassMap() || !BinParser::ParseVariantMap())
		return false;

	ThemeINIFile rmap;
	if (!BinParser::ParseRecordResource(L"RMAP", L"RMAP", &rmap, ParseRecordToThemeINIFile))
		return false;


}