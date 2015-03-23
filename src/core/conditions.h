#pragma once

#include <Eigen/Dense>

#include <algorithm>

#include "vec.h"
#include "cloth.h"

// the amount by which we perturb the points in finding partials
#define PERTURB_QUANT 0.0001

using namespace Eigen;
// TODO add changeable stretch factors
Vector2d scaleCondition(Cloth &, int, int, int);
MatrixXd scalePartial(Cloth &, int, int, int, int);
