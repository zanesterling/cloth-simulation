#pragma once

#include <math.h>

class Vec3 {
public:
	double x;
	double y;
	double z;
	Vec3() : x(0), y(0), z(0) {}
	Vec3(const double x, const double y, const double z)
		: x(x), y(y), z(z) {}
	Vec3(const double *fp)
		: x(fp[0]), y(fp[1]), z(fp[2]) {}

	double len() const;
	void normalize(double newlen);
};

bool operator==(const Vec3&, const Vec3&);
bool operator!=(const Vec3&, const Vec3&);

Vec3 operator+(const Vec3&, const Vec3&);
Vec3 operator+=(Vec3&, Vec3);

Vec3 operator-(const Vec3&, const Vec3&);
Vec3 operator-=(Vec3&, Vec3&);

Vec3 operator*(const Vec3&, double);
Vec3 operator*=(Vec3&, double);

Vec3 operator/(const Vec3&, double);
Vec3 operator/=(Vec3&, double);

double dot(Vec3, Vec3);
Vec3 cross(Vec3, Vec3);
