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

// to pixel shader
out vec3 vPos;
out vec2 txr;
out vec3 fCol;
float PIE = 3.1415926535;



// function definition (wb_)
void tudeRst(inout float vec_1, inout float vec_2, float locT, int mode);
float pyth2(float x, float y);
float pyth3(float x, float y, float z);
float atan2(float x, float y);
float ClcHypbFromEuc(float dst);
float ClcEucFromHypb(float dst);

// function definition (À)
void tudeRst(inout float vec_1, inout float vec_2, float locT, int mode)
{//-- Ü,o,[Zbgñ]
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

// Euc£©çoÈ£ÉÏ·
float ClcHypbFromEuc(float dst)
{
	float dstSrc = cosh(dst);
	return sqrt((dstSrc - 1.0) / (1.0 + dstSrc));
}
// oÈ£©çEuc£ÉÏ·
float ClcEucFromHypb(float dst)
{
	float dstPh = dst * dst;
	return acosh(1.0 + ((2.0 * dstPh) / (1.0 - dstPh)));
}



// ---------> GgÖ <-----------
void main()
{
	//// ¸_Êu½f
	//vec3 pts = vec3(
	//	0.0,
	//	scl_rad[0] * ClcHypbFromEuc(vPosition.z) / scl_rad[1],
	//	0.0
	//);
	//tudeRst(pts.z, pts.y, vPosition.y, 1);	// px1
	//tudeRst(pts.x, pts.z, vPosition.x, 1);	// px2

	//// ©]Ì½f
	//tudeRst(pts.x, pts.y, objRot.z, 1);	// px3
	//tudeRst(pts.y, pts.z, objRot.y, 1);	// px2
	//tudeRst(pts.x, pts.z, objRot.x, 1);	// px1

	//// ´_Ú®ÏstdÌ½f
	//tudeRst(pts.x, pts.y, objStd[2], 1);	// ûü3
	//tudeRst(pts.y, pts.z, objStd[1], 1);	// ûü2
	//tudeRst(pts.x, pts.y, objStd[0], 1);	// ûü1

	//// ³ÌÊuÉß·
	////...

	vec3 pts = locR;

	// Eè/¶ènÝ·
	float tmptY = pts.y;
	pts.y = pts.z;
	pts.z = tmptY;


	// ÊðsNZVF[_Ö
	gl_Position = MVP * vec4(pts, 1);
	txr = TXR;
	fCol = vColor;
}
