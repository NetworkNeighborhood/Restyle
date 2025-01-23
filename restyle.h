#pragma once
#include "restylebase.h"
#include <stdio.h>
#include <vector>
#include <string>
#include <assert.h>
#include "restyle_TmSchema.h"
#define SCHEMA_STRINGS
#include "restyle_TmSchema.h"

extern WCHAR g_szThemeFilePath[MAX_PATH];
extern HMODULE g_hThemeModule;

enum class EParseResult
{
	Success = 0,
	UnknownType = 1,
	Fail = 2,
};

#define VER_MAJOR       0
#define VER_MINOR       1
#define VER_REVISION    0
#define VER_STRING      "0.1.0"

#define WIDE2(x) L##x
#define WIDE1(x) WIDE2(x)

#define __WDATE__  WIDE1(__DATE__)
#define __WTIME__  WIDE1(__TIME__)

// Buffer size for a base-10 representation of an int32
#define INT_STRING_CCH_MAX  sizeof("-2147483648")
