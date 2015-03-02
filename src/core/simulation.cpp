#include "simulation.h"

Simulation::Simulation(int clothWidth, int clothHeight)
	: cloth(Cloth(clothWidth, clothHeight)) {}

void Simulation::update() {}

GLfloat *Simulation::genTrisFromMesh() {
	GLfloat *tris = new GLfloat[3 * getNumTris()];

	for (int i = 0; i < cloth.h - 1; i++) {
		for (int j = 0 ; j < cloth.w - 1; j++) {
			tris[6 * (i*cloth.w + j)]     = cloth.worldPoint(j,   i);
			tris[6 * (i*cloth.w + j) + 1] = cloth.worldPoint(j,   i+1);
			tris[6 * (i*cloth.w + j) + 2] = cloth.worldPoint(j+1, i);

			tris[6 * (i*cloth.w + j) + 5] = cloth.worldPoint(j+1, i);
			tris[6 * (i*cloth.w + j) + 4] = cloth.worldPoint(j,   i+1);
			tris[6 * (i*cloth.w + j) + 3] = cloth.worldPoint(j+1, i+1);
		}
	}
}

int Simulation::getNumTris() {
	return 2 * (cloth.w - 1) * (cloth.h - 1);
}
