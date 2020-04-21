#version 330 core

in vec3 vPos[3];
in vec3 fColor[3];
in vec2 txrG[3];

uniform mat4 MVP;
uniform vec4 WH_CR;

out vec2 txr[3];
out vec3 fCol;
out vec4 ptsE1, ptsE2, ptsE3;

// new
out vec4 fpNorm;	// 法線
out float fn3Rt;
out float fr;


float PIE = 3.1415926535;


//-----------------------------------
void tudeRst(inout float, inout float, float, int);///ok
void all_tudeRst_0(inout vec4 vect, vec3 locT);///ok
void all_tudeRst_1(inout vec4 vect, vec3 locT);///ok
vec4 tudeToEuc(vec3 locT);
float pyth2(float x, float y);
float pyth3(float x, float y, float z);
float atan2(float x, float y);

//-----------------------------------


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

vec4 tudeToEuc(vec3 locT){	// [緯,経,深]座標を[XYZ]W座標に変換
	float sin_locTz = sin(locT.z);
	float sin_locTy = sin(locT.y);

	vec4 vecT = vec4( 
				 sin_locTz * sin_locTy * sin(locT.x),//X
				 sin_locTz * sin_locTy * cos(locT.x),//Y
				 sin_locTz * cos(locT.y),		//Z
				 cos(locT.z)	//W
	);
	return vecT;
}
float pyth2(float x, float y){ return sqrt(pow(x,2)+pow(y,2)); }///ok
float pyth3(float x, float y, float z){ return sqrt(pow(x,2)+pow(y,2)+pow(z,2)); }///ok
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
float pyth4(vec4 vec){
	 return sqrt( vec.x*vec.x +vec.y*vec.y +vec.z*vec.z +vec.w*vec.w ); 
}///ok
//--------------


void main(){
	//--まず色設定
	fCol = fColor[0];
	vec4 a = ptsE1 = tudeToEuc( vPos[0] );
	vec4 b = ptsE2 = tudeToEuc( vPos[1] );
	vec4 c = ptsE3 = tudeToEuc( vPos[2] );
	//-- テスクチャ
	txr[0] = txrG[0];
	txr[1] = txrG[1];
	txr[2] = txrG[2];
	//--

	///-- 点情報コピー, EUC座標に変換, Y-Z軸間で90度回転, Z軸を反転
	vec3 drawPt[3], drawPt_td[3];
	vec4 drawP4[3];
	for(int j=0;j<3;j++){	
		drawPt_td[j] = vPos[j];
		drawP4[j] = tudeToEuc(drawPt_td[j]);
		drawPt[j] = drawP4[j].xyz;
			
		tudeRst(drawPt[j].y, drawPt[j].z, 0.5*PIE, 1); //-- Y軸を奥行きに (互換のため)
		drawPt[j].z *= -1;
	}
	////---計算のために値yを調整 > さらにカメラ前面の線分を切り出す ---///
	//bool nRev = adjY_pls(drawPt);
	
	/// ベクトル積(垂直)
	vec4 prod = vec4
	(
		-a[1]*b[2]*c[3] -a[2]*b[3]*c[1] -a[3]*b[1]*c[2] 
		+a[1]*b[3]*c[2] +a[2]*b[1]*c[3] +a[3]*b[2]*c[1],

		+a[0]*b[2]*c[3] +a[2]*b[3]*c[0] +a[3]*b[0]*c[2]
		-a[0]*b[3]*c[2] -a[2]*b[0]*c[3] -a[3]*b[2]*c[0],

		-a[0]*b[1]*c[3] -a[1]*b[3]*c[0] -a[3]*b[0]*c[1]
		+a[0]*b[3]*c[1] +a[1]*b[0]*c[3] +a[3]*b[1]*c[0],

		+a[0]*b[1]*c[2] +a[1]*b[2]*c[0] +a[2]*b[0]*c[1]
		-a[0]*b[2]*c[1] -a[1]*b[0]*c[2] -a[2]*b[1]*c[0]
	);
	float prodRate = pyth4(prod);
	fpNorm = prod*(1.0/prodRate);
	fn3Rt = 1.0/pyth3(fpNorm.x, fpNorm.y, fpNorm.z);

	
	
	fr = 1.0;
	for(int i=0;i<3;i++){
		gl_Position = MVP * vec4(drawPt[i], 1);
		EmitVertex();
	}
    EndPrimitive();

	fr = -1.0;
	//-- 半周以上
	if(drawPt[0].y<0 || drawPt[1].y<0 || drawPt[2].y<0){
		for(int i=0;i<3;i++){
			drawPt[i] *= -1;
			gl_Position = MVP * vec4(drawPt[i], 1);
			EmitVertex();
		}
		EndPrimitive();

	}


}

