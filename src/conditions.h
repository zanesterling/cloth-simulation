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
typedef Eigen::Vector2d Vector2d;

WuvMatrix wuvMatrix(Cloth &, int, int, int);

Vector2d scaleCondition(Cloth &, int *, bool, Vector2d);
Eigen::Matrix<double, 3, 2> scalePartial(
	Cloth &cloth,
	int pt,
	int *tri,
	bool isBl,
	Vector2d buv
);

double shearCondition(Cloth &, int *, bool);
RowVector3d shearPartial(Cloth &, int, int *, bool);

double bendCondition(Cloth &, int *);
double bendCondition(Cloth &, int, int, int, int);
RowVector3d bendPartial(Cloth &, int, int *);
RowVector3d bendPartial(Cloth &, int, int, int, int, int);
