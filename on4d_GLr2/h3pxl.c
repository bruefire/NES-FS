#version 330 core

// from vertex shader
in vec3 fCol;
in vec2 txr[3];
in vec3 ptsE[3];
in vec3 fNome, fnRadius;
in float inscR;

// uniform data
uniform vec3 locR;
uniform vec4 WH_CR;
uniform float H3_REF_RADIUS;

// output data
out vec3 color;


// functions (declare)
float pyth3(float x, float y, float z);
float ClcHypbFromEuc(float dst);
float ClcEucFromHypb(float dst);
vec3 toPoinCoord(vec3 tmpPt);
float pyth3(vec3 pts);
float pyth3OS(vec3 pts);


// functions (define)
float pyth3(float x, float y, float z) { return sqrt(x * x + y * y + z * z); }///ok

// Euc距離から双曲距離に変換
float ClcHypbFromEuc(float dst)
{
	return atanh(dst);
}
// 双曲距離からEuc距離に変換
float ClcEucFromHypb(float dst)
{
	return tanh(dst);
}

// ポアンカレモデル座標に変換
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


// ---------> エントリ関数 <----------
void main()
{
	vec2 gl = vec2(gl_FragCoord.x, gl_FragCoord.y);

	// 視線方向の傾きを計算
	float gzX = ((gl.x / WH_CR.x - 0.5) * 2.0 * WH_CR.z);
	float gzY = ((gl.y / WH_CR.y - 0.5) * 2.0 * WH_CR.w);
	float gzZ = 1.0;
	float gzRate0 = pyth3(gzX, gzY, gzZ);
	vec3 gaze = vec3(gzX, gzY, gzZ) / gzRate0;

	float ip = dot(gaze, -1.0 * normalize(fNome));
	vec3 gazeK = gaze * pyth3(fNome) * abs(1.0 / ip);

	// 深度の算出
	float dec = ClcHypbFromEuc(pyth3(gazeK)) / ClcHypbFromEuc(H3_REF_RADIUS);

	// Lighting
	// ポアンカレ座標に変換
	vec3 gazeP = toPoinCoord(gazeK);
	vec3 fnrNome = gazeP - fnRadius;
	float nDeg = dot(normalize(fnrNome), -normalize(gazeP));

	color = vec3(0.0, 0.5, 1.0) * dec + fCol * nDeg * (1.0 - dec);
	//gl_FragDepth = 1.001 - dec;

}