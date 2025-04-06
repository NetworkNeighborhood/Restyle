#pragma once
#include "restyle.h"

namespace IniParser
{

enum EParseErrorCode
{
    ExpectedCharacter,
    ExpectedSymbol,
    ExpectedNumber,
    UnexpectedSpace,
    UnexpectedSymbol,
    UnexpectedSequence,
    InvalidSymbol,
    TypeDoesNotExist,
    ItemCountMismatch,
};

struct ParseError
{
    EParseErrorCode eCode;
    std::wstring strMessage;
    int iLine;
};

DECLARE_INTERFACE(IIniParser)
{
    virtual HRESULT Parse() PURE;
    virtual ParseError GetParseError() PURE;
};

// C++ unique-pointer-based interfaces:
std::unique_ptr<IIniParser> CreateUniqueIniParser(LPCWSTR szText, DWORD cchText);
std::unique_ptr<IIniParser> CreateUniqueIniParser(std::wstring text);

// C raw-pointer-based interfaces:
IIniParser *CreateIniParser(LPCWSTR szText, DWORD cchText);
IIniParser *CreateIniParser(std::wstring text);
HRESULT DestroyIniParser(IIniParser *pParser);

HRESULT ParseIniFile(LPCWSTR szPath);

}