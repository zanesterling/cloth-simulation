#pragma once

#include <string.h>
#include <iostream>
#include <random>

#include "cloth.h"
#include "conditions.h"

#define LOCK_TOP_ROW true

#define GRAVITY_ENABLED true
#define FLOOR_ENABLED false
#define ACCEL_LOCK true

#define SCALE_STIFF 20
#define DAMP_STIFF  2
#define SHEAR_STIFF 1
#define BEND_STIFF  0.0001

#define TIMESTEP 1

#define GRAVITY_ACCEL 0.0003

#define FLOOR_HEIGHT -1

#define MAX_SCALE 0.001
#define MAX_BEND 0.000003

typedef Eigen::Matrix<Vector3d, Eigen::Dynamic, Eigen::Dynamic> ForceMatrix;

#define NUM_SCENES 4
enum SceneId {
	Perturb,
	Bend,
	Swing,
	Hang
};
struct Scene {
	SceneId id;
};
const Scene SCENES[NUM_SCENES] = {
	{.id = SceneId::Perturb},
	{.id = SceneId::Bend},
	{.id = SceneId::Swing},
	{.id = SceneId::Hang},
};

class Simulation {
	double maxScale;
	double scaleX;
	double scaleY;
	double cuffScale;

	void handleScaleCondition(int offset);
	void scaleHelper(int *triPts, bool isBl, int y);

	void handleShearCondition(int offset);
	void shearHelper(int *triPts, bool isBl);

	void handleBendCondition(int offset);
	void bendHelper(int *tris);

public:
	Cloth cloth;
	double *clothNorms;
	double *triVerts;
	double *triNorms;
	bool running = true;
	bool cuffing = false;

	// Determines the state of the cloth on reset.
	Scene scene = SCENES[SceneId::Swing];

	ForceMatrix totalForces;
	ForceMatrix scaleForces;
	ForceMatrix scaleDampForces;
	ForceMatrix shearForces;
	ForceMatrix shearDampForces;
	ForceMatrix bendForces;
	ForceMatrix bendDampForces;

	Simulation(int, int);

	void update();
	void reset();
	int getNumTris() { return 2 * (cloth.xRes - 1) * (cloth.yRes - 1); }
	int getNumPoints() { return cloth.xRes * cloth.yRes; }

	void changeScaleX(double d) { scaleX += d; }
	void changeScaleY(double d) { scaleY += d; }
	void changeCuff(double d)   { cuffScale += d; }

	void prevScene() {
		int nextId = (scene.id - 1 + NUM_SCENES) % NUM_SCENES;
		scene = SCENES[nextId];
		reset();
	}
	void nextScene() {
		int nextId = (scene.id + 1) % NUM_SCENES;
		scene = SCENES[nextId];
		reset();
	}
};
