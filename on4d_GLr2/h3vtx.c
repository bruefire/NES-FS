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



// function definition (ヘッダ)
void tudeRst(inout float vec_1, inout float vec_2, float locT, int mode);
float pyth2(float x, float y);
float pyth3(float x, float y, float z);
float atan2(float x, float y);
float ClcHypbFromEuc(float dst);
float ClcEucFromHypb(float dst);

// function definition (実装)
void tudeRst(inout float vec_1, inout float vec_2, float locT, int mode)
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

// Euc距離から双曲距離に変換
float ClcHypbFromEuc(float dst)
{
	float dstSrc = cosh(dst);
	return sqrt((dstSrc - 1.0) / (1.0 + dstSrc));
}
// 双曲距離からEuc距離に変換
float ClcEucFromHypb(float dst)
{
	float dstPh = dst * dst;
	return acosh(1.0 + ((2.0 * dstPh) / (1.0 - dstPh)));
}



// ---------> エントリ関数 <-----------
void main()
{
	//// 頂点位置反映
	//vec3 pts = vec3(
	//	0.0,
	//	scl_rad[0] * ClcHypbFromEuc(vPosition.z) / scl_rad[1],
	//	0.0
	//);
	//tudeRst(pts.z, pts.y, vPosition.y, 1);	// 角度1
	//tudeRst(pts.x, pts.z, vPosition.x, 1);	// 角度2

	//// 自転の反映
	//tudeRst(pts.x, pts.y, objRot.z, 1);	// 角度3
	//tudeRst(pts.y, pts.z, objRot.y, 1);	// 角度2
	//tudeRst(pts.x, pts.z, objRot.x, 1);	// 角度1

	//// 原点移動済stdの反映
	//tudeRst(pts.x, pts.y, objStd[2], 1);	// 方向3
	//tudeRst(pts.y, pts.z, objStd[1], 1);	// 方向2
	//tudeRst(pts.x, pts.y, objStd[0], 1);	// 方向1

	//// 元の位置に戻す
	////...

	vec3 pts = locR;

	// 右手/左手系互換
	float tmptY = pts.y;
	pts.y = pts.z;
	pts.z = tmptY;


	// 結果をピクセルシェーダへ
	gl_Position = MVP * vec4(pts, 1);
	txr = TXR;
	fCol = vColor;
}
