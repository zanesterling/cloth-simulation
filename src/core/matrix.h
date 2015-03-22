#pragma once

class Matrix {
	double *data;

public:
	int w, h;

	Matrix(int, int);
	~Matrix();

	// returns row at y
	double *operator[](int);

	// returns value at x,y
	double get(int, int);
	void set(int, int, double);
};
