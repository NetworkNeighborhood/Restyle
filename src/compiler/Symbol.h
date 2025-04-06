#pragma once
#include "restyle.h"

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