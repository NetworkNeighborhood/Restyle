#pragma once
#include "restyle.h"

template <class Self, typename BaseType, int kArrayCount>
class CTBaseArena
{
protected:
    static constexpr size_t kInitialSize = kArrayCount * sizeof(BaseType);
    
    bool _fInitialized = false;
    BYTE *_pvData = nullptr;
    BYTE *_pvCur = nullptr;
    DWORD _dwSize = 0;
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
            _dwSize = kInitialSize;
            _pvCur = _pvData;
            _fInitialized = true;
            return S_OK;
        }
        
        return E_OUTOFMEMORY;
    }
    
    HRESULT Add(BaseType *pData, DWORD cbData)
    {
        Self *pSelf = GetEffectiveSelf();
    
        if (FAILED(pSelf->ResizeIfNecessary(cbData)))
        {
            return E_OUTOFMEMORY;
        }
        
        memcpy(pSelf->_pvCur, pData, cbData);
        pSelf->_dwSize += cbData;
        
        return S_OK;
    }
    
    HRESULT Pop(DWORD cb)
    {
        Self *pSelf = GetEffectiveSelf();
    
        pSelf->_pCur -= cb;
        ZeroMemory(pSelf->_pCur, cb);
        
        return S_OK;
    }
    
    HRESULT Resize()
    {
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
        Self *pSelf = GetEffectiveSelf();
    
        if ((size_t)pSelf->_pvCur + cbRequested - (size_t)pSelf->_pvData > _dwSize)
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
