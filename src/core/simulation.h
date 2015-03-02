#pragma once

#include "cloth.h"

class Simulation {
public:
	Cloth cloth;
	GLfloat *triVerts;

	Simulation(int, int);

	void update();
	int getNumTris();
	GLfloat *genTrisFromMesh();
};
