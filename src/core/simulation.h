#pragma once

#include <string.h>

#include "cloth.h"

class Simulation {
public:
	Cloth cloth;
	GLfloat *triVerts;

	Simulation(int, int);

	void update();
	GLfloat *genTrisFromMesh();
	int getNumTris();
	void copyPoint(GLfloat *, GLfloat *, int);
};
