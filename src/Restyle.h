#pragma once
#include "RestyleBase.h"
#include <stdio.h>
#include <vector>
#include <string>
#include <memory>
#include <assert.h>
#include <shlwapi.h>
#include "schema/restyle_TmSchema.h"
#define SCHEMA_STRINGS
#include "schema/restyle_TmSchema.h"

extern WCHAR g_szThemeFilePath[MAX_PATH];
extern HMODULE g_hThemeModule;

enum class EParseResult
{
	Success = 0,
	UnknownType = 1,
	Fail = 2,
};

#define PARSE_FAILED(r)                 (r == EParseResult::Fail)
#define RETURN_IF_PARSE_FAILED(r, v)    if (PARSE_FAILED(r)) return v

#define WIDE2(x) L##x
#define WIDE1(x) WIDE2(x)

#define __WDATE__  WIDE1(__DATE__)
#define __WTIME__  WIDE1(__TIME__)

// Buffer size for a base-10 representation of an int32
#define INT_STRING_CCH_MAX  sizeof("-2147483648")
