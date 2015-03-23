#pragma once

#include <Eigen/Dense>

#include <algorithm>

#include "vec.h"
#include "cloth.h"

using namespace Eigen;
// TODO add changeable stretch factors
RowVector2d scaleCondition(Cloth &, int, int, int);
