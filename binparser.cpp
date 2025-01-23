#include "restyle.h"
#include "binparser.h"
#include "schemapriv.h"
#include "util.h"

namespace BinParser
{

std::vector<std::wstring> classMap;
std::vector<BASECLASS> baseClassMap;
std::vector<VSVARIANT> variantMap;

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
			classMap.push_back(pszClass);
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
			baseClassMap.push_back(bc);
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
				variantMap.push_back(var);
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

ERecordValueStringResult GetRecordValueString(const VSRECORD *lpRecord, LPWSTR pszBuffer, DWORD cchBufferMax)
{
	if (!lpRecord || !pszBuffer || !cchBufferMax)
		return ERecordValueStringResult::Failed;

	ZeroMemory(pszBuffer, cchBufferMax * sizeof(WCHAR));

	switch (lpRecord->lType)
	{
		case Restyle::TMT_BOOL:
		{
			if (lpRecord->cbData < sizeof(BOOL))
				return ERecordValueStringResult::Failed;

			BOOL fValue = *(BOOL *)((BYTE *)lpRecord + sizeof(VSRECORD));
			swprintf_s(pszBuffer, cchBufferMax, L"%s", fValue ? L"true" : L"false");
			return ERecordValueStringResult::Succeeded;
		}

		case Restyle::TMT_INT:
		case Restyle::TMT_SIZE:
		{
			if (lpRecord->cbData < sizeof(int))
				return ERecordValueStringResult::Failed;

			int iValue = *(int *)((BYTE *)lpRecord + sizeof(VSRECORD));
			swprintf_s(pszBuffer, cchBufferMax, L"%d", iValue);
			return ERecordValueStringResult::Succeeded;
		}

		case Restyle::TMT_FLOAT:
		{
			if (lpRecord->cbData < sizeof(float))
				return ERecordValueStringResult::Failed;

			float flValue = *(float *)((BYTE *)lpRecord + sizeof(VSRECORD));
			swprintf_s(pszBuffer, cchBufferMax, L"%f", flValue);
			return ERecordValueStringResult::Succeeded;
		}

		case Restyle::TMT_ENUM:
		{
			if (lpRecord->cbData < sizeof(int))
				return ERecordValueStringResult::Failed;

			const Restyle::TMSCHEMAINFO *pSchemaInfo = Restyle::GetSchemaInfo();
			const Restyle::TMPROPINFO *pPropInfo = pSchemaInfo->pPropTable;

			int eValue = *(int *)((BYTE *)lpRecord + sizeof(VSRECORD));
			LPCWSTR szEnumName = nullptr;
			int iEnumDefOffset = 0;
			LPCWSTR szValueName = nullptr;

			// First pass: find the enum name in the property table:
			for (int i = 0; i < pSchemaInfo->iPropCount; i++)
			{
				if (pPropInfo[i].bPrimVal == Restyle::TMT_ENUM && pPropInfo[i].sEnumVal == lpRecord->lSymbolVal)
				{
					szEnumName = pPropInfo[i].pszName;
					break;
				}
			}
			// Second pass: find the enum definition in the schema:
			for (int i = 0; i < pSchemaInfo->iPropCount; i++)
			{
				if (pPropInfo[i].bPrimVal == Restyle::TMT_ENUMDEF && 0 == AsciiStrCmpI(szEnumName, pPropInfo[i].pszName))
				{
					iEnumDefOffset = i + 1;
					break;
				}
			}
			// Third pass: find the enum value name:
			for (int i = iEnumDefOffset; i < pSchemaInfo->iPropCount; i++)
			{
				if (pPropInfo[i].bPrimVal != Restyle::TMT_ENUMVAL)
				{
					break;
				}

				if (pPropInfo[i].sEnumVal == eValue)
				{
					szValueName = pPropInfo[i].pszName;
					break;
				}
			}

			if (szValueName)
			{
				swprintf_s(pszBuffer, cchBufferMax, L"%s", szValueName);
				return ERecordValueStringResult::Succeeded;
			}
			return ERecordValueStringResult::Failed;
		}

		case Restyle::TMT_STRING:
		{
			LPCWSTR szValue = (LPCWSTR)((BYTE *)lpRecord + sizeof(VSRECORD));
			swprintf_s(pszBuffer, cchBufferMax, L"%s", szValue);
			return ERecordValueStringResult::Succeeded;
		}

		case Restyle::TMT_RECT:
		{
			if (lpRecord->cbData != sizeof(RECT))
				return ERecordValueStringResult::Failed;

			RECT *lpRect = (RECT *)((BYTE *)lpRecord + sizeof(VSRECORD));
			swprintf_s(
				pszBuffer, cchBufferMax, L"%d, %d, %d, %d",
				lpRect->left,
				lpRect->top,
				lpRect->right,
				lpRect->bottom
			);
			return ERecordValueStringResult::Succeeded;
		}

		case Restyle::TMT_MARGINS:
		{
			if (lpRecord->cbData != sizeof(MARGINS))
				return ERecordValueStringResult::Failed;

			MARGINS *lpMargins = (MARGINS *)((BYTE *)lpRecord + sizeof(VSRECORD));
			swprintf_s(
				pszBuffer, cchBufferMax, L"%d, %d, %d, %d",
				lpMargins->cxLeftWidth,
				lpMargins->cxRightWidth,
				lpMargins->cyTopHeight,
				lpMargins->cyBottomHeight
			);
			return ERecordValueStringResult::Succeeded;
		}

		// Not used prior to V4 as far as I can tell.
		// But it existed and had syntax in V3, so this copies that,
		// as do the other types.
		case Restyle::TMT_INTLIST:
		{
			if (lpRecord->cbData < sizeof(int))
				return ERecordValueStringResult::Failed;

			INTLIST *lpIntList = (INTLIST *)((BYTE *)lpRecord + sizeof(VSRECORD));
			// Revalidate:
			if (lpRecord->cbData < sizeof(int) + sizeof(int) * lpIntList->iValueCount)
				return ERecordValueStringResult::Failed;
			
			std::wstring text;
			for (int i = 0; i < lpIntList->iValueCount; i++)
			{
				text += lpIntList->iValues[i];
				if (i != lpIntList->iValueCount - 1)
					text += L", ";
			}
			swprintf_s(pszBuffer, cchBufferMax, L"%s", text.c_str());
			return ERecordValueStringResult::Succeeded;
		}

		case Restyle::TMT_FLOATLIST:
		{
			if (lpRecord->cbData < sizeof(float))
				return ERecordValueStringResult::Failed;

			FLOATLIST *lpIntList = (FLOATLIST *)((BYTE *)lpRecord + sizeof(VSRECORD));
			// Revalidate:
			if (lpRecord->cbData < sizeof(int) + sizeof(float) * lpIntList->iValueCount)
				return ERecordValueStringResult::Failed;

			std::wstring text;
			for (int i = 0; i < lpIntList->iValueCount; i++)
			{
				text += lpIntList->flValues[i];
				if (i != lpIntList->iValueCount - 1)
					text += L", ";
			}
			swprintf_s(pszBuffer, cchBufferMax, L"%s", text.c_str());
			return ERecordValueStringResult::Succeeded;
		}

		case Restyle::TMT_POSITION:
		{
			if (lpRecord->cbData != sizeof(POINT))
				break;

			POINT *lpPoint = (POINT *)((BYTE *)lpRecord + sizeof(VSRECORD));
			swprintf_s(pszBuffer, cchBufferMax, L"Value = (XY){ %d, %d }\n", lpPoint->x, lpPoint->y);
			break;
		}

		case Restyle::TMT_COLOR:
		{
			if (lpRecord->cbData != sizeof(COLORREF))
				return ERecordValueStringResult::Failed;

			COLORREF crColor = *(COLORREF *)((BYTE *)lpRecord + sizeof(VSRECORD));
			BYTE bRed = GetRValue(crColor);
			BYTE bGreen = GetGValue(crColor);
			BYTE bBlue = GetBValue(crColor);
			swprintf_s(pszBuffer, cchBufferMax, L"%d %d %d", bRed, bGreen, bBlue);
			return ERecordValueStringResult::Succeeded;
		}

		// The data on fonts and filenames is worthless afaik. The header member
		// uResID contains the resource ID of the string these use.
		case Restyle::TMT_FONT:
		{
			// Contradictory to my statement above, UXTheme
			// fails if data size is less than 0x5C.
			if (!lpRecord->uResID || lpRecord->cbData < 0x5C)
				return ERecordValueStringResult::Failed;

			LoadStringW(g_hThemeModule, lpRecord->uResID, pszBuffer, cchBufferMax);
			return ERecordValueStringResult::Succeeded;
		}

		// FileName = IMAGE resource
		// DiskStream = STREAM resource
		case Restyle::TMT_FILENAME:
		case Restyle::TMT_DISKSTREAM:
		{
			if (!lpRecord->uResID)
				return ERecordValueStringResult::Failed;

			// You can't really print an image into a Win32 console window.
			// There will need to be a special handler for this when decompiling
			// themes!
			wcscpy_s(pszBuffer, cchBufferMax, L"<binary data>");
			return ERecordValueStringResult::Succeeded;
		}

		// Give raw data on unknown type for debugging
		default:
			BYTE *lpData = (BYTE *)lpRecord + sizeof(VSRECORD);
			std::wstring text;
			WCHAR szTemp[3];
			for (int i = 0; i < lpRecord->cbData; i++)
			{
				swprintf_s(szTemp, L"%02X", lpData[i]);
				text += szTemp;
				if (i != (lpRecord->cbData - 1))
					text += L" ";
			}
			swprintf_s(pszBuffer, cchBufferMax, L"%s", text.c_str());
			return ERecordValueStringResult::UnknownType;
	}
}

} // namespace BinParser