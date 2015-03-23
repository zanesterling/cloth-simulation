#pragma once

#include <Eigen/Dense>

#include <algorithm>

#include "vec.h"
#include "cloth.h"

using namespace Eigen;
// TODO add changeable stretch factors
Vector2d scaleCondition(Cloth &, int, int, int);
