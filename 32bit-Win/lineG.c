#version 330 core

in vec3 vPos[];
in vec3 fColor[];
uniform vec4 WH_CR;
out vec3 fCol;
out float Dec;

float PIE = 3.1415926535;


//-----------------------------------
void tudeRst(inout float, inout float, float, int);///ok
void all_tudeRst_0(inout vec4 vect, vec3 locT);///ok
void all_tudeRst_1(inout vec4 vect, vec3 locT);///ok
vec3 eucToTude(vec4 vecT);	 ///ok
vec4 tudeToEuc(vec3 locT);
float pyth2(float x, float y);
float pyth3(float x, float y, float z);
float atan2(float x, float y);

//-----------------------------------

void tudeRst(inout float vec_1, inout float vec_2, float locT, int mode)///ok
{//-- ˆÜ,Œo,[ƒŠƒZƒbƒg‰ñ“]
	float tRot = atan2(vec_1, vec_2);
	float R = pyth2(vec_1, vec_2);
	if(0==mode){
		vec_1 = R * sin(tRot - locT);
		vec_2 = R * cos(tRot - locT);
	}else{
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
vec3 eucToTude(vec4 vecT){	 ///ok
	vec3 locT;
	locT.x = atan2(vecT.x, vecT.y);								
	locT.y = atan2(pyth2(vecT.x,vecT.y), vecT.z);	
	locT.z = atan2(pyth3(vecT.x,vecT.y,vecT.z), vecT.w);

	return locT;
}
vec4 tudeToEuc(vec3 locT){	// [ˆÜ,Œo,[]À•W‚ğ[XYZ]WÀ•W‚É•ÏŠ·
	vec4 vecT = vec4( 
				 sin(locT.z) * sin(locT.y) * sin(locT.x),//X
				 sin(locT.z) * sin(locT.y) * cos(locT.x),//Y
				 sin(locT.z) * cos(locT.y),		//Z
				 cos(locT.z)	//W
	);
	return vecT;
}
float pyth2(float x, float y){ return sqrt(pow(x,2)+pow(y,2)); }///ok
float pyth3(float x, float y, float z){ return sqrt(pow(x,2)+pow(y,2)+pow(z,2)); }///ok
float atan2(float x, float y){///ok
	float deg = atan(x / y); deg += PIE*float(y<0);
	return deg;
}
//--------------


void main(){

	
		vec3 curPt = vPos[0];
		float length = tan(curPt.y);
		float scX = length * sin(curPt.x);
		float scY = length * cos(curPt.x);
		scX = scX/WH_CR.z;
		scY = scY/WH_CR.w;

		float dec;
		if(curPt.y<0.5*PIE) dec = (PIE-curPt.z*0.5) /PIE; else dec = curPt.z*0.5 /PIE;
		Dec = dec;
	
		gl_Position = vec4(scX, scY, 0, 1);
		fCol = fColor[0];//-- Fİ’è
		
		EmitVertex();
		EndPrimitive();
}

