#include "Decompiler.h"

struct ThemeINILine
{
	std::wstring spszKey;
	std::wstring spszValue;
	std::wstring spszComment;
};

typedef std::map<std::wstring, std::vector<ThemeINILine>> ThemeINIFile;

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
		std::wstring spszSearchName = GetClassSearchName(lpClassName);
		std::wstring spszStateSearchName = spszSearchName;

		if (lpRecord->iPart)
		{
			spszResult += L'.';

			std::wstring spszEnumName = spszSearchName;
			spszEnumName += L"PARTS";

			const Restyle::TMPROPINFO *pPropInfo =
				Restyle::FindEnumValueInfo(spszEnumName.c_str(), lpRecord->iPart);

			if (pPropInfo)
			{
				spszResult += pPropInfo->pszName;
				
			}
			else
			{
				spszResult += L"*Part";
				spszResult += std::to_wstring(lpRecord->iPart);
			}
		}

		if (lpRecord->iState)
		{
			spszResult += L'(';
			
			std::wstring spszEnumName = spszSearchName;
			spszEnumName += L"STATES";

			const Restyle::TMPROPINFO *pPropInfo =
				Restyle::FindEnumValueInfo(spszEnumName.c_str(), lpRecord->iPart);

			if (pPropInfo)
			{
				spszResult += pPropInfo->pszName;
			}
			else
			{
				spszResult += L"*State";
				spszResult += std::to_wstring(lpRecord->iPart);
			}

			spszResult += L')';
		}
	}
	return spszResult;
}

bool ParseRecordToThemeINIFile(const VSRECORD *lpRecord, void *lpParam)
{
	ThemeINIFile *lpFile = (ThemeINIFile *)lpParam;
	std::wstring spszHeaderName = GetSectionName(lpRecord);
	Log(L"%s\n", spszHeaderName.c_str());
	return true;
}

bool DecompileTheme(LPCWSTR pszOutFolder)
{
	if (!BinParser::ParseClassMap() || !BinParser::ParseBaseClassMap() || !BinParser::ParseVariantMap())
		return false;

	ThemeINIFile rmap;
	if (!BinParser::ParseRecordResource(L"VARIANT", L"NORMAL", &rmap, ParseRecordToThemeINIFile))
		return false;


}