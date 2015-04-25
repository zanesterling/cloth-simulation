#pragma once

#include <Eigen/Dense>

#include <algorithm>

#include "cloth.h"

// the amount by which we perturb the points in finding partials
#define PERTURB_QUANT 0.0001

using namespace Eigen;

Matrix<double, 3, 2> wuvMatrix(Cloth &, int, int, int);

// TODO add changeable stretch factors
double scaleXCondition(Cloth &, int *);
double scaleXCondition(Cloth &, int, int, int);
RowVector3d scaleXPartial(Cloth &, int, int *);
RowVector3d scaleXPartial(Cloth &, int, int, int, int);
double scaleYCondition(Cloth &, int *);
double scaleYCondition(Cloth &, int, int, int);
RowVector3d scaleYPartial(Cloth &, int, int *);
RowVector3d scaleYPartial(Cloth &, int, int, int, int);
Matrix3d scaleXSecondPartial(Cloth &, int, int, int *);
Matrix3d scaleYSecondPartial(Cloth &, int, int, int *);

double shearCondition(Cloth &, int *);
double shearCondition(Cloth &, int, int, int);
RowVector3d shearPartial(Cloth &, int, int *);
RowVector3d shearPartial(Cloth &, int, int, int, int);

double bendCondition(Cloth &, int *);
double bendCondition(Cloth &, int, int, int, int);
RowVector3d bendPartial(Cloth &, int, int *);
RowVector3d bendPartial(Cloth &, int, int, int, int, int);
