#include "iniparser.h"
#include "util.h"
#include "file.h"
#include "scanner.h"
#include <stringapiset.h>
#include <strsafe.h>
#include <winnls.h>

namespace IniParser
{

LPWSTR g_szIniText = nullptr;

class CIniParser
{
    CScanner _scanner;
    
public:
    CIniParser(LPCWSTR szText, DWORD cchText);
    bool ReadName(IN OUT LPWSTR szId, DWORD cchId);
    
    HRESULT Test();
};

CIniParser::CIniParser(LPCWSTR szText, DWORD cchText)
    : _scanner(szText, cchText)
{
}

// Expects test file:
#if 0
[Fawk]
Hello = 1

[aaaa::bbbb.fuck(aaaaaa) : hello]
aaaaaaaaa = sijdisjfid

[animation.Fawk(dd)]
Transforms = {
    Translate2D {
        Duration = 20
    }
}
#endif
HRESULT CIniParser::Test()
{
    if (!_scanner.GetChar(L'['))
    {
        Log(L"First character bad. Parser probably getting bad input u_u. Check debugger.", ELogLevel::Fatal);
        return E_FAIL;
    }
    
    if (!_scanner.GetKeyword(L"Fawk"))
    {
        Log(L"Expected \"Fawk\"", ELogLevel::Fatal);
        return E_FAIL;
    }
    
    if (!_scanner.GetChar(L']'))
    {
        Log(L"Expected \"]\"", ELogLevel::Fatal);
        return E_FAIL;
    }
    
    if (!_scanner.GetKeyword(L"Hello"))
    {
        Log(L"Expected \"Hello\"", ELogLevel::Fatal);
        return E_FAIL;
    }
    
    if (!_scanner.GetChar(L'='))
    {
        Log(L"Expected \"=\"", ELogLevel::Fatal);
        return E_FAIL;
    }
    
    int iNumber = 0;
    if (!_scanner.GetNumber(&iNumber) || iNumber != 1)
    {
        Log(L"Expected number 1, got %d", iNumber, ELogLevel::Fatal);
        return E_FAIL;
    }
    
    Log(L"Succeeded INI parsing test with no problems.", ELogLevel::Info);
    return S_OK;
}

bool CIniParser::ReadName(IN OUT LPWSTR szId, DWORD cchId)
{
    if (cchId == 0)
    {
        return false;
    }
    
    _scanner.SkipSpaces();
    
    WCHAR *szRead = szId;
    
    while (_scanner.IsNameChar(false) && --cchId)
    {
        *szRead++ = _scanner.ReadNext();
    }
    *szRead = 0; // Add null terminator
    
    if (szRead == szId)
    {
        // If we hit this case, then we didn't end up reading anything.
        return false;
    }
    
    if (_scanner.IsNameChar(false))
    {
        // Buffer too small
        return false;
    }
    
    return true;
}


HRESULT ParseIniFile(LPCWSTR szPath)
{
    CSimpleFile file;
    HRESULT hr = file.Open(szPath);

    if (FAILED(hr))
    {
        Log(L"FATAL: Failed to open file.", ELogLevel::Fatal);
        return 0;
    }

    DWORD dwFileSizeHigh = 0;
    DWORD dwFileSize = file.GetFileSize(&dwFileSizeHigh);

    if (dwFileSize == 0)
    {
        Log(L"FATAL: Failed to get file size.", ELogLevel::Fatal);
        return 0;
    }

    LPWSTR szString = new WCHAR[dwFileSize + sizeof('\n')];

    DWORD cbRead = 0;
    hr = file.Read((void *)szString, dwFileSize * sizeof(WCHAR) + sizeof(L'\n'), &cbRead);

    if (FAILED(hr))
    {
        delete[] szString;
        Log(L"FATAL: Failed to read file.", ELogLevel::Fatal);
        return 0;
    }
    
    CIniParser iniParser(szString, dwFileSize + sizeof('\n'));
    iniParser.Test();
    
    // TODO: Add INI parsing test code here.

    if (szString)
    {
        delete[] szString;
    }
    
    return S_OK;
}

}