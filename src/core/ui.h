#pragma once

#include <GL/freeglut.h>

#include "simulation.h"

#define FOV_Y 30.
#define CLICK_RADIUS 3.

class UI {
	int selectedVert = -1;
	Vector2d mouseLoc;

public:
	int width, height;
	Simulation &sim;
	double azimuth = 0;
	double altitude = 0;
	double vAzim = 0;
	double vAlt = 0;

	UI(int, int, Simulation &);

	void update();
	void render();
	void resize(int, int);

	void mouse(int, int, int, int);
	void mouseMotion(int, int);
};
