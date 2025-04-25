#include "cloth.h"

Cloth::Cloth(int xRes, int yRes, double w, double h)
	: xRes(xRes), yRes(yRes), w(w), h(h) {
	int numPoints = xRes * yRes;
	int numTriangles = (xRes-1) * (yRes-1) * 2;

	double uvDensity = 15.0;
	double triangleUvArea = w*h / numTriangles;
	triangleMass = triangleUvArea * uvDensity;

	double oneThirdTriangleMass = triangleMass / 3.0;
	invertedPointMasses = new double[numPoints];
	double interiorInvertedMass = 1.0 / (oneThirdTriangleMass * 8);
	for (int pointIndex = 0; pointIndex < numPoints; pointIndex++) {
		invertedPointMasses[pointIndex] = interiorInvertedMass;
	}
	double sideInvertedMass = 1.0 / (oneThirdTriangleMass * 4);
	for (int i = 1; i < xRes-1; i++) {
		invertedPointMasses[i]                 = sideInvertedMass; // top edge
		invertedPointMasses[xRes*(yRes-1) + i] = sideInvertedMass; // bottom edge
	}
	for (int i = 1; i < yRes-1; i++) {
		invertedPointMasses[i*xRes]            = sideInvertedMass; // left edge
		invertedPointMasses[i*xRes + xRes-1]   = sideInvertedMass; // right edge
	}
	double cornerInvertedMass = 1.0 / (oneThirdTriangleMass * 2);
	invertedPointMasses[0]                      = cornerInvertedMass;
	invertedPointMasses[xRes-1]                 = cornerInvertedMass;
	invertedPointMasses[xRes*(yRes-1) + 0]      = cornerInvertedMass;
	invertedPointMasses[xRes*(yRes-1) + xRes-1] = cornerInvertedMass;

	initUvPoints();
	initWorldPoints();
	worldVels = new double[3 * numPoints];
	memset(worldVels, 0, 3 * numPoints * sizeof(worldVels[0]));

	triUvArea = getTriUvArea();
}

// initialize uvPoints with uniform grid
void Cloth::initUvPoints() {
	uvPoints = new double[2 * xRes * yRes];
	for (int i = 0; i < yRes; i++) {
		for (int j = 0; j < xRes; j++) {
			uvPoints[2 * (i*xRes + j)]     = j * w / (xRes-1);
			uvPoints[2 * (i*xRes + j) + 1] = i * h / (yRes-1);
		}
	}
}

// initializte cloth world-state
void Cloth::initWorldPoints() {
	// init world positions
	worldPoints = new double[3 * xRes * yRes];
	for (int i = 0; i < yRes; i++) {
		for (int j = 0; j < xRes; j++) {
			double *point = getUvPoint(j, i);
			setWorldPoint(j, i, point[0] - w/2, point[1] - h/2, 0);
		}
	}

	// init velocities
	worldVels = new double[3 * xRes * yRes];
	for (int i = 0; i < 3 * yRes*xRes; i++) {
		worldVels[i] = 0;
	}
}

void Cloth::setWorldPoint(int xCor, int yCor, double xPos, double yPos,
	double zPos) {
	double *point = getWorldPoint(xCor, yCor);
	point[0] = xPos;
	point[1] = yPos;
	point[2] = zPos;
}

double Cloth::getTriUvArea() {
	return w * h / (2 * (xRes - 1) * (yRes - 1));
}

Vector3d Cloth::triNormal(int pi1, int pi2, int pi3) {
	auto p1 = Vector3d(getWorldPoint(pi1));

	// get side vectors
	auto s1 = Vector3d(getWorldPoint(pi2)) - p1;
	auto s2 = Vector3d(getWorldPoint(pi3)) - p1;

	return s1.cross(s2).normalized();
}

int Cloth::numPoints() {
	return xRes * yRes;
}
