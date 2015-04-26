#include "cloth.h"

Cloth::Cloth(int xRes, int yRes) : Cloth(xRes, yRes, 1, 1) {}
Cloth::Cloth(int xRes, int yRes, double w, double h)
	: xRes(xRes), yRes(yRes), w(w), h(h) {

	mass = 40;
	massPerVertI = (xRes * yRes) / mass;

	initUvPoints();
	initWorldPoints();
	worldVels = new double[3 * xRes * yRes];

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
