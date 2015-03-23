#include "conditions.h"

RowVector2d scaleCondition(Cloth &cloth, int i, int j, int k) {
	RowVector2d condition;

	// get delta x_1, x_2
	Vector3d wpi = Vector3d(cloth.getWorldPoint(i));
	Vector3d dx1 = Vector3d(cloth.getWorldPoint(j)) - wpi;
	Vector3d dx2 = Vector3d(cloth.getWorldPoint(k)) - wpi;

	// find inverted [uv][12] matrix
	Matrix2d uvMatrix;
	uvMatrix << cloth.getUvPoint(j)[0] - cloth.getUvPoint(i)[0],
	            cloth.getUvPoint(k)[0] - cloth.getUvPoint(i)[0],
	            cloth.getUvPoint(j)[1] - cloth.getUvPoint(i)[1],
	            cloth.getUvPoint(k)[1] - cloth.getUvPoint(i)[1];
	uvMatrix = uvMatrix.inverse().eval();

	// find ||w||s
	Vector3d wu = dx1 * uvMatrix(0, 0) + dx2 * uvMatrix(1, 0);
	Vector3d wv = dx1 * uvMatrix(0, 1) + dx2 * uvMatrix(1, 1);

	// get final condition values
	condition[0] = 0.5 * (wu.norm() - 1);
	condition[1] = 0.5 * (wv.norm() - 1);
	return condition;
}
