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
};

struct ParseError
{
    EParseErrorCode eCode;
    std::wstring strMessage;
    int iLine;
};

HRESULT ParseIniFile(LPCWSTR szPath);

}