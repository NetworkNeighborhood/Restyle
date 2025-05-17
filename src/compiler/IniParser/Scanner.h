#pragma once
#include "Restyle.h"
#include <string>
#include <concepts>

namespace IniParser
{
    
HRESULT CopyString(LPWSTR szDest, DWORD cchDest, LPCWSTR szSrc);

template <typename NumberType, bool IsUnsigned = std::is_unsigned_v<NumberType>>
    requires std::integral<NumberType>
inline NumberType ParseIntegerNumberLiteral(LPCWSTR sz, int *piRead = nullptr)
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

class CScanner
{
public:
    enum class ECommentType
    {
        None = 0,
        Semicolon,
    };
    
private:
    bool IsNextSequenceCommentDelinatingToken();
    
    bool _fBlankSoFar = true;
    bool _fEndOfFile = false;
    ECommentType _eCommentType = ECommentType::Semicolon;
    const WCHAR *_pEndOfFile = nullptr;
    const WCHAR *_pszFile = nullptr;
    WCHAR _szLineBuff[512 + 1];
    UINT _iLineNum = 0;

public:
    CScanner(LPCWSTR szText, DWORD cchText);
    CScanner(std::wstring &text);

    inline bool Next()
    {
        if (_p <= _pEndOfFile)
        {
            _p++;
            return true;
        }

        return false;
    }

    inline WCHAR Read() const
    {
        return *_p;
    }

    inline int GetLineNum() const
    {
        return _iLineNum;
    }

    WCHAR ReadNext();
    bool ReadNextLine();
    bool IsNameChar(bool fSkipSpaces);
    bool IsFileNameChar(bool fSkipSpaces);
    bool IsNumStart();
    bool SkipSpaces(bool fProgressLine = true);
    bool GetChar(const WCHAR sz, bool fSkipSpacesToNextLine = true);
    bool GetNumber(PINT pInt, bool fSkipSpacesToNextLine = true);
    bool GetFloatNumber(float *pFloat, bool fSkipSpacesToNextLine = true);
    bool GetKeyword(LPCWSTR szKeyword);
    bool EndOfLine();
    bool EndOfFile();

    static bool IsSpace(WCHAR c);
    static bool IsHexDigit(WCHAR c);
    static bool IsDigit(WCHAR c);
    
    const WCHAR *_p = nullptr;
    const WCHAR *_pLine = nullptr;
};

}