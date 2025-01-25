#include "iniparser.h"
#include <stringapiset.h>
#include <winnls.h>

namespace IniParser
{

LPWSTR g_szIniText = nullptr;

inline bool IsSpace(WCHAR c)
{
    WORD wCharType = 0;
    GetStringTypeW(CT_CTYPE1, &c, 1, &wCharType);
    return wCharType & C1_SPACE;
}

class CScanner
{
    bool _fBlankSoFar = false;
    
    bool SkipSpaces();
};

HRESULT ReadFile(LPCWSTR szPath)
{
    
    
    return S_OK;
}

}