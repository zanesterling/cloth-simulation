#pragma once

#include <GL/freeglut.h>
#include <math.h>

#include "simulation.h"

#define FOV_Y 30.
#define CLICK_RADIUS 3.

struct Color {
	float red;
	float green;
	float blue;

	bool operator==(const Color& right) {
		return this->red == right.red
			&& this->green == right.green
			&& this->blue == right.blue;
	}
};

constexpr Color DEFAULT_FRONT_COLOR = {.red=0.3, .green=0.8, .blue=0.9};
constexpr Color DEFAULT_BACK_COLOR  = {.red=0.9, .green=0.4, .blue=0.1};
constexpr Color WHITE               = {.red=1.0, .green=1.0, .blue=1.0};

class UI {
	float mouseX;
	float mouseY;

	int button;

public:
	int width, height;
	Simulation &sim;
	double azimuth = M_PI / 4;
	double altitude = M_PI / 8;
	double vAzim = 0;
	double vAlt = 0;
	bool fillMode = true;

	Color frontColor = DEFAULT_FRONT_COLOR;
	Color backColor  = DEFAULT_BACK_COLOR;

	UI(int, int, Simulation &);

	void update();
	void render();
	void resize(int, int);

	void mouse(int, int, int, int);
	void mouseMotion(int, int);
	
	GLfloat *genNorms(GLfloat *);
};
