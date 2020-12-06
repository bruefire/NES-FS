#version 400 core

// transfared vertices
layout(location = 0) in vec3 vPosition;
layout(location = 1) in vec3 vColor;

layout(location = 2) in vec3 vPos1;
layout(location = 3) in vec3 vPos2;
layout(location = 4) in vec3 vPos3;
layout(location = 5) in vec2 tPos1;
layout(location = 6) in vec2 tPos2;
layout(location = 7) in vec2 tPos3;

// uniform data
uniform mat4 MVP;
uniform vec2 scl_rad;
uniform vec3 objRot;
uniform vec3 objStd;
uniform vec3 locR;
uniform float H3_REF_RADIUS;

// to pixel shader
out vec3 fCol;
out vec2 txr[3];
out vec3 ptsE[3];
out vec3 fNome, fnRadius;
out double inscR;

// constant
double PIE = 3.1415926535;



// function definition (�w�b�_)
double sin_d(double val);
double cos_d(double val);
double tan_d(double val);
double atan_d(double val);
double tanh_d(double val);
double atanh_d(double val);

void tudeRst(inout double vec_1, inout double vec_2, double locT, int mode);
double pyth2(double x, double y);
double pyth3(double x, double y, double z);
double pyth3(dvec3 pts);
double pyth4(dvec4 vec);
double pyth3OS(dvec3 pts);
double pythOS(double val);
double atan2(double x, double y);
double ClcHypbFromEuc(double dst);
double ClcEucFromHypb(double dst);
void ParallelMove(dvec3 tLoc, bool mode, inout dvec3 mvPt[3], int len);
void ReflectionH3(dvec3 dst, dvec3 ctr, inout dvec3 mvPt[3], int len);
dvec4 ClcReflected(dvec4 grdPt, dvec3 trg);
dvec3 RelocPts(dvec3 pts0);

// function definition (����)
double sin_d(double val) { return double(sin(float(val))); };
double cos_d(double val) { return double(cos(float(val))); };
double tan_d(double val) { return double(tan(float(val))); };
double atan_d(double val) { return double(atan(float(val))); };
double tanh_d(double val) { return double(tanh(float(val))); };
double atanh_d(double val) { return double(atanh(float(val))); };

void tudeRst(inout double vec_1, inout double vec_2, double locT, int mode)
{//-- ��,�o,�[���Z�b�g��]
	double tRot = atan2(vec_1, vec_2);
	double R = pyth2(vec_1, vec_2);
	if (0 == mode) {
		vec_1 = R * sin_d(tRot - locT);
		vec_2 = R * cos_d(tRot - locT);
	}
	else {
		vec_1 = R * sin_d(tRot + locT);
		vec_2 = R * cos_d(tRot + locT);
	}
}
double pyth2(double x, double y) { return sqrt(x * x + y * y); }///ok
double pyth3(double x, double y, double z) { return sqrt(x * x + y * y + z * z); }///ok
double pyth3(dvec3 pts) { return sqrt(pts.x * pts.x + pts.y * pts.y + pts.z * pts.z); }///ok
double pyth4(dvec4 vec) {
	return sqrt(vec.x * vec.x + vec.y * vec.y + vec.z * vec.z + vec.w * vec.w);
}///ok
double pyth3OS(dvec3 pts)
{
	return sqrt(1.0 - (pts.x * pts.x + pts.y * pts.y + pts.z * pts.z));
}
double pythOS(double val)
{
	return sqrt(1.0 - val * val);
}
double atan2(double x, double y) {///ok
	double deg = atan_d(x / y); deg += PIE * double(y < 0);
	if (x == 0) {
		deg = 0.0; deg += PIE * double(y < 0);
	}
	if (y == 0) {
		deg = 0.5 * PIE; deg += PIE * double(x < 0);
	}
	return deg;
}

// Euc��������o�ȋ����ɕϊ�
double ClcHypbFromEuc(double dst)
{
	//double dstPh = dst * dst;
	//return acosh(1.0 + ((2.0 * dstPh) / (1.0 - dstPh)));
	return atanh_d(dst);
}
// �o�ȋ�������Euc�����ɕϊ�
double ClcEucFromHypb(double dst)
{
	//double dstSrc = cosh(dst);
	//return sqrt((dstSrc - 1.0) / (1.0 + dstSrc));
	return tanh_d(dst);
}

/// <summary>
/// H3 ���s�ړ� ���f2��
/// </summary>
void ParallelMove(dvec3 tLoc, bool mode, inout dvec3 mvPt[3], int len)
{
	double tLocPh = pyth3(tLoc);
	dvec3 refVec = (tLocPh < 0.001)
		? dvec3(0.0, 0.0, H3_REF_RADIUS)
		: tLoc * (H3_REF_RADIUS / tLocPh);

	dvec3 bgnPt, endPt;
	if (mode)
	{
		bgnPt = dvec3(0.0, 0.0, 0.0);
		endPt = tLoc;
	}
	else
	{
		bgnPt = tLoc;
		endPt = dvec3(0.0, 0.0, 0.0);
	}

	ReflectionH3(refVec, bgnPt, mvPt, len);
	ReflectionH3(endPt, refVec, mvPt, len);
}

// ���f (H3)
// dstPts: �ړ������x�N�g�� (���_���痣�ꂽ�_���w�肷��)
void ReflectionH3(dvec3 dst, dvec3 ctr, inout dvec3 mvPt[3], int len)
{
	// ���f�p���ʏ�̓_ src, dst
	dvec4 ctrR = dvec4( ctr.x, ctr.y, ctr.z, pyth3OS(ctr));
	dvec4 dstR = dvec4(dst.x, dst.y, dst.z, pyth3OS(dst));

	// locR, dstR��ʂ�N���C�����ʂɐڂ��钼��
	// �ؕЁA�X���Z�o
	dvec4 ldDif = ctrR - dstR;
	double slopeX = ldDif.x / ldDif.w;
	double slopeY = ldDif.y / ldDif.w;
	double slopeZ = ldDif.z / ldDif.w;
	double segmX = ctrR.x - ctrR.w * slopeX;
	double segmY = ctrR.y - ctrR.w * slopeY;
	double segmZ = ctrR.z - ctrR.w * slopeZ;

	// �e�ؕЂ𐬕��Ƃ����_���ڒn�_
	dvec4 grdPt = dvec4(segmX, segmY, segmZ, 0.0);


	// ���f���ʂ��Z�o
	for (int i=0; i<len; i++)
	{
		mvPt[i] = ClcReflected(grdPt, mvPt[i]).xyz;
	}
}

// ���f���ʂ��Z�o
dvec4 ClcReflected(dvec4 grdPt, dvec3 trg)
{
	// ���f�p���ʏ�̓_ std1, std2
	dvec4 trgPt = dvec4(trg.x, trg.y, trg.z, pyth3OS(trg));

	// ���ʌ��_����̐����x�N�g�� (�ړ_)
	dvec4 trgToGrd = grdPt - trgPt;
	double ip = dot(normalize(trgToGrd), normalize(trgPt));
	double ttgRate = pyth4(trgPt) / pyth4(trgToGrd);
	dvec4 ttgNorm = trgToGrd * ttgRate * ip;

	// ����
	dvec4 result = trgPt - ttgNorm - ttgNorm;

	return result;
};

// ���_�ʒu�č\�z
dvec3 RelocPts(dvec3 pts0)
{
	// ���_�ʒu���f
	dvec3 pts = dvec3(
		0.0,
		ClcEucFromHypb(scl_rad[0] * pts0.z / scl_rad[1]),
		0.0
		);
	tudeRst(pts.z, pts.y, pts0.y, 1);	// �p�x1
	tudeRst(pts.x, pts.z, pts0.x, 1);	// �p�x2

	// ���]�̔��f
	tudeRst(pts.x, pts.y, objRot.z, 1);	// �p�x3
	tudeRst(pts.y, pts.z, objRot.y, 1);	// �p�x2
	tudeRst(pts.x, pts.z, objRot.x, 1);	// �p�x1

	// ���_�ړ���std�̔��f
	tudeRst(pts.x, pts.y, objStd[2], 1);	// ����3
	tudeRst(pts.y, pts.z, objStd[1], 1);	// ����2
	tudeRst(pts.x, pts.y, objStd[0], 1);	// ����1

	return pts;
}



// ---------> �G���g���֐� <-----------
void main()
{
	// ���_�ʒu���f
	dvec3 pts = RelocPts(vPosition);

	// ���̈ʒu�ɖ߂�
	dvec3 pmVec[3] = dvec3[](pts, dvec3(0.0, 0.0, 0.0), dvec3(0.0, 0.0, 0.0));	// ���2�͎̂Ēl
	ParallelMove(locR, true, pmVec, 1);
	pts = pmVec[0];

	// �E��/����n�݊�
	double tmptY = pts.y;
	pts.y = pts.z;
	pts.z = tmptY;


	// ���ʂ��s�N�Z���V�F�[�_��
	gl_Position = MVP * vec4(pts, 1);

	// �F���
	fCol = vec3(vColor);

	//-- �@���Z�o
	pmVec[0] = RelocPts(vPos1);
	pmVec[1] = RelocPts(vPos2);
	pmVec[2] = RelocPts(vPos3);
	ParallelMove(locR, true, pmVec, 3);

	dvec3 norm0 = cross(pmVec[1]-pmVec[0], pmVec[2]-pmVec[1]);	// �@��
	dvec3 norm0n = normalize(norm0);
	double ratio = abs(dot(norm0n, normalize(pmVec[0])));
	double normL = pyth3(pmVec[0]) * ratio;
	double normLos = pythOS(normL);
	double restL = normLos * tan_d(0.5 * PIE - atan_d(normL / normLos));

	double inscR = pyth2(normLos, restL);
	fNome = vec3(norm0n * normL);
	fnRadius = vec3(norm0n * (normL + restL));

	//-- �ʒu���
	ptsE[0] = vec3(pmVec[0]);
	ptsE[1] = vec3(pmVec[1]);
	ptsE[2] = vec3(pmVec[2]);

	//-- �e�X�N�`��
	txr[0] = vec2(tPos1);
	txr[1] = vec2(tPos2);
	txr[2] = vec2(tPos3);
}
