#include "restyle.h"
#include "binparser.h"
#include "schemapriv.h"
#include "util.h"

namespace BinParser
{

std::vector<std::wstring> g_classMap;
std::vector<BASECLASS> g_baseClassMap;
std::vector<VSVARIANT> g_variantMap;

bool ParseClassMap(void)
{
	LPVOID lpResource;
	DWORD dwSize;
	if (!GetBinaryResource(L"CMAP", L"CMAP", &lpResource, &dwSize))
		return false;

	LPWSTR pszClass = (LPWSTR)lpResource;
	while ((UINT_PTR)pszClass < ((UINT_PTR)lpResource + dwSize))
	{
		if (*pszClass)
			g_classMap.push_back(pszClass);
		pszClass += wcslen(pszClass) + 1;
	}

	return true;
}

bool ParseBaseClassMap(void)
{
	LPBCMAP lpBCMap;
	DWORD dwSize;
	if (!GetBinaryResource(L"BCMAP", L"BCMAP", (LPVOID *)&lpBCMap, &dwSize))
		return false;

	for (DWORD i = 0; i < lpBCMap->dwLength; i++)
	{
		// 0xFFFFFFFF means no inherited class
		if (lpBCMap->pdwItems[i] != UINT32_MAX)
		{
			BASECLASS bc = { lpBCMap->pdwItems[i], i };
			g_baseClassMap.push_back(bc);
		}
	}

	return true;
}

bool ParseVariantMap(void)
{
	LPVMAPENTRY lpVMap;
	DWORD dwSize;
	if (!GetBinaryResource(L"VMAP", L"VMAP", (LPVOID *)&lpVMap, &dwSize))
		return false;

	VSVARIANT var;
	LPVMAPENTRY lpEntry = lpVMap;
	for (int i = 0;; i++)
	{
		// if we exceed bounds
		if ((long long)lpEntry > ((long long)lpVMap + dwSize))
			break;

		// if string exceeds bounds
		if ((long long)lpEntry + sizeof(DWORD) + (sizeof(WCHAR) * lpEntry->dwLength) > ((long long)lpVMap + dwSize))
			break;

		LPWSTR pszBuffer = new WCHAR[lpEntry->dwLength];
		wcsncpy(pszBuffer, lpEntry->pszString, lpEntry->dwLength);
		switch (i % 3)
		{
			case 0:
				var.resourceName = pszBuffer;
				break;
			case 1:
				var.sizeName = pszBuffer;
				break;
			case 2:
				var.colorName = pszBuffer;
				g_variantMap.push_back(var);
				ZeroMemory(&var, sizeof(var));
				break;
		}
		delete[] pszBuffer;

		LPBYTE lpNewEntry = (LPBYTE)lpEntry;
		lpNewEntry += sizeof(DWORD);
		lpNewEntry += sizeof(WCHAR) * (lpEntry->dwLength + 1);
		lpEntry = (LPVMAPENTRY)lpNewEntry;
	}
	
	return true;
}

bool ParseRecordResource(LPCWSTR lpType, LPCWSTR lpName, RecordParserCallback pfnCallback)
{
	if (!pfnCallback)
		return false;

	LPVOID lpResource;
	DWORD dwSize;
	if (!GetBinaryResource(lpType, lpName, &lpResource, &dwSize))
		return false;
	
	VSRECORD *lpRecord = (VSRECORD *)lpResource;
	while ((long long)lpRecord < ((long long)lpResource + dwSize))
	{
		if (!pfnCallback(lpRecord))
			break;

		DWORD dwNextOffset = 0;
		if (!lpRecord->uResID)
			dwNextOffset = lpRecord->cbData;
		// Aligns on 8-bit boundary. UXTheme does this.
		dwNextOffset = (dwNextOffset + (sizeof(VSRECORD) | 7)) & 0xFFFFFFF8;
		lpRecord = (VSRECORD *)((char *)lpRecord + dwNextOffset);
	}
	
	return true;
}

} // namespace BinParser