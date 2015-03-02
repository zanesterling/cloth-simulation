#pragma once

#include "simulation.h"

class UI {
public:
	int width, height;
	Simulation &sim;

	UI(int w, int h, Simulation &sim) : width(w), height(h), sim(sim) {}

	void render();
	void resize(int, int);
};
