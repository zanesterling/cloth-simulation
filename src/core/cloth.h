#pragma once

#include <GL/freeglut.h>

struct Cloth {
	int w, h;
	GLfloat *uvPoints;
	GLfloat *worldPoints;

	Cloth(int, int);

	void initUvPoints();
	void initWorldPoints();

	GLfloat *getUvPoint(int, int);
	GLfloat *getWorldPoint(int, int);
	void setWorldPoint(int, int, GLfloat, GLfloat, GLfloat);
};
