#pragma once

#include <GL/freeglut.h>

struct Cloth {
	int w, h;
	GLfloat *uvPoints;
	GLfloat *worldPoints;

	Cloth(int, int);

	GLfloat *worldPoint(int, int);
};
