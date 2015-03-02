#pragma once

#include <GL/glew.h>
#include <GL/freeglut.h>

#include "core/ui.h"
#include "core/simulation.h"

UI ui(720, 480);
Simulation sim;

int main(int, char **);
void initGlut(int, char **);
void initLights();

void display();
void keyboard(unsigned char, int, int);
void idle();
