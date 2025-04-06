#pragma once
#include "Restyle.h"
#include "Util.h"
#include "Arena.h"
#include <iterator>
#include "Symbol.h"

bool IsSymbolTypePredefined(ESymbolType eSymType);
bool IsSymbolTypeManual(ESymbolType eSymType);

/**
 * Stores unique names used within a parsing context, especially class names.
 */
class CNameArena
#define BASECLASS CTBaseArena<CNameArena, const WCHAR, 1024>
    : public BASECLASS
{
    using Super = BASECLASS;
#undef BASECLASS

public:
    struct Iterator
    {
        using iterator_category = std::forward_iterator_tag;
        using difference_type = std::ptrdiff_t;
        using value_type = LPCWSTR;
        using pointer = value_type;
        using reference = const value_type &;

        // Beginning (real) iterator constructor
        Iterator(CNameArena *nameArena)
            : _pNameArena(nameArena)
            , _p((pointer)nameArena->_pvData)
        {
        }

        // Ending (sentinel) iterator constructor
        Iterator()
            : _pNameArena(nullptr)
            , _p(nullptr)
        {
        }

        reference operator*() const
        {
            return _p;
        }

        pointer operator->() const
        {
            return _p;
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

        friend bool operator==(const Iterator &a, const Iterator &b)
        {
            // The end iterator is always a sentinel object, with its pointer and CNameArena
            // pointers both being nullptr.
            assert(a._p == nullptr || b._p == nullptr);

            const Iterator *pRealIterator = a._p != nullptr ? &a : &b;

            LPCWSTR pEndOfData = pRealIterator->GetEndOfData();

            if (pRealIterator->_p > pEndOfData || pRealIterator->_fIsEnd)
            {
                // i == iterator.end()
                return true;
            }

            return false;
        }

        friend bool operator!=(const Iterator &a, const Iterator &b)
        {
            return !(operator==(a, b));
        }

    private:
        void UseNextArena()
        {
            _pNameArena = _pNameArena->_pNext;
            _p = (pointer)_pNameArena->_pvData;
        }

        pointer GetEndOfData() const
        {
            return (LPCWSTR)((size_t)_pNameArena->_pvData + _pNameArena->_dwSize);
        }

        pointer GetNextString()
        {
            do
            {
                while (++_p < GetEndOfData() && *_p != L'\0');
                LPCWSTR szResult = ++_p;

                if (_pNameArena->_pNext)
                {
                    UseNextArena();
                    continue;
                }
                else if (*szResult == '\0')
                {
                    _fIsEnd = true;
                }

                return szResult;
            } while (1);
        }

        CNameArena *_pNameArena;
        bool _fIsEnd = false;
        pointer _p;
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

/**
 * Manages symbols.
 */
class CSymbolManager
{
    std::vector<Symbol> _rgSymbols;
    CNameArena _nameArena;

public:
    ValueResult<Symbol *> AddSymbol(LPCWSTR szSymName, ESymbolType eSymType);
    ValueResult<Symbol *> AddManualSymbol(int iVal, ESymbolType eSymType, OPTIONAL int iType = 0);
    LPCWSTR GetGlobalSymbolName(LPCWSTR szSymName, OUT OPTIONAL int *piSchemaOffset = nullptr);
    Symbol *FindSymbolPointer(LPCWSTR szSymName);
    bool HasSymbol(LPCWSTR szSymName);
};