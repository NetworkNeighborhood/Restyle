#pragma once
#include "Restyle.h"
#include "Util.h"
#include "Arena.h"
#include "ArenaArray.h"
#include <iterator>
#include "Symbol.h"

bool IsSymbolTypePredefined(ESymbolType eSymType);
bool IsSymbolTypeManual(ESymbolType eSymType);

/**
 * Stores unique names used within a parsing context, especially class names.
 */
class CNameArena
#define BASECLASS CTArenaArray<CNameArena, const WCHAR, 1024>
    : public BASECLASS
{
    using Super = BASECLASS;
#undef BASECLASS

public:
    struct Iterator : public Super::CTIterator<const WCHAR, LPCWSTR, LPCWSTR &>
    {
        LPCWSTR &operator*() const
        {
            return (LPCWSTR &)_p;
        }

        Iterator &operator++()
        {
            GetNextString();
            return *this;
        }

        Iterator &operator++(int)
        {
            GetNextString();
            return *this;
        }

    private:
        LPCWSTR GetEndOfData() const
        {
            return (LPCWSTR)((size_t)_pArena->_pvData + _pArena->_dwSize);
        }

        LPCWSTR GetNextString()
        {
            do
            {
                while (++_p < GetEndOfData() && *_p != L'\0');
                LPCWSTR szResult = (LPCWSTR)++_p;

                if (_pArena->_pNext)
                {
                    UseNextArena();
                    continue;
                }
                else if (*szResult == '\0')
                {
                    _fIsEnd = true;
                }

                return szResult;
            }
            while (1);
        }

    public:
        // Beginning (real) iterator constructor
        Iterator(CNameArena *nameArena)
            : CTIterator(nameArena)
        {
        }

        // Ending (sentinel) iterator constructor
        Iterator()
            : CTIterator()
        {
        }
    };

    // C++ iterator concept
    Iterator begin()
    {
        return Iterator(this);
    }

    // C++ iterator concept
    Iterator end()
    {
        return Iterator();
    }

    ValueResult<LPCWSTR> Add(LPCWSTR sz);
};

class CSymbolArena : public CTArenaArray<CSymbolArena, Symbol, 512> {};

/**
 * Manages symbols.
 */
class CSymbolManager
{
    CSymbolArena _symbolArena;
    CNameArena _nameArena;

public:
    HRESULT Initialize();
    ValueResult<const Symbol *> AddSymbol(LPCWSTR szSymName, ESymbolType eSymType);
    ValueResult<const Symbol *> AddManualSymbol(int iVal, ESymbolType eSymType, OPTIONAL int iType = 0);
    LPCWSTR GetGlobalSymbolName(LPCWSTR szSymName, OUT OPTIONAL int *piSchemaOffset = nullptr);
    const Symbol *FindSymbolPointer(LPCWSTR szSymName);
    bool HasSymbol(LPCWSTR szSymName);
};