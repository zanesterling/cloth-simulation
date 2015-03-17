#include "conditions.h"

double *scaleCondition(Cloth &cloth, int i, int j, int k) {
	double *condition = new double[2];
	condition[0] = 0;
	condition[1] = 0;

	// get the delta x's
	Vec3 dx1 = Vec3(cloth.getWorldPoint(j)) - Vec3(cloth.getWorldPoint(i));
	Vec3 dx2 = Vec3(cloth.getWorldPoint(k)) - Vec3(cloth.getWorldPoint(i));

	double u1 = cloth.getUvPoint(j)[0] - cloth.getUvPoint(i)[0];
	double u2 = cloth.getUvPoint(k)[0] - cloth.getUvPoint(i)[0];
	double v1 = cloth.getUvPoint(j)[1] - cloth.getUvPoint(i)[1];
	double v2 = cloth.getUvPoint(k)[1] - cloth.getUvPoint(i)[1];

	// take the inverse of the [uv][12] matrix
	double det = u1 * v2 - u2 * v1;
	u1 /= det;
	u2 /= -det;
	v1 /= -det;
	v2 /= det;
	std::swap(u1, v2);

	// find wu and wv
	Vec3 wu = dx1 * u1 + dx2 * v1;
	Vec3 wv = dx1 * u2 + dx2 * v2;

	// set final condition values
	condition[0] = 0.5 * wu.len() - 1;
	condition[1] = 0.5 * wv.len() - 1;
}
