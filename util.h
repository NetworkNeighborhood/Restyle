#pragma once
#include "restyle.h"

enum class ELogLevel
{
	Info = 0,
	Warning = 1,
	Fatal = 2,
};


// Outputs a user-displayed message to stderr. For output that can be written
// to files by the user (e.g. /precord option), use wprintf.
void Log(LPCWSTR pszFormat, ELogLevel eLevel, ...);
void Log(LPCWSTR pszFormat, ...);

int AsciiStrCmpI(const WCHAR *dst, const WCHAR *src);
bool GetBinaryResource(LPCWSTR lpType, LPCWSTR lpName, LPVOID *ppvOut, DWORD *pcbOut);

LPCWSTR GetSymbolValueName(long lSymbolVal);
EParseResult GetPropName(long lPartId, BYTE bPrimType, std::unique_ptr<WCHAR[]> &pszResult);
EParseResult GetPartAndStateName(
	LPCWSTR pszClassName,
	std::unique_ptr<WCHAR[]> &pszPartName,
	std::unique_ptr<WCHAR[]> &pszStateName,
	int iPart,
	int iState
);

/**
 * A result tuple which contains both a user-specified value type and a HRESULT.
 */
template <typename T = int>
struct ValueResult
{
    HRESULT hr = E_FAIL;
    T value{};

    inline FORCEINLINE bool Succeeded() const
    {
        return SUCCEEDED(hr);
    }

    inline FORCEINLINE bool Failed() const
    {
        return FAILED(hr);
    }

    inline FORCEINLINE HRESULT GetResult() const
    {
        return hr;
    }

    inline FORCEINLINE T &Unwrap()
    {
        return value;
    }

    inline FORCEINLINE ValueResult &ResultInto(HRESULT *p)
    {
        *p = hr;
        return *this;
    }

    inline FORCEINLINE operator HRESULT() const
    {
        return hr;
    }

    inline FORCEINLINE operator T() const
    {
        return value;
    }

    inline ValueResult(HRESULT hr)
        : hr(hr)
    {
        assert(FAILED(hr));
    }

    inline ValueResult(T value)
        : hr(S_OK)
        , value(value)
    {
    }

    inline ValueResult(HRESULT hr, T value)
        : hr(hr)
        , value(value)
    {
    }
};

bool StrEndsWithW(LPCWSTR a, LPCWSTR b);