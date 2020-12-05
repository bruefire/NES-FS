#version 330 core

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
out float inscR;

// constant
float PIE = 3.1415926535;



// function definition (�w�b�_)
void tudeRst(inout float vec_1, inout float vec_2, float locT, int mode);
float pyth2(float x, float y);
float pyth3(float x, float y, float z);
float pyth3(vec3 pts);
float pyth4(vec4 vec);
float pyth3OS(vec3 pts);
float pythOS(float val);
float atan2(float x, float y);
float ClcHypbFromEuc(float dst);
float ClcEucFromHypb(float dst);
void ParallelMove(vec3 tLoc, bool mode, inout vec3 mvPt[3], int len);
void ReflectionH3(vec3 dst, vec3 ctr, inout vec3 mvPt[3], int len);
vec4 ClcReflected(vec4 grdPt, vec3 trg);
vec3 RelocPts(vec3 pts0);

// function definition (����)
void tudeRst(inout float vec_1, inout float vec_2, float locT, int mode)
{//-- ��,�o,�[���Z�b�g��]
	float tRot = atan2(vec_1, vec_2);
	float R = pyth2(vec_1, vec_2);
	if (0 == mode) {
		vec_1 = R * sin(tRot - locT);
		vec_2 = R * cos(tRot - locT);
	}
	else {
		vec_1 = R * sin(tRot + locT);
		vec_2 = R * cos(tRot + locT);
	}
}
float pyth2(float x, float y) { return sqrt(x * x + y * y); }///ok
float pyth3(float x, float y, float z) { return sqrt(x * x + y * y + z * z); }///ok
float pyth3(vec3 pts) { return sqrt(pts.x * pts.x + pts.y * pts.y + pts.z * pts.z); }///ok
float pyth4(vec4 vec) {
	return sqrt(vec.x * vec.x + vec.y * vec.y + vec.z * vec.z + vec.w * vec.w);
}///ok
float pyth3OS(vec3 pts)
{
	return sqrt(1.0 - (pts.x * pts.x + pts.y * pts.y + pts.z * pts.z));
}
float pythOS(float val)
{
	return sqrt(1.0 - val * val);
}
float atan2(float x, float y) {///ok
	float deg = atan(x / y); deg += PIE * float(y < 0);
	if (x == 0) {
		deg = 0.0; deg += PIE * float(y < 0);
	}
	if (y == 0) {
		deg = 0.5 * PIE; deg += PIE * float(x < 0);
	}
	return deg;
}

// Euc��������o�ȋ����ɕϊ�
float ClcHypbFromEuc(float dst)
{
	//float dstPh = dst * dst;
	//return acosh(1.0 + ((2.0 * dstPh) / (1.0 - dstPh)));
	return atanh(dst);
}
// �o�ȋ�������Euc�����ɕϊ�
float ClcEucFromHypb(float dst)
{
	//float dstSrc = cosh(dst);
	//return sqrt((dstSrc - 1.0) / (1.0 + dstSrc));
	return tanh(dst);
}

/// <summary>
/// H3 ���s�ړ� ���f2��
/// </summary>
void ParallelMove(vec3 tLoc, bool mode, inout vec3 mvPt[3], int len)
{
	float tLocPh = pyth3(tLoc);
	vec3 refVec = (tLocPh < 0.001)
		? vec3(0.0, 0.0, H3_REF_RADIUS)
		: tLoc * (H3_REF_RADIUS / tLocPh);

	vec3 bgnPt, endPt;
	if (mode)
	{
		bgnPt = vec3(0.0, 0.0, 0.0);
		endPt = tLoc;
	}
	else
	{
		bgnPt = tLoc;
		endPt = vec3(0.0, 0.0, 0.0);
	}

	ReflectionH3(refVec, bgnPt, mvPt, len);
	ReflectionH3(endPt, refVec, mvPt, len);
}

// ���f (H3)
// dstPts: �ړ������x�N�g�� (���_���痣�ꂽ�_���w�肷��)
void ReflectionH3(vec3 dst, vec3 ctr, inout vec3 mvPt[3], int len)
{
	// ���f�p���ʏ�̓_ src, dst
	vec4 ctrR = vec4( ctr.x, ctr.y, ctr.z, pyth3OS(ctr));
	vec4 dstR = vec4(dst.x, dst.y, dst.z, pyth3OS(dst));

	// locR, dstR��ʂ�N���C�����ʂɐڂ��钼��
	// �ؕЁA�X���Z�o
	vec4 ldDif = ctrR - dstR;
	float slopeX = ldDif.x / ldDif.w;
	float slopeY = ldDif.y / ldDif.w;
	float slopeZ = ldDif.z / ldDif.w;
	float segmX = ctrR.x - ctrR.w * slopeX;
	float segmY = ctrR.y - ctrR.w * slopeY;
	float segmZ = ctrR.z - ctrR.w * slopeZ;

	// �e�ؕЂ𐬕��Ƃ����_���ڒn�_
	vec4 grdPt = vec4(segmX, segmY, segmZ, 0.0);


	// ���f���ʂ��Z�o
	for (int i=0; i<len; i++)
	{
		mvPt[i] = ClcReflected(grdPt, mvPt[i]).xyz;
	}
}

// ���f���ʂ��Z�o
vec4 ClcReflected(vec4 grdPt, vec3 trg)
{
	// ���f�p���ʏ�̓_ std1, std2
	vec4 trgPt = vec4(trg.x, trg.y, trg.z, pyth3OS(trg));

	// ���ʌ��_����̐����x�N�g�� (�ړ_)
	vec4 trgToGrd = grdPt - trgPt;
	float ip = dot(normalize(trgToGrd), normalize(trgPt));
	float ttgRate = pyth4(trgPt) / pyth4(trgToGrd);
	vec4 ttgNorm = trgToGrd * ttgRate * ip;

	// ����
	vec4 result = trgPt - ttgNorm - ttgNorm;

	return result;
};

// ���_�ʒu�č\�z
vec3 RelocPts(vec3 pts0)
{
	// ���_�ʒu���f
	vec3 pts = vec3(
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
	vec3 pts = RelocPts(vPosition);

	// ���̈ʒu�ɖ߂�
	vec3 pmVec[3] = vec3[](pts, vec3(0.0, 0.0, 0.0), vec3(0.0, 0.0, 0.0));	// ���2�͎̂Ēl
	ParallelMove(locR, true, pmVec, 1);
	pts = pmVec[0];

	// �E��/����n�݊�
	float tmptY = pts.y;
	pts.y = pts.z;
	pts.z = tmptY;


	// ���ʂ��s�N�Z���V�F�[�_��
	gl_Position = MVP * vec4(pts, 1);

	// �F���
	fCol = vColor;

	//-- �@���Z�o
	pmVec[0] = RelocPts(vPos1);
	pmVec[1] = RelocPts(vPos2);
	pmVec[2] = RelocPts(vPos3);
	ParallelMove(locR, true, pmVec, 3);

	vec3 norm0 = cross(pmVec[1]-pmVec[0], pmVec[2]-pmVec[1]);	// �@��
	vec3 norm0n = normalize(norm0);
	float ratio = abs(dot(norm0n, normalize(pmVec[0])));
	float normL = pyth3(pmVec[0]) * ratio;
	float normLos = pythOS(normL);
	float restL = normLos * tan(0.5 * PIE - atan(normL / normLos));

	float inscR = pyth2(normLos, restL);
	fNome = norm0n * normL;
	fnRadius = norm0n * (normL + restL);

	//-- �ʒu���
	ptsE[0] = pmVec[0];
	ptsE[1] = pmVec[1];
	ptsE[2] = pmVec[2];

	//-- �e�X�N�`��
	txr[0] = tPos1;
	txr[1] = tPos2;
	txr[2] = tPos3;
}
