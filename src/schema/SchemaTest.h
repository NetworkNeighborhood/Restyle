#pragma once

#include "Restyle.h"

enum class ESchemaTestMode
{
	PrintHelpMessage = 0,
	PrintEntryInfo = 1,
	ValidateSymbols = 2,
};

void TestSchema(ESchemaTestMode eMode, unsigned uEntryId);