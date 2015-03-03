#include "simulation.h"

Simulation::Simulation(int clothWidth, int clothHeight)
	: cloth(Cloth(clothWidth, clothHeight)) {
	triVerts = genTrisFromMesh();
}

void Simulation::update() {}

GLfloat *Simulation::genTrisFromMesh() {
	GLfloat *tris = new GLfloat[9 * getNumTris()];

	for (int i = 0; i < cloth.h - 1; i++) {
		for (int j = 0 ; j < cloth.w - 1; j++) {
			copyPoint(tris + 18 * (i*cloth.w + j),
			          cloth.worldPoint(j, i), 3);
			copyPoint(tris + 18 * (i*cloth.w + j) + 3,
			          cloth.worldPoint(j, i+1), 3);
			copyPoint(tris + 18 * (i*cloth.w + j) + 6,
			          cloth.worldPoint(j+1, i), 3);

			copyPoint(tris + 18 * (i*cloth.w + j) + 9,
			          cloth.worldPoint(j+1, i), 3);
			copyPoint(tris + 18 * (i*cloth.w + j) + 12,
			          cloth.worldPoint(j, i+1), 3);
			copyPoint(tris + 18 * (i*cloth.w + j) + 15,
			          cloth.worldPoint(j+1, i+1), 3);
		}
	}

	return tris;
}

int Simulation::getNumTris() {
	return 2 * (cloth.w - 1) * (cloth.h - 1);
}

void Simulation::copyPoint(GLfloat *dest, GLfloat *src, int dim) {
	memcpy(dest, src, dim * sizeof(GLfloat));
}
