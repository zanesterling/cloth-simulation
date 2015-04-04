#pragma once

#include <string.h>
#include <iostream>

#include "cloth.h"
#include "conditions.h"

#define LOCK_TOP_POINTS true
#define BEND_STIFFNESS 0.01

class Simulation {
	void handleScaleCondition(int);
	void handleShearCondition(int);
	void handleBendCondition(int);
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
