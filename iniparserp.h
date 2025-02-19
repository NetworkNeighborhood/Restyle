#include "iniparser.h"

namespace IniParser
{

enum class EParseMode
{
    Assoc,
    SectionHeader,
    Preprocessor,
    BreakParseLoop,
};

enum class ESymbolType
{
    // -- Predefined types --
    FirstPredefined,
    Type = FirstPredefined,
    Class,
    Part,
    State,
    BaseClass,
    PropertyKey,
    EnumValue,
    LastPredefined = EnumValue,
    
    // -- Manual types --
    FirstManual,
    ManualPart = FirstManual,  // *Part#1
    ManualState,               // *State#1
    ManualPropertyKey,         // *Prop#1<Type>
    LastManual = ManualPropertyKey,
};

/**
 * A parse symbol.
 */
struct Symbol
{
    /**
     * The type of the symbol.
     *
     * There are two notable types of symbols: predefined (those which use a string name which
     * Restyle defines for identifying a type) and manual (those defining their own internal
     * value).
     *
     * The use of predefined symbols is preferred by Restyle. Manual symbols exist to give
     * theme authors flexibility.
     */
    ESymbolType eSymType;
    
    // Name of property.
    // Sharing memory because these are mutually-exclusive members:
    union
    {
        /**
         * Pointer to the name of the symbol in the name arena or schema, in the case of
         * predefined symbols.
         */
        LPCWSTR szName;
        
        /**
         * Integer value for the name of the symbol, in the case of manual symbols.
         */
        INT_PTR iName;
    };
    
    // Property information.
    // Sharing memory because these are mutually-exclusive members:
    union
    {
        /**
         * Offset in the schema table information about the symbol can be found, 
         * in the case of predefined symbols.
         */
        int iSchemaOffset;
        
        /**
         * The primitive type of the property, in the case of the ManualPropertyKey type.
         */
        int iPrimType;
    };
};

template <typename T>
struct ValueResult
{
    HRESULT hr = E_FAIL;
    T value = nullptr;
    
    inline FORCEINLINE bool Succeeded()
    {
        return SUCCEEDED(hr);
    }
    
    inline FORCEINLINE bool Failed()
    {
        return FAILED(hr);
    }
    
    inline FORCEINLINE HRESULT GetResult()
    {
        return hr;
    }
    
    inline FORCEINLINE T &Unwrap()
    {
        return value;
    }
    
    inline FORCEINLINE operator HRESULT()
    {
        return hr;
    }
    
    inline FORCEINLINE operator T()
    {
        return value;
    }
    
    inline ValueResult(HRESULT hr)
        : hr(hr)
        , value(nullptr)
    {
        assert(FAILED(hr));
    }
    
    inline ValueResult(T value)
        : hr(S_OK)
        , value(value)
    {
    }
    
    inline ValueResult(HRESULT hr, T value)
        : hr(hr)
        , value(value)
    {
    }
};

template <typename NativeType = void, int iPrimTypeVal = 0>
struct ValueBase
{
    int iPrimType = iPrimTypeVal;
    int cbSize = sizeof(NativeType);
};

struct IniSection
{
    Symbol *pClassNameSymbol;
    Symbol *pPartNameSymbol;
    Symbol *pStateNameSymbol;
    Symbol *pBaseClassNameSymbol;
};

struct IniAssociation
{
    IniSection section;
    Symbol *pKeySymbol;
    ValueBase<> *pVal;
};

struct IntValue : public ValueBase<int, Restyle::TMT_INT>
{
    int iVal;
};

struct SizeValue : public ValueBase<int, Restyle::TMT_SIZE>
{
    int iVal;
};

struct BoolValue : public ValueBase<BOOL, Restyle::TMT_BOOL>
{
    BOOL fVal;
};

struct RectValue : public ValueBase<RECT, Restyle::TMT_RECT>
{
    RECT rcVal;
};

struct MarginsValue : public ValueBase<MARGINS, Restyle::TMT_MARGINS>
{
    MARGINS marVal;
};

struct StringValue : public ValueBase<WCHAR, Restyle::TMT_STRING>
{
    WCHAR szVal[];
};

struct AnimationSetValue : public ValueBase<IniAssociation, Restyle::TMT_ANIMATIONSET>
{
    UINT cAnimations;
    IniAssociation rgAssociations[];
};

}