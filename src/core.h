#pragma once

#include <GL/glew.h>
#include <GL/freeglut.h>

#include <stdio.h>

#include "core/ui.h"
#include "core/simulation.h"

Simulation sim(40, 40);
UI ui(1360, 840, sim);

int main(int, char **);
void initGlut(int, char **);
void initLights();

void display();
void idle();
void resize(int, int);
void keyboard(unsigned char, int, int);
void mouse(int, int, int, int);
void mouseMotion(int, int);
void quit();
