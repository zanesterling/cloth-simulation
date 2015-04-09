#pragma once

#include <Eigen/Sparse>

#include <string.h>
#include <iostream>

#include "cloth.h"
#include "conditions.h"

#define LOCK_TOP_POINTS false

#define SCALE_STIFFNESS 1
#define SHEAR_STIFFNESS 1
#define BEND_STIFFNESS 0.001
#define DAMP_STIFFNESS 0.3

#define TIMESTEP 1

typedef Matrix<Vector3d, Dynamic, Dynamic> ForceMatrix;

class Simulation {
	void handleScaleCondition(int, ForceMatrix &);
	void handleShearCondition(int, ForceMatrix &);
	void handleBendCondition(int,  ForceMatrix &);
	double *genTrisFromMesh();
	void copyPoint(double *, double *, int);

public:
	Cloth cloth;
	double *triVerts;
	bool running = false;

	Simulation(int, int);

	void update();
	void reset();
	int getNumTris();
};
