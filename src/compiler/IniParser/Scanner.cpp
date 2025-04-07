#include "Scanner.h"
#include "Util.h"
#include <stringapiset.h>
#include <strsafe.h>
#include <winnls.h>
#include <string>
#include <locale.h>
#include <concepts>

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

template <typename NumberType, bool IsUnsigned = std::is_unsigned_v<NumberType>>
    requires std::integral<NumberType>
static NumberType ParseIntegerNumberLiteral(LPCWSTR sz, int *piRead = nullptr)
{
    NumberType iBuf = 0;
    NumberType iBase = 10;
    NumberType nNeg = 1;
    LPCWSTR szStart = sz;
    
    if (*sz == L'-')
    {
        if (IsUnsigned)
        {
            // Report error?
        }
        else
        {
            nNeg = -1;
        }

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
                    if (piRead) *piRead = sz - szStart;
                    return nNeg * iBuf;
                }
                
                iBuf = (iBuf * iBase) + (*sz++ - iHexIndexLetter + 10);
                break;
            }
            
            // Any other character is non-numerical, and thus should immediately
            // return.
            default:
            {
                if (piRead) *piRead = sz - szStart;
                return nNeg * iBuf;
            }
        }
    }
    
    if (piRead) *piRead = sz - szStart;
    return nNeg * iBuf;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////\
//
// Floating point number parsing.
// 
// We use the C standard library functions "_(str|wcs)to(f|d)_l" for this. The locale-independent "_l"
// variants are used in order to ensure that our result parses correctly regardless of the user's
// operating system language.
//

template <typename FloatType>
constexpr void *FloatParserForType;

template <> constexpr decltype(&_wcstof_l) FloatParserForType<float> = _wcstof_l;
template <> constexpr decltype(&_wcstod_l) FloatParserForType<double> = _wcstod_l;

template <typename FloatType = float, auto pfnParse = FloatParserForType<FloatType>>
static FloatType ParseFloatingPointNumberLiteral(LPCWSTR sz, int *piRead = nullptr)
{
    int nNeg = 1;

    _locale_t hLocale = _wcreate_locale(LC_CTYPE | LC_NUMERIC, L"C");

    LPCWSTR pszEnd = sz;
    auto flRes = pfnParse(sz, (LPWSTR *)&pszEnd, hLocale);

    _free_locale(hLocale);

    *piRead = pszEnd - sz;

    return nNeg * flRes;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

CScanner::CScanner(LPCWSTR szText, DWORD cchText)
    : _pszFile(szText)
    , _pEndOfFile(szText + cchText)
{
    ReadNextLine();
}

CScanner::CScanner(std::wstring &text)
    : _pszFile(text.c_str())
    , _pEndOfFile(text.c_str() + text.length())
{
    ReadNextLine();
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
    _pLine = _szLineBuff;
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

bool CScanner::SkipSpaces(bool fProgressLine)
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
                if (!fProgressLine)
                {
                    break;
                }
            }
            
            if (!_fEndOfFile && fProgressLine)
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
    *pInt = ParseIntegerNumberLiteral<int>(_p, &iRead);
    _p += iRead;
    
    while (IsHexDigit(*_p))
    {
        Next();
    }
    
    return true;
}

bool CScanner::GetFloatNumber(float *pFloat)
{
    SkipSpaces();

    if (!IsNumStart())
    {
        return false;
    }

    int iRead = 0;
    *pFloat = ParseFloatingPointNumberLiteral(_p, &iRead);
    _p += iRead;

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