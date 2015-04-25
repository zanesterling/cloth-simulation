#include "cloth.h"

Cloth::Cloth(int xRes, int yRes) : Cloth(xRes, yRes, 1, 1) {}
Cloth::Cloth(int xRes, int yRes, double w, double h)
	: xRes(xRes), yRes(yRes), w(w), h(h) {

	mass = 40;
	massPerVertex = mass / (xRes * yRes);

	initUvPoints();
	initWorldPoints();
	worldVels = new double[3 * xRes * yRes];
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

double *Cloth::getUvPoint(int i) {
	return uvPoints + 2 * i;
}

// NOTE: doesn't check bounds!
double *Cloth::getUvPoint(int x, int y) {
	return uvPoints + 2 * (y*xRes + x);
}

double *Cloth::getWorldPoint(int i) {
	return worldPoints + 3 * i;
}

// NOTE: doesn't check bounds!
double *Cloth::getWorldPoint(int x, int y) {
	return worldPoints + 3 * (y*xRes + x);
}

double *Cloth::getWorldVel(int i) {
	return worldVels + 3 * i;
}

// NOTE: doesn't check bounds!
double *Cloth::getWorldVel(int x, int y) {
	return worldVels + 3 * (y*xRes + x);
}

void Cloth::setWorldPoint(int xCor, int yCor, double xPos, double yPos,
	double zPos) {
	double *point = getWorldPoint(xCor, yCor);
	point[0] = xPos;
	point[1] = yPos;
	point[2] = zPos;
}

double Cloth::getTriUvArea() {
	return w / (xRes - 1) * h / (yRes - 1);
}

Vector3d Cloth::triNormal(int pi1, int pi2, int pi3) {
	auto p1 = Vector3d(getWorldPoint(pi1));

	// get side vectors
	auto s1 = Vector3d(getWorldPoint(pi2)) - p1;
	auto s2 = Vector3d(getWorldPoint(pi3)) - p1;

	return s1.cross(s2).normalized();
}
