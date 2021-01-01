#version 330 core

// from vertex shader
in vec3 fCol;
in vec3 fNome, fnRadius;
in float inscR;

// uniform data
uniform vec3 locR;
uniform vec4 WH_CR;
uniform sampler2D sfTex;
uniform int texJD;
uniform float H3_MAX_RADIUS;
uniform int decMode;

// output data
out vec3 color;

// constant
float H3_REF_RADIUS = 0.99;	// cpp���Ƃ͖��֌W
float refBrRatio = 0.8;


// functions (declare)
float pyth3(float x, float y, float z);
float pyth4(vec4 vec);
float ClcHypbFromEuc(float dst);
float ClcHypbFromEuc(vec3 dst1, vec3 dst2);
float ClcEucFromHypb(float dst);
vec3 toPoinCoord(vec3 tmpPt);
float pyth3(vec3 pts);
float pyth3OS(vec3 pts);
void ParallelMove(vec3 tLoc, bool mode, inout vec3 mvPt[3], int len);
void ReflectionH3(vec3 dst, vec3 ctr, inout vec3 mvPt[3], int len);
vec4 ClcReflected(vec4 grdPt, vec3 trg);


// functions (define)
float pyth3(float x, float y, float z) { return sqrt(x * x + y * y + z * z); }///ok
float pyth4(vec4 vec) {
	return sqrt(vec.x * vec.x + vec.y * vec.y + vec.z * vec.z + vec.w * vec.w);
}///ok

// Euc��������o�ȋ����ɕϊ� (���_-�C�ӓ_)
float ClcHypbFromEuc(float dst)
{
	return atanh(dst);
}
// Euc��������o�ȋ����ɕϊ� (�C�ӓ_1-�C�ӓ_2)
float ClcHypbFromEuc(vec3 dst1, vec3 dst2)
{
	// ���W�ϊ�
	dst1 = toPoinCoord(dst1);
	dst2 = toPoinCoord(dst2);

	// �����Z�o
	float dst1_2L = pyth3(dst1 - dst2);
	float dst1L = pyth3(dst1);
	float dst2L = pyth3(dst2);
	return acosh(1.0 + (2 * dst1_2L * dst1_2L) / ((1.0 - dst1L * dst1L) * (1.0 - dst2L * dst2L)));

}
// �o�ȋ�������Euc�����ɕϊ� (���_-�C�ӓ_)
float ClcEucFromHypb(float dst)
{
	return tanh(dst);
}

// �|�A���J�����f�����W�ɕϊ�
vec3 toPoinCoord(vec3 tmpPt)
{
	float tmpPtW = pyth3OS(tmpPt);
	vec4 tmpP4 = vec4(tmpPt.x, tmpPt.y, tmpPt.z, 1.0 + tmpPtW);
	return (tmpP4 * (1.0 / (1.0 + tmpPtW))).xyz;
}
float pyth3(vec3 pts) { return sqrt(pts.x * pts.x + pts.y * pts.y + pts.z * pts.z); }
float pyth3OS(vec3 pts)
{
	return sqrt(1.0 - (pts.x * pts.x + pts.y * pts.y + pts.z * pts.z));
}

/// <summary>
/// H3 ���s�ړ� ���f2��
/// </summary>
void ParallelMove(vec3 tLoc, bool mode, inout vec3 mvPt[3], int len)
{
	float tLocPh = pyth3(tLoc);

	// ���f1��ڂ̈ʒu����
	vec3 refVec;
	if (tLocPh < 0.001)
		refVec = vec3(0.0, 0.0, H3_REF_RADIUS);
	else if (tLocPh < H3_REF_RADIUS * refBrRatio)
		refVec = tLoc * (H3_REF_RADIUS / tLocPh);
	else
		refVec = tLoc * (H3_REF_RADIUS / tLocPh) * 0.5;

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
	vec4 ctrR = vec4(ctr.x, ctr.y, ctr.z, pyth3OS(ctr));
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
	for (int i = 0; i < len; i++)
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


// ---------> �G���g���֐� <----------
void main()
{
	vec2 gl = vec2(gl_FragCoord.x, gl_FragCoord.y);

	// ���������̌X�����v�Z
	float gzX = ((gl.x / WH_CR.x - 0.5) * 2.0 * WH_CR.z);
	float gzY = ((gl.y / WH_CR.y - 0.5) * 2.0 * WH_CR.w);
	float gzZ = 1.0;
	float gzRate0 = pyth3(gzX, gzY, gzZ);
	vec3 gaze = vec3(gzX, gzY, gzZ) / gzRate0;

	float ip = dot(gaze, -1.0 * normalize(fNome));
	vec3 gazeK = gaze * pyth3(fNome) * abs(1.0 / ip);

	//-----> �[�x�̎Z�o
	float dec = ClcHypbFromEuc(pyth3(gazeK)) / ClcHypbFromEuc(H3_MAX_RADIUS);



	vec3 oCol = fCol;

	gl_FragDepth = dec;

	if (decMode == 0)
		color = oCol;
	else
		color = oCol * (1.0 - dec);

}