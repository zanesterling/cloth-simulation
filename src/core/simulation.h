#pragma once

#include <Eigen/Sparse>

#include <string.h>
#include <iostream>
#include <random>

#include "cloth.h"
#include "conditions.h"

#define LOCK_TOP_POINTS false
#define GRAVITY_ENABLED true
#define FLOOR_ENABLED true

#define SCALE_STIFF 30
#define DAMP_STIFF  2
#define SHEAR_STIFF 1
#define BEND_STIFF  0.0001

#define TIMESTEP 1

#define GRAVITY_ACCEL 0.0001

#define RESET_PERTURB 0
#define RESET_BEND    1
#define RESET_SWING   2
#define RESET_SCENE RESET_BEND

#define FLOOR_HEIGHT -1

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
