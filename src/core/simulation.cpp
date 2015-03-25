#include "simulation.h"

using namespace std;
Simulation::Simulation(int clothWidth, int clothHeight)
	: cloth(Cloth(clothWidth, clothHeight)) {
	cloth.worldPoints[3 + 1] += 0.03;
	triVerts = genTrisFromMesh();
}

void Simulation::update() {
	for (int i = 0; i < cloth.yRes-1; i++) {
		for (int j = 0; j < cloth.xRes-1; j++) {
			int offset = i * cloth.xRes + j;
			handleScaleCondition(offset);
			handleShearCondition(offset);
			handleBendCondition(offset);
		}
	}

	for (int i = 0; i < cloth.xRes * cloth.yRes; i++) {
		cloth.worldPoints[i] += cloth.worldVels[i];
	}

	triVerts = genTrisFromMesh();
}

void Simulation::handleScaleCondition(int offset) {
	// bottom-left triangle
	int blPoints[3] = {offset, offset + 1, offset + cloth.xRes};
	auto scCond = scaleCondition(cloth, blPoints);

	for (int pt : blPoints) {
		auto scPart = scalePartial(cloth, pt, blPoints);
		auto force = -scPart.transpose() * scCond;
		cloth.worldVels[pt*3 + 0] += force[0];
		cloth.worldVels[pt*3 + 1] += force[1];
		cloth.worldVels[pt*3 + 2] += force[2];
	}

	// top-right triangle
	int trPoints[3] = {offset + cloth.xRes, offset + 1,
			           offset + cloth.xRes + 1};
	scCond = scaleCondition(cloth, trPoints);

	for (int pt : trPoints) {
		auto scPart = scalePartial(cloth, pt, trPoints);
		auto force = -scPart.transpose() * scCond;
		cloth.worldVels[pt*3 + 0] += force[0];
		cloth.worldVels[pt*3 + 1] += force[1];
		cloth.worldVels[pt*3 + 2] += force[2];
	}
}

void Simulation::handleShearCondition(int offset) {
	// bottom-left triangle
	int blPoints[3] = {offset, offset + 1, offset + cloth.xRes};
	auto shCond = shearCondition(cloth, blPoints);

	for (int pt : blPoints) {
		auto shPart = shearPartial(cloth, pt, blPoints);
		auto force = -shPart.transpose() * shCond;
		cloth.worldVels[pt*3 + 0] += force[0];
		cloth.worldVels[pt*3 + 1] += force[1];
		cloth.worldVels[pt*3 + 2] += force[2];
	}

	for (int pt : blPoints) {
		cloth.worldPoints[pt*3 + 0] += cloth.worldVels[pt*3 + 0];
		cloth.worldPoints[pt*3 + 1] += cloth.worldVels[pt*3 + 1];
		cloth.worldPoints[pt*3 + 2] += cloth.worldVels[pt*3 + 2];
	}

	// top-right triangle
	int trPoints[3] = {offset + cloth.xRes, offset + 1,
			           offset + cloth.xRes + 1};
	shCond = shearCondition(cloth, trPoints);

	for (int pt : trPoints) {
		auto shPart = shearPartial(cloth, pt, trPoints);
		auto force = -shPart.transpose() * shCond;
		cloth.worldVels[pt*3 + 0] += force[0];
		cloth.worldVels[pt*3 + 1] += force[1];
		cloth.worldVels[pt*3 + 2] += force[2];
	}
}

void Simulation::handleBendCondition(int offset) {
	// bottom-left triangle
	int blPoints[4] = {
		offset,
		offset + 1,
		offset + cloth.xRes,
		offset + cloth.xRes + 1
	};
	auto bdCond = bendCondition(cloth, blPoints);

	for (int pt : blPoints) {
		auto bdPart = bendPartial(cloth, pt, blPoints);
		auto force = -bdPart.transpose() * bdCond;
		cloth.worldVels[pt*3 + 0] += force[0];
		cloth.worldVels[pt*3 + 1] += force[1];
		cloth.worldVels[pt*3 + 2] += force[2];
	}
}

double *Simulation::genTrisFromMesh() {
	double *tris = new double[9 * getNumTris()];

	// copy in triangles
	for (int i = 0; i < cloth.yRes - 1; i++) {
		for (int j = 0 ; j < cloth.xRes - 1; j++) {
			double *triPairStart = tris + 18 * (i*(cloth.xRes-1) + j);

			copyPoint(triPairStart,      cloth.getWorldPoint(j,   i),   3);
			copyPoint(triPairStart + 3,  cloth.getWorldPoint(j,   i+1), 3);
			copyPoint(triPairStart + 6,  cloth.getWorldPoint(j+1, i),   3);

			copyPoint(triPairStart + 9,  cloth.getWorldPoint(j+1, i),   3);
			copyPoint(triPairStart + 12, cloth.getWorldPoint(j,   i+1), 3);
			copyPoint(triPairStart + 15, cloth.getWorldPoint(j+1, i+1), 3);
		}
	}

	return tris;
}

int Simulation::getNumTris() {
	return 2 * (cloth.xRes - 1) * (cloth.yRes - 1);
}

void Simulation::copyPoint(double *dest, double *src, int dim) {
	memcpy(dest, src, dim * sizeof(double));
}
