#version 330 core

// transfared vertices
layout(location = 0) in vec3 vPosition;
layout(location = 1) in vec3 vColor;
layout(location = 2) in vec2 TXR;

// uniform data
uniform mat4 MVP;
uniform vec2 scl_rad;
uniform vec3 objRot;
uniform vec3 objStd;
uniform vec3 locR;
uniform float H3_REF_RADIUS;

// to pixel shader
out vec3 vPos;
out vec2 txr;
out vec3 fCol;
float PIE = 3.1415926535;



// function definition (�w�b�_)
void tudeRst(inout float vec_1, inout float vec_2, float locT, int mode);
float pyth2(float x, float y);
float pyth3(float x, float y, float z);
float pyth3(vec3 pts);
float pyth4(vec4 vec);
float pyth3OS(vec3 pts, float hLen);
float atan2(float x, float y);
float ClcHypbFromEuc(float dst);
float ClcEucFromHypb(float dst);
vec3 ParallelMove(vec3 tLoc, vec3 ctr, vec3 mvPt);
vec3 ReflectionH3(vec3 dst, vec3 ctr, vec3 mvPt);
vec4 ClcReflected(vec4 grdPt, vec3 trg);

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
	float dstPh = dst * dst;
	return acosh(1.0 + ((2.0 * dstPh) / (1.0 - dstPh)));
}
// �o�ȋ�������Euc�����ɕϊ�
float ClcEucFromHypb(float dst)
{
	float dstSrc = cosh(dst);
	return sqrt((dstSrc - 1.0) / (1.0 + dstSrc));
}

/// <summary>
/// H3 ���s�ړ� ���f2��
/// </summary>
vec3 ParallelMove(vec3 tLoc, vec3 ctr, vec3 mvPt)
{
	float tLocPh = pyth3(tLoc);
	vec3 refVec = (tLocPh < 0.001)
		? vec3(0.0, 0.0, H3_REF_RADIUS)
		: tLoc * (H3_REF_RADIUS / tLocPh);

	vec3 mrr = ReflectionH3(refVec, ctr, mvPt);
	vec3 rst = ReflectionH3(tLoc, refVec, mrr);

	// ���ʔ��f
	return rst;
}

// ���f (H3)
// dstPts: �ړ������x�N�g�� (���_���痣�ꂽ�_���w�肷��)
vec3 ReflectionH3(vec3 dst, vec3 ctr, vec3 mvPt)
{
	// ���f�p���ʏ�̓_ src, dst
	vec4 ctrR = vec4( ctr.x, ctr.y, ctr.z, pyth3OS(ctr));
	vec4 dstR = vec4(dst.x, dst.y, dst.z, pyth3OS(dst));

	// locR, dstR��ʂ�|�A���J�����ʂɐڂ��钼��
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
	vec4 mvptR = ClcReflected(grdPt, mvPt);

	return mvptR.xyz;
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



// ---------> �G���g���֐� <-----------
void main()
{
	// ���_�ʒu���f
	vec3 pts = vec3(
		0.0,
		ClcEucFromHypb(scl_rad[0] * vPosition.z / scl_rad[1]),
		0.0
	);
	tudeRst(pts.z, pts.y, vPosition.y, 1);	// �p�x1
	tudeRst(pts.x, pts.z, vPosition.x, 1);	// �p�x2

	// ���]�̔��f
	tudeRst(pts.x, pts.y, objRot.z, 1);	// �p�x3
	tudeRst(pts.y, pts.z, objRot.y, 1);	// �p�x2
	tudeRst(pts.x, pts.z, objRot.x, 1);	// �p�x1

	// ���_�ړ���std�̔��f
	tudeRst(pts.x, pts.y, objStd[2], 1);	// ����3
	tudeRst(pts.y, pts.z, objStd[1], 1);	// ����2
	tudeRst(pts.x, pts.y, objStd[0], 1);	// ����1

	// ���̈ʒu�ɖ߂�
	pts = ParallelMove(locR, vec3(0.0, 0.0, 0.0), pts);


	// �E��/����n�݊�
	float tmptY = pts.y;
	pts.y = pts.z;
	pts.z = tmptY;


	// ���ʂ��s�N�Z���V�F�[�_��
	gl_Position = MVP * vec4(pts, 1);
	txr = TXR;
	fCol = vColor;
}
