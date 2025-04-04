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

HRESULT ParseIniFile(LPCWSTR szPath);

}