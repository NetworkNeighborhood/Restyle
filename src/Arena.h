#pragma once
#include "Restyle.h"

template <class Self, typename BaseType, int kArrayCount>
class CTBaseArena
{
protected:
    static constexpr size_t kInitialSize = kArrayCount * sizeof(BaseType);
    
    bool _fInitialized = false;
    BYTE *_pvData = nullptr;
    BYTE *_pvCur = nullptr;
    DWORD _dwSize = 0;
    DWORD _dwMaxSize = kInitialSize;
    Self *_pNext = nullptr;
    
    Self *GetEffectiveSelf()
    {
        // We are always ourself, so there's absolutely no case where the type contract is
        // broken. This is simply required because the base class is different, and our
        // this variable is of the base class rather than a child class.
        Self *pSelf = static_cast<Self *>(this);
        while (pSelf->_pNext) pSelf = pSelf->_pNext;
        return pSelf;
    }
    
public:
    ~CTBaseArena()
    {
        CTBaseArena *pCur = this;
        
        // First pass: reverse the pointers in the linked list to point backwards.
        while (pCur->_pNext)
        {
            CTBaseArena *pTemp = pCur->_pNext;
            std::swap(pCur->_pNext, pCur->_pNext->_pNext);
            pCur = pTemp;
        }
        
        // Second pass: free all branches.
        while (pCur)
        {
            if (pCur->_pvData)
            {
                delete pCur->_pvData;
            }
            
            pCur = pCur->_pNext;
        }
    }
    
    HRESULT EnsureInitialized()
    {
        if (!_fInitialized)
        {
            return Initialize();
        }
        
        return S_OK;
    }
    
    HRESULT Initialize()
    {
        if ((_pvData = new BYTE[kInitialSize]))
        {
            ZeroMemory(_pvData, kInitialSize);
            _dwSize = 0;
            _pvCur = _pvData;
            _fInitialized = true;
            return S_OK;
        }
        
        return E_OUTOFMEMORY;
    }
    
    HRESULT Add(BaseType *pData, DWORD cbData)
    {
        assert(_fInitialized);
        Self *pSelf = GetEffectiveSelf();
    
        if (FAILED(pSelf->ResizeIfNecessary(cbData)))
        {
            return E_OUTOFMEMORY;
        }
        
        memcpy(pSelf->_pvCur, pData, cbData);
        pSelf->_dwSize += cbData;

        _pvCur += cbData;
        
        return S_OK;
    }

    HRESULT Push(DWORD cb)
    {
        assert(_fInitialized);
        Self *pSelf = GetEffectiveSelf();

        if (FAILED(pSelf->ResizeIfNecessary(cb)))
        {
            return E_OUTOFMEMORY;
        }

        pSelf->_dwSize += cb;
        _pvCur += cb;
        ZeroMemory(pSelf->_pvCur, cb);

        return S_OK;
    }
    
    HRESULT Pop(DWORD cb)
    {
        assert(_fInitialized);
        Self *pSelf = GetEffectiveSelf();
    
        pSelf->_pCur -= cb;
        ZeroMemory(pSelf->_pvCur, cb);
        
        return S_OK;
    }

    BaseType *GetCurrent()
    {
        return (BaseType *)_pvCur;
    }
    
    HRESULT Resize()
    {
        assert(_fInitialized);
        Self *pCur = GetEffectiveSelf();
    
        pCur->_pNext = new Self();
        
        if (!pCur->_pNext)
        {
            return E_OUTOFMEMORY;
        }
        
        HRESULT hr = pCur->_pNext->Initialize();
        
        if (FAILED(hr))
        {
            return hr;
        }
        
        return S_OK;
    }
    
    HRESULT ResizeIfNecessary(DWORD cbRequested)
    {
        assert(_fInitialized);
        Self *pSelf = GetEffectiveSelf();
    
        if ((size_t)pSelf->_pvCur + cbRequested - (size_t)pSelf->_pvData > _dwMaxSize)
        {
            if (FAILED(pSelf->Resize()))
            {
                // Out of memory:
                return E_OUTOFMEMORY;
            }
        }
        
        return S_OK;
    }
};
