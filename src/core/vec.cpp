#include "vec.h"

double Vec3::len() const {
    return sqrt(x*x + y*y + z*z);
}

void Vec3::normalize(double newlen) {
    double length = len();
    x = x / length * newlen;
    y = y / length * newlen;
    z = z / length * newlen;
}

bool operator==(const Vec3& first, const Vec3& second) {
    return (first.x == second.x &&
            first.y == second.y &&
            first.z == second.z);
}

bool operator!=(const Vec3& first, const Vec3& second) {
    return !(first == second);
}

Vec3 operator+(const Vec3& first, const Vec3& second) {
    Vec3 ret(first);
    ret.x += second.x;
    ret.y += second.y;
    ret.z += second.z;
    return ret;
}

Vec3 operator+=(Vec3& first, Vec3 second) {
    first.x += second.x;
    first.y += second.y;
    first.z += second.z;
    return first;
}

Vec3 operator-(const Vec3& first, const Vec3& second) {
    Vec3 ret(first);
    ret.x -= second.x;
    ret.y -= second.y;
    ret.z -= second.z;
    return ret;
}

Vec3 operator-=(Vec3& first, Vec3& second) {
    first.x -= second.x;
    first.y -= second.y;
    first.z -= second.z;
    return first;
}

Vec3 operator*(const Vec3& vec, double factor) {
    Vec3 ret(vec);
    ret.x *= factor;
    ret.y *= factor;
    ret.z *= factor;
    return ret;
}

Vec3 operator*=(Vec3& vec, double factor) {
    vec.x *= factor;
    vec.y *= factor;
    vec.z *= factor;
    return vec;
}

Vec3 operator/(const Vec3& vec, double factor) {
    Vec3 ret(vec);
    ret.x /= factor;
    ret.y /= factor;
    ret.z /= factor;
    return ret;
}

Vec3 operator/=(Vec3& vec, double factor) {
    vec.x /= factor;
    vec.y /= factor;
    vec.z /= factor;
    return vec;
}

double dot(Vec3 v1, Vec3 v2) {
	return v1.x*v2.x + v1.y*v2.y + v1.z*v2.z;
}

Vec3 cross(Vec3 v1, Vec3 v2) {
	Vec3 newVec;
	newVec.x = v1.y*v2.z - v1.z*v2.y;
	newVec.y = v1.z*v2.x - v1.x*v2.z;
	newVec.z = v1.x*v2.y - v1.y*v2.x;
	return newVec;
}

Vec3 normalize(Vec3 vec) {
	Vec3 newVec(vec);
	newVec.normalize(1);
	return newVec;
}
