#version 330 core

// from vertex shader
in vec3 fCol;
in vec2 txr[3];
in vec3 ptsE[3];
in vec3 fNome, fnRadius;
in float inscR;

// uniform data
uniform vec3 locR;
uniform vec4 WH_CR;
uniform sampler2D sfTex;
uniform int texJD;
uniform float H3_MAX_RADIUS;
uniform int decMode;

// output data
out vec3 color;

// constant
float H3_REF_RADIUS = 0.99;	// cpp側とは無関係
float refBrRatio = 0.8;


// functions (declare)
float pyth3(float x, float y, float z);
float pyth4(vec4 vec);
float ClcHypbFromEuc(float dst);
float ClcHypbFromEuc(vec3 dst1, vec3 dst2);
float ClcEucFromHypb(float dst);
vec3 toPoinCoord(vec3 tmpPt);
float pyth3(vec3 pts);
float pyth3OS(vec3 pts);
void ParallelMove(vec3 tLoc, bool mode, inout vec3 mvPt[3], int len);
void ReflectionH3(vec3 dst, vec3 ctr, inout vec3 mvPt[3], int len);
vec4 ClcReflected(vec4 grdPt, vec3 trg);


// functions (define)
float pyth3(float x, float y, float z) { return sqrt(x * x + y * y + z * z); }///ok
float pyth4(vec4 vec) {
	return sqrt(vec.x * vec.x + vec.y * vec.y + vec.z * vec.z + vec.w * vec.w);
}///ok

// Euc距離から双曲距離に変換 (原点-任意点)
float ClcHypbFromEuc(float dst)
{
	return atanh(dst);
}
// Euc距離から双曲距離に変換 (任意点1-任意点2)
float ClcHypbFromEuc(vec3 dst1, vec3 dst2)
{
	// 座標変換
	dst1 = toPoinCoord(dst1);
	dst2 = toPoinCoord(dst2);

	// 距離算出
	float dst1_2L = pyth3(dst1 - dst2);
	float dst1L = pyth3(dst1);
	float dst2L = pyth3(dst2);
	return acosh(1.0 + (2 * dst1_2L * dst1_2L) / ((1.0 - dst1L * dst1L) * (1.0 - dst2L * dst2L)));

}
// 双曲距離からEuc距離に変換 (原点-任意点)
float ClcEucFromHypb(float dst)
{
	return tanh(dst);
}

// ポアンカレモデル座標に変換
vec3 toPoinCoord(vec3 tmpPt)
{
	float tmpPtW = pyth3OS(tmpPt);
	vec4 tmpP4 = vec4(tmpPt.x, tmpPt.y, tmpPt.z, 1.0 + tmpPtW);
	return (tmpP4 * (1.0 / (1.0 + tmpPtW))).xyz;
}
float pyth3(vec3 pts) { return sqrt(pts.x * pts.x + pts.y * pts.y + pts.z * pts.z); }
float pyth3OS(vec3 pts)
{
	return sqrt(1.0 - (pts.x * pts.x + pts.y * pts.y + pts.z * pts.z));
}

/// <summary>
/// H3 平行移動 鏡映2回
/// </summary>
void ParallelMove(vec3 tLoc, bool mode, inout vec3 mvPt[3], int len)
{
	float tLocPh = pyth3(tLoc);

	// 鏡映1回目の位置決定
	vec3 refVec;
	if (tLocPh < 0.001)
		refVec = vec3(0.0, 0.0, H3_REF_RADIUS);
	else if (tLocPh < H3_REF_RADIUS * refBrRatio)
		refVec = tLoc * (H3_REF_RADIUS / tLocPh);
	else
		refVec = tLoc * (H3_REF_RADIUS / tLocPh) * 0.5;

	vec3 bgnPt, endPt;
	if (mode)
	{
		bgnPt = vec3(0.0, 0.0, 0.0);
		endPt = tLoc;
	}
	else
	{
		bgnPt = tLoc;
		endPt = vec3(0.0, 0.0, 0.0);
	}

	ReflectionH3(refVec, bgnPt, mvPt, len);
	ReflectionH3(endPt, refVec, mvPt, len);
}

// 鏡映 (H3)
// dstPts: 移動方向ベクトル (原点から離れた点を指定する)
void ReflectionH3(vec3 dst, vec3 ctr, inout vec3 mvPt[3], int len)
{
	// 鏡映用球面上の点 src, dst
	vec4 ctrR = vec4(ctr.x, ctr.y, ctr.z, pyth3OS(ctr));
	vec4 dstR = vec4(dst.x, dst.y, dst.z, pyth3OS(dst));

	// locR, dstRを通りクライン球面に接する直線
	// 切片、傾き算出
	vec4 ldDif = ctrR - dstR;
	float slopeX = ldDif.x / ldDif.w;
	float slopeY = ldDif.y / ldDif.w;
	float slopeZ = ldDif.z / ldDif.w;
	float segmX = ctrR.x - ctrR.w * slopeX;
	float segmY = ctrR.y - ctrR.w * slopeY;
	float segmZ = ctrR.z - ctrR.w * slopeZ;

	// 各切片を成分とした点が接地点
	vec4 grdPt = vec4(segmX, segmY, segmZ, 0.0);


	// 鏡映結果を算出
	for (int i = 0; i < len; i++)
	{
		mvPt[i] = ClcReflected(grdPt, mvPt[i]).xyz;
	}
}

// 鏡映結果を算出
vec4 ClcReflected(vec4 grdPt, vec3 trg)
{
	// 鏡映用球面上の点 std1, std2
	vec4 trgPt = vec4(trg.x, trg.y, trg.z, pyth3OS(trg));

	// 球面原点からの垂線ベクトル (接点)
	vec4 trgToGrd = grdPt - trgPt;
	float ip = dot(normalize(trgToGrd), normalize(trgPt));
	float ttgRate = pyth4(trgPt) / pyth4(trgToGrd);
	vec4 ttgNorm = trgToGrd * ttgRate * ip;

	// 結果
	vec4 result = trgPt - ttgNorm - ttgNorm;

	return result;
};


// ---------> エントリ関数 <----------
void main()
{
	vec2 gl = vec2(gl_FragCoord.x, gl_FragCoord.y);

	// 視線方向の傾きを計算
	float gzX = ((gl.x / WH_CR.x - 0.5) * 2.0 * WH_CR.z);
	float gzY = ((gl.y / WH_CR.y - 0.5) * 2.0 * WH_CR.w);
	float gzZ = 1.0;
	float gzRate0 = pyth3(gzX, gzY, gzZ);
	vec3 gaze = vec3(gzX, gzY, gzZ) / gzRate0;

	float ip = dot(gaze, -1.0 * normalize(fNome));
	vec3 gazeK = gaze * pyth3(fNome) * abs(1.0 / ip);

	//-----> 深度の算出
	float dec = ClcHypbFromEuc(pyth3(gazeK)) / ClcHypbFromEuc(H3_MAX_RADIUS);


	//-----> Lighting
	// ポアンカレ座標に変換
	vec3 gazeP = toPoinCoord(gazeK);
	vec3 fnrNome = gazeP - fnRadius;
	float nDeg = dot(normalize(fnrNome), -normalize(gazeP));


	//-----> Texture
	vec3 ptEc[3] = vec3[](gazeK, ptsE[1], ptsE[2]);
	// 原点に移動
	ParallelMove(ptsE[0], false, ptEc, 3);

	// 原点の対辺との交点を導く
	float texL1 = ClcHypbFromEuc(pyth3(ptEc[0]));
	vec3 ptEc0N = normalize(ptEc[0]);

	// 属三角形1
	float ptEc1Len = pyth3(ptEc[1]);
	float tip1 = dot(ptEc0N, ptEc[1] * (1.0 / ptEc1Len));
	vec3 XsOs11 = ptEc0N * ptEc1Len * tip1;
	vec3 XsOs12 = ptEc[1] - XsOs11;

	// 属三角形2
	float ptEc2Len = pyth3(ptEc[2]);
	float tip2 = dot(ptEc0N, ptEc[2] * (1.0 / ptEc2Len));
	vec3 XsOs21 = ptEc0N * ptEc2Len * tip2;
	vec3 XsOs22 = ptEc[2] - XsOs21;

	//
	vec3 XsOs2111 = XsOs21 - XsOs11;
	vec3 XsOsZ = XsOs2111 * (pyth3(XsOs12) / pyth3(XsOs12 - XsOs22));
	vec3 XsOs = XsOs11 + XsOsZ;

	float texL2 = ClcHypbFromEuc(pyth3(XsOs));
	float lenRt = texL1 / texL2;	/// tx1

	// 線分割合
	float sOsRt = ClcHypbFromEuc(ptEc[1], XsOs) / ClcHypbFromEuc(ptEc[1], ptEc[2]);

	//--- 二辺間の角度算出 テクスチャサイド
	vec2 txPt1 = txr[1] - txr[0];
	vec2 txPt2 = txr[2] - txr[0];
	//
	vec2 txVec = (txPt1 + (txPt2 - txPt1) * sOsRt) * lenRt + txr[0];
	//

	vec3 oCol = fCol;
	if (texJD == 1) oCol = texture(sfTex, txVec).rgb;	//--( texture end

	gl_FragDepth = dec;

	if (decMode == 0)
		color = oCol;
	else if (decMode == 1)
		color = oCol * (1.0 - dec);
	else 
		color = oCol * nDeg;
	//color = vec3(0.0, 0.5, 1.0) * dec + oCol * nDeg * (1.0 - dec);

}