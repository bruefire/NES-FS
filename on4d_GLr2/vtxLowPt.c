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
{	///-- 4ŸŒ³’¼ü‚ÌŒX‚«‚ğŒvZ
	///--

	//	adjY4(&drawP1, gen1);	//--’l’²®
	slopeC.x = (drawP1.x - drawP2.x) / (drawP1.y - drawP2.y);
	slopeC.z = (drawP1.z - drawP2.z) / (drawP1.y - drawP2.y);
	slopeC.w = (drawP1.w - drawP2.w) / (drawP1.y - drawP2.y);
	betaC.x = drawP1.x - (slopeC.x * drawP1.y);
	betaC.z = drawP1.z - (slopeC.z * drawP1.y);
	betaC.w = drawP1.w - (slopeC.w * drawP1.y);
}
void tudeRst(inout float vec_1, inout float vec_2, float locT, int mode)///ok
{//-- ˆÜ,Œo,[ƒŠƒZƒbƒg‰ñ“]
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
{//-- ˆÜ,Œo,[ƒŠƒZƒbƒg‰ñ“]
	tudeRst(vect.x, vect.y, locT.x, 0);//-- X-Y ‰ñ“]
	tudeRst(vect.y, vect.z, locT.y, 0);//-- Y-Z ‰ñ“]
	tudeRst(vect.z, vect.w, locT.z, 0);//-- Z-W ‰ñ“]
}
void all_tudeRst_1(inout vec4 vect, vec3 locT)///ok
{//-- ˆÜ,Œo,[ƒŠƒZƒbƒg‰ñ“]
	tudeRst(vect.z, vect.w, locT.z, 1);//-- Z-W ‰ñ“]
	tudeRst(vect.y, vect.z, locT.y, 1);//-- Y-Z ‰ñ“]
	tudeRst(vect.x, vect.y, locT.x, 1);//-- X-Y ‰ñ“]
}

vec4 tudeToEuc(vec3 locT) {	// [ˆÜ,Œo,[]À•W‚ğ[XYZ]WÀ•W‚É•ÏŠ·
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
	vec4 pts1 = vec4(	// n“_ XYZW
		0.0,
		sin(scl_rad[0] * vPos.z / scl_rad[1]),
		0.0,
		cos(scl_rad[0] * vPos.z / scl_rad[1])
		);
	tudeRst(pts1.z, pts1.y, vPos.y, 1);	// Šp“x1
	tudeRst(pts1.x, pts1.z, vPos.x, 1);	// Šp“x2

	///-- ©“]‚Ì”½‰f
	tudeRst(pts1.x, pts1.y, objRot.z, 1);	// Šp“x3
	tudeRst(pts1.y, pts1.z, objRot.y, 1);	// Šp“x2
	tudeRst(pts1.x, pts1.z, objRot.x, 1);	// Šp“x1

	///-- Šî€(‰ñ“])‚É‡‚í‚¹‚é
	tudeRst(pts1.x, pts1.y, objStd[2], 1);	// •ûŒü3
	tudeRst(pts1.y, pts1.z, objStd[1], 1);	// •ûŒü2
	tudeRst(pts1.x, pts1.y, objStd[0], 1);	// •ûŒü1
	///-- ƒIƒuƒWƒFƒNƒgÀ•W‚É‡‚í‚¹‚é
	all_tudeRst_1(pts1, locR);	// •ûŒü3
	//---- À•W•ÏŠ·‚µ‚ÄŠi”[ (end ----
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
	tudeRst(drawPts.y, drawPts.z, 0.5 * PIE, 1); //-- Y²‚ğ‰œs‚«‚É (ŒİŠ·‚Ì‚½‚ß)
	drawPts.z *= -1.0;

	// ’¸“_‰‰Z‚Í•’Ê‚Ì3D‚Æ“¯—l
	drawPts *= revMd;
	gl_Position = MVP * vec4(drawPts, 1.0);


	///-- “_î•ñƒRƒs[, EUCÀ•W‚É•ÏŠ·, Y-Z²ŠÔ‚Å90“x‰ñ“], Z²‚ğ”½“]
	vec3 drawPt[2];
	vec4 drawP4[2];
	for (int j = 0; j < 2; j++) {
		drawP4[j] = tudeToEuc(vPos[j]);
		drawPt[j] = drawP4[j].xyz;

		tudeRst(drawPt[j].y, drawPt[j].z, 0.5 * PIE, 1); //-- Y²‚ğ‰œs‚«‚É (ŒİŠ·‚Ì‚½‚ß)
		drawPt[j].z *= -1.0;
	}


	///-- Œ·‚ÌŒX‚«ŒvZ
	vec4 tmPt[2] = vec4[](drawP4[0], drawP4[1]);
	clcSlope4(tmPt[0], tmPt[1], slopeC, betaC);

	//--scã3“_‚Ì ŒX‚« & —]‚è--
	vec3 glVtx0 = vec3(drawPt[0].x / (WH_CR.z * drawPt[0].y), drawPt[0].z / (WH_CR.w * drawPt[0].y), 0);
	vec3 glVtx1 = vec3(drawPt[1].x / (WH_CR.z * drawPt[1].y), drawPt[1].z / (WH_CR.w * drawPt[1].y), 0);

	vec2 sPt[2] = vec2[](vec2(0.5 * (glVtx0.x + 1) * WH_CR.x, 0.5 * (glVtx0.y + 1) * WH_CR.y),
		vec2(0.5 * (glVtx1.x + 1) * WH_CR.x, 0.5 * (glVtx1.y + 1) * WH_CR.y));

	float slp, beta;
	slp = (sPt[1].x - sPt[0].x) / (sPt[1].y - sPt[0].y);
	beta = sPt[0].x - sPt[0].y * slp;

	s_b = vec2(slp, beta);


	//-- Fİ’è
	fCol = vColor;
}

