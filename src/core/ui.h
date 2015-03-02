#pragma once

#include <GL/freeglut.h>

#include "simulation.h"

class UI {
public:
	int width, height;
	Simulation &sim;

	UI(int, int, Simulation &);

	void render();
	void resize(int, int);
};
