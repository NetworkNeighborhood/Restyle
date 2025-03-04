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
};

struct ParseError
{
    EParseErrorCode eCode;
    std::wstring strMessage;
    int iLine;
};

HRESULT ParseIniFile(LPCWSTR szPath);

}