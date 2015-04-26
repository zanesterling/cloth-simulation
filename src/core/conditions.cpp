#include "conditions.h"

// cached uvMatrix inverses for bot-left, top-right triangles
bool setBlUvMatrix = false;
Matrix2d blUvMatrixI;
bool setTrUvMatrix = false;
Matrix2d trUvMatrixI;

WuvMatrix wuvMatrix(Cloth &cloth, int i, int j, int k, bool isBl) {
	// get delta x_1, x_2
	Matrix<double, 3, 2> dxMatrix;
	auto wpI = cloth.getWorldPoint(i);
	auto wpJ = cloth.getWorldPoint(j);
	auto wpK = cloth.getWorldPoint(k);
	for (int elem = 0; elem < 3; elem++) {
		dxMatrix(elem, 0) = wpJ[elem] - wpI[elem];
		dxMatrix(elem, 1) = wpK[elem] - wpI[elem];
	}

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
		return dxMatrix * blUvMatrixI;
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
		return dxMatrix * trUvMatrixI;
	}
}

double scaleXCondition(Cloth &cloth, int *tri, bool isBl) {
	return scaleXCondition(cloth, tri[0], tri[1], tri[2], isBl);
}

double scaleXCondition(Cloth &cloth, int i, int j, int k, bool isBl) {
	double area = cloth.triUvArea;
	auto wm = wuvMatrix(cloth, i, j, k, isBl);

	// get final condition value
	return area * (wm.col(0).norm() - 1);
}

double scaleYCondition(Cloth &cloth, int *tri, bool isBl) {
	return scaleYCondition(cloth, tri[0], tri[1], tri[2], isBl);
}

double scaleYCondition(Cloth &cloth, int i, int j, int k, bool isBl) {
	double area = cloth.triUvArea;
	auto wm = wuvMatrix(cloth, i, j, k, isBl);

	// get final condition value
	return area * (wm.col(1).norm() - 1);
}

RowVector3d scaleXPartial(Cloth &cloth, int pt, int *tri, bool isBl) {
	return scaleXPartial(cloth, pt, tri[0], tri[1], tri[2], isBl);
}

RowVector3d scaleXPartial(Cloth &cloth, int pt,
                          int i, int j, int k, bool isBl) {
	RowVector3d partial; // two rows, three columns

	auto localCond = scaleXCondition(cloth, i, j, k, isBl);
	double *worldPt = cloth.getWorldPoint(pt);

	for (int col = 0; col < 3; col++) {
		// perturb the cloth
		worldPt[col] += PERTURB_QUANT;

		auto pCond1 = scaleXCondition(cloth, i, j, k, isBl);
		worldPt[col] -= 2 * PERTURB_QUANT;
		auto pCond2 = scaleXCondition(cloth, i, j, k, isBl);
		partial[col] = (pCond1 - pCond2) / (2 * PERTURB_QUANT);

		// de-perturb cloth
		worldPt[col] += PERTURB_QUANT;
	}

	return partial;
}

RowVector3d scaleYPartial(Cloth &cloth, int pt, int *tri, bool isBl) {
	return scaleYPartial(cloth, pt, tri[0], tri[1], tri[2], isBl);
}

RowVector3d scaleYPartial(Cloth &cloth, int pt,
                          int i, int j, int k, bool isBl) {
	RowVector3d partial; // two rows, three columns

	auto localCond = scaleYCondition(cloth, i, j, k, isBl);
	double *worldPt = cloth.getWorldPoint(pt);

	for (int col = 0; col < 3; col++) {
		// perturb the cloth
		worldPt[col] += PERTURB_QUANT;

		auto pCond1 = scaleYCondition(cloth, i, j, k, isBl);
		worldPt[col] -= 2 * PERTURB_QUANT;
		auto pCond2 = scaleYCondition(cloth, i, j, k, isBl);
		partial[col] = (pCond1 - pCond2) / (2 * PERTURB_QUANT);

		// de-perturb cloth
		worldPt[col] += PERTURB_QUANT;
	}

	return partial;
}

// returns the p / p pJ of p / p pI of the condition on tri
Matrix3d scaleXSecondPartial(Cloth &cloth, int i, int j, int *tri,
                             bool isBl) {
	Matrix3d partial;

	// perform the fetch once
	int ptA = tri[0];
	int ptB = tri[1];
	int ptC = tri[2];

	auto localPartial = scaleXPartial(cloth, i, ptA, ptB, ptC, isBl);
	auto ptJ = cloth.getWorldPoint(j);

	for (int col = 0; col < 3; col++) {
		// perturb the cloth
		ptJ[col] += PERTURB_QUANT;

		auto pPartial1 = scaleXPartial(cloth, i, ptA, ptB, ptC, isBl);
		ptJ[col] -= 2 * PERTURB_QUANT;
		auto pPartial2 = scaleXPartial(cloth, i, ptA, ptB, ptC, isBl);
		partial.col(col) = (pPartial1 - pPartial2) / (2 * PERTURB_QUANT);

		// de-perturb cloth
		ptJ[col] += PERTURB_QUANT;
	}

	return partial;
}

// returns the p / p pJ of p / p pI of the condition on tri
Matrix3d scaleYSecondPartial(Cloth &cloth, int i, int j, int *tri,
                             bool isBl) {
	Matrix3d partial;

	// perform the fetch once
	int ptA = tri[0];
	int ptB = tri[1];
	int ptC = tri[2];

	auto localPartial = scaleYPartial(cloth, i, ptA, ptB, ptC, isBl);
	auto ptJ = cloth.getWorldPoint(j);

	for (int col = 0; col < 3; col++) {
		// perturb the cloth
		ptJ[col] += PERTURB_QUANT;

		auto pPartial1 = scaleYPartial(cloth, i, ptA, ptB, ptC, isBl);
		ptJ[col] -= 2 * PERTURB_QUANT;
		auto pPartial2 = scaleYPartial(cloth, i, ptA, ptB, ptC, isBl);
		partial.col(col) = (pPartial1 - pPartial2) / (2 * PERTURB_QUANT);

		// de-perturb cloth
		ptJ[col] += PERTURB_QUANT;
	}

	return partial;
}

double shearCondition(Cloth &cloth, int *tri, bool isBl) {
	return shearCondition(cloth, tri[0], tri[1], tri[2], isBl);
}

double shearCondition(Cloth &cloth, int i, int j, int k, bool isBl) {
	auto wuvm = wuvMatrix(cloth, i, j, k, isBl);
	auto area = cloth.triUvArea;

	return area * wuvm.col(0).dot(wuvm.col(1));
}

RowVector3d shearPartial(Cloth &cloth, int pt, int *tri, bool isBl) {
	return shearPartial(cloth, pt, tri[0], tri[1], tri[2], isBl);
}

RowVector3d shearPartial(Cloth &cloth, int pt, int i, int j, int k,
                         bool isBl) {
	RowVector3d partial;

	double localCond = shearCondition(cloth, i, j, k, isBl);
	double *worldPt = cloth.getWorldPoint(pt);

	for (int col = 0; col < 3; col++) {
		// perturb the cloth
		worldPt[col] += PERTURB_QUANT;

		double pCond1 = shearCondition(cloth, i, j, k, isBl);
		worldPt[col] -= 2 * PERTURB_QUANT;
		double pCond2 = shearCondition(cloth, i, j, k, isBl);
		partial[col] = (pCond1 - pCond2) / (2 * PERTURB_QUANT);

		// de-perturb cloth
		worldPt[col] += PERTURB_QUANT;
	}

	return partial;
}

double bendCondition(Cloth &cloth, int *tris) {
	return bendCondition(cloth, tris[0], tris[1], tris[2], tris[3]);
}

// p1,p2,p3 and p4,p3,p2 are counter-clockwise
// p2 and p3 are shared between the two triangles
double bendCondition(Cloth &cloth, int p1, int p2, int p3, int p4) {
	Vector3d n1 = cloth.triNormal(p1, p2, p3);
	Vector3d n2 = cloth.triNormal(p3, p2, p4);
	Vector3d e = Vector3d(cloth.getWorldPoint(p2)) -
	             Vector3d(cloth.getWorldPoint(p3));

	double st = n1.cross(n2).dot(e);
	double ct = n1.dot(n2);

	return atan2(st, ct);
}

RowVector3d bendPartial(Cloth &cloth, int pt, int *tris) {
	return bendPartial(cloth, pt, tris[0], tris[1], tris[2], tris[3]);
}

RowVector3d bendPartial(Cloth &cloth, int pt,
                        int p1, int p2, int p3, int p4) {
	RowVector3d partial;

	auto localCond = bendCondition(cloth, p1, p2, p3, p4);
	double *worldPt = cloth.getWorldPoint(pt);

	for (int col = 0; col < 3; col++) {
		// perturb the cloth
		worldPt[col] += PERTURB_QUANT;

		auto pCond1 = bendCondition(cloth, p1, p2, p3, p4);
		worldPt[col] -= 2 * PERTURB_QUANT;
		auto pCond2 = bendCondition(cloth, p1, p2, p3, p4);
		partial[col] = (pCond1 - pCond2) / (2 * PERTURB_QUANT);

		// de-perturb cloth
		worldPt[col] += PERTURB_QUANT;
	}

	return partial;
}
