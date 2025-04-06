#include "ValueArena.h"

#include "iniparserp.h"

using namespace Restyle;

// Precached values:
const BoolValue c_valBoolFalse = { TMT_BOOL, sizeof(BOOL), FALSE };
const BoolValue c_valBoolTrue = { TMT_BOOL, sizeof(BOOL), TRUE };

#define CREATE_T_VALUE(TmType, specializedName) CreateTValue<Value<TmType>, &Value<TmType>::specializedName>
#define CREATE_T_LIST_VALUE(TmType, specializedName) CreateTListValue<Value<TmType>, &Value<TmType>::specializedName>

ValueResult<const IntValue *> CValueArena::CreateIntValue(int iVal)
{
    return CREATE_T_VALUE(TMT_INT, iVal)(iVal);
}

ValueResult<const EnumValue *> CValueArena::CreateEnumValue(int iVal)
{
    return CREATE_T_VALUE(TMT_ENUM, iVal)(iVal);
}

ValueResult<const FloatValue *> CValueArena::CreateFloatValue(float flVal)
{
    return CREATE_T_VALUE(TMT_FLOAT, flVal)(flVal);
}

ValueResult<const SizeValue *> CValueArena::CreateSizeValue(int iVal)
{
    return CREATE_T_VALUE(TMT_SIZE, iVal)(iVal);
}

ValueResult<const BoolValue *> CValueArena::CreateBoolValue(BOOL fVal)
{
    return fVal ? &c_valBoolTrue : &c_valBoolFalse;
}

ValueResult<const RectValue *> CValueArena::CreateRectValue(RECT rcVal)
{
    return CREATE_T_VALUE(TMT_RECT, rcVal)(rcVal);
}

ValueResult<const MarginsValue *> CValueArena::CreateMarginsValue(MARGINS marVal)
{
    return CREATE_T_VALUE(TMT_MARGINS, marVal)(marVal);
}

ValueResult<const ColorValue *> CValueArena::CreateColorValue(COLORREF crVal)
{
    return CREATE_T_VALUE(TMT_COLOR, crVal)(crVal);
}

ValueResult<const PositionValue *> CValueArena::CreatePositionValue(POINT ptVal)
{
    return CREATE_T_VALUE(TMT_POSITION, ptVal)(ptVal);
}

ValueResult<const IntListValue *> CValueArena::CreateIntListValue(int *rgiVal, UINT cVals)
{
    return CREATE_T_LIST_VALUE(TMT_INTLIST, rgiVal)(rgiVal, cVals);
}

ValueResult<const FloatListValue *> CValueArena::CreateFloatListValue(float *rgflVal, UINT cVals)
{
    return CREATE_T_LIST_VALUE(TMT_FLOATLIST, rgflVal)(rgflVal, cVals);
}

ValueResult<const StringValue *> CValueArena::CreateStringValue(LPCWSTR szVal)
{
    CEnsureArenaPointerChanged ensurePointerChanged(this);

    size_t cch = wcslen(szVal);
    size_t targetSize = cch * sizeof(WCHAR) + sizeof(L'\0');

    HRESULT hr = ResizeIfNecessary(sizeof(StringValue) + targetSize);
    if (FAILED(hr))
    {
        return hr;
    }

    StringValue *pResult = new (_pvCur) StringValue();
    pResult->cbSize = targetSize;
    if (!memcpy((void *)&pResult->szVal[0], szVal, targetSize))
    {
        return E_OUTOFMEMORY;
    }

    _pvCur += sizeof(Value<>) + pResult->cbSize;
    return pResult;
}

ValueResult<const AnimationSetValue *> CValueArena::CreateAnimationSetValue(std::vector<IniAssociation> rgAssociations)
{
    CEnsureArenaPointerChanged ensurePointerChanged(this);

    size_t targetSize = sizeof(IniAssociation) + rgAssociations.size();

    HRESULT hr = ResizeIfNecessary(sizeof(AnimationSetValue) + targetSize);
    if (FAILED(hr))
    {
        return hr;
    }

    AnimationSetValue *pResult = new (_pvCur) AnimationSetValue();
    pResult->cbSize = targetSize;
    if (!memcpy(pResult->rgAssociations, rgAssociations.data(), targetSize))
    {
        return E_OUTOFMEMORY;
    }

    _pvCur += sizeof(Value<>) + pResult->cbSize;
    return pResult;
}