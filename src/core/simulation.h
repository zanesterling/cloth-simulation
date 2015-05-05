#pragma once

#include <Eigen/Sparse>

#include <string.h>
#include <iostream>
#include <random>

#include "cloth.h"
#include "conditions.h"

#define LOCK_TOP_POINTS true
#define GRAVITY_ENABLED true

#define SCALE_STIFF 30
#define DAMP_STIFF  2
#define SHEAR_STIFF 1
#define BEND_STIFF  0.001

#define TIMESTEP 1

#define GRAVITY_ACCEL 0.0001

typedef Matrix<Vector3d, Dynamic, Dynamic> ForceMatrix;
typedef Matrix<double, Dynamic, Dynamic> ForcePartialMatrix;

class Simulation {
	void handleScaleCondition(int);
	void scaleHelper(int *, bool);

	void handleShearCondition(int);
	void shearHelper(int *, bool);

	void handleBendCondition(int);
	void bendHelper(int *);

	double *genTrisFromMesh();
	void copyPoint(double *, double *, int);

public:
	Cloth cloth;
	double *triVerts;
	bool running = false;

	ForceMatrix forces;
	ForcePartialMatrix forcePartialX;

	Simulation(int, int);

	void update();
	void reset();
	int getNumTris();
};
