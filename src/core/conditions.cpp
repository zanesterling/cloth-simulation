#include "conditions.h"

Vector2d scaleCondition(Cloth &cloth, int i, int j, int k) {
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

	// find triangle area
	double area = cloth.w / (cloth.xRes - 1) * cloth.h / (cloth.yRes - 1);

	// get final condition values
	Vector2d condition;
	condition[0] = area * (wu.norm() - 1);
	condition[1] = area * (wv.norm() - 1);
	return condition;
}

Matrix<double, 2, 3> scalePartial(Cloth &cloth, int pt,
                                  int i, int j, int k) {
	Matrix<double, 2, 3> partial; // two rows, three columns

	Vector2d localCond = scaleCondition(cloth, i, j, k);
	double *worldPt = cloth.getWorldPoint(pt);

	for (int col = 0; col < 3; col++) {
		// perturb the cloth
		worldPt[col] += PERTURB_QUANT;

		Vector2d perturbedCond = scaleCondition(cloth, i, j, k);
		partial.col(col) = perturbedCond - localCond;

		// de-perturb cloth
		worldPt[col] -= PERTURB_QUANT;
	}

	return partial;
}
