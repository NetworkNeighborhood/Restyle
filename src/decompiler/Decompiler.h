#pragma once
#include "Restyle.h"
#include "Util.h"
#include "BinParser.h"
#include "File.h"
#include "schema/SchemaUtils.h"
#include <unordered_map>

bool DecompileTheme(LPCWSTR pszOutFolder, Restyle::ESupportedOS eSupportedOS);