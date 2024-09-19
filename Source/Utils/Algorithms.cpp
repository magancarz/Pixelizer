#include "Algorithms.h"

namespace Algorithms
{
    bool equal(float first, float second, float precision)
    {
        return std::fabs(first - second) < precision;
    }
}