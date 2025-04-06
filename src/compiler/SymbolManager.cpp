#include "SymbolManager.h"
#include "arena.h"
#include "SchemaUtils.h"

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

    for (const LPCWSTR &szExisting : *this)
    {
        if (AsciiStrCmpI(szExisting, sz) == 0)
        {
            // Avoid inserting duplicate items:
            return szExisting;
        }
    }

    HRESULT hr = Super::Add(sz, cbsz);
    if (FAILED(hr))
    {
        return hr;
    }

    return (LPCWSTR)_pvCur;
}

ValueResult<Symbol *> CSymbolManager::AddSymbol(LPCWSTR szSymName, ESymbolType eSymType)
{
    assert(IsSymbolTypePredefined(eSymType));

    if (Symbol *p = FindSymbolPointer(szSymName))
    {
        return p;
    }

    int iSchemaOffset = -1;
    LPCWSTR szSafeSymName = GetGlobalSymbolName(szSymName, &iSchemaOffset);

    if (!szSafeSymName)
    {
        // Probably out of memory...
        return E_FAIL;
    }

    Symbol sym{};

    sym.szName = szSafeSymName;
    sym.eSymType = eSymType;
    sym.iSchemaOffset = iSchemaOffset;

    _rgSymbols.push_back(sym);
    return &_rgSymbols[_rgSymbols.size() - 1];
}

ValueResult<Symbol *> CSymbolManager::AddManualSymbol(int iVal, ESymbolType eSymType, OPTIONAL int iType)
{
    assert(IsSymbolTypeManual(eSymType));

    // Try to find a duplicate symbol in the array to optimise memory usage:
    for (Symbol &s : _rgSymbols)
    {
        if (s.eSymType == eSymType && s.iName == iVal)
        {
            return &s;
        }
    }

    Symbol sym{};

    sym.iName = iVal;
    sym.eSymType = eSymType;
    sym.iPrimType = iType;

    _rgSymbols.push_back(sym);
    return &_rgSymbols[_rgSymbols.size() - 1];
}

LPCWSTR CSymbolManager::GetGlobalSymbolName(LPCWSTR szSymName, OUT OPTIONAL int *piSchemaOffset)
{
    LPCWSTR pszResult = nullptr;

    const Restyle::TMPROPINFO *pPropInfo = Restyle::SearchSchema(Restyle::ESchemaSearchQuery::SearchWholeSchema, szSymName);

    // This is a unique name which only presents itself inside this INI file. This case
    // includes all of the class names. In this case, we'll copy the names over to our 
    // name arena so that we can preserve them even after the INI file is freed from memory.
    if (!pszResult)
    {
        _nameArena.EnsureInitialized();
        pszResult = _nameArena.Add(szSymName);
    }

    return pszResult;
}

Symbol *CSymbolManager::FindSymbolPointer(LPCWSTR szSymName)
{
    for (Symbol &s : _rgSymbols)
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