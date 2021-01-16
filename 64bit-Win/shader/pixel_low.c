#version 330 core


varying vec2 txr[3];
varying vec3 fCol;
varying vec4 ptsE1, ptsE2, ptsE3;

// 面が属する球体に対する法線
varying vec4 fpNorm;
varying float fn3Rt;
varying float fr;

uniform vec4 WH_CR;
uniform vec4 sLoc;
uniform sampler2D sfTex;
uniform int texJD;
uniform int decMode;
uniform int sunJD;
out vec3 color;

float PIE = 3.1415926535;

//-----------------------------------
void clcSlope4(vec4 drawP1, vec4 drawP2, out vec4 slopeC, out vec4 betaC);///ok
void tudeRst(inout float, inout float, float, int);///ok
void all_tudeRst_0(inout vec4 vect, vec3 locT);///ok
void all_tudeRst_1(inout vec4 vect, vec3 locT);///ok
vec3 eucToTude(vec4 vecT);	 ///ok
float pyth2(float x, float y);
float pyth3(float x, float y, float z);
float pyth3(vec3 vec);
float pyth4(vec4 vec);
float atan2(float x, float y);
float pow2(float val);

//-----------------------------------

vec4 getTd(float x, float y, vec4 slopeC, vec4 betaC) {///ok	簡易版
	///--交点を求める
	float slopeV = x / (y + 0.0000000000000001);
	float crossY = betaC.x / (slopeV - slopeC.x);

	vec4 crossPt = vec4(slopeC.x * crossY + betaC.x,
		crossY,
		slopeC.z * crossY + betaC.z,
		slopeC.w * crossY + betaC.w);

	float scale = pyth4(crossPt);
	crossPt = (1.0 / scale) * crossPt;

	return crossPt;
}
void clcSlope4(vec4 drawP1, vec4 drawP2, out vec4 slopeC, out vec4 betaC)///ok
{	///-- 4次元直線の傾きを計算
	///--
	
	slopeC.x = (drawP1.x-drawP2.x)/(drawP1.y-drawP2.y);
	slopeC.z = (drawP1.z-drawP2.z)/(drawP1.y-drawP2.y);
	slopeC.w = (drawP1.w-drawP2.w)/(drawP1.y-drawP2.y);
	betaC.x = drawP1.x-(slopeC.x*drawP1.y);
	betaC.z = drawP1.z-(slopeC.z*drawP1.y);
	betaC.w = drawP1.w-(slopeC.w*drawP1.y);
}
void tudeRst(inout float vec_1, inout float vec_2, float locT, int mode)///ok
{//-- 緯,経,深リセット回転
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
float pyth2(float x, float y){ return sqrt(pow2(x)+pow2(y)); }///ok
float pyth3(float x, float y, float z)
{ 
	return sqrt(x*x + y*y + z*z); 
}///ok
float pyth3(vec3 vec)
{ 
	return sqrt(vec.x*vec.x + vec.y*vec.y + vec.z*vec.z); 
}///ok
float pyth3ae(vec3 vec)
{
	return sqrt( 1.0 - (vec.x*vec.x + vec.y*vec.y + vec.z*vec.z) );
}
float pyth4(vec4 vec){
	 return sqrt( vec.x*vec.x +vec.y*vec.y +vec.z*vec.z +vec.w*vec.w ); 
}///ok
float atan2(float x, float y){///ok
	float deg = atan(x / y); deg += PIE*float(y<0.0);
	if(x==0.0){
		deg = 0.0; deg += PIE*float(y<0.0);
	}
	if(y==0.0){
		deg = 0.5*PIE; deg += PIE*float(x<0.0);
	}
	return deg;
}
float pow2(float val){
	return val * val;
}
vec3 eucToTude(vec4 vecT){	 ///ok
	vec3 locT;
	locT.x = atan2(vecT.x, vecT.y);								
	locT.y = atan2(pyth2(vecT.x,vecT.y), vecT.z);	
	locT.z = atan2(pyth3(vecT.x,vecT.y,vecT.z), vecT.w);

	return locT;
}

bool isNan( float val )
{
  return ( val < 0.0 || 0.0 < val || val == 0.0 ) ? false : true;
  // important: some nVidias failed to cope with version below.
  /*return ( val <= 0.0 || 0.0 <= val ) ? false : true;*/
}




//----------------------------------------
void main()
{
	vec2 gl = vec2(gl_FragCoord.x, gl_FragCoord.y);



	// 面属球法線から深度を導く//

	// 視線方向の傾きを計算
	float gzX = ((gl.x/WH_CR.x - 0.5)*2.0*WH_CR.z);
	float gzY = ((gl.y/WH_CR.y - 0.5)*2.0*WH_CR.w);
	float gzZ = 1.0;
	float gzRate0 = pyth3(gzX, gzY, gzZ);
	vec3 gaze = vec3(gzX, gzY, gzZ) / gzRate0;	// 再考不要


	// 軸方向に対する内積(xyz)
	vec3 fpNorm3 = fpNorm.xyz*fn3Rt;
	float gzRate = dot(fpNorm3, gaze);

	vec3 gzXtnd = gaze + fpNorm3*gzRate*(1.0/fpNorm.w-1.0);	// 引き延ばし
	float gzXtScale = pyth3(gzXtnd);//
	vec3 xt = gzXtnd*(1.0/gzXtScale);
	// 元に縮める
	xt += fpNorm3 * gzRate * (1.0/(fpNorm.w*gzXtScale)) * (fpNorm.w-1.0);
	// wの符号算出
	float xSign = -1.0*(float(0.0<fpNorm.w)*2.0-1.0)*(float(0.0<gzRate)*2.0-1.0);
	vec4 curEc = vec4(xt, xSign * pyth3ae(xt)) * fr;
	
	// 面属球法線との内積が0であれば問題なし
	/*float ckHf = pyth4((fpNorm-curEc)*0.5);
	if(0.5<abs(1.0 - 2.0 * ckHf * ckHf)) curEc = vec4(gaze, 0)*fr;*/
	if(isNan(curEc.w)) curEc = vec4(gaze, 0)*fr;
	//-------
	float dec;
	vec3 curTd = eucToTude( curEc );
	
	if(curTd.y<0.5*PIE) dec = (PIE-curTd.z*0.5) /PIE; else dec = curTd.z*0.5 /PIE;
	///--
	vec3 oCol = fCol;



	///------ Lighting ------///

	// ベクトル積から法線算出
	float Lu1L = pyth3(curEc.xyz);
	vec3 Lu1 = (1.0 / Lu1L) * curEc.xyz;
	float Lu2L = curEc.w;
	float Lu2 = 1.0;
	vec4 lVec = vec4(Lu2L * Lu1, -1 * Lu1L * Lu2);

	// 法線との角度
	float nDeg = abs(dot(lVec, fpNorm));

	//...
	//-- 輝度を算出
	float bright_ = 0.2 * nDeg / (curEc.x * curEc.x + curEc.y * curEc.y + curEc.z * curEc.z);// 旧
	float bright;
	if (curTd.y < 0.5 * PIE) bright = 1.2 / (curTd.z * curTd.z); else bright = 1.2 / ((2 * PIE - curTd.z) * (2 * PIE - curTd.z));
	if (2.0 < bright) bright = 2.0;	//最大輝度
	bright = bright * nDeg;

	///-- Lighting_(end ▲
	///--- texture ---///
	vec4 ptEc[3] = vec4[](curEc, ptsE2, ptsE3);

	// 新 //
	vec4 txAxsW = vec4(0, 0, 0, 1);
	vec4 txAxs3 = vec4(normalize(ptsE1.xyz), 0);

	float pXip = ptsE1.w;	// 内積
	float pYip = dot(txAxs3, ptsE1);

	for (int i = 0; i < 3; i++)
	{
		float p0ipW = dot(txAxsW, ptEc[i]);
		float p0ip3 = dot(txAxs3, ptEc[i]);

		vec4 txLc1 = p0ipW * txAxsW + p0ip3 * txAxs3;
		vec4 txLc2 = -1 * p0ip3 * txAxsW + p0ipW * txAxs3;

		ptEc[i] += -txLc1 + (txLc1 * pXip) + (-txLc2 * pYip);
	}
	vec3 txCrs = normalize(cross(ptEc[1].xyz, ptEc[2].xyz));

	vec3 txAxsZ = vec3(0, 0, 1);
	vec3 txAxs2 = vec3(normalize(txCrs).xy, 0);

	float pXiq = txCrs.z;
	float pYiq = dot(txAxs2, txCrs);

	for (int i = 0; i < 3; i++)
	{
		float p0ipZ = dot(txAxsZ, ptEc[i].xyz);
		float p0ip2 = dot(txAxs2, ptEc[i].xyz);

		vec3 txLc1 = p0ipZ * txAxsZ + p0ip2 * txAxs2;
		vec3 txLc2 = -1 * p0ip2 * txAxsZ + p0ipZ * txAxs2;

		ptEc[i].xyz += -txLc1 + (txLc1 * pXiq) + (-txLc2 * pYiq);
	}
	// 新end


	//-- 交点を導く
	float texL1 = eucToTude(ptEc[0]).z;
	vec2 ce2 = vec2(ptEc[0].x, ptEc[0].y);
	vec4 slopeE, betaE;
	clcSlope4(ptEc[1], ptEc[2], slopeE, betaE);
	vec4 XsOs = getTd(ce2.x, ce2.y, slopeE, betaE); //-- 交点
	float texL2 = eucToTude(XsOs).z;
	float lenRt = texL1 / texL2;	/// tx1
	//-- 線分割合
	float sOsRt = asin(pyth4(XsOs - ptEc[1]) * 0.5) / asin(pyth4(ptEc[2] - ptEc[1]) * 0.5);


	//--- 二辺間の角度算出 テクスチャサイド
	vec2 txPt1 = txr[1] - txr[0];
	vec2 txPt2 = txr[2] - txr[0];
	//
	vec2 txVec = (txPt1 + (txPt2 - txPt1) * sOsRt) * lenRt + txr[0];
	//


	if (texJD == 1) oCol = texture(sfTex, txVec).rgb;	//--( texture end


	///------ Lighting 2nd------///

	// 面に交わる光のベクトルを算出
	float csIp = dot(curEc, sLoc);
	vec4 d3c = curEc - csIp * sLoc;

	vec4 Lu3 = sLoc;
	float Lu3L = (float(0 < csIp) * 2 - 1) * pyth4(sLoc * csIp);
	float Lu4L = pyth4(d3c);
	vec4 Lu4 = d3c / Lu4L;

	vec4 lVec2 = vec4(Lu4L * Lu3 + -1 * Lu3L * Lu4);

	// 法線との角度
	nDeg = dot(lVec2, fpNorm);
	if (nDeg < 0) nDeg = 0;


	///-- Lighting 2nd_(end ▲
	if (decMode == 3) {	//太陽なら
		//-- 適当な霧風味の特殊効果
		color = oCol * (dec - 0.33) * 1.5 + vec3(0.2, 0.8, 1) * (1 - (dec - 0.33) * 1.5);
		if (dec - 0.33 < 0) color = vec3(0.2, 0.8, 1);

	}
	else if (sunJD == 1) {
		//-- 輝度を算出
		bright = 2 * nDeg / (d3c.x * d3c.x + d3c.y * d3c.y + d3c.z * d3c.z + d3c.w * d3c.w) + 0.0;
		oCol = oCol * bright;
		//-- 適当な霧風味の特殊効果
		color = oCol * (dec - 0.33) * 1.5 + vec3(0.2, 0.8, 1) * (1 - (dec - 0.33) * 1.5);
		if (dec - 0.33 < 0) color = vec3(0.2, 0.8, 1);

	}
	else if (decMode == 0)
		color = oCol;
	else if (decMode == 1)
		color = oCol * dec;
	else if (decMode == 2)
		color = oCol * bright;
	else
		color = oCol * bright_;
	//if(isnan(curEc.w)) color = vec3(0,1,0);

	gl_FragDepth = 1.001 - dec;
}

