/*
 * Referenced from https://color.fandom.com/wiki/Colour_namer 
 */
#include "Restyle.h"
#include <string>

struct HsvColor
{
	double h;
	double s;
	double v;
};

/**
 * Colour names in order of clockwise divisions of the HSV wheel, indexing at
 * red and ending with redish colours once again.
 */
static LPCWSTR g_rgColorNames[] = {
	L"red",
	L"orange",
	L"yellow",
	L"green",
	L"cyan",
	L"azure",
	L"blue",
	L"violet",
	L"magenta",
	L"rose",
};

/**
 * 
 */
static LPCWSTR g_rgSaturateNames[] = {
	L"pale",
	L"dull",
	L"bright",
	L"dark",
};

/**
 * Grey names in order of darkest to lightest.
 */
static LPCWSTR g_rgGreyNames[] = {
	L"black",
	L"extradark grey",
	L"dark grey",
	L"semidark grey",
	L"mid grey",
	L"semilight grey",
	L"light grey",
	L"extralight grey",
	L"white",
};

// https://stackoverflow.com/a/6930407
HsvColor RgbToHsv(COLORREF crRgb)
{
	HsvColor out;

	double fMin = min(min(GetRValue(crRgb), GetGValue(crRgb)), GetBValue(crRgb));
	double fMax = max(max(GetRValue(crRgb), GetGValue(crRgb)), GetBValue(crRgb));

	out.v = fMax;
	
	double fDelta = fMax - fMin;

	if (fDelta < 0.00001)
	{
		out.s = 0;
		out.h = 0;
		return out;
	}

	if (fMax > 0.0)
	{
		out.s = (fDelta / fMax);
	}
	else
	{
		out.s = 0.0;
		out.h = NAN;
		return out;
	}

	if (GetRValue(crRgb) >= fMax)
	{
		out.h = (GetGValue(crRgb) - GetBValue(crRgb)) / fDelta;
	}
	else if (GetGValue(crRgb) >= fMax)
	{
		out.h = 2.0 + (GetBValue(crRgb) - GetRValue(crRgb)) / fDelta;
	}
	else
	{
		out.h = 4.0 + (GetRValue(crRgb) - GetGValue(crRgb)) / fDelta;
	}

	out.h *= 60.0;

	if (out.h < 0.0)
	{
		out.h += 360.0;
	}

	return out;
}

std::wstring GetColorName(COLORREF crRgb)
{
	HsvColor hsv = RgbToHsv(crRgb);


}