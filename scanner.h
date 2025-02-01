#pragma once
#include "restyle.h"

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
    inline bool Next();
    inline WCHAR Read();
    WCHAR ReadNext();
    bool ReadNextLine();
    bool IsNameChar(bool fSkipSpaces);
    bool IsFileNameChar(bool fSkipSpaces);
    bool IsNumStart();
    bool SkipSpaces();
    bool GetChar(const WCHAR sz);
    bool GetNumber(PINT pInt);
    bool GetKeyword(LPCWSTR szKeyword);
    bool EndOfLine();
    bool EndOfFile();
    
    const WCHAR *_p;
};

}