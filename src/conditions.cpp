#include "conditions.h"

// cached uvMatrix inverses for bot-left, top-right triangles
bool setBlUvMatrix = false;
Matrix2d blUvMatrixI;
bool setTrUvMatrix = false;
Matrix2d trUvMatrixI;

WuvMatrix wuvMatrix(Cloth &cloth, int i, int j, int k, bool isBl) {
	// get delta x_1, x_2
	Eigen::Matrix<double, 3, 2> dxMatrix;
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

double scaleXCondition(Cloth &cloth, int *tri, bool isBl, double b) {
	double area = cloth.triUvArea;
	auto wm = wuvMatrix(cloth, tri[0], tri[1], tri[2], isBl);

	// get final condition value
	return area * (wm.col(0).norm() - b);
}

double scaleYCondition(Cloth &cloth, int *tri, bool isBl, double b) {
	double area = cloth.triUvArea;
	auto wm = wuvMatrix(cloth, tri[0], tri[1], tri[2], isBl);

	// get final condition value
	return area * (wm.col(1).norm() - b);
}

RowVector3d scaleXPartial(Cloth &cloth, int pt, int *tri, bool isBl,
                          double b) {
	RowVector3d partial; // two rows, three columns

	double *worldPt = cloth.getWorldPoint(pt);
	double originalPoint[3] = {worldPt[0], worldPt[1], worldPt[2]};

	for (int col = 0; col < 3; col++) {
		// perturb the cloth
		worldPt[col] = originalPoint[col] + PERTURB_QUANT;
		auto pCond1 = scaleXCondition(cloth, tri, isBl, b);

		worldPt[col] = originalPoint[col] - PERTURB_QUANT;
		auto pCond2 = scaleXCondition(cloth, tri, isBl, b);

		partial[col] = (pCond1 - pCond2) / (2 * PERTURB_QUANT);

		// de-perturb cloth
		worldPt[col] = originalPoint[col];
	}

	return partial;
}

RowVector3d scaleYPartial(Cloth &cloth, int pt, int *tri, bool isBl,
                          double b) {
	RowVector3d partial; // two rows, three columns

	double *worldPt = cloth.getWorldPoint(pt);
	double originalPoint[3] = {worldPt[0], worldPt[1], worldPt[2]};

	for (int col = 0; col < 3; col++) {
		// perturb the cloth
		worldPt[col] = originalPoint[col] + PERTURB_QUANT;
		auto pCond1 = scaleYCondition(cloth, tri, isBl, b);

		worldPt[col] = originalPoint[col] - PERTURB_QUANT;
		auto pCond2 = scaleYCondition(cloth, tri, isBl, b);

		partial[col] = (pCond1 - pCond2) / (2 * PERTURB_QUANT);

		// de-perturb cloth
		worldPt[col] = originalPoint[col];
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

	double *worldPt = cloth.getWorldPoint(pt);
	double originalPoint[3] = {worldPt[0], worldPt[1], worldPt[2]};

	for (int col = 0; col < 3; col++) {
		// perturb the cloth
		worldPt[col] = originalPoint[col] + PERTURB_QUANT;
		double pCond1 = shearCondition(cloth, i, j, k, isBl);

		worldPt[col] = originalPoint[col] - PERTURB_QUANT;
		double pCond2 = shearCondition(cloth, i, j, k, isBl);

		partial[col] = (pCond1 - pCond2) / (2 * PERTURB_QUANT);

		// de-perturb cloth
		worldPt[col] = originalPoint[col];
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

	double *worldPoint = cloth.getWorldPoint(pt);
	double originalPoint[3] = {worldPoint[0], worldPoint[1], worldPoint[2]};

	for (int col = 0; col < 3; col++) {
		// perturb the cloth
		worldPoint[col] = originalPoint[col] + PERTURB_QUANT;
		auto pCond1 = bendCondition(cloth, p1, p2, p3, p4);

		worldPoint[col] = originalPoint[col] - PERTURB_QUANT;
		auto pCond2 = bendCondition(cloth, p1, p2, p3, p4);

		partial[col] = (pCond1 - pCond2) / (2 * PERTURB_QUANT);

		// de-perturb cloth
		worldPoint[col] = originalPoint[col];
	}

	return partial;
}
