#version 330 core


attribute vec3 vPosition;
attribute vec3 vColor;

attribute vec3 vPos1;
attribute vec3 vPos2;


uniform vec2 scl_rad;
uniform vec3 objRot;
uniform vec3 objStd;
uniform vec3 locR;
uniform float revMd;

uniform mat4 MVP;
uniform vec4 WH_CR;
uniform vec4 cRange;

out vec2 s_b;
out vec4 slopeC, betaC;
out vec3 fCol;


float PIE = 3.1415926535;


//-----------------------------------
void clcSlope4(vec4 drawP1, vec4 drawP2, out vec4 slopeC, out vec4 betaC);///ok
void tudeRst(inout float, inout float, float, int);///ok
void all_tudeRst_0(inout vec4 vect, vec3 locT);///ok
void all_tudeRst_1(inout vec4 vect, vec3 locT);///ok
vec4 tudeToEuc(vec3 locT);
vec3 eucToTude(vec4 vecT);	 ///ok
float pyth2(float x, float y);
float pyth3(float x, float y, float z);
float atan2(float x, float y);
float pyth4(vec4 vec);
//-----------------------------------


float pyth4(vec4 vec) {
	return sqrt(vec.x * vec.x + vec.y * vec.y + vec.z * vec.z + vec.w * vec.w);
}///ok
void clcSlope4(vec4 drawP1, vec4 drawP2, out vec4 slopeC, out vec4 betaC)///ok
{	///-- 4次元直線の傾きを計算
	///--

	//	adjY4(&drawP1, gen1);	//--値調整
	slopeC.x = (drawP1.x - drawP2.x) / (drawP1.y - drawP2.y);
	slopeC.z = (drawP1.z - drawP2.z) / (drawP1.y - drawP2.y);
	slopeC.w = (drawP1.w - drawP2.w) / (drawP1.y - drawP2.y);
	betaC.x = drawP1.x - (slopeC.x * drawP1.y);
	betaC.z = drawP1.z - (slopeC.z * drawP1.y);
	betaC.w = drawP1.w - (slopeC.w * drawP1.y);
}
void tudeRst(inout float vec_1, inout float vec_2, float locT, int mode)///ok
{//-- 緯,経,深リセット回転
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
void all_tudeRst_0(inout vec4 vect, vec3 locT)///ok
{//-- 緯,経,深リセット回転
	tudeRst(vect.x, vect.y, locT.x, 0);//-- X-Y 回転
	tudeRst(vect.y, vect.z, locT.y, 0);//-- Y-Z 回転
	tudeRst(vect.z, vect.w, locT.z, 0);//-- Z-W 回転
}
void all_tudeRst_1(inout vec4 vect, vec3 locT)///ok
{//-- 緯,経,深リセット回転
	tudeRst(vect.z, vect.w, locT.z, 1);//-- Z-W 回転
	tudeRst(vect.y, vect.z, locT.y, 1);//-- Y-Z 回転
	tudeRst(vect.x, vect.y, locT.x, 1);//-- X-Y 回転
}

vec4 tudeToEuc(vec3 locT) {	// [緯,経,深]座標を[XYZ]W座標に変換
	vec4 vecT = vec4(
		sin(locT.z) * sin(locT.y) * sin(locT.x),//X
		sin(locT.z) * sin(locT.y) * cos(locT.x),//Y
		sin(locT.z) * cos(locT.y),		//Z
		cos(locT.z)	//W
		);
	return vecT;
}
vec3 eucToTude(vec4 vecT) {	 ///ok
	vec3 locT;
	locT.x = atan2(vecT.x, vecT.y);
	locT.y = atan2(pyth2(vecT.x, vecT.y), vecT.z);
	locT.z = atan2(pyth3(vecT.x, vecT.y, vecT.z), vecT.w);

	return locT;
}
float pyth2(float x, float y) { return sqrt(pow(x, 2.0) + pow(y, 2.0)); }///ok
float pyth3(float x, float y, float z) { return sqrt(pow(x, 2.0) + pow(y, 2.0) + pow(z, 2.0)); }///ok
float atan2(float x, float y) {///ok
	float deg = atan(x / y); deg += PIE * float(y < 0.0);
	if (x == 0.0) {
		deg = 0.0; deg += PIE * float(y < 0.0);
	}
	if (y == 0.0) {
		deg = 0.5 * PIE; deg += PIE * float(x < 0.0);
	}
	return deg;
}

void setVertex(inout vec3 vPos)
{
	vec4 pts1 = vec4(	// 始点 XYZW
		0.0,
		sin(scl_rad[0] * vPos.z / scl_rad[1]),
		0.0,
		cos(scl_rad[0] * vPos.z / scl_rad[1])
		);
	tudeRst(pts1.z, pts1.y, vPos.y, 1);	// 角度1
	tudeRst(pts1.x, pts1.z, vPos.x, 1);	// 角度2

	///-- 自転の反映
	tudeRst(pts1.x, pts1.y, objRot.z, 1);	// 角度3
	tudeRst(pts1.y, pts1.z, objRot.y, 1);	// 角度2
	tudeRst(pts1.x, pts1.z, objRot.x, 1);	// 角度1

	///-- 基準(回転)に合わせる
	tudeRst(pts1.x, pts1.y, objStd[2], 1);	// 方向3
	tudeRst(pts1.y, pts1.z, objStd[1], 1);	// 方向2
	tudeRst(pts1.x, pts1.y, objStd[0], 1);	// 方向1
	///-- オブジェクト座標に合わせる
	all_tudeRst_1(pts1, locR);	// 方向3
	//---- 座標変換して格納 (end ----
	vPos = eucToTude(pts1);

}

//--------------=============================
//--------------=============================
//--------------=============================



void main()
{
	vec3 vPosX = vPosition;
	vec3 vPos[2] = vec3[](vPos1, vPos2);

	setVertex(vPosX);
	for (int i = 0; i < 2; i++)
		setVertex(vPos[i]);

	vec3 drawPts = tudeToEuc(vPosX).xyz;
	tudeRst(drawPts.y, drawPts.z, 0.5 * PIE, 1); //-- Y軸を奥行きに (互換のため)
	drawPts.z *= -1.0;

	// 頂点演算は普通の3Dと同様
	drawPts *= revMd;
	gl_Position = MVP * vec4(drawPts, 1.0);


	///-- 点情報コピー, EUC座標に変換, Y-Z軸間で90度回転, Z軸を反転
	vec3 drawPt[2];
	vec4 drawP4[2];
	for (int j = 0; j < 2; j++) {
		drawP4[j] = tudeToEuc(vPos[j]);
		drawPt[j] = drawP4[j].xyz;

		tudeRst(drawPt[j].y, drawPt[j].z, 0.5 * PIE, 1); //-- Y軸を奥行きに (互換のため)
		drawPt[j].z *= -1.0;
	}


	///-- 弦の傾き計算
	vec4 tmPt[2] = vec4[](drawP4[0], drawP4[1]);
	clcSlope4(tmPt[0], tmPt[1], slopeC, betaC);

	//--sc上3点の 傾き & 余り--
	float cRangeLR = cRange[0] + cRange[1];
	float cRangeTB = cRange[2] + cRange[3];
	vec2 glVtx0 = vec2(
			(drawPt[0].x / drawPt[0].y + cRange[0]) / cRangeLR * WH_CR.x,
			(drawPt[0].z / drawPt[0].y + cRange[3]) / cRangeTB * WH_CR.y);
	vec2 glVtx1 = vec2(
			(drawPt[1].x / drawPt[1].y + cRange[0]) / cRangeLR * WH_CR.x,
			(drawPt[1].z / drawPt[1].y + cRange[3]) / cRangeTB * WH_CR.y);

	vec2 sPt[2] = vec2[](glVtx0, glVtx1);

	float slp, beta;
	slp = (sPt[1].x - sPt[0].x) / (sPt[1].y - sPt[0].y);
	beta = sPt[0].x - sPt[0].y * slp;

	s_b = vec2(slp, beta);


	//-- 色設定
	fCol = vColor;
}

