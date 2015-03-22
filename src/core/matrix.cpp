#include "matrix.h"

Matrix::Matrix(int w, int h) : w(w), h(h) {
	data = new double[w * h];
}

Matrix::~Matrix() {
	delete data;
}

double *Matrix::operator[](int y) {
	return data + h * y;
}

double Matrix::get(int x, int y) {
	return data[x + y * h];
}

void Matrix::set(int x, int y, double d) {
	data[x + y * h] = d;
}
