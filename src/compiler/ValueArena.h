#pragma once
#include "Restyle.h"

#include "Arena.h"
#include "Value.h"

/**
 * Stores unique names used within a parsing context, especially class names.
 */
class CValueArena : public CTBaseArena<CValueArena, BYTE[kLargestValueTypeSize], 256>
{
    /**
     * This class is used as an RAII wrapper to ensure that the methods of this
     * function actually update the offset.
     *
     * In release builds, this class will be erased by the optimising compiler.
     */
    class CEnsureArenaPointerChanged
    {
    public:
        #if DEBUG
        CValueArena *_pParent;
        BYTE *_pvOriginal;

        inline [[nodiscard]] CEnsureArenaPointerChanged(CValueArena *pParent)
            : _pParent(pParent)
            , _pvOriginal(pParent->_pvCur)
        {
        }

        inline ~CEnsureArenaPointerChanged()
        {
            assert(_pParent->_pvCur != _pvOriginal);
        }
        #else
        inline FORCEINLINE CEnsureArenaPointerChanged(CValueArena *pParent)
        {
        }
        #endif
    };

    template <typename TValue, auto TValue:: *pValue>
    ValueResult<const TValue *> CreateTValue(auto nVal)
    {
        CEnsureArenaPointerChanged ensurePointerChanged(this);

        HRESULT hr = ResizeIfNecessary(sizeof(TValue));
        if (FAILED(hr))
        {
            return hr;
        }

        TValue *pResult = new (_pvCur) TValue();
        pResult->*pValue = nVal;
        _pvCur += sizeof(Value<>) + pResult->cbSize;
        return pResult;
    }

    template <typename TValue, auto TValue :: *pValue>
    ValueResult<const TValue *> CreateTListValue(auto rgnVal, UINT cVals)
    {
        CEnsureArenaPointerChanged ensurePointerChanged(this);

        size_t cb = sizeof(int) * cVals;

        HRESULT hr = ResizeIfNecessary(sizeof(TValue) + cb);
        if (FAILED(hr))
        {
            return hr;
        }

        TValue *pResult = new (_pvCur) TValue();
        pResult->cbSize = cb;
        if (!memcpy((void *)&(pResult->*pValue)[0], rgnVal, cb))
        {
            return E_OUTOFMEMORY;
        }

        _pvCur += sizeof(Value<>) + pResult->cbSize;
        return pResult;
    }

public:
    ValueResult<const IntValue *> CreateIntValue(int iVal);
    ValueResult<const EnumValue *> CreateEnumValue(int iVal);
    ValueResult<const FloatValue *> CreateFloatValue(float flVal);
    ValueResult<const SizeValue *> CreateSizeValue(int iVal);
    ValueResult<const BoolValue *> CreateBoolValue(BOOL fVal);
    ValueResult<const RectValue *> CreateRectValue(RECT rcVal);
    ValueResult<const MarginsValue *> CreateMarginsValue(MARGINS marVal);
    ValueResult<const PositionValue *> CreatePositionValue(POINT ptVal);
    ValueResult<const ColorValue *> CreateColorValue(COLORREF crVal);
    ValueResult<const IntListValue *> CreateIntListValue(int *rgiVal, UINT cVals);
    ValueResult<const FloatListValue *> CreateFloatListValue(float *rgflVal, UINT cVals);
    ValueResult<const StringValue *> CreateStringValue(LPCWSTR szVal);
    ValueResult<const AnimationSetValue *> CreateAnimationSetValue(std::vector<IniAssociation> rgAssociations);
};