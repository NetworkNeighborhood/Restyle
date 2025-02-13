#include "restyle.h"

struct Range
{
    int iBegin;
    int iEnd;
};

Range g_rangePrimitiveTypes = { 200, 299 };

bool InRange(int iNum, Range range)
{
    return iNum >= range.iBegin && iNum <= range.iEnd;
}

