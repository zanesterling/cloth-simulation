#include "simulation.h"

using namespace std;
Simulation::Simulation(int clothXRes, int clothYRes)
	: cloth(Cloth(clothXRes, clothYRes)) {
	reset();

	forces.resize(1, getNumPoints());
}

void copyPoint(double *dest, double *src) {
	memcpy(dest, src, 3 * sizeof(double));
}

// triangles must be a pointer to an array of floats big enough
// to store 18 floats per cloth square.
void copyTrianglesFromMesh(double *triangles, Cloth& cloth) {
	for (int i = 0; i < cloth.yRes - 1; i++) {
		for (int j = 0 ; j < cloth.xRes - 1; j++) {
			double *triPairStart = triangles + 18 * (i*(cloth.xRes-1) + j);
			copyPoint(triPairStart,      cloth.getWorldPoint(j,   i));
			copyPoint(triPairStart + 3,  cloth.getWorldPoint(j+1, i));
			copyPoint(triPairStart + 6,  cloth.getWorldPoint(j,   i+1));

			copyPoint(triPairStart + 9,  cloth.getWorldPoint(j,   i+1));
			copyPoint(triPairStart + 12, cloth.getWorldPoint(j+1, i));
			copyPoint(triPairStart + 15, cloth.getWorldPoint(j+1, i+1));
		}
	}
}

void Simulation::update() {
	// if simulation is paused, don't update
	if (!running) return;

	// zero forces matrix
	for (int pt = 0; pt < cloth.xRes * cloth.yRes; pt++) {
		forces(0, pt).setZero();
	}

	// get condition-forces 
	for (int i = 0; i < cloth.yRes-1; i++) {
		for (int j = 0; j < cloth.xRes-1; j++) {
			int offset = i * cloth.xRes + j;
			handleScaleCondition(offset);
			handleShearCondition(offset);
			handleBendCondition (offset);
		}
	}

	for (int i = 0; i < cloth.xRes * cloth.yRes; i++) {
		// update velocities by condition-forces
		for (int j = 0; j < 3; j++) {
			cloth.worldVels[i*3 + j] += forces(0, i)[j] *
			                            cloth.massPerVertI;
		}

		// apply gravitic acceleration
		cloth.worldVels[i*3 + 1] -= GRAVITY_ACCEL;

		// lock floor as lower bound
		if (FLOOR_ENABLED) {
			if (cloth.worldPoints[i*3 + 1] <= FLOOR_HEIGHT &&
				cloth.worldVels[i*3 + 1] < 0) {
				cloth.worldVels[i*3 + 1] = 0;
			}
		}
	}

	// lock the top row of points, if we've enabled that setting
	int lastPoint = 3 * cloth.xRes * cloth.yRes;
	if (LOCK_TOP_ROW) lastPoint -= 3 * cloth.xRes;

	// move the points by their velocities
	for (int i = 0; i < lastPoint; i++) {
		cloth.worldPoints[i] += cloth.worldVels[i] * TIMESTEP;
	}

	// generate new triangles from the mesh
	copyTrianglesFromMesh(triVerts, cloth);
	if (norms) delete norms;
	norms = genTriNorms();
}

void Simulation::reset() {
	// regenerate cloth
	cloth = Cloth(cloth.xRes, cloth.yRes);
	maxScale = MAX_SCALE * 30 * 30 / (cloth.xRes * cloth.yRes);

	random_device rd;
	mt19937 gen(rd());
	uniform_real_distribution<> dis(0, 1);

	switch (RESET_SCENE) {
		case RESET_PERTURB:
			// perturb cloth f
			for (int i = 0; i < cloth.yRes - 1; i++) {
				for (int j = 0; j < cloth.xRes; j++) {
					cloth.getWorldPoint(j, i)[0] += dis(gen) / 80;
					cloth.getWorldPoint(j, i)[1] += dis(gen) / 80;
					cloth.getWorldPoint(j, i)[2] += dis(gen) / 80;
				}
			}
			break;
		case RESET_BEND:
			for (int i = 0; i < cloth.yRes / 2; i++) {
				for (int j = 0; j < cloth.xRes; j++) {
					cloth.worldPoints[(i * cloth.xRes + j) * 3 + 1] +=
						(cloth.yRes / 2 - i) * 0.001;
					cloth.worldPoints[(i * cloth.xRes + j) * 3 + 2] -=
						(cloth.yRes / 2 - i) * 0.01;
				}
			}
			break;
		case RESET_SWING:
			for (int i = 0; i < cloth.yRes; i++) {
				for (int j = 0; j < cloth.xRes; j++) {
					auto point = cloth.getWorldPoint(j, i);
					swap(point[1], point[2]);
					point[1] += 0.5;
					point[2] -= 0.5;
				}
			}
			break;
		case RESET_HANG:
			// do nothing. leave cloth hanging straight.
			break;
	}

	// regenerate triangles from the mesh
	triVerts = new double[9 * getNumTris()];
	copyTrianglesFromMesh(triVerts, cloth);
	norms = genTriNorms();

	scaleX = 1;
	scaleY = 1;
	cuffScale = 1;
}

void Simulation::handleScaleCondition(int offset) {
	int botLeftTri[3]  = {offset, offset + 1, offset + cloth.xRes};
	int topRightTri[3] = {offset + cloth.xRes, offset + 1,
	                offset + cloth.xRes + 1};

	int x = offset % cloth.xRes;
	int y = offset / cloth.xRes;

	scaleHelper(botLeftTri, true, y);
	scaleHelper(topRightTri, false, y);
}

void Simulation::scaleHelper(int *triPts, bool isBl, int y) {
	double stretchX = scaleX;
	double stretchY = scaleY;
	if (bannerStyle) {
		stretchX = 0.5;
		stretchY = 1;
	}
	if (y < 5) {
		stretchX = cuffScale * y / 5. + 1 * (5 - y) / 5.;
	}
	if (cuffing) {
		if (y == 0)
			stretchX = 0.3;
		else if (y == 1)
			stretchX = 0.5;
	}

	auto condX = scaleXCondition(cloth, triPts, isBl, stretchX);
	auto condY = scaleYCondition(cloth, triPts, isBl, stretchY);
	for (int i = 0; i < 3; i++) {
		int ptI = triPts[i];

		// acount for scaling force
		auto partialIX = scaleXPartial(cloth, ptI, triPts, isBl, stretchX);
		auto partialIY = scaleYPartial(cloth, ptI, triPts, isBl, stretchY);
		Vector3d force = -SCALE_STIFF * (partialIX.transpose() * condX +
		                                 partialIY.transpose() * condY);
		for (int j; j < 3; j++) {
			if (force[j] >  maxScale) force[j] =  maxScale;
			if (force[j] < -maxScale) force[j] = -maxScale;
		}
		forces(0, ptI) += force;

		// account for damping force
		auto velI = Vector3d(cloth.getWorldVel(ptI));
		Vector3d dampForce = -DAMP_STIFF *
		                 (partialIX.transpose() * partialIX +
		                  partialIY.transpose() * partialIY) * velI;
		for (int j; j < 3; j++) {
			if (abs(dampForce[j]) > abs(velI[j] + force[j]))
				dampForce[j] = -(velI[j] + force[j]);
		}
		forces(0, ptI) += dampForce;
	}
}

void Simulation::handleShearCondition(int offset) {
	int botLeftTri[3] = {offset, offset + 1, offset + cloth.xRes};
	int topRightTri[3] = {offset + cloth.xRes, offset + 1,
			              offset + cloth.xRes + 1};

	shearHelper(botLeftTri, true);
	shearHelper(topRightTri, false);
}

void Simulation::shearHelper(int *triPts, bool isBl) {
	auto cond = shearCondition(cloth, triPts, isBl);

	for (int i = 0; i < 3; i++) {
		int ptI = triPts[i];

		auto partialI = shearPartial(cloth, ptI, triPts, isBl);
		auto force = -SHEAR_STIFF * partialI.transpose() * cond;
		forces(0, ptI) += force;
	}
}

void Simulation::handleBendCondition(int offset) {
	int xOff = offset % cloth.xRes;
	int yOff = offset / cloth.xRes;

	// diagonal triangle pair
	int diagPts[4] = {
		offset,
		offset + 1,
		offset + cloth.xRes,
		offset + cloth.xRes + 1
	};

	// right-side triangle pair
	int rightPts[4] = {
		offset + cloth.xRes,
		offset + 1,
		offset + cloth.xRes + 1,
		offset + 2
	};

	// top-side triangle pair
	int topPts[4] = {
		offset + 1,
		offset + cloth.xRes + 1,
		offset + cloth.xRes,
		offset + 2 * cloth.xRes
	};

	bendHelper(diagPts);

	if (xOff < cloth.xRes - 2)
		bendHelper(rightPts);

	if (yOff < cloth.yRes - 2)
		bendHelper(topPts);
}

void Simulation::bendHelper(int *tris) {
	auto cond = bendCondition(cloth, tris);

	for (int i = 0; i < 4; i++) {
		int ptI = tris[i];

		auto partialI = bendPartial(cloth, ptI, tris);
		Vector3d force = -BEND_STIFF * partialI.transpose() * cond;
		for (int j = 0; j < 3; j++) {
			if (force[j] > MAX_BEND) force[j] = MAX_BEND;
			if (force[j] < -MAX_BEND) force[j] = -MAX_BEND;
		}
		forces(0, ptI) += force;
	}
}

double *Simulation::genNorms() {
	// zero new norms
	double *norms = new double[3 * getNumPoints()];
	for (int i = 0; i < 3 * getNumPoints(); i++) norms[i] = 0;

	// additively generate norms per triangle
	for (int y = 0; y < cloth.yRes - 1; y++) {
		for (int x = 0; x < cloth.xRes - 1; x++) {
			auto p1 = Vector3d(cloth.getWorldPoint(x, y));
			auto p2 = Vector3d(cloth.getWorldPoint(x+1, y));
			auto p3 = Vector3d(cloth.getWorldPoint(x, y+1));
			auto norm = (p2 - p1).cross(p3 - p1);
			norm.normalize();
			for (int i = 0; i < 3; i++) {
				norms[(y     * cloth.xRes + x)     * 3 + i] += norm[i];
				norms[(y     * cloth.xRes + (x+1)) * 3 + i] += norm[i];
				norms[((y+1) * cloth.xRes + x)     * 3 + i] += norm[i];
			}

			p1 = Vector3d(cloth.getWorldPoint(x, y+1));
			p2 = Vector3d(cloth.getWorldPoint(x+1, y));
			p3 = Vector3d(cloth.getWorldPoint(x+1, y+1));
			norm = (p2 - p1).cross(p3 - p1);
			norm.normalize();
			for (int i = 0; i < 3; i++) {
				norms[((y+1) * cloth.xRes + x)     * 3 + i] += norm[i];
				norms[(y     * cloth.xRes + (x+1)) * 3 + i] += norm[i];
				norms[((y+1) * cloth.xRes + (x+1)) * 3 + i] += norm[i];
			}
		}
	}

	// normalize each point's norm (for smoother jazz)
	for (int y = 0; y < cloth.yRes; y++) {
		for (int x = 0; x < cloth.xRes; x++) {
			auto norm = norms + 3 * (y * cloth.xRes + x);
			auto normv = Vector3d(norm);
			normv.normalize();
			for (int i = 0; i < 3; i++)
				norm[i] = normv[i];
		}
	}

	return norms;
}

double *Simulation::genTriNorms() {
	auto norms = genNorms();

	double *triNorms = new double[9 * getNumTris()];
	for (int y = 0; y < cloth.yRes - 1; y++) {
		for (int x = 0; x < cloth.xRes - 1; x++) {
			auto start = triNorms + 18 * (y * (cloth.xRes - 1) + x);
			copyPoint(start,      norms + (y     * cloth.xRes + x)    * 3);
			copyPoint(start + 3,  norms + (y     * cloth.xRes + (x+1))* 3);
			copyPoint(start + 6,  norms + ((y+1) * cloth.xRes + x)    * 3);

			copyPoint(start + 9,  norms + ((y+1) * cloth.xRes + x)    * 3);
			copyPoint(start + 12, norms + (y     * cloth.xRes + (x+1))* 3);
			copyPoint(start + 15, norms + ((y+1) * cloth.xRes + (x+1))* 3);
		}
	}
	
	delete norms;
	return triNorms;
}
