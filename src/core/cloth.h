#pragma once

struct Cloth {
	int xRes, yRes;
	double w, h;
	double *uvPoints;
	double *worldPoints;
	double *worldVels;

	Cloth(int, int);
	Cloth(int, int, double, double);

	void initUvPoints();
	void initWorldPoints();

	double *getUvPoint(int);
	double *getUvPoint(int, int);
	double *getWorldPoint(int);
	double *getWorldPoint(int, int);
	void setWorldPoint(int, int, double, double, double);
};
