#pragma once

#include <string.h>
#include <iostream>

#include "cloth.h"
#include "conditions.h"

#define LOCK_TOP_POINTS false

#define SCALE_STIFFNESS 1
#define SHEAR_STIFFNESS 1
#define BEND_STIFFNESS 0.001
#define DAMP_STIFFNESS 0.3

class Simulation {
	void handleScaleCondition(int, double *, double *);
	void handleShearCondition(int, double *);
	void handleBendCondition(int, double *);
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
