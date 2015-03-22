#include "matrix.h"

Matrix::Matrix(Matrix m) 
	: w(m.w), h(m.h), standalone(m.standalone) {
	// if m is standalone we need to copy the data, else just copy the ref
	if (m.standalone) {
		data = new double[w * h];
		memcpy(data, m.data, w * h * sizeof(double));
	} else {
		data = m.data;
	}
}

Matrix::Matrix(int w, int h)
	: w(w), h(h), standalone(true) {
	// init data segment
	data = new double[w * h];
	for (int i = 0; i < w * h; i++) data[i] = 0;
}

Matrix::Matrix(int w, int h, double *data)
	: w(w), h(h), data(data), standalone(false) {}

Matrix::~Matrix() {
	if (standalone) delete data;
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
