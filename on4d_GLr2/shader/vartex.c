#version 330 core

// Input vertex data, different for all executions of this shader.
layout(location = 0) in vec3 vPosition;
layout(location = 1) in vec3 vColor;
layout(location = 2) in vec2 TXR;

uniform vec2 scl_rad;
uniform vec3 objRot;
uniform vec3 objStd;
uniform vec3 locR;

out vec3 vPos;
out vec2 txrG;
out vec3 fColor;
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

void tudeRst(inout float vec_1, inout float vec_2, float locT, int mode)///ok
{//-- à‹,åo,ê[ÉäÉZÉbÉgâÒì]

	vec2 axs1 = vec2(vec_1, vec_2);
	vec2 axs2 = vec2(vec_2, -vec_1);
	float asign = 1.0;
	if (mode == 0) asign = -1.0;

	vec2 result = vec2(0.0, 0.0)
		+ (axs1 * cos(asign * locT))
		+ (axs2 * sin(asign * locT));

	vec_1 = result.x;
	vec_2 = result.y;
}
void all_tudeRst_1(inout vec4 vect, vec3 locT)///ok
{//-- à‹,åo,ê[ÉäÉZÉbÉgâÒì]
		tudeRst(vect.z, vect.w, locT.z, 1);//-- Z-W âÒì]
		tudeRst(vect.y, vect.z, locT.y, 1);//-- Y-Z âÒì]
		tudeRst(vect.x, vect.y, locT.x, 1);//-- X-Y âÒì]
}
vec3 eucToTude(vec4 vecT){	 ///ok
	vec3 locT;
	locT.x = atan2(vecT.x, vecT.y);								
	locT.y = atan2(pyth2(vecT.x,vecT.y), vecT.z);	
	locT.z = atan2(pyth3(vecT.x,vecT.y,vecT.z), vecT.w);

	return locT;
}
float pyth2(float x, float y){ return sqrt(x*x + y*y); }///ok
float pyth3(float x, float y, float z){ return sqrt(x*x + y*y + z*z); }///ok
float atan2(float x, float y){///ok
	float deg = atan(x / y); deg += PIE*float(y<0);
	if(x==0){
		deg = 0.0; deg += PIE*float(y<0);
	}
	if(y==0){
		deg = 0.5*PIE; deg += PIE*float(x<0);
	}
	return deg;
}



void main()
{

	vec4 pts1 = vec4(	// énì_ XYZW
		0,
		sin(scl_rad[0] * vPosition.z / scl_rad[1]),
		0,
		cos(scl_rad[0] * vPosition.z / scl_rad[1])
	);
	tudeRst(pts1.z, pts1.y, vPosition.y, 1);	// äpìx1
	tudeRst(pts1.x, pts1.z, vPosition.x, 1);	// äpìx2

	///-- é©ì]ÇÃîΩâf
	tudeRst(pts1.x, pts1.y, objRot.z, 1);	// äpìx3
	tudeRst(pts1.y, pts1.z, objRot.y, 1);	// äpìx2
	tudeRst(pts1.x, pts1.z, objRot.x, 1);	// äpìx1

	///-- äÓèÄ(âÒì])Ç…çáÇÌÇπÇÈ
	tudeRst(pts1.x, pts1.y, objStd[2], 1);	// ï˚å¸3
	tudeRst(pts1.y, pts1.z, objStd[1], 1);	// ï˚å¸2
	tudeRst(pts1.x, pts1.y, objStd[0], 1);	// ï˚å¸1
	///-- ÉIÉuÉWÉFÉNÉgç¿ïWÇ…çáÇÌÇπÇÈ
	all_tudeRst_1(pts1, locR);	// ï˚å¸3
	//---- ç¿ïWïœä∑ÇµÇƒäiî[ (end ----
    vPos = eucToTude(pts1); 
	gl_Position = vec4( vPos, 1 );

	fColor = vColor;
    txrG = TXR;

}

