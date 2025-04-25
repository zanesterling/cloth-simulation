#include "conditions.h"

// cached uvMatrix inverses for bot-left, top-right triangles
bool setBlUvMatrix = false;
Matrix2d blUvMatrixI;
bool setTrUvMatrix = false;
Matrix2d trUvMatrixI;

Matrix2d duvMatrixInverse(Cloth &cloth, int i, int j, int k, bool isBl) {
	auto uvI = cloth.getUvPoint(i);
	auto uvJ = cloth.getUvPoint(j);
	auto uvK = cloth.getUvPoint(k);

	if (isBl) {
		if (!setBlUvMatrix) {
			setBlUvMatrix = true;
			// find inverted [uv][12] matrix
			blUvMatrixI(0, 0) = uvJ[0] - uvI[0];
			blUvMatrixI(0, 1) = uvK[0] - uvI[0],
			blUvMatrixI(1, 0) = uvJ[1] - uvI[1],
			blUvMatrixI(1, 1) = uvK[1] - uvI[1];
			blUvMatrixI = blUvMatrixI.inverse().eval();
		}
		return blUvMatrixI;
	} else {
		if (!setTrUvMatrix) {
			setTrUvMatrix = true;
			// find inverted [uv][12] matrix
			trUvMatrixI(0, 0) = uvJ[0] - uvI[0];
			trUvMatrixI(0, 1) = uvK[0] - uvI[0],
			trUvMatrixI(1, 0) = uvJ[1] - uvI[1],
			trUvMatrixI(1, 1) = uvK[1] - uvI[1];
			trUvMatrixI = trUvMatrixI.inverse().eval();
		}
		return trUvMatrixI;
	}
}

Eigen::Matrix<double, 3, 2> dxMatrix(Cloth &cloth, int i, int j, int k) {
	Eigen::Matrix<double, 3, 2> dxMatrix;
	auto wpI = cloth.getWorldPointVec(i);
	auto wpJ = cloth.getWorldPointVec(j);
	auto wpK = cloth.getWorldPointVec(k);
	dxMatrix.col(0) = wpJ - wpI;
	dxMatrix.col(1) = wpK - wpI;
	return dxMatrix;
}

Vector2d scaleCondition(Cloth &cloth, int *tri, bool isBl, Vector2d buv) {
	double area = cloth.triUvArea;
	int i = tri[0], j = tri[1], k = tri[2];
	Eigen::Matrix<double, 3, 2> wuv =
		dxMatrix(cloth, i, j, k) * duvMatrixInverse(cloth, i, j, k, isBl);
	Vector2d wnorms = wuv.colwise().norm();

	// get final condition value
	return area * (wnorms - buv);
}

Eigen::Matrix<double, 3, 2> scalePartial(
	Cloth &cloth,
	int pt,
	int *tri,
	bool isBl,
	Vector2d buv
) {
	Eigen::Matrix<double, 3, 2> partial;

	double *worldPt = cloth.getWorldPoint(pt);
	double originalPoint[3] = {worldPt[0], worldPt[1], worldPt[2]};

	for (int col = 0; col < 3; col++) {
		// perturb the cloth
		worldPt[col] = originalPoint[col] + PERTURB_QUANT;
		Vector2d pCond1 = scaleCondition(cloth, tri, isBl, buv);

		worldPt[col] = originalPoint[col] - PERTURB_QUANT;
		Vector2d pCond2 = scaleCondition(cloth, tri, isBl, buv);

		partial.row(col) = (pCond1 - pCond2) / (2 * PERTURB_QUANT);

		// de-perturb cloth
		worldPt[col] = originalPoint[col];
	}

	return partial;
}

double shearCondition(Cloth &cloth, int *tri, bool isBl) {
	int i = tri[0], j = tri[1], k = tri[2];
	auto wuv = dxMatrix(cloth, i, j, k) * duvMatrixInverse(cloth, i, j, k, isBl);
	auto area = cloth.triUvArea;

	return area * wuv.col(0).dot(wuv.col(1));
}

Vector3d shearPartial(Cloth &cloth, int pt, int *tri, bool isBl) {
	int i = tri[0], j = tri[1], k = tri[2];
	Vector3d partial;

	double *worldPt = cloth.getWorldPoint(pt);
	double originalPoint[3] = {worldPt[0], worldPt[1], worldPt[2]};

	for (int col = 0; col < 3; col++) {
		// perturb the cloth
		worldPt[col] = originalPoint[col] + PERTURB_QUANT;
		double pCond1 = shearCondition(cloth, tri, isBl);

		worldPt[col] = originalPoint[col] - PERTURB_QUANT;
		double pCond2 = shearCondition(cloth, tri, isBl);

		partial[col] = (pCond1 - pCond2) / (2 * PERTURB_QUANT);

		// de-perturb cloth
		worldPt[col] = originalPoint[col];
	}

	return partial;
}

// tris points to an array of four points, p1, p2, p3, and p4.
// p1,p2,p3 and p4,p3,p2 are counter-clockwise sequences.
// p2 and p3 are shared between the two triangles.
double bendCondition(Cloth &cloth, int *tris) {
	int p1 = tris[0], p2 = tris[1], p3 = tris[2], p4 = tris[3];
	Vector3d n1 = cloth.triNormal(p1, p2, p3);
	Vector3d n2 = cloth.triNormal(p3, p2, p4);
	Vector3d e = Vector3d(cloth.getWorldPoint(p2)) -
	             Vector3d(cloth.getWorldPoint(p3));

	double st = n1.cross(n2).dot(e);
	double ct = n1.dot(n2);

	return atan2(st, ct);
}

Vector3d bendPartial(Cloth &cloth, int pt, int *tris) {
	int p1 = tris[0], p2 = tris[1], p3 = tris[2], p4 = tris[3];
	Vector3d partial;

	double *worldPoint = cloth.getWorldPoint(pt);
	double originalPoint[3] = {worldPoint[0], worldPoint[1], worldPoint[2]};

	for (int col = 0; col < 3; col++) {
		// perturb the cloth
		worldPoint[col] = originalPoint[col] + PERTURB_QUANT;
		auto pCond1 = bendCondition(cloth, tris);

		worldPoint[col] = originalPoint[col] - PERTURB_QUANT;
		auto pCond2 = bendCondition(cloth, tris);

		partial[col] = (pCond1 - pCond2) / (2 * PERTURB_QUANT);

		// de-perturb cloth
		worldPoint[col] = originalPoint[col];
	}

	return partial;
}
