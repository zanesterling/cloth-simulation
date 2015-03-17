#pragma once

#include <GL/freeglut.h>

struct Cloth {
	int xRes, yRes;
	GLfloat w, h;
	GLfloat *uvPoints;
	GLfloat *worldPoints;
	GLfloat *worldVels;

	Cloth(int, int);
	Cloth(int, int, GLfloat, GLfloat);

	void initUvPoints();
	void initWorldPoints();

	GLfloat *getUvPoint(int);
	GLfloat *getUvPoint(int, int);
	GLfloat *getWorldPoint(int);
	GLfloat *getWorldPoint(int, int);
	void setWorldPoint(int, int, GLfloat, GLfloat, GLfloat);
};
