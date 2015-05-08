#pragma once

#include <Eigen/Sparse>

#include <string.h>
#include <iostream>
#include <random>

#include "cloth.h"
#include "conditions.h"

#define LOCK_TOP_POINTS true
#define GRAVITY_ENABLED true
#define FLOOR_ENABLED false
#define ACCEL_LOCK true

#define SCALE_STIFF 20
#define DAMP_STIFF  2
#define SHEAR_STIFF 1
#define BEND_STIFF  0.0001

#define TIMESTEP 1

#define GRAVITY_ACCEL 0.0003

#define RESET_PERTURB 0
#define RESET_BEND    1
#define RESET_SWING   2
#define RESET_SCENE RESET_SWING

#define FLOOR_HEIGHT -1

#define MAX_SCALE 0.001
#define MAX_BEND 0.000003

typedef Matrix<Vector3d, Dynamic, Dynamic> ForceMatrix;

class Simulation {
	double maxScale;
	double scaleX;
	double scaleY;
	double cuffScale;

	void handleScaleCondition(int);
	void scaleHelper(int *, bool, int);

	void handleShearCondition(int);
	void shearHelper(int *, bool);

	void handleBendCondition(int);
	void bendHelper(int *);

	double *genTrisFromMesh();
	double *genNorms();
	double *genTriNorms();
	void copyPoint(double *, double *);

public:
	Cloth cloth;
	double *triVerts;
	double *norms;
	bool running = false;
	bool bannerStyle = false;
	bool cuffing = false;

	ForceMatrix forces;

	Simulation(int, int);

	void update();
	void reset();
	int getNumTris() { return 2 * (cloth.xRes - 1) * (cloth.yRes - 1); }
	int getNumPoints() { return cloth.xRes * cloth.yRes; }

	void changeScaleX(double d) { scaleX += d; }
	void changeScaleY(double d) { scaleY += d; }
	void changeCuff(double d)   { cuffScale += d; }
};
