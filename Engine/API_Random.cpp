#include <cstdlib>
#include "API_Random.h"

float API_Random::range(float _min, float _max)
{
    float random = ((float)rand()) / (float)RAND_MAX;
    float diff = _max - _min;
    float r = random * diff;
    return _min + r;
}
