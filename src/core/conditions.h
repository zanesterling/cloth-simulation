#pragma once

#include <algorithm>

#include "vec.h"
#include "cloth.h"

// TODO add changeable stretch factors
double *scaleCondition(Cloth &, int, int, int);
