#include "iniparserp.h"
#include "restyle.h"
#include "util.h"
#include "file.h"
#include "scanner.h"
#include "arena.h"
#include <iterator>
#include <stringapiset.h>
#include <strsafe.h>
#include <winerror.h>
#include <winnls.h>
#include <string>
#include <vector>
#include <winnt.h>

// Uncomment to enable preprocessor support, which is currently incomplete.
//#define ENABLE_PREPROCESSOR

namespace IniParser
{

bool IsSymbolTypePredefined(ESymbolType eSymType)
{
    return eSymType >= ESymbolType::FirstPredefined && eSymType <= ESymbolType::LastPredefined;
}

bool IsSymbolTypeManual(ESymbolType eSymType)
{
    return eSymType >= ESymbolType::FirstManual && eSymType <= ESymbolType::LastManual;
}

/**
 * Stores unique names used within a parsing context, especially class names.
 */
class CNameArena
#define BASECLASS CTBaseArena<CNameArena, const WCHAR, 1024>
    : public BASECLASS
{    
    using Super = BASECLASS;
    
public:
    struct Iterator
    {
        using iterator_category = std::forward_iterator_tag;
        using difference_type = std::ptrdiff_t;
        using value_type = LPCWSTR;
        using pointer = value_type;
        using reference = const value_type &;
        
        // Beginning (real) iterator constructor
        Iterator(CNameArena *nameArena)
            : _pNameArena(nameArena)
            , _p((pointer)nameArena->_pvData)
        {
        }
        
        // Ending (sentinel) iterator constructor
        Iterator()
            : _pNameArena(nullptr)
            , _p(nullptr)
        {
        }
        
        reference operator*() const
        {
            return _p;
        }
        
        pointer operator->() const
        {
            return _p;
        }
        
        Iterator &operator++()
        {
            GetNextString();
            return *this;
        }
        
        Iterator &operator++(int)
        {
            GetNextString();
            return *this;
        }
        
        friend bool operator==(const Iterator &a, const Iterator &b)
        {
            // The end iterator is always a sentinel object, with its pointer and CNameArena
            // pointers both being nullptr.
            assert(a._p == nullptr || b._p == nullptr);
            
            const Iterator *pRealIterator = a._p != nullptr ? &a : &b;
            
            LPCWSTR pEndOfData = pRealIterator->GetEndOfData();
            
            if (pRealIterator->_p > pEndOfData || pRealIterator->_fIsEnd)
            {
                // i == iterator.end()
                return true;
            }
            
            return false;
        }
        
        friend bool operator!=(const Iterator &a, const Iterator &b)
        {
            return !(operator==(a, b));
        }
        
    private:
        void UseNextArena()
        {
            _pNameArena = _pNameArena->_pNext;
            _p = (pointer)_pNameArena->_pvData;
        }
    
        pointer GetEndOfData() const
        {
            return (LPCWSTR)((size_t)_pNameArena->_pvData + _pNameArena->_dwSize);
        }
    
        pointer GetNextString()
        {
            do
            {
                while (++_p < GetEndOfData() && *_p != L'\0');
                LPCWSTR szResult = ++_p;
                
                if (_pNameArena->_pNext)
                {
                    UseNextArena();
                    continue;
                }
                else if (*szResult == '\0')
                {
                    _fIsEnd = true;
                }
                
                return szResult;
            }
            while (1);
        }
    
        CNameArena *_pNameArena;
        bool _fIsEnd = false;
        pointer _p;
    };
    
    // C++ iterator concept
    Iterator begin()
    {
        return Iterator(this);
    }
    
    // C++ iterator concept
    Iterator end()
    {
        return Iterator();
    }

    ValueResult<LPCWSTR> Add(LPCWSTR sz);
};

ValueResult<LPCWSTR> CNameArena::Add(LPCWSTR sz)
{
    size_t cbsz = (wcslen(sz) + sizeof(L'\0')) * sizeof(WCHAR);
    
    for (const LPCWSTR &szExisting : *this)
    {
        if (AsciiStrCmpI(szExisting, sz))
        {
            // Avoid inserting duplicate items:
            return szExisting;
        }
    }
    
    HRESULT hr = Super::Add(sz, cbsz);
    if (FAILED(hr))
    {
        return hr;
    }
    
    return (LPCWSTR)_pvCur;
}

/**
 * Manages symbols.
 */
class CSymbolManager
{
    std::vector<Symbol> _rgSymbols;
    CNameArena _nameArena;
    
public:
    ValueResult<Symbol *> AddSymbol(LPCWSTR szSymName, ESymbolType eSymType);
    ValueResult<Symbol *> AddManualSymbol(int iVal, ESymbolType eSymType);
    LPCWSTR GetGlobalSymbolName(LPCWSTR szSymName, OUT OPTIONAL int *piSchemaOffset = nullptr);
    Symbol *FindSymbolPointer(LPCWSTR szSymName);
    bool HasSymbol(LPCWSTR szSymName);
};

ValueResult<Symbol *> CSymbolManager::AddSymbol(LPCWSTR szSymName, ESymbolType eSymType)
{
    assert(IsSymbolTypePredefined(eSymType));

    if (Symbol *p = FindSymbolPointer(szSymName))
    {
        return p;
    }
    
    int iSchemaOffset = -1;
    LPCWSTR szSafeSymName = GetGlobalSymbolName(szSymName, &iSchemaOffset);
    
    if (!szSafeSymName)
    {
        // Probably out of memory...
        return E_FAIL;
    }
    
    Symbol sym {};

    sym.szName = szSafeSymName;
    sym.eSymType = eSymType;
    sym.iSchemaOffset = iSchemaOffset;
    
    _rgSymbols.push_back(sym);
    return &_rgSymbols[_rgSymbols.size() - 1];
}

ValueResult<Symbol *> CSymbolManager::AddManualSymbol(int iVal, ESymbolType eSymType)
{
    assert(IsSymbolTypeManual(eSymType));

    // Try to find a duplicate symbol in the array to optimise memory usage:
    for (Symbol &s : _rgSymbols)
    {
        if (s.eSymType == eSymType && s.iName == iVal)
        {
            return &s;
        }
    }

    Symbol sym {};

    sym.iName = iVal;
    sym.eSymType = eSymType;

    _rgSymbols.push_back(sym);
    return &_rgSymbols[_rgSymbols.size() - 1];
}

LPCWSTR CSymbolManager::GetGlobalSymbolName(LPCWSTR szSymName, OUT OPTIONAL int *piSchemaOffset)
{
    LPCWSTR pszResult = nullptr;
    const Restyle::TMSCHEMAINFO *pSchemaInfo = Restyle::GetSchemaInfo();
    
    // This is an existing name stored statically in the module in the schema:
    for (int i = 0; i < pSchemaInfo->iPropCount; i++)
    {
        const Restyle::TMPROPINFO *pPropInfo = &pSchemaInfo->pPropTable[i];
        
        if (AsciiStrCmpI(pPropInfo->pszName, szSymName))
        {
            pszResult = pPropInfo->pszName;
            break;
        }
    }
    
    // This is a unique name which only presents itself inside this INI file. This case
    // includes all of the class names. In this case, we'll copy the names over to our 
    // name arena so that we can preserve them even after the INI file is freed from memory.
    if (!pszResult)
    {
        _nameArena.EnsureInitialized();
        pszResult = _nameArena.Add(szSymName);
    }
    
    return pszResult;
}

Symbol *CSymbolManager::FindSymbolPointer(LPCWSTR szSymName)
{
    for (Symbol &s : _rgSymbols)
    {
        if (AsciiStrCmpI(s.szName, szSymName) == 0)
        {
            return &s;
        }
    }

    return nullptr;
}

bool CSymbolManager::HasSymbol(LPCWSTR szSymName)
{
    return FindSymbolPointer(szSymName) != nullptr;
}

/**
 * Stores unique names used within a parsing context, especially class names.
 */
class CValueArena : public CTBaseArena<CValueArena, RectValue, 64>
{
    /**
     * This class is used as an RAII wrapper to ensure that the methods of this
     * function actually update the offset.
     * 
     * In release builds, this class will be erased by the optimising compiler.
     */
    class CEnsureArenaPointerChanged
    {
#if DEBUG
        CValueArena *_pParent;
        BYTE *_pvOriginal;

    public:
        inline [[nodiscard]] CEnsureArenaPointerChanged(CValueArena *pParent)
            : _pParent(pParent)
            , _pvOriginal(pParent->_pvCur)
        {
        }

        inline ~CEnsureArenaPointerChanged()
        {
            assert(_pParent->_pvCur != _pvOriginal);
        }
#else
        inline FORCEINLINE CEnsureArenaPointerChanged(CValueArena *pParent)
        {
        }
#endif
    };

public:
    ValueResult<IntValue *> CreateIntValue(int iVal);
    ValueResult<BoolValue *> CreateBoolValue(BOOL fVal);
    ValueResult<StringValue *> CreateStringValue(LPCWSTR szVal);
};

ValueResult<IntValue *> CValueArena::CreateIntValue(int iVal)
{
    CEnsureArenaPointerChanged ensurePointerChanged(this);

    HRESULT hr = ResizeIfNecessary(sizeof(IntValue));
    if (FAILED(hr))
    {
        return hr;
    }
    
    IntValue *pResult = new (_pvCur) IntValue();
    pResult->iVal = iVal;
    _pvCur += pResult->cbSize;
    return pResult;
}

ValueResult<BoolValue *> CValueArena::CreateBoolValue(BOOL fVal)
{
    CEnsureArenaPointerChanged ensurePointerChanged(this);

    HRESULT hr = ResizeIfNecessary(sizeof(BoolValue));
    if (FAILED(hr))
    {
        return hr;
    }
    
    BoolValue *pResult = new (_pvCur) BoolValue();
    pResult->fVal = fVal;
    _pvCur += pResult->cbSize;
    return pResult;
}

ValueResult<StringValue *> CValueArena::CreateStringValue(LPCWSTR szVal)
{
    CEnsureArenaPointerChanged ensurePointerChanged(this);

    size_t cch = wcslen(szVal);
    size_t targetSize = cch * sizeof(WCHAR) + sizeof(L'\0');
    
    HRESULT hr = ResizeIfNecessary(sizeof(StringValue) + targetSize);
    if (FAILED(hr))
    {
        return hr;
    }
    
    StringValue *pResult = new (_pvCur) StringValue();
    pResult->cbSize = targetSize;
    if (!memcpy((void *)&pResult->szVal[0], szVal, targetSize))
    {
        return E_FAIL;
    }

    _pvCur += pResult->cbSize;
    return pResult;
}

class CIniParser
{
    CScanner _scanner;
    CSymbolManager *_pSymbolManager;
    std::vector<IniAssociation> _associations;

    // Precached symbols:
    Symbol *_pSymNullBaseClass = nullptr;
    
    IniSection _iniSectionCur;
    
    EParseMode _eMode;
    bool _fParsingIncludeChild = false;

    HRESULT Initialize();
    
    std::wstring ReadNextWord();
    
    HRESULT ParseNextSectionHeader();
    HRESULT ParseNextAssociation();
    ValueResult<std::wstring> ParseNextClassName();

    struct ParseManualSymbolResult
    {
        int iSymbolName = 0;
        int iType = 0;
    };

    ValueResult<ParseManualSymbolResult> ParseNextManualSymbolSegment(ESymbolType eExpectType, bool fParsedAsterisk = true);
    
#ifdef ENABLE_PREPROCESSOR
    HRESULT ParseNextCPreprocessor();

    HRESULT ParseCPreprocessorInclude();
    HRESULT ParseCPreprocessorIf();
    HRESULT ParseCPreprocessorElif();
    HRESULT ParseCPreprocessorElse();
    HRESULT ParseCPreprocessorEndif();
    
    static constexpr struct
    {
        // Must be the length of the longest word in the map.
        WCHAR szCommand[sizeof(L"include")];
        HRESULT (CIniParser:: *pfnCallback)();
    } s_rgPreprocessorCommandMap[] = {
        { L"include", &CIniParser::ParseCPreprocessorInclude },
        { L"if", &CIniParser::ParseCPreprocessorIf },
        { L"elif", &CIniParser::ParseCPreprocessorElif },
        { L"else", &CIniParser::ParseCPreprocessorElse },
        { L"endif", &CIniParser::ParseCPreprocessorEndif },
    };
#endif
    
public:
    CIniParser(LPCWSTR szText, DWORD cchText);
    CIniParser(std::wstring text);
    
    // static CIniParser CreateIncludeIniParser(CIniParser *pParent, CSimpleFile *pChildFile);
    
    HRESULT Parse();
    
    bool ReadName(IN OUT LPWSTR szId, DWORD cchId);
    
    HRESULT Test();
};

CIniParser::CIniParser(LPCWSTR szText, DWORD cchText)
    : _scanner(szText, cchText)
{
    Initialize();
}

CIniParser::CIniParser(std::wstring text)
    : _scanner(text)
{
    Initialize();
}

HRESULT CIniParser::Initialize()
{
    // TODO: The symbol manager needs to be loaded into the INI parser from somewhere.
    // Where do we get it from? Do we create it ourselves and just relinquish control
    // once we don't need it anymore?

    // TODO: How do we elegantly make the null base class symbol work out?
    // It's a weird edge case.
    _pSymNullBaseClass = _pSymbolManager->AddSymbol(nullptr, ESymbolType::BaseClass);

    return S_OK;
}

std::wstring CIniParser::ReadNextWord()
{
    std::wstring result;
    
    while (_scanner.IsNameChar(false))
    {
        result.push_back(_scanner.ReadNext());
    }
    
    return result;
}

HRESULT CIniParser::Parse()
{
    HRESULT hr;
    
    while (!_scanner.EndOfFile() && _eMode != EParseMode::BreakParseLoop)
    {
        switch (_eMode)
        {
#ifdef ENABLE_PREPROCESSOR
            case EParseMode::Preprocessor:
            {
                hr = ParseNextCPreprocessor();
                break;
            }
#endif
            
            case EParseMode::SectionHeader:
            {
                hr = ParseNextSectionHeader();
                break;
            }
            
            case EParseMode::Assoc:
            {
                hr = ParseNextAssociation();
                break;
            }
            
            case EParseMode::BreakParseLoop:
            {
                // This is useless because it's handled in the above while loop, however Intellisense
                // won't shut up.
                break;
            }
        }
    }
    
    return S_OK;
}

/**
 * Parses manual symbol segments.
 * 
 * Manual symbols follow the format:
 *     *Identifer#N<Type>
 * 
 * The * character is often used for detection by other parser functions, and thus is usually
 * skipped by this function.
 * 
 * The components must be of the following types:
 *     * = A required prefix character to denote manual symbols.
 *     Identifier = One of case-insensitive built-in symbols "Part", "State", or interchangable
 *                  "Prop"/"Property". This is a required component.
 *     # = An optional character that may precede the number for aesthetic purposes.
 *     N = An integer number literal, which may be written in decimal or hexadecimal. For more
 *         information, see the general documentation on parsing integer numbers. This is a
 *         required component.
 *     Type = An optionally-parsed type annotation, required for property types and illegal
 *            for all other types. When used, the value must be a valid type symbol or enum
 *            name as specified in the schema.
 * 
 * Examples of major valid constructions include:
 *     *Part#1
 *     *State0xBADF00D
 *     *Property#1<Int>
 */
auto CIniParser::ParseNextManualSymbolSegment(ESymbolType eExpectType, bool fParsedAsterisk) -> ValueResult<ParseManualSymbolResult>
{
    assert(IsSymbolTypeManual(eExpectType));

    if (!fParsedAsterisk)
    {
        if (!_scanner.GetChar(L'*'))
        {
            Log(L"Expected '*'.");
            return E_FAIL;
        }
    }

    std::wstring strIdentifier = ReadNextWord();

    if (strIdentifier.empty())
    {
        Log(L"Expected a word.");
        return E_FAIL;
    }

    if (eExpectType == ESymbolType::ManualPart && !AsciiStrCmpI(strIdentifier.c_str(), L"Part"))
    {
        Log(L"Unexpected identifier '%s', expected 'Part'.", strIdentifier.c_str());
        return E_FAIL;
    }
    else if (eExpectType == ESymbolType::ManualState && !AsciiStrCmpI(strIdentifier.c_str(), L"State"))
    {
        Log(L"Unexpected identifier '%s', expected 'State'.", strIdentifier.c_str());
        return E_FAIL;
    }
    else if (eExpectType == ESymbolType::ManualPropertyKey &&
        !AsciiStrCmpI(strIdentifier.c_str(), L"Prop") ||
        !AsciiStrCmpI(strIdentifier.c_str(), L"Property"))
    {
        Log(L"Unexpected identifier '%s', expected 'Prop' or 'Property'.", strIdentifier.c_str());
        return E_FAIL;
    }

    // Skip the optional "#" if it is there.
    _scanner.GetChar(L'#');

    int iValue;
    if (!_scanner.GetNumber(&iValue))
    {
        Log(L"Expected number");
        return E_FAIL;
    }

    int iType = 0;

    // If we're parsing a property key type, then we always require a type name which is in the
    // schema. Since the schema is static data, we can validate that a specified name is legal
    // during this parsing pass.
    if (eExpectType == ESymbolType::ManualPropertyKey)
    {
        if (_scanner.GetChar(L'<'))
        {
            Log(L"Expected '<'");
            return E_FAIL;
        }

        std::wstring strType = ReadNextWord();

        if (strType.empty())
        {
            Log(L"Expected a type name.");
            return E_FAIL;
        }

        // TODO: Restructure this in the future to use schemautils.
        const Restyle::TMSCHEMAINFO *pSchemaInfo = Restyle::GetSchemaInfo();
        for (int i = 0; i < pSchemaInfo->iPropCount; i++)
        {
            const Restyle::TMPROPINFO *pPropInfo = &pSchemaInfo->pPropTable[i];

            // Is outside primitive range or is not an enum definition.
            if ((pPropInfo->sEnumVal < 200 && pPropInfo->sEnumVal > 299) || pPropInfo->bPrimVal != Restyle::TMT_ENUMDEF)
            {
                continue;
            }

            if (AsciiStrCmpI(pPropInfo->pszName, strType.c_str()))
            {
                iType = pPropInfo->sEnumVal;
                break;
            }
        }

        // Fail: the above loop fell through without setting a value.
        if (iType == 0)
        {
            Log(L"Unknown type name '%s' specified", strType.c_str());
            return E_FAIL;
        }

        if (_scanner.GetChar(L'>'))
        {
            Log(L"Expected '>'");
            return E_FAIL;
        }
    }

    return ParseManualSymbolResult { iValue, iType };
}

/**
 * Parses class names.
 * 
 * Class names follow the format as ordained by Microsoft:
 *     AppName::ClassName
 * 
 * where the only required component is the ClassName.
 * 
 * The components must be of the following types:
 *     AppName = A class name fragment. This is a standard alphanumeric string.
 *     :: = An optionally-parsed character sequence which is required when an
 *          AppName component is provided.
 *     ClassName = A class name fragment. This is a standard alphanumeric string.
 * 
 * Examples of major valid constructions include:
 *     Button
 *     Start::Button
 */
ValueResult<std::wstring> CIniParser::ParseNextClassName()
{
    std::wstring strFinalClass;
    std::wstring strFirst = strFinalClass = ReadNextWord();

    if (strFirst.empty())
    {
        Log(L"Expected a word.");
        return E_FAIL;
    }

    // If we're going to follow with a "::" sequence, then we must not allow spaces
    // to surround it. While Restyle makes a distinction between the main class name
    // and the semantic base class, the visual styles engine itself does not care.
    // They are considered one unit, and the full name with the "::" sequence is
    // considered to be one single qualified name.
    bool fNextCharIsSpace = _scanner.IsSpace(_scanner.Read());

    if (_scanner.GetKeyword(L"::"))
    {
        if (fNextCharIsSpace)
        {
            Log(L"Unexpected space.");
            return E_FAIL;
        }

        strFinalClass += L"::";

        std::wstring strBaseClass = ReadNextWord();

        if (strBaseClass.empty())
        {
            Log(L"Expected a word.");
            return E_FAIL;
        }

        strFinalClass += strBaseClass;
    }

    // We'll check again to dispatch appropriate meaningful parse errors:
    fNextCharIsSpace = _scanner.IsSpace(_scanner.Read());

    if (_scanner.GetKeyword(L"::"))
    {
        if (fNextCharIsSpace)
        {
            Log(L"Unexpected '::'. You might have meant ':' instead.");
            return E_FAIL;
        }
        else
        {
            Log(L"Unexpected '::'. Class names may not exceed a maximum of 2 levels of an app name and a base class.");
            return E_FAIL;
        }
    }

    return strFinalClass;
}

/**
 * Parses section headers.
 * 
 * Section headers follow the format:
 *     [Class.Part(State) : BaseClass]
 * 
 * where the only required component is the Class.
 * 
 * The components must be of the following types:
 *     Class = A class name. If this name does not already exist, it will be registered.
 *     Part = A qualified part name associated with the class, or a custom class name
 *            of the formats "*PartN" or "*Part#N" (where N is an integer number between
 *            1 and 65535), or unspecified.
 *     State = A qualified state name associated with the class or part, or a custom
 *             part name of the format "*StateN" or "*State#N" (where N is an integer
 *             number between 1 and 65535), or unspecified.
 *     BaseClass = A qualified class name, or unspecified.
 * 
 * All qualified types must be referenced elsewhere in the theme codebase. Nonexistent
 * names will cause a parse error.
 * 
 * State component names may correlate with either the symbol for the Part component or
 * the Class component. If the Part was specified, then the State is associated
 * with it, otherwise it is associated with the Class. In other words, it binds to
 * the last specified component.
 * 
 * The BaseClass component may not co-exist with a Part and/or a State. The
 * backed component, the inheriting base class that is compiled into the BCMAP in the
 * final msstyles, is usually implied from the semantic base claass in the Class component. The
 * BaseClass component may be used for complex inheritance or otherwise
 * overriding the implicit base class. In order to force the use of no base class, the
 * case-insensitive special symbol "@None" may be used instead of a qualified class
 * name, or alternatively the integer number "0" may be used for the same purpose.
 * 
 * Examples of major valid constructions include:
 *     [Class]
 *     [Class(State)]
 *         whereby the State name must correlate with the ClassName.
 *     [Class.Part(State)]
 *         whereby the State name must correlate with the Part name
 *     [Class : BaseClass]
 */
HRESULT CIniParser::ParseNextSectionHeader()
{
    if (!_scanner.GetChar(L'['))
    {
        Log(L"Expected [", ELogLevel::Fatal);
        return E_FAIL;
    }

    std::wstring strClass, strState, strPart, strBaseClass;
    bool fEmptyBaseClass = false;
    bool fManualState = false, fManualPart = false;
    ParseManualSymbolResult manualState, manualPart;

    // The next word should always be the class name. This is the only required part
    // of the section header.
    PROPAGATE_ERROR_IF_FAILED(strClass = ParseNextClassName());

    // If we're going to follow with a "::" sequence, then we must not allow spaces
    // to surround it. While Restyle makes a distinction between the semantic base
    // class and the main class name, the visual styles engine itself does not care.
    // They are considered one unit, and the full name with the "::" sequence is
    // considered to be one single qualified name.
    bool fNextCharIsSpace = _scanner.IsSpace(_scanner.Read());

    // We loop so we can easily report error messages for out-of-order (i.e. accidentally
    // malformed) text. The statements within the loop should be in the correct strict
    // order, separated within an else-if chain.
    while (!_scanner.EndOfFile())
    {
        // If we follow with a "::" sequence, then we're parsing a semantic base class.
        if (_scanner.GetKeyword(L"::"))
        {
            // Ensure that we're in the right position:
            if (!strPart.empty() || !strState.empty() && !strBaseClass.empty())
            {
                Log(L"Unexpected '::'. You probably meant ':'.");
                return E_FAIL;
            }
            // TODO: add other cases when the error handling system is improved.
        }
        // If we follow with a "." character, then we're parsing a part specifier.
        else if (_scanner.GetChar(L'.'))
        {
            // Ensure that we're in the right position:
            // TODO: add cases

            if (_scanner.GetChar(L'*'))
            {
                fManualPart = true;

                auto result = ParseNextManualSymbolSegment(ESymbolType::ManualPart);
                if (result.Succeeded())
                {
                    manualPart = result;
                }
                else
                {
                    // Propagate error:
                    return result;
                }
            }
            else
            {
                strPart = ReadNextWord();

                if (strPart.empty())
                {
                    Log(L"Expected a word.");
                    return E_FAIL;
                }
            }
        }
        // If we follow with a "(" character, then we're parsing a state specifier.
        else if (_scanner.GetChar(L'('))
        {
            // Ensure that we're in the right position:
            // TODO: add cases

            if (_scanner.GetChar(L'*'))
            {
                fManualState = true;

                auto result = ParseNextManualSymbolSegment(ESymbolType::ManualState);
                if (result.Succeeded())
                {
                    manualState = result;
                }
                else
                {
                    // Propagate error:
                    return result;
                }
            }
            else
            {
                strState = ReadNextWord();

                if (strState.empty())
                {
                    Log(L"Expected a word.");
                    return E_FAIL;
                }
            }

            if (!_scanner.GetChar(L')'))
            {
                Log(L"Expected ')'");
                return E_FAIL;
            }
        }
        // If we follow with a ":" character, then we're parsing a manual inheriting
        // class name specifier.
        else if (_scanner.GetChar(L':'))
        {
            // Ensure that we're in the right position:
            // TODO: add cases

            int i;
            if (_scanner.GetKeyword(L"@None") || (_scanner.GetNumber(&i) && i == 0))
            {
                fEmptyBaseClass = true;
            }
            else
            {
                PROPAGATE_ERROR_IF_FAILED(strBaseClass = ParseNextClassName());

                if (strBaseClass.empty())
                {
                    Log(L"Expected a word.");
                    return E_FAIL;
                }
            }
        }
        // If we follow with a "]" character, then we will terminate the whole sequence
        // and verify that we have some form of valid output.
        else if (_scanner.GetChar(L']'))
        {
            break;
        }
        // Otherwise, we have a completely unexpected sequence, and so we'll just return
        // a parse error.
        else
        {
            Log(L"Unexpected sequence.");
            return E_FAIL;
        }
    }

    std::wstring strFullClassName = strClass;

    //
    // Create symbols for each of the components.
    //
    Symbol *pSymClass = nullptr, *pSymPart = nullptr, *pSymState = nullptr, *pSymBaseClass = nullptr;

    pSymClass = _pSymbolManager->AddSymbol(strClass.c_str(), ESymbolType::Class);

    if (!strPart.empty())
    {
        pSymPart = _pSymbolManager->AddSymbol(strPart.c_str(), ESymbolType::Part);
    }
    else if (fManualPart)
    {
        pSymPart = _pSymbolManager->AddManualSymbol(manualPart.iSymbolName, ESymbolType::ManualPart);
    }

    if (!strState.empty())
    {
        pSymState = _pSymbolManager->AddSymbol(strState.c_str(), ESymbolType::State);
    }
    else if (fManualState)
    {
        pSymState = _pSymbolManager->AddManualSymbol(manualState.iSymbolName, ESymbolType::ManualState);
    }

    if (!strBaseClass.empty())
    {
        pSymBaseClass = _pSymbolManager->AddSymbol(strBaseClass.c_str(), ESymbolType::BaseClass);
    }
    else if (fEmptyBaseClass)
    {
        pSymBaseClass = _pSymNullBaseClass;
    }

    //
    // Create the internal representation of the INI section and set it as the currently-active one.
    //

    _iniSectionCur = {};

    _iniSectionCur.pSymClass = pSymClass;
    _iniSectionCur.pSymPart = pSymPart;
    _iniSectionCur.pSymState = pSymState;
    _iniSectionCur.pSymBaseClass = pSymBaseClass;

    return S_OK;
}

/**
 * Parses an association.
 * 
 * Associations follow the format:
 *     Property = Value
 * 
 * The components must be of the following types:
 *     Property = A qualified property name as defined in the Theme Manager schema, or a custom
 *                property of the formats "*Property#N<Type>", "*Prop#N<Type>" where N is an
 *                integer number between 1 and 65535, and Type is a qualified primitive type name
 *                or enum name as defined in the Theme Manager schema. In either case, this
 *                component has a primitive type, either implicitly retrieved from the schema, or
 *                manually specified in the case of custom properties.
 *     Value = A value matching the format outlined for the primitive type of the Property
 *             component. For more information, see the documentation for the parsers for each of
 *             the value types.
 * 
 * Examples of major valid constructions include:
 *     Transparent = True
 *     ImageCount = 8
 *     ImageFile1 = "ButtonBackground.png"
 *     ContentMargins = 5, 5, 5, 5
 *     *Property#9000<Float> = 3.2
 */
HRESULT CIniParser::ParseNextAssociation()
{
    return E_NOTIMPL;
}

#ifdef ENABLE_PREPROCESSOR
HRESULT CIniParser::ParseNextCPreprocessor()
{
    std::wstring strNextWord = ReadNextWord();
    HRESULT hr = E_FAIL;
    
    if (strNextWord.empty())
    {
        return E_FAIL;
    }
    
    bool fHandled = false;
    
    for (auto &cmd : s_rgPreprocessorCommandMap)
    {
        if (wcscmp(strNextWord.c_str(), cmd.szCommand) == 0)
        {
            fHandled = true;
            hr = (this->*cmd.pfnCallback)();
        }
    }
    
    if (!fHandled)
    {
        // Invalid keyword.
        return E_FAIL;
    }
    
    return hr;
}

HRESULT CIniParser::ParseCPreprocessorInclude()
{
    // Include is a tricky one. This will be written later.
    return S_OK;
}

// // static
// CIniParser CIniParser::CreateIncludeIniParser(CIniParser *pParent, CSimpleFile *pChildFile)
// {
//     CIniParser iniParser(pChildFile, pParent->);
// }
#endif

// Expects test file:
#if 0
[Fawk]
Hello = 1

[aaaa::bbbb.fuck(aaaaaa) : hello]
aaaaaaaaa = sijdisjfid

[animation.Fawk(dd)]
Transforms = {
    Translate2D {
        Duration = 20
    }
}
#endif
HRESULT CIniParser::Test()
{
    if (!_scanner.GetChar(L'['))
    {
        Log(L"First character bad. Parser probably getting bad input u_u. Check debugger.", ELogLevel::Fatal);
        return E_FAIL;
    }
    
    if (!_scanner.GetKeyword(L"Fawk"))
    {
        Log(L"Expected \"Fawk\"", ELogLevel::Fatal);
        return E_FAIL;
    }
    
    if (!_scanner.GetChar(L']'))
    {
        Log(L"Expected \"]\"", ELogLevel::Fatal);
        return E_FAIL;
    }
    
    if (!_scanner.GetKeyword(L"Hello"))
    {
        Log(L"Expected \"Hello\"", ELogLevel::Fatal);
        return E_FAIL;
    }
    
    if (!_scanner.GetChar(L'='))
    {
        Log(L"Expected \"=\"", ELogLevel::Fatal);
        return E_FAIL;
    }
    
    int iNumber = 0;
    if (!_scanner.GetNumber(&iNumber) || iNumber != 1)
    {
        Log(L"Expected number 1, got %d", iNumber, ELogLevel::Fatal);
        return E_FAIL;
    }
    
    Log(L"Succeeded INI parsing test with no problems.", ELogLevel::Info);
    return S_OK;
}

bool CIniParser::ReadName(IN OUT LPWSTR szId, DWORD cchId)
{
    if (cchId == 0)
    {
        return false;
    }
    
    _scanner.SkipSpaces();
    
    WCHAR *szRead = szId;
    
    while (_scanner.IsNameChar(false) && --cchId)
    {
        *szRead++ = _scanner.ReadNext();
    }
    *szRead = 0; // Add null terminator
    
    if (szRead == szId)
    {
        // If we hit this case, then we didn't end up reading anything.
        return false;
    }
    
    if (_scanner.IsNameChar(false))
    {
        // Buffer too small
        return false;
    }
    
    return true;
}


HRESULT ParseIniFile(LPCWSTR szPath)
{
    CSimpleFile file;
    HRESULT hr = file.Open(szPath);

    if (FAILED(hr))
    {
        Log(L"FATAL: Failed to open file.", ELogLevel::Fatal);
        return 0;
    }

    DWORD dwFileSizeHigh = 0;
    DWORD dwFileSize = file.GetFileSize(&dwFileSizeHigh);

    if (dwFileSize == 0)
    {
        Log(L"FATAL: Failed to get file size.", ELogLevel::Fatal);
        return 0;
    }

    LPWSTR szString = new WCHAR[dwFileSize + sizeof('\n')];

    DWORD cbRead = 0;
    hr = file.Read((void *)szString, dwFileSize * sizeof(WCHAR) + sizeof(L'\n'), &cbRead);

    if (FAILED(hr))
    {
        delete[] szString;
        Log(L"FATAL: Failed to read file.", ELogLevel::Fatal);
        return 0;
    }
    
    CIniParser iniParser(szString, dwFileSize + sizeof('\n'));
    iniParser.Test();
    
    // TODO: Add INI parsing test code here.

    if (szString)
    {
        delete[] szString;
    }
    
    return S_OK;
}

}