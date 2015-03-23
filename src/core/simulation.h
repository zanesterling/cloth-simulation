#pragma once

#include <string.h>
#include <iostream>

#include "cloth.h"
#include "conditions.h"

class Simulation {
public:
	Cloth cloth;
	double *triVerts;

	Simulation(int, int);

	void update();
	double *genTrisFromMesh();
	int getNumTris();
	void copyPoint(double *, double *, int);
};
