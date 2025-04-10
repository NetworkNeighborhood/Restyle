#pragma once
#include "Arena.h"
#include <iterator>

template <class Self, typename BaseType, int kArrayCount>
class CTArenaArray : public CTBaseArena<Self, BaseType, kArrayCount>
{
protected:
    Self *ResolveLinkedListNodeForArrayOffset(int iArrayOffset)
    {
        // Resolve the arena to use:
        Self *pEffectiveArena = this;
        int idxEffective = iArrayOffset / kArrayCount; // Integer division results in the correct offsets.

        for (int it = idxEffective; it > 0; --it)
        {
            pEffectiveArena = pEffectiveArena->_pNext;
        }

        return pEffectiveArena;
    }

public:
    // Base iterator class:
    template <typename TValue = const BaseType, typename TPointer = const BaseType *, typename TReference = const BaseType &>
    class CTIterator
    {
    public:
        using iterator_category = std::forward_iterator_tag;
        using difference_type = std::ptrdiff_t;
        using value_type = TValue;
        using pointer = TPointer;
        using reference = TReference;

        // Beginning (real) iterator constructor
        CTIterator(Self *pArena)
            : _pArena(pArena)
            , _p((pointer)pArena->_pvData)
        {
        }

        // Ending (sentinel) iterator constructor
        CTIterator()
            : _pArena(nullptr)
            , _p(nullptr)
        {
        }

        reference operator*() const
        {
            return (reference)*_p;
        }

        pointer operator->() const
        {
            return _p;
        }

        CTIterator &operator++()
        {
            UseNextArenaIfNecessary();
            _p += 1;
            return *this;
        }

        CTIterator &operator++(int)
        {
            UseNextArenaIfNecessary();
            _p -= 1;
            return *this;
        }

        friend bool operator==(const CTIterator &a, const CTIterator &b)
        {
            // The end iterator is always a sentinel object, with its pointer and arena
            // pointers both being nullptr.
            assert(a._p == nullptr || b._p == nullptr);

            const CTIterator *pRealIterator = a._p != nullptr ? &a : &b;

            pointer pEndOfData = pRealIterator->GetEndOfData();

            if (pRealIterator->_p >= pEndOfData || pRealIterator->_fIsEnd)
            {
                // i == iterator.end()
                return true;
            }

            return false;
        }

        friend bool operator!=(const CTIterator &a, const CTIterator &b)
        {
            return !(operator==(a, b));
        }

        void UseNextArenaIfNecessary(int iProjectAddition = 0)
        {
            if (!_pArena)
                return;

            if (_p + iProjectAddition > GetEndOfData() && _pArena->_pNext)
            {
                UseNextArena();
            }
        }

        void UseNextArena()
        {
            _pArena = _pArena->_pNext;
            _p = (pointer)_pArena->_pvData;
        }

        pointer GetEndOfData() const
        {
            if (!_pArena)
                return nullptr;

            return (pointer)((size_t)_pArena->_pvData + _pArena->_dwSize);
        }

        Self *_pArena = nullptr;
        bool _fIsEnd = false;
        pointer _p;
    };

    // Default iterator implementation.
    struct Iterator : public CTIterator<>
    {
        // Beginning (real) iterator constructor
        inline Iterator(Self *pArena) : CTIterator<>(pArena) {}

        // Ending (sentinel) iterator constructor
        inline Iterator() : CTIterator<>() {}
    };

    // C++ iterator concept
    Iterator begin() const
    {
        return Iterator((Self *)this);
    }

    // C++ iterator concept
    Iterator end() const
    {
        return Iterator();
    }

    BaseType operator[](int i) const
    {
        // Resolve the arena to use:
        Self *pEffectiveArena = ResolveLinkedListNodeForArrayOffset(i);
        return pEffectiveArena->_pvCur + (i * sizeof(BaseType));
    }

    BaseType &operator[](int i)
    {
        // Resolve the arena to use:
        Self *pEffectiveArena = ResolveLinkedListNodeForArrayOffset(i);
        return pEffectiveArena->_pvCur + (i * sizeof(BaseType));
    }
};