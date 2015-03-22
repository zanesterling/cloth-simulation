#pragma once

class Matrix {
	double *data;
	bool standalone; // whether or not this matrix owns its data

public:
	int w, h;

	Matrix(int, int);
	Matrix(int, int, double *);
	~Matrix();

	// returns row at y
	double *operator[](int);

	// returns value at x,y
	double get(int, int);
	void set(int, int, double);

};
