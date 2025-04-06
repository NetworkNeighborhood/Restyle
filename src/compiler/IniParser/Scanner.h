#pragma once
#include "Restyle.h"
#include <string>

namespace IniParser
{
    
HRESULT CopyString(LPWSTR szDest, DWORD cchDest, LPCWSTR szSrc);

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
    CScanner(std::wstring text);

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
    bool GetChar(const WCHAR sz);
    bool GetNumber(PINT pInt);
    bool GetFloatNumber(float *pFloat);
    bool GetKeyword(LPCWSTR szKeyword);
    bool EndOfLine();
    bool EndOfFile();

    static bool IsSpace(WCHAR c);
    static bool IsHexDigit(WCHAR c);
    static bool IsDigit(WCHAR c);
    
    const WCHAR *_p;
};

}