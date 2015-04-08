#include "simulation.h"

using namespace std;
Simulation::Simulation(int clothXRes, int clothYRes)
	: cloth(Cloth(clothXRes, clothYRes)) {
	reset();

	triVerts = genTrisFromMesh();
}

void Simulation::update() {
	// if simulation is paused, don't update
	if (!running) return;

	// zero new forces matrix
	Matrix<Vector3d, Dynamic, Dynamic> forces(1, cloth.xRes * cloth.yRes);
	for (int pt = 0; pt < cloth.xRes * cloth.yRes; pt++) {
		forces(0, pt)[0] = 0;
		forces(0, pt)[1] = 0;
		forces(0, pt)[2] = 0;
	}

	// get condition-forces and update velocities accordingly
	for (int i = 0; i < cloth.yRes-1; i++) {
		for (int j = 0; j < cloth.xRes-1; j++) {
			int offset = i * cloth.xRes + j;
			handleScaleCondition(offset, forces);
			handleShearCondition(offset, forces);
			handleBendCondition (offset, forces);
		}
	}
	for (int i = 0; i < 3 * cloth.xRes * cloth.yRes; i++) {
		cloth.worldVels[i] += forces(0, i / 3)[i % 3];
	}

	// lock the top row of points, if we've enabled that setting
	int lastPoint = 3 * cloth.xRes * cloth.yRes;
	if (LOCK_TOP_POINTS) lastPoint -= 3 * cloth.xRes;

	// move the points by their velocities
	for (int i = 0; i < lastPoint; i++) {
		cloth.worldPoints[i] += cloth.worldVels[i];
	}

	// generate new triangles from the mesh
	triVerts = genTrisFromMesh();
}

void Simulation::reset() {
	// regenerate cloth
	cloth = Cloth(cloth.xRes, cloth.yRes);

	// perturb cloth for the test case
	for (int i = 0; i < 10; i++) {
		for (int j = 0; j < cloth.xRes; j++) {
			cloth.worldPoints[(i * cloth.xRes + j)*3 + 1] -= (10-i) * 0.01;
			cloth.worldPoints[(i * cloth.xRes + j)*3 + 2] -= (10-i) * 0.01;
		}
	}

	// regenerate triangles from the mesh
	triVerts = genTrisFromMesh();
}

void Simulation::handleScaleCondition(int offset,
                                      Matrix<Vector3d, Dynamic, Dynamic> &forces) {
	// bottom-left triangle
	int blPoints[3] = {offset, offset + 1, offset + cloth.xRes};
	auto scCond = scaleCondition(cloth, blPoints);

	for (int pt : blPoints) {
		auto scPart = scalePartial(cloth, pt, blPoints);
		auto force = -SCALE_STIFFNESS * scPart.transpose() * scCond;
		forces(0, pt) += force;

		auto vel = Vector3d(cloth.getWorldVel(pt));
		auto dampForce = -DAMP_STIFFNESS * scPart.transpose() * scPart *
		                  vel;
		forces(0, pt) += dampForce;
	}

	// top-right triangle
	int trPoints[3] = {offset + cloth.xRes, offset + 1,
			           offset + cloth.xRes + 1};
	scCond = scaleCondition(cloth, trPoints);

	for (int pt : trPoints) {
		auto scPart = scalePartial(cloth, pt, trPoints);
		auto force = -SCALE_STIFFNESS * scPart.transpose() * scCond;
		forces(0, pt) += force;

		auto vel = Vector3d(cloth.getWorldVel(pt));
		auto dampForce = -DAMP_STIFFNESS * scPart.transpose() * scPart *
		                  vel;
		forces(0, pt) += dampForce;
	}
}

void Simulation::handleShearCondition(int offset,
                                      Matrix<Vector3d, Dynamic, Dynamic> &forces) {
	// bottom-left triangle
	int blPoints[3] = {offset, offset + 1, offset + cloth.xRes};
	auto shCond = shearCondition(cloth, blPoints);

	for (int pt : blPoints) {
		auto shPart = shearPartial(cloth, pt, blPoints);
		auto force = -SHEAR_STIFFNESS * shPart.transpose() * shCond;
		forces(0, pt) += force;
	}

	// top-right triangle
	int trPoints[3] = {offset + cloth.xRes, offset + 1,
			           offset + cloth.xRes + 1};
	shCond = shearCondition(cloth, trPoints);

	for (int pt : trPoints) {
		auto shPart = shearPartial(cloth, pt, trPoints);
		auto force = -SHEAR_STIFFNESS * shPart.transpose() * shCond;
		forces(0, pt) += force;
	}
}

void Simulation::handleBendCondition(int offset,
                                     Matrix<Vector3d, Dynamic, Dynamic> &forces) {
	int xOff = offset % cloth.xRes;
	int yOff = offset / cloth.xRes;

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
		auto force = -BEND_STIFFNESS * bdPart.transpose() * bdCond;
		forces(0, pt) += force;
	}

	if (xOff < cloth.xRes - 2) {
		// right-side triangle
		int rtPoints[4] = {
			offset + cloth.xRes,
			offset + 1,
			offset + cloth.xRes + 1,
			offset + 2
		};
		bdCond = bendCondition(cloth, rtPoints);

		for (int pt : rtPoints) {
			auto bdPart = bendPartial(cloth, pt, rtPoints);
			auto force = -BEND_STIFFNESS * bdPart.transpose() * bdCond;
			forces(0, pt) += force;
		}
	}

	if (yOff < cloth.yRes - 2) {
		// top-side triangle
		int tpPoints[4] = {
			offset + 1,
			offset + cloth.xRes + 1,
			offset + cloth.xRes,
			offset + 2 * cloth.xRes
		};
		bdCond = bendCondition(cloth, tpPoints);

		for (int pt : tpPoints) {
			auto bdPart = bendPartial(cloth, pt, tpPoints);
			auto force = -BEND_STIFFNESS * bdPart.transpose() * bdCond;
			forces(0, pt) += force;
		}
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
