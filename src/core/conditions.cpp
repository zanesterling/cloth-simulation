#include "conditions.h"

Matrix<double, 3, 2> wuvMatrix(Cloth &cloth, int i, int j, int k) {
	// get delta x_1, x_2
	Matrix<double, 3, 2> dxMatrix;
	Vector3d worldPointI = Vector3d(cloth.getWorldPoint(i));
	dxMatrix.col(0) = Vector3d(cloth.getWorldPoint(j)) - worldPointI;
	dxMatrix.col(1) = Vector3d(cloth.getWorldPoint(k)) - worldPointI;

	// find inverted [uv][12] matrix
	Matrix2d uvMatrix;
	uvMatrix << cloth.getUvPoint(j)[0] - cloth.getUvPoint(i)[0],
	            cloth.getUvPoint(k)[0] - cloth.getUvPoint(i)[0],
	            cloth.getUvPoint(j)[1] - cloth.getUvPoint(i)[1],
	            cloth.getUvPoint(k)[1] - cloth.getUvPoint(i)[1];
	uvMatrix = uvMatrix.inverse().eval();

	// find ||w||s
	return dxMatrix * uvMatrix;
}

Vector2d scaleCondition(Cloth &cloth, int *tri) {
	return scaleCondition(cloth, tri[0], tri[1], tri[2]);
}

Vector2d scaleCondition(Cloth &cloth, int i, int j, int k) {
	double area = cloth.getTriUvArea();
	auto wm = wuvMatrix(cloth, i, j, k);

	// get final condition values
	Vector2d condition;
	condition[0] = area * (wm.col(0).norm() - 1);
	condition[1] = area * (wm.col(1).norm() - 1);
	return condition;
}

Matrix<double, 2, 3> scalePartial(Cloth &cloth, int pt, int *tri) {
	return scalePartial(cloth, pt, tri[0], tri[1], tri[2]);
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
		partial.col(col) = (perturbedCond - localCond) / PERTURB_QUANT;

		// de-perturb cloth
		worldPt[col] -= PERTURB_QUANT;
	}

	return partial;
}

double shearCondition(Cloth &cloth, int *tri) {
	return shearCondition(cloth, tri[0], tri[1], tri[2]);
}

double shearCondition(Cloth &cloth, int i, int j, int k) {
	auto wuvm = wuvMatrix(cloth, i, j, k);
	auto area = cloth.getTriUvArea();

	return area * wuvm.col(0).dot(wuvm.col(1));
}

RowVector3d shearPartial(Cloth &cloth, int pt, int *tri) {
	return shearPartial(cloth, pt, tri[0], tri[1], tri[2]);
}

RowVector3d shearPartial(Cloth &cloth, int pt, int i, int j, int k) {
	RowVector3d partial;

	double localCond = shearCondition(cloth, i, j, k);
	double *worldPt = cloth.getWorldPoint(pt);

	for (int col = 0; col < 3; col++) {
		// perturb the cloth
		worldPt[col] += PERTURB_QUANT;

		double perturbedCond = shearCondition(cloth, i, j, k);
		partial[col] = (perturbedCond - localCond) / PERTURB_QUANT;

		// de-perturb cloth
		worldPt[col] -= PERTURB_QUANT;
	}

	return partial;
}

double bendCondition(Cloth &cloth, int *tris) {
	return bendCondition(cloth, tris);
}

// p1,p2,p3 and p4,p3,p2 are counter-clockwise
// p2 and p3 are shared between the two triangles
double bendCondition(Cloth &cloth, int p1, int p2, int p3, int p4) {
	Vector3d n1 = cloth.triNormal(p1, p2, p3);
	Vector3d n2 = cloth.triNormal(p4, p3, p2);
	Vector3d e = Vector3d(cloth.getWorldPoint(p2)) -
	             Vector3d(cloth.getWorldPoint(p3));

	double st = n1.cross(n2).dot(e);
	double ct = n1.dot(n2);

	return atan2(st, ct);
}

RowVector3d bendPartial(Cloth &cloth, int pt, int *tris) {
	return bendPartial(cloth, pt, tris);
}

RowVector3d bendPartial(Cloth &cloth, int pt,
                        int p1, int p2, int p3, int p4) {
	RowVector3d partial;

	auto localCond = bendCondition(cloth, p1, p2, p3, p4);
	double *worldPt = cloth.getWorldPoint(pt);

	for (int col = 0; col < 3; col++) {
		// perturb the cloth
		worldPt[col] += PERTURB_QUANT;

		auto perturbedCond = bendCondition(cloth, p1, p2, p3, p4);
		partial[col] = (perturbedCond - localCond) / PERTURB_QUANT;

		// de-perturb cloth
		worldPt[col] -= PERTURB_QUANT;
	}

	return partial;
}
