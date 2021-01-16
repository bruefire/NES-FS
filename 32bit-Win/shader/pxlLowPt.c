#version 330 core

// Ouput data
out vec3 color;

in vec2 s_b;
in vec4 slopeC, betaC;
in vec3 fCol;

uniform vec4 WH_CR;
uniform vec3 sLoc;
uniform int decMode;
uniform int bgMode;

float PIE = 3.1415926535;

//-----------------------------------
vec4 getTude(float scX, float scY, vec4 slopeC, vec4 betaC);///ok
void clcSlope4(vec4 drawP1, vec4 drawP2, out vec4 slopeC, out vec4 betaC);///ok
void tudeRst(inout float, inout float, float, int);///ok
void all_tudeRst_0(inout vec4 vect, vec3 locT);///ok
void all_tudeRst_1(inout vec4 vect, vec3 locT);///ok
vec3 eucToTude(vec4 vecT);	 ///ok
float pyth2(float x, float y);
float pyth3(float x, float y, float z);
float atan2(float x, float y);

//-----------------------------------

vec4 getTude(float scX, float scY, vec4 slopeC, vec4 betaC) {///ok
	///--åì_ÇãÅÇﬂÇÈ
	float xa = ((scX - 0.5 * WH_CR.x) / (0.5 * WH_CR.x)) * WH_CR.z;
	float ya = ((scY - 0.5 * WH_CR.y) / (0.5 * WH_CR.y)) * WH_CR.w + pow(0.1, 25.0);
	float slopeV = xa / ya;	//-- xÇ…ëŒÇ∑ÇÈyÇÃëùâ¡ó 
	float crossY = betaC.x / (slopeV - slopeC.x);

	vec4 crossPt = vec4(slopeC.x * crossY + betaC.x,
		crossY,
		slopeC.z * crossY + betaC.z,
		slopeC.w * crossY + betaC.w);

	return crossPt;
}
void clcSlope4(vec4 drawP1, vec4 drawP2, out vec4 slopeC, out vec4 betaC)///ok
{
	slopeC.x = (drawP1.x - drawP2.x) / (drawP1.y - drawP2.y);
	slopeC.z = (drawP1.z - drawP2.z) / (drawP1.y - drawP2.y);
	slopeC.w = (drawP1.w - drawP2.w) / (drawP1.y - drawP2.y);
	betaC.x = drawP1.x - (slopeC.x * drawP1.y);
	betaC.z = drawP1.z - (slopeC.z * drawP1.y);
	betaC.w = drawP1.w - (slopeC.w * drawP1.y);
}
void tudeRst(inout float vec_1, inout float vec_2, float locT, int mode)///ok
{//-- à‹,åo,ê[ÉäÉZÉbÉgâÒì]
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
{//-- à‹,åo,ê[ÉäÉZÉbÉgâÒì]
	tudeRst(vect.x, vect.y, locT.x, 0);//-- X-Y âÒì]
	tudeRst(vect.y, vect.z, locT.y, 0);//-- Y-Z âÒì]
	tudeRst(vect.z, vect.w, locT.z, 0);//-- Z-W âÒì]
}
void all_tudeRst_1(inout vec4 vect, vec3 locT)///ok
{//-- à‹,åo,ê[ÉäÉZÉbÉgâÒì]
	tudeRst(vect.z, vect.w, locT.z, 1);//-- Z-W âÒì]
	tudeRst(vect.y, vect.z, locT.y, 1);//-- Y-Z âÒì]
	tudeRst(vect.x, vect.y, locT.x, 1);//-- X-Y âÒì]
}
vec3 eucToTude(vec4 vecT) {	 ///ok
	vec3 locT;
	locT.x = atan2(vecT.x, vecT.y);
	locT.y = atan2(pyth2(vecT.x, vecT.y), vecT.z);
	locT.z = atan2(pyth3(vecT.x, vecT.y, vecT.z), vecT.w);

	return locT;
}
float pyth2(float x, float y) { return sqrt(pow(x, 2) + pow(y, 2)); }///ok
float pyth3(float x, float y, float z) { return sqrt(pow(x, 2) + pow(y, 2) + pow(z, 2)); }///ok
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





//----------------------------------------
void main()
{
	vec2 gl = vec2(gl_FragCoord.x, gl_FragCoord.y);

	float dec;
	vec4 curEc = getTude(gl.x, gl.y, slopeC, betaC);
	vec3 curTd = eucToTude(curEc);
	if (curTd.y < 0.5 * PIE) dec = (PIE - curTd.z * 0.5) / PIE; else dec = curTd.z * 0.5 / PIE;


	gl_FragDepth = 1.001 - dec;

	vec3 white = vec3(1, 1, 1);
	if (bgMode == 1) color = (white - fCol) * dec;
	else color = fCol * dec;
}

