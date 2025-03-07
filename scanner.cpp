#include "scanner.h"
#include "util.h"
#include <stringapiset.h>
#include <strsafe.h>
#include <winnls.h>
#include <string>

namespace IniParser
{

HRESULT CopyString(LPWSTR szDest, DWORD cchDest, LPCWSTR szSrc)
{
    HRESULT hr = E_INVALIDARG;
    
    if (szDest && cchDest > 0)
    {
        if (szSrc)
        {
            hr = StringCchCopyW(szDest, cchDest, szSrc);
        }
        else
        {
            *szDest = 0;
            hr = E_FAIL;
        }
    }
    
    return hr;
}

static int ParseNumberLiteral(LPCWSTR sz, int *iRead = nullptr)
{
    int iBuf = 0;
    int iBase = 10;
    int nNeg = 1;
    LPCWSTR szStart = sz;
    
    if (*sz == L'-')
    {
        nNeg = -1;
        sz++;
    }
    else if (*sz == L'+')
    {
        sz++;
    }
    
    if (*sz == L'0')
    {
        switch (*++sz)
        {
            // We're dealing with 0x prefix:
            case L'X':
            case L'x':
            {
                ++sz;
                iBase = 16;
                break;
            }
        }
    }
    
    while (*sz)
    {
        int iHexIndexLetter = 0; // Used for parsing hex letters
        
        switch (*sz)
        {
            case L'0':
            case L'1':
            case L'2':
            case L'3':
            case L'4':
            case L'5':
            case L'6':
            case L'7':
            case L'8':
            case L'9':
            {
                iBuf = (iBuf * iBase) + (*sz++ - L'0');
                break;
            }
            
            case L'a':
            case L'b':
            case L'c':
            case L'd':
            case L'e':
            case L'f':
                iHexIndexLetter = L'a';
                // [[ fallthrough ]]
            case L'A':
            case L'B':
            case L'C':
            case L'D':
            case L'E':
            case L'F':
            {
                if (iHexIndexLetter == 0)
                {
                    iHexIndexLetter = L'A';
                }
                
                // Stop parsing if we're parsing a base 10 number.
                if (iBase == 10)
                {
                    if (iRead) *iRead = szStart - sz;
                    return nNeg * iBuf;
                }
                
                iBuf = (iBuf * iBase) + (*sz++ - iHexIndexLetter + 10);
                break;
            }
            
            // Any other character is non-numerical, and thus should immediately
            // return.
            default:
            {
                if (iRead) *iRead = szStart - sz;
                return nNeg * iBuf;
            }
        }
    }
    
    if (iRead) *iRead = szStart - sz;
    return nNeg * iBuf;
}

CScanner::CScanner(LPCWSTR szText, DWORD cchText)
    : _p(szText)
    , _pEndOfFile(szText + cchText)
{
}

CScanner::CScanner(std::wstring text)
    : _p(text.c_str())
    , _pEndOfFile(text.c_str() + text.length())
{
}

bool CScanner::IsNextSequenceCommentDelinatingToken()
{
    switch (_eCommentType)
    {
        case ECommentType::Semicolon:
        {
            return *_p == L';';
        }
        
        default: return false;
    }
}

WCHAR CScanner::ReadNext()
{
    WCHAR chRes = *_p;
    Next();
    return chRes;
}

bool CScanner::ReadNextLine()
{
    if (!_pszFile || !*_pszFile)
    {
        _fEndOfFile = true;
        return false;
    }
    
    WCHAR *szRead = _szLineBuff;
    while (*_pszFile && *_pszFile != L'\r' && *_pszFile != L'\n')
    {
        *szRead++ = *_pszFile++;
    }
    
    *szRead = L'\0';
    
    if (*_pszFile == L'\r')
        _pszFile++;
    
    if (*_pszFile == L'\n')
        _pszFile++;
    
    _p = _szLineBuff;
    _fBlankSoFar = true;
    _iLineNum++;
    
    return true;
}

bool CScanner::IsNameChar(bool fSkipSpaces)
{
    if (fSkipSpaces)
    {
        SkipSpaces();
    }
    
    return IsCharAlphaNumericW(*_p)
        || *_p == L'_'
        || *_p == L'-';
}

bool CScanner::IsFileNameChar(bool fSkipSpaces)
{
    if (fSkipSpaces)
    {
        SkipSpaces();
    }
    
    return IsNameChar(false)
        || *_p == L':'
        || *_p == L'\\'
        || *_p == L'.';
}

bool CScanner::IsNumStart()
{
    SkipSpaces();
    
    return IsDigit(*_p) || *_p == '-' || *_p == '+';
}

bool CScanner::SkipSpaces()
{
    while (1)
    {
        while (IsSpace(*_p))
        {
            Next();
        }
        
        if (!*_p || IsNextSequenceCommentDelinatingToken())
        {
            if (_fBlankSoFar && !_fEndOfFile)
            {
                ReadNextLine();
                continue;
            }
            
            if (IsNextSequenceCommentDelinatingToken())
            {
                _p += lstrlenW(_p);
            }
            
            return false;
        }
        
        _fBlankSoFar = false;
        break;
    }
    
    return *_p;
}

bool CScanner::GetChar(const WCHAR sz)
{
    SkipSpaces();
    
    if (*_p != sz)
    {
        return false;
    }
    
    Next();
    return true;
}

bool CScanner::GetNumber(PINT pInt)
{
    SkipSpaces();
    
    if (!IsNumStart())
    {
        return false;
    }
    
    int iRead = 0;
    *pInt = ParseNumberLiteral(_p, &iRead);
    _p += iRead;
    
    while (IsHexDigit(*_p))
    {
        Next();
    }
    
    return true;
}

bool CScanner::GetKeyword(LPCWSTR szKeyword)
{
    bool fRet = false;
    
    SkipSpaces();
    
    if (_p)
    {
        int cch = lstrlenW(szKeyword);
        
        WCHAR *szBuffer = new WCHAR[cch + 1];
        if (szBuffer)
        {
            StringCchCopyW(szBuffer, cch + 1, _p);
            
            if (AsciiStrCmpI(szBuffer, szKeyword) == 0)
            {
                _p += cch;
                fRet = true;
            }
            
            delete[] szBuffer;
        }
    }
    
    return fRet;
}

bool CScanner::EndOfLine()
{
    SkipSpaces();
    return *_p == 0;
}

bool CScanner::EndOfFile()
{
    return _fEndOfFile;
}

// static
bool CScanner::IsSpace(WCHAR c)
{
    WORD wCharType = 0;
    GetStringTypeW(CT_CTYPE1, &c, 1, &wCharType);
    return wCharType & C1_SPACE;
}

// static
bool CScanner::IsHexDigit(WCHAR c)
{
    WORD wCharType = 0;
    GetStringTypeW(CT_CTYPE1, &c, 1, &wCharType);
    return wCharType & C1_XDIGIT;
}

// static
bool CScanner::IsDigit(WCHAR c)
{
    WORD wCharType = 0;
    GetStringTypeW(CT_CTYPE1, &c, 1, &wCharType);
    return wCharType & C1_DIGIT;
}

}