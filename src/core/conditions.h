#pragma once

#include <Eigen/Dense>

#include <algorithm>

#include "vec.h"
#include "cloth.h"

// the amount by which we perturb the points in finding partials
#define PERTURB_QUANT 0.0001

using namespace Eigen;

Matrix<double, 3, 2> wuvMatrix(Cloth &, int, int, int);

// TODO add changeable stretch factors
Vector2d scaleCondition(Cloth &, int, int, int);
Matrix<double, 2, 3> scalePartial(Cloth &, int, int, int, int);

double shearCondition(Cloth &, int, int, int);
RowVector3d shearPartial(Cloth &, int, int, int, int);

double bendCondition(Cloth &, int, int, int, int);
RowVector3d bendPartial(Cloth &, int, int, int, int, int);
