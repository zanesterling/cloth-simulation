#include "simulation.h"

using namespace std;
Simulation::Simulation(int clothXRes, int clothYRes)
	: cloth(Cloth(clothXRes, clothYRes)) {
	reset();

	int numPoints = getNumPoints();
	totalForces.resize(1, numPoints);
	scaleForces.resize(1, numPoints);
	scaleDampForces.resize(1, numPoints);
	shearForces.resize(1, numPoints);
	shearDampForces.resize(1, numPoints);
	bendForces.resize(1, numPoints);
	bendDampForces.resize(1, numPoints);
}

void copyPoint(double *dest, double *src) {
	memcpy(dest, src, 3 * sizeof(double));
}

inline void copyForce(double *dest, Vector3d force) {
	dest[0] = force[0];
	dest[1] = force[1];
	dest[2] = force[2];
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

// Matrix is of shape (1, nPoints).
void copyForcesFromMatrix(double *dest, ForceMatrix &matrix, int xRes, int yRes) {
	for (int squareY = 0; squareY < yRes - 1; squareY++) {
		for (int squareX = 0; squareX < xRes - 1; squareX++) {
			int squareIndex = squareX + squareY * (xRes-1);
			double *triPairStart = dest + 18 * squareIndex;
			copyForce(triPairStart,      matrix(0, squareX   + xRes*(squareY)));
			copyForce(triPairStart + 3,  matrix(0, squareX+1 + xRes*(squareY)));
			copyForce(triPairStart + 6,  matrix(0, squareX   + xRes*(squareY+1)));

			copyForce(triPairStart + 9,  matrix(0, squareX   + xRes*(squareY+1)));
			copyForce(triPairStart + 12, matrix(0, squareX   + xRes*(squareY)));
			copyForce(triPairStart + 15, matrix(0, squareX+1 + xRes*(squareY+1)));
		}
	}
}

void copyTriangleNormsFromClothNorms(double *triangleNorms, double *clothNorms, Cloth& cloth) {
	for (int y = 0; y < cloth.yRes - 1; y++) {
		for (int x = 0; x < cloth.xRes - 1; x++) {
			auto start = triangleNorms + 18 * (y * (cloth.xRes - 1) + x);
			copyPoint(start,      clothNorms + (y     * cloth.xRes + x)    * 3);
			copyPoint(start + 3,  clothNorms + (y     * cloth.xRes + (x+1))* 3);
			copyPoint(start + 6,  clothNorms + ((y+1) * cloth.xRes + x)    * 3);

			copyPoint(start + 9,  clothNorms + ((y+1) * cloth.xRes + x)    * 3);
			copyPoint(start + 12, clothNorms + (y     * cloth.xRes + (x+1))* 3);
			copyPoint(start + 15, clothNorms + ((y+1) * cloth.xRes + (x+1))* 3);
		}
	}
}

void generateClothNormsFromMesh(double* clothNorms, Cloth& cloth) {
	int numPoints = cloth.xRes * cloth.yRes;
	memset(clothNorms, 0, 3 * numPoints * sizeof(*clothNorms));

	// We're computing here a normal for each point in the mesh.
	// We define that to be the average of the normals of the eight triangles
	// that it's included in.
	// To compute that average we find the normal for each triangle and add
	// it into the per-point normal for each of the triangle's three vertices,
	// then normalize the resulting vector.
	//
	// This loop is that first step of summing up triangle normals.
	for (int y = 0; y < cloth.yRes - 1; y++) {
		for (int x = 0; x < cloth.xRes - 1; x++) {
			auto p1 = Vector3d(cloth.getWorldPoint(x, y));
			auto p2 = Vector3d(cloth.getWorldPoint(x+1, y));
			auto p3 = Vector3d(cloth.getWorldPoint(x, y+1));
			auto norm = (p2 - p1).cross(p3 - p1);
			norm.normalize();
			for (int i = 0; i < 3; i++) {
				clothNorms[(y     * cloth.xRes + x)     * 3 + i] += norm[i];
				clothNorms[(y     * cloth.xRes + (x+1)) * 3 + i] += norm[i];
				clothNorms[((y+1) * cloth.xRes + x)     * 3 + i] += norm[i];
			}

			p1 = Vector3d(cloth.getWorldPoint(x, y+1));
			p2 = Vector3d(cloth.getWorldPoint(x+1, y));
			p3 = Vector3d(cloth.getWorldPoint(x+1, y+1));
			norm = (p2 - p1).cross(p3 - p1);
			norm.normalize();
			for (int i = 0; i < 3; i++) {
				clothNorms[((y+1) * cloth.xRes + x)     * 3 + i] += norm[i];
				clothNorms[(y     * cloth.xRes + (x+1)) * 3 + i] += norm[i];
				clothNorms[((y+1) * cloth.xRes + (x+1)) * 3 + i] += norm[i];
			}
		}
	}

	// normalize each point's norm (for smoother jazz)
	for (int y = 0; y < cloth.yRes; y++) {
		for (int x = 0; x < cloth.xRes; x++) {
			auto norm = clothNorms + 3 * (y * cloth.xRes + x);
			auto normv = Vector3d(norm);
			normv.normalize();
			for (int i = 0; i < 3; i++)
				norm[i] = normv[i];
		}
	}
}

void Simulation::update() {
	if (!running) return;

	double largestDistance = 0;
	for (int i = 0; i < cloth.numPoints(); i++) {
		double px = cloth.worldPoints[3*i + 0];
		double py = cloth.worldPoints[3*i + 1];
		double pz = cloth.worldPoints[3*i + 2];
		double magnitude = sqrt(px*px + py+py + pz*pz);
		largestDistance = max(largestDistance, magnitude);
	}
	constexpr double MAX_DIST = 2;
	if (largestDistance > MAX_DIST) {
		running = false;
		printf("point outside MAX_DIST=%f: distance=%f\n", MAX_DIST, largestDistance);
	}

	// if simulation is paused, don't update
	if (!running) return;

	// zero forces matrices
	for (int pt = 0; pt < cloth.xRes * cloth.yRes; pt++) {
		totalForces(0, pt).setZero();
		scaleForces(0, pt).setZero();
		scaleDampForces(0, pt).setZero();
		shearForces(0, pt).setZero();
		shearDampForces(0, pt).setZero();
		bendForces(0, pt).setZero();
		bendDampForces(0, pt).setZero();
	}

	computeForces(totalForces);

	for (int i = 0; i < cloth.xRes * cloth.yRes; i++) {
		// update velocities by condition-forces
		auto force = totalForces(0, i);
		auto massInverted = cloth.invertedPointMasses[i];
		cloth.worldVels[i*3 + 0] += force[0] * massInverted * TIMESTEP;
		cloth.worldVels[i*3 + 1] += force[1] * massInverted * TIMESTEP;
		cloth.worldVels[i*3 + 2] += force[2] * massInverted * TIMESTEP;

		// apply gravitic acceleration
		cloth.worldVels[i*3 + 1] -= GRAVITY_ACCEL * TIMESTEP;

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
	generateClothNormsFromMesh(clothNorms, cloth);
	copyTriangleNormsFromClothNorms(triNorms, clothNorms, cloth);
}

void Simulation::reset() {
	// regenerate cloth
	cloth = Cloth(cloth.xRes, cloth.yRes);

	switch (scene.id) {
		case SceneId::Perturb: {
			random_device rd;
			mt19937 gen(rd());
			uniform_real_distribution<> dis(0, 1);

			// perturb cloth f
			for (int i = 0; i < cloth.yRes - 1; i++) {
				for (int j = 0; j < cloth.xRes; j++) {
					cloth.getWorldPoint(j, i)[0] += dis(gen) / 80;
					cloth.getWorldPoint(j, i)[1] += dis(gen) / 80;
					cloth.getWorldPoint(j, i)[2] += dis(gen) / 80;
				}
			}
		} break;

		case SceneId::Bend:
			for (int i = 0; i < cloth.yRes / 2; i++) {
				for (int j = 0; j < cloth.xRes; j++) {
					cloth.worldPoints[(i * cloth.xRes + j) * 3 + 1] +=
						(cloth.yRes / 2 - i) * 0.001;
					cloth.worldPoints[(i * cloth.xRes + j) * 3 + 2] -=
						(cloth.yRes / 2 - i) * 0.01;
				}
			}
			break;

		case SceneId::Swing:
			for (int i = 0; i < cloth.yRes; i++) {
				for (int j = 0; j < cloth.xRes; j++) {
					auto point = cloth.getWorldPoint(j, i);
					swap(point[1], point[2]);
					point[1] += 0.5;
					point[2] -= 0.5;
				}
			}
			break;

		case SceneId::Hang:
			// do nothing. leave cloth hanging straight.
			break;
	}

	// regenerate triangles from the mesh
	triVerts = new double[9 * getNumTris()];
	clothNorms = new double[3 * getNumPoints()];
	triNorms = new double[9 * getNumTris()];
	copyTrianglesFromMesh(triVerts, cloth);
	generateClothNormsFromMesh(clothNorms, cloth);
	copyTriangleNormsFromClothNorms(triNorms, clothNorms, cloth);

	scaleU = 1;
	scaleV = 1;
	cuffScale = 1;
}

inline double lerp(double a, double b, double t) {
	return a * (1 - t) + b * t;
}

inline double clamp(double low, double x, double high) {
	if (x < low)  return low;
	if (x > high) return high;
	return x;
}

void Simulation::computeForces(ForceMatrix &forces) {
	for (int i = 0; i < cloth.yRes-1; i++) {
		for (int j = 0; j < cloth.xRes-1; j++) {
			int offset = i * cloth.xRes + j;
			computeScaleForces(scaleForces, scaleDampForces, offset);
			computeShearForces(shearForces, shearDampForces, offset);
			computeBendForces (bendForces, bendDampForces, j, i);
		}
	}

	int numPoints = cloth.xRes * cloth.yRes;
	for (int i = 0; i < numPoints; i++) {
		forces(0, i).setZero();
	}
	forces += scaleForces;
	forces += scaleDampForces;
	forces += shearForces;
	forces += shearDampForces;
	forces += bendForces;
	forces += bendDampForces;
}

void Simulation::computeScaleForces(
	ForceMatrix &scaleForces,
	ForceMatrix &scaleDampForces,
	int offset
) {
	int botLeftTri[3]  = {
		offset,
		offset + 1,
		offset + cloth.xRes
	};
	int topRightTri[3] = {
		offset + cloth.xRes,
		offset + 1,
	    offset + cloth.xRes + 1
	};

	int y = offset / cloth.xRes;
	Vector2d stretchUV(scaleU, scaleV);
	if (y < 5) {
		double t = y / 5.0;
		stretchUV[0] = lerp(cuffScale, stretchUV[0], t);
	}
	if (cuffing) {
		if (y == 0) {
			stretchUV[0] = 0.3;
		} else if (y == 1) {
			stretchUV[0] = 0.5;
		}
	}

	scaleHelper(scaleForces, scaleDampForces, botLeftTri,  true,  stretchUV);
	scaleHelper(scaleForces, scaleDampForces, topRightTri, false, stretchUV);
}

void Simulation::scaleHelper(
	ForceMatrix &scaleForces,
	ForceMatrix &scaleDampForces,
	int *triPts,
	bool isBl,
	Vector2d buv
) {
	Vector2d condition = scaleCondition(cloth, triPts, isBl, buv);
	double condX = condition[0];
	double condY = condition[1];
	for (int i = 0; i < 3; i++) {
		int ptI = triPts[i];

		// account for scaling force
		Eigen::Matrix<double, 3, 2> partial = scalePartial(cloth, ptI, triPts, isBl, buv);
		Vector3d force = -SCALE_STIFF * partial * condition;
		scaleForces(0, ptI) += force;

		// account for damping force
		Vector3d velI = Vector3d(cloth.getWorldVel(ptI));
		Vector3d dampForce = -SCALE_DAMP_STIFF * partial * partial.transpose() * velI;
		scaleDampForces(0, ptI) += dampForce;
	}
}

void Simulation::computeShearForces(
	ForceMatrix &shearForces,
	ForceMatrix &shearDampForces,
	int offset
) {
	int botLeftTri[3] = {offset, offset + 1, offset + cloth.xRes};
	int topRightTri[3] = {offset + cloth.xRes, offset + 1,
			              offset + cloth.xRes + 1};

	shearHelper(shearForces, shearDampForces, botLeftTri, true);
	shearHelper(shearForces, shearDampForces, topRightTri, false);
}

void Simulation::shearHelper(
	ForceMatrix &shearForces,
	ForceMatrix &shearDampForces,
	int *triPts,
	bool isBl
) {
	double cond = shearCondition(cloth, triPts, isBl);

	for (int i = 0; i < 3; i++) {
		int pointIndex = triPts[i];

		Vector3d partial = shearPartial(cloth, pointIndex, triPts, isBl);
		Vector3d force = -SHEAR_STIFF * partial * cond;
		shearForces(0, pointIndex) += force;

		Vector3d velocity = Vector3d(cloth.getWorldVel(pointIndex));
		Vector3d dampForce = -SHEAR_DAMP_STIFF * partial * partial.transpose() * velocity;
		shearDampForces(0, pointIndex) += dampForce;
	}
}

void Simulation::computeBendForces(
	ForceMatrix &bendForces,
	ForceMatrix &bendDampForces,
	int squareX,
	int squareY
) {
	int offset = squareX + squareY * cloth.xRes;

	// diagonal triangle pair
	int diagPts[4] = {
		offset,
		offset + 1,
		offset + cloth.xRes,
		offset + cloth.xRes + 1
	};
	bendHelper(bendForces, bendDampForces, diagPts);

	// right-side triangle pair
	if (squareX < cloth.xRes - 2) {
		int rightPts[4] = {
			offset + cloth.xRes,
			offset + 1,
			offset + cloth.xRes + 1,
			offset + 2
		};
		bendHelper(bendForces, bendDampForces, rightPts);
	}

	// top-side triangle pair
	if (squareY < cloth.yRes - 2) {
		int topPts[4] = {
			offset + 1,
			offset + cloth.xRes + 1,
			offset + cloth.xRes,
			offset + 2 * cloth.xRes
		};
		bendHelper(bendForces, bendDampForces, topPts);
	}
}

void Simulation::bendHelper(
	ForceMatrix &bendForces,
	ForceMatrix &bendDampForces,
	int *tris
) {
	auto cond = bendCondition(cloth, tris);

	for (int i = 0; i < 4; i++) {
		int pointIndex = tris[i];

		auto partial = bendPartial(cloth, pointIndex, tris);
		Vector3d force = -BEND_STIFF * partial * cond;
		for (int j = 0; j < 3; j++) {
			force[j] = clamp(-MAX_BEND, force[j], MAX_BEND);
		}
		bendForces(0, pointIndex) += force;

		Vector3d velocity = Vector3d(cloth.getWorldVel(pointIndex));
		Vector3d dampForce = -BEND_DAMP_STIFF * partial * partial.transpose() * velocity;
		bendDampForces(0, pointIndex) += dampForce;
	}
}
