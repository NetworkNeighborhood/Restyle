#include "IniParserP.h"

#if ENABLE_INIPARSER_TEST

namespace IniParser
{

// Expects test file:
#if 0
[Fawk]
Hello = 1

[aaaa::bbbb.fuck(aaaaaa):hello]
aaaaaaaaa = sijdisjfid

[animation.Fawk(dd)]
Transforms = {
    Translate2D {
        Duration = 20
    }
}
#endif
HRESULT CIniParser::TestScanner()
{
    //
    // Scanner testing
    //

    if (!_scanner.GetChar(L'['))
    {
        Log(L"First character bad. Parser probably getting bad input u_u. Check debugger.\n", ELogLevel::Fatal);
        return E_FAIL;
    }

    if (!_scanner.GetKeyword(L"Fawk"))
    {
        Log(L"Expected \"Fawk\"\n", ELogLevel::Fatal);
        return E_FAIL;
    }

    if (!_scanner.GetChar(L']'))
    {
        Log(L"Expected \"]\"\n", ELogLevel::Fatal);
        return E_FAIL;
    }

    if (!_scanner.GetKeyword(L"Hello"))
    {
        Log(L"Expected \"Hello\"\n", ELogLevel::Fatal);
        return E_FAIL;
    }

    if (!_scanner.GetChar(L'='))
    {
        Log(L"Expected \"=\"\n", ELogLevel::Fatal);
        return E_FAIL;
    }

    int iNumber = 0;
    if (!_scanner.GetNumber(&iNumber) || iNumber != 1)
    {
        Log(L"Expected number 1, got %d\n", iNumber, ELogLevel::Fatal);
        return E_FAIL;
    }

    Log(L"Succeeded INI parsing test with no problems.\n", ELogLevel::Info);
    return S_OK;
}

HRESULT CIniParser::TestParser()
{
    HRESULT hr = Parse();

    if (FAILED(hr))
    {
        Log(L"Parse error at line %d: %s\n", GetParseError().iLine, GetParseError().strMessage.c_str(), ELogLevel::Fatal);
    }
    else if (SUCCEEDED(hr))
    {
        Log(L"Parsing purportedly succeeded with no errors!\n", ELogLevel::Info);
        Log(L"Now, step through it with the debugger to make sure it actually did work correctly.\n", ELogLevel::Info);
    }

    return hr;
}

HRESULT RunTests()
{
    // Test the scanner:
    {
        //CIniParser parser2(L"Hello");

        //RETURN_IF_FAILED(parser2.TestScanner());

        std::wstring spszTest = LR""""([Fawk]
Hello = 1

[aaaa::bbbb.fuck(aaaaaa):hello]
aaaaaaaaa = sijdisjfid

[animation.Fawk(dd)]
Transforms = {
    Translate2D {
        Duration = 20
    }
})"""";

        CIniParser parser(spszTest);

        RETURN_IF_FAILED(parser.TestScanner());
    }

    // Test the INI parser:
    {
        std::wstring spszText = LR""""([Button]
BgType = ImageFile
ContentAlignment = Center
CharSet = 234
FromColor1 = #3e3
FromColor2 = 255, 255 255
FromColor3 = #30efab
)"""";

        CIniParser parser(spszText);

        RETURN_IF_FAILED(parser.TestParser());
    }

    return S_OK;
}

}

#endif