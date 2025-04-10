#include "SymbolManager.h"
#include "Arena.h"
#include "schema/SchemaUtils.h"

bool IsSymbolTypePredefined(ESymbolType eSymType)
{
    return eSymType >= ESymbolType::FirstPredefined && eSymType <= ESymbolType::LastPredefined;
}

bool IsSymbolTypeManual(ESymbolType eSymType)
{
    return eSymType >= ESymbolType::FirstManual && eSymType <= ESymbolType::LastManual;
}

ValueResult<LPCWSTR> CNameArena::Add(LPCWSTR sz)
{
    size_t cbsz = (wcslen(sz) + sizeof('\0')) * sizeof(WCHAR);

    for (LPCWSTR &szExisting : *this)
    {
        if (AsciiStrCmpI(szExisting, sz) == 0)
        {
            // Avoid inserting duplicate items:
            return szExisting;
        }
    }

    LPCWSTR pszResult = (LPCWSTR)_pvCur;

    HRESULT hr = Super::Add(sz, cbsz);
    if (FAILED(hr))
    {
        return hr;
    }

    return pszResult;
}

HRESULT CSymbolManager::Initialize()
{
    HRESULT hr = _nameArena.EnsureInitialized();
    hr = _symbolArena.EnsureInitialized();
    return S_OK;
}

ValueResult<const Symbol *> CSymbolManager::AddSymbol(LPCWSTR szSymName, ESymbolType eSymType)
{
    assert(IsSymbolTypePredefined(eSymType));

    if (const Symbol *p = FindSymbolPointer(szSymName))
    {
        return p;
    }

    int iSchemaOffset = -1;

    // szSymName can be nullptr in precisely one single case: the special null base class
    // symbol. In order to prevent an attempted read-to-zero in the name arena which will
    // cause a crash, do not attempt to look up this name at all. We will just add it.
    LPCWSTR szSafeSymName = nullptr;
    if (szSymName)
    {
        szSafeSymName = GetGlobalSymbolName(szSymName, &iSchemaOffset);

        if (!szSafeSymName)
        {
            // Probably out of memory...
            return E_FAIL;
        }
    }

    _symbolArena.EnsureInitialized();
    Symbol *pSym = _symbolArena.GetCurrent();

    pSym->szName = szSafeSymName;
    pSym->eSymType = eSymType;
    pSym->iSchemaOffset = iSchemaOffset;

    _symbolArena.Push(sizeof(Symbol));
    return pSym;
}

ValueResult<const Symbol *> CSymbolManager::AddManualSymbol(int iVal, ESymbolType eSymType, OPTIONAL int iType)
{
    assert(IsSymbolTypeManual(eSymType));

    // Try to find a duplicate symbol in the array to optimise memory usage:
    for (const Symbol &s : _symbolArena)
    {
        if (s.eSymType == eSymType && s.iName == iVal)
        {
            return &s;
        }
    }

    _symbolArena.EnsureInitialized();
    Symbol *pSym = _symbolArena.GetCurrent();

    pSym->iName = iVal;
    pSym->eSymType = eSymType;
    pSym->iPrimType = iType;

    _symbolArena.Push(sizeof(Symbol));
    return pSym;
}

LPCWSTR CSymbolManager::GetGlobalSymbolName(LPCWSTR szSymName, OUT OPTIONAL int *piSchemaOffset)
{
    const Restyle::TMPROPINFO *pPropInfo = Restyle::SearchSchema(Restyle::ESchemaSearchQuery::SearchWholeSchema, szSymName);
    LPCWSTR pszResult = nullptr;

    // This is a unique name which only presents itself inside this INI file. This case
    // includes all of the class names. In this case, we'll copy the names over to our 
    // name arena so that we can preserve them even after the INI file is freed from memory.
    if (!pPropInfo)
    {
        _nameArena.EnsureInitialized();
        pszResult = _nameArena.Add(szSymName);
    }
    else
    {
        pszResult = pPropInfo->pszName;
        *piSchemaOffset = Restyle::GetPropInfoIndex(pPropInfo);
    }

    return pszResult;
}

const Symbol *CSymbolManager::FindSymbolPointer(LPCWSTR szSymName)
{
    for (const Symbol &s : _symbolArena)
    {
        if (AsciiStrCmpI(s.szName, szSymName) == 0)
        {
            return &s;
        }
    }

    return nullptr;
}

bool CSymbolManager::HasSymbol(LPCWSTR szSymName)
{
    return FindSymbolPointer(szSymName) != nullptr;
}