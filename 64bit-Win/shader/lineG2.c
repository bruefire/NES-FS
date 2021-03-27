#version 330 core

in vec3 vPos[2];
in vec3 fColor[];
uniform mat4 MVP;
uniform vec4 WH_CR;
uniform vec4 cRange;
out vec4 slopeC, betaC;
out vec2 s_b;
out vec3 fCol;


float PIE = 3.1415926535;


//-----------------------------------
vec4 getTude(float scX, float scY, vec4 slopeC, vec4 betaC);///ok
void clcSlope4(vec4 drawP1, vec4 drawP2, out vec4 slopeC, out vec4 betaC);///ok
void tudeRst(inout float, inout float, float, int);///ok
void all_tudeRst_0(inout vec4 vect, vec3 locT);///ok
void all_tudeRst_1(inout vec4 vect, vec3 locT);///ok
vec3 eucToTude(vec4 vecT);	 ///ok
vec4 tudeToEuc(vec3 locT);
float pyth2(float x, float y);
float pyth3(float x, float y, float z);
float atan2(float x, float y);

//-----------------------------------

vec4 getTude(float scX, float scY, vec4 slopeC, vec4 betaC){///ok
	///--Œğ“_‚ğ‹‚ß‚é
	float cRangeLR = cRange[0] + cRange[1];
	float cRangeTB = cRange[2] + cRange[3];
	float xa = scX / WH_CR.x * cRangeLR - cRange[0];
	float ya = scY / WH_CR.y * cRangeTB - cRange[3] + pow(0.1, 25.0);
	float slopeV = xa/ya;	//-- x‚É‘Î‚·‚éy‚Ì‘‰Á—Ê
	float crossY = betaC.x/(slopeV-slopeC.x);

	vec4 crossPt = vec4(	slopeC.x * crossY + betaC.x,
						crossY,
						slopeC.z * crossY + betaC.z,
						slopeC.w * crossY + betaC.w);

	return crossPt;
}
void clcSlope4(vec4 drawP1, vec4 drawP2, out vec4 slopeC, out vec4 betaC)///ok
{	///-- 4ŸŒ³’¼ü‚ÌŒX‚«‚ğŒvZ
	vec3 drwTd_1 = eucToTude(drawP1);
	///-- ŒvZ‚ğˆÀˆÕ‚É‚·‚é•â³
	vec4 tmpt2 = drawP2;
	all_tudeRst_0(tmpt2, drwTd_1);
	///-- is•ûŒü1,2‚Ì“Á’è
	float rotEn[2];
	rotEn[0] = atan2(tmpt2.x, tmpt2.y);
	rotEn[1] = atan2(pyth2(tmpt2.x, tmpt2.y), tmpt2.z);
	///--

	slopeC.x = (drawP1.x-drawP2.x)/(drawP1.y-drawP2.y);
	slopeC.z = (drawP1.z-drawP2.z)/(drawP1.y-drawP2.y);
	slopeC.w = (drawP1.w-drawP2.w)/(drawP1.y-drawP2.y);
	betaC.x = drawP1.x-(slopeC.x*drawP1.y);
	betaC.z = drawP1.z-(slopeC.z*drawP1.y);
	betaC.w = drawP1.w-(slopeC.w*drawP1.y);
}
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
	//--‚Ü‚¸Fİ’è
	fCol = fColor[0];

	///-- “_î•ñƒRƒs[, EUCÀ•W‚É•ÏŠ·, Y-Z²ŠÔ‚Å90“x‰ñ“], Z²‚ğ”½“]
	vec3 drawPt[2], drawPt_td[2];
	vec4 drawP4[2];
	for(int j=0;j<2;j++){	
		drawPt_td[j] = vPos[j];
		drawP4[j] = tudeToEuc(drawPt_td[j]);
		drawPt[j] = drawP4[j].xyz;
			
		tudeRst(drawPt[j].y, drawPt[j].z, 0.5*PIE, 1); //-- Y²‚ğ‰œs‚«‚É (ŒİŠ·‚Ì‚½‚ß)
		drawPt[j].z *= -1;
	}


	///-- Œ·‚ÌŒX‚«ŒvZ
	vec4 tmPt[2] =vec4[](  drawP4[0], drawP4[1] );
	clcSlope4(tmPt[0], tmPt[1], slopeC, betaC);


	//--scã3“_‚Ì ŒX‚« & —]‚è--
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
	slp = (sPt[1].x-sPt[0].x) / (sPt[1].y-sPt[0].y);
	beta = sPt[0].x - sPt[0].y*slp;

	s_b = vec2( slp, beta );
	//--
	
	for(int i=0;i<2;i++){
		gl_Position = MVP * vec4(drawPt[i], 1);
		EmitVertex();
	}
    EndPrimitive();

	//-- ”¼üˆÈã
	if(drawPt[0].y<0 || drawPt[1].y<0){
		for(int i=0;i<2;i++){
			drawPt[i] *= -1;
			gl_Position = MVP * vec4(drawPt[i], 1);
			EmitVertex();
		}
		EndPrimitive();

	}


}

