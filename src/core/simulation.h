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

class Simulation {
	void handleScaleCondition(int, Matrix<Vector3d, Dynamic, Dynamic> &,
	                          double *);
	void handleShearCondition(int, Matrix<Vector3d, Dynamic, Dynamic> &);
	void handleBendCondition(int,  Matrix<Vector3d, Dynamic, Dynamic> &);
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
