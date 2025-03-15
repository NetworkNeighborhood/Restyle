// This file rebuilds every time the project is built, even during incremental builds,
// so we keep it lightweight. No dependencies.

typedef const wchar_t *LPCWSTR;

#define WIDE2(x) L##x
#define WIDE1(x) WIDE2(x)

#define __WDATE__  WIDE1(__DATE__)
#define __WTIME__  WIDE1(__TIME__)

LPCWSTR GetBuildDate()
{
	return L"Built " __WDATE__ L" " __WTIME__;
}