#pragma once

#include <Eigen/Dense>

#include <algorithm>

#include "cloth.h"

// the amount by which we perturb the points in finding partials
#define PERTURB_QUANT 0.0001

typedef Eigen::Matrix<double, 3, 2> WuvMatrix;
typedef Eigen::Matrix2d Matrix2d;
typedef Eigen::RowVector3d RowVector3d;
typedef Eigen::Vector3d Vector3d;

WuvMatrix wuvMatrix(Cloth &, int, int, int);

double scaleUCondition(Cloth &, int *, bool, double);
double scaleUCondition(Cloth &, int, int, int, bool, double);
RowVector3d scaleUPartial(Cloth &, int, int *, bool, double);
RowVector3d scaleUPartial(Cloth &, int, int, int, int, bool, double);
double scaleVCondition(Cloth &, int *, bool, double);
double scaleVCondition(Cloth &, int, int, int, bool, double);
RowVector3d scaleVPartial(Cloth &, int, int *, bool, double);
RowVector3d scaleVPartial(Cloth &, int, int, int, int, bool, double);

double shearCondition(Cloth &, int *, bool);
double shearCondition(Cloth &, int, int, int, bool);
RowVector3d shearPartial(Cloth &, int, int *, bool);
RowVector3d shearPartial(Cloth &, int, int, int, int, bool);

double bendCondition(Cloth &, int *);
double bendCondition(Cloth &, int, int, int, int);
RowVector3d bendPartial(Cloth &, int, int *);
RowVector3d bendPartial(Cloth &, int, int, int, int, int);
