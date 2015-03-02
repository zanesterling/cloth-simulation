#pragma once

#include "cloth.h"

class Simulation {
	Cloth cloth;
public:
	GLfloat *triVerts;

	Simulation(int, int);

	void update();
	int getNumTris();
	GLfloat *genTrisFromMesh();
};
