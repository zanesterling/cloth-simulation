#pragma once

#include <Eigen/Dense>
using namespace Eigen;

struct Cloth {
	int xRes, yRes;
	double w, h;
	double *uvPoints;
	double *worldPoints;
	double *worldVels;
	double massPerVertI; // inverted mass-per-vertex
	double mass;
	double triUvArea;

	Cloth(int, int);
	Cloth(int, int, double, double);

	void initUvPoints();
	void initWorldPoints();

	double *getUvPoint(int i) { return uvPoints + 2 * i; }
	double *getUvPoint(int x, int y) { return uvPoints + 2 * (y*xRes + x);}
	double *getWorldPoint(int i) { return worldPoints + 3 * i; }
	double *getWorldPoint(int x, int y) {
		return worldPoints + 3 * (y*xRes + x);
	}
	double *getWorldVel(int i) { return worldVels + 3 * i; }
	double *getWorldVel(int x, int y){return worldVels + 3 * (y*xRes + x);}
	void setWorldPoint(int, int, double, double, double);

	double getTriUvArea();
	Vector3d triNormal(int, int, int);
};
