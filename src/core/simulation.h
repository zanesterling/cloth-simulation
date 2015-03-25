#pragma once

#include <string.h>
#include <iostream>

#include "cloth.h"
#include "conditions.h"

class Simulation {
	void handleScaleCondition(int);
	void handleShearCondition(int);
	void handleBendCondition(int);
	double *genTrisFromMesh();
	void copyPoint(double *, double *, int);

public:
	Cloth cloth;
	double *triVerts;

	Simulation(int, int);

	void update();
	int getNumTris();
};
