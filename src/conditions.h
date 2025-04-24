#pragma once

#include <Eigen/Dense>

#include <algorithm>

#include "cloth.h"

// the amount by which we perturb the points in finding partials
#define PERTURB_QUANT 0.0001

using namespace Eigen;

typedef Matrix<double, 3, 2> WuvMatrix;

WuvMatrix wuvMatrix(Cloth &, int, int, int);

double scaleXCondition(Cloth &, int *, bool, double);
double scaleXCondition(Cloth &, int, int, int, bool, double);
RowVector3d scaleXPartial(Cloth &, int, int *, bool, double);
RowVector3d scaleXPartial(Cloth &, int, int, int, int, bool, double);
double scaleYCondition(Cloth &, int *, bool, double);
double scaleYCondition(Cloth &, int, int, int, bool, double);
RowVector3d scaleYPartial(Cloth &, int, int *, bool, double);
RowVector3d scaleYPartial(Cloth &, int, int, int, int, bool, double);

double shearCondition(Cloth &, int *, bool);
double shearCondition(Cloth &, int, int, int, bool);
RowVector3d shearPartial(Cloth &, int, int *, bool);
RowVector3d shearPartial(Cloth &, int, int, int, int, bool);

double bendCondition(Cloth &, int *);
double bendCondition(Cloth &, int, int, int, int);
RowVector3d bendPartial(Cloth &, int, int *);
RowVector3d bendPartial(Cloth &, int, int, int, int, int);
