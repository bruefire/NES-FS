#version 330 core

// from vertex shader
in vec3 vPos;
in vec3 fCol;
in vec2 txr[3];
in vec3 ptsE[3];

// uniform data
uniform vec3 locR;

// output data
out vec3 color;


// functions (declare)
float pyth3(float x, float y, float z);
float ClcHypbFromEuc(float dst);
float ClcEucFromHypb(float dst);


// functions (define)
float pyth3(float x, float y, float z) { return sqrt(x * x + y * y + z * z); }///ok

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


// ---------> �G���g���֐� <----------
void main()
{
	vec2 gl = vec2(gl_FragCoord.x, gl_FragCoord.y);

	// �[�x�̎Z�o

	float dec;
	//...

	dec = pyth3(locR.x, locR.y, locR.z);
	color = vec3(0.5, 0.0, 1.0) * dec + fCol * (1.0 - dec);
	//gl_FragDepth = 1.001 - dec;

}