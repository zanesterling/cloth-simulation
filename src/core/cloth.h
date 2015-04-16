#pragma once

#include <Eigen/Dense>
using namespace Eigen;

struct Cloth {
	int xRes, yRes;
	double w, h;
	double *uvPoints;
	double *worldPoints;
	double *worldVels;
	double massPerVertex;
	double mass;

	Cloth(int, int);
	Cloth(int, int, double, double);

	void initUvPoints();
	void initWorldPoints();

	double *getUvPoint(int);
	double *getUvPoint(int, int);
	double *getWorldPoint(int);
	double *getWorldPoint(int, int);
	double *getWorldVel(int);
	double *getWorldVel(int, int);
	void setWorldPoint(int, int, double, double, double);

	double getTriUvArea();
	Vector3d triNormal(int, int, int);
};
