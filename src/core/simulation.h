#pragma once

#include <Eigen/Sparse>

#include <string.h>
#include <iostream>

#include "cloth.h"
#include "conditions.h"

#define LOCK_TOP_POINTS false

#define SCALE_STIFF 1
#define DAMP_STIFF  0.3
#define SHEAR_STIFF 1
#define BEND_STIFF  0.001

#define TIMESTEP 1

typedef Matrix<Vector3d, Dynamic, Dynamic> ForceMatrix;
typedef SparseMatrix<double> ForcePartialMatrix;

class Simulation {
	void handleScaleCondition(int, ForceMatrix &, ForcePartialMatrix &);
	void scaleHelper(int *, ForceMatrix &, ForcePartialMatrix &);

	void handleShearCondition(int, ForceMatrix &, ForcePartialMatrix &);
	void shearHelper(int *, ForceMatrix &, ForcePartialMatrix &);

	void handleBendCondition(int,  ForceMatrix &, ForcePartialMatrix &);
	void bendHelper(int *, ForceMatrix &, ForcePartialMatrix &);

	double *genTrisFromMesh();
	void copyPoint(double *, double *, int);

public:
	Cloth cloth;
	double *triVerts;
	bool running = false;

	Simulation(int, int);

	void update();
	void reset();
	int getNumTris();
};
