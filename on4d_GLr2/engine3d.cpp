#include <stdint.h>
#include <Complex>
#include <algorithm>
#include <iostream>
#include <time.h>
#include <String>
#include <chrono>
#include <thread>
#include "constants.h"
#include "engine3d.h"
#include "S3ErrorManager.h"
#include "functions.h"

using namespace std;
using namespace std::chrono;


engine3d::engine3d()
	: BWH_QTY(1)	// 軸
	, PLR_QTY(10)	// プレイヤー
	, ENR_QTY(550)	// 隕石()
	, OBJ_QTY(BWH_QTY + PLR_QTY + ENR_QTY)
	, EFE_QTY(1)	// エフェクト(飾り/相互関係なし)
	, ATK_QTY(1)	// ﾌﾟﾚｲﾔと敵の攻撃
	, ALL_QTY(OBJ_QTY + EFE_QTY + ATK_QTY)

	, obMove(false)	//todo★ 暫定
	, radius(30)
	, radius_min(20)
	, SPEED_MAX(0.2)
	, GRAVITY(0)
	, loop(0)
	, clsType(CLS_TYPE::LOGIC)
	, disposeFlg(false)
	, STD_PMSEC(1000 / 30)
	, MAX_PMSEC(1000 / 60)
	, MIN_PMSEC(1000 / 25)
	, stdRefSpan(600)
	, mapDir(MapDirection::FRONT)
	, mapMode(MapMode::SINGLE)
	, useJoyPad(false)

	// 1.0 radian
	, SIN_1(0.8414709848)
	, COS_1(0.54030230586)
	, SIN_1R(1.0 / SIN_1)
	, sun(this)
	, markObj(this)
	, vrMenuObj(this)
	, worldGeo(WorldGeo::HYPERBOLIC)
	, H3_STD_LEN(0.1)
	, selectedIdx(-1)
	//, H3_MAX_RADIUS(0.995) // 双曲長で 約6.0
	//, H3_REF_RADIUS(0.999) // 双曲長で 約7.7
	, H3_MAX_RADIUS(0.999995) // 双曲長で約12.9	//=0.995 約6.0
	, H3_REF_RADIUS(0.999999) // 双曲長で約??.?	//=0.999 約7.7
	, h3objLoop(true)
	, viewTrackIdx(-1)
	, vrFlag(false)
{
	adjTime[0] = adjTime[1] = 0;
	
	vrHand[0].owner = this;
	vrHand[1].owner = this;

	// 瞬間移動
	mvObjFlg = false;
	mvObjParam.loc = pt3(0, 0, 0);
	mvObjParam.rot = pt3(0, 0, 0);

}
engine3d::~engine3d() {
}


//**** 基本3メソッド ****//

// 初期化
int engine3d::init()
{
	int rtnVal = 1;

	//-- プレイヤー空き番号を設定
	PLR_No = BWH_QTY;	

	// メッシュ定義
	meshLen = 23;
	meshNames = new char*[meshLen];
	meshNames[0] = "wLines", 
	meshNames[1] = "player", 
	meshNames[2] = "earth", 
	meshNames[3] = "wLines3",
	meshNames[4] = "cube8", 
	meshNames[5] = "horse", 
	meshNames[6] = "sun", 
	//meshNames[7] = "plane",
	meshNames[7] = "sphereM",
	meshNames[8] = "earth2", 
	meshNames[9] = "cube8", 
	meshNames[10] = "p120", 
	meshNames[11] = "p120c", 
	meshNames[12] = "torus", 
	meshNames[13] = "torus2", 
	meshNames[14] = "rock";
	meshNames[15] = "map2";
	meshNames[16] = "map1";
	meshNames[17] = "mapElm1";
	meshNames[18] = "mapElm2";
	meshNames[19] = "mapElm3";
	meshNames[20] = "mapElm4";
	meshNames[21] = "plane";
	meshNames[22] = "hand";

	///-- 雛形メッシュ
	allocMesh();
	try
	{
		for(int i=0;i<meshLen;i++)//-- メッシュ作成
		{	
			if(!meshs[i].meshInit( meshNames[i], i+1, 0))
				rtnVal = 0;
		}
	}
	// disposeして終了に向かう
	catch(S3ErrorManager ex)
	{
		dispose();

		if(clsType == CLS_TYPE::LOGIC)
		{
			cout << ex.errMsg << endl;
			return 0;
		}
		else
		{
			throw  ex;
		}
	}

	///-- 初期オブジェクト作成
	objs = new object3d[ OBJ_QTY ];
	for (int i = 0; i < OBJ_QTY; i++) 
	{
		objs[i].owner = this;
	}
	objCnt = OBJ_QTY;


	//-- フレームレート
	fps = STD_PMSEC;
	adjSpd = 1.0;

	// 軌跡用バッファ確保
	markMesh.meshInitB((OBJ_QTY) * (object3d::PAST_QTY - 1), (meshLen - 1) + 2);

	srand(time(NULL));
	InitWorld();

	// S3用マップ
	mapMeshLen = 2;
	mapMesh[0] = meshs + 15;
	mapMesh[1] = meshs + 16;

	// menu
	menuLgc.Init();


	return rtnVal;
}


// フレーム単位の更新
int engine3d::update()
{

	////-----------------time
	// 計測終了時刻を保存
	long long newTime = duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();
	adjTime[1] = (newTime < adjTime[0]) ? 0 : newTime - adjTime[0];

	if (adjTime[1] < fps)
		this_thread::sleep_for(milliseconds(fps - adjTime[1]));
	
	adjTime[0] = duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();

	////-----------------time

	loop = (loop+1) % INT_MAX;//std::numeric_limits<int>::max(); <- いずれこっちに変更

	// World Geometry
	if (worldGeo == WorldGeo::SPHERICAL)
		UpdateS3();
	else if (worldGeo == WorldGeo::HYPERBOLIC)
		UpdateH3();

	// 入力状態 次回待機処理
	PrepareInParamForNext();

	return 1;
};

void engine3d::PrepareInParamForNext()
{
	if (!useJoyPad)
		ope.ClearUnkeepedParam();

	ope.chgMapStateOld = ope.chgMapState;
	//----

	ope.inputByKey = false;
}

/// <summary>
/// S3世界更新
/// </summary>
void engine3d::UpdateS3()
{
	menuLgc.InputProc(ope.menuAction);

	// S3演算処理
	if (GRAVITY && obMove) physics();
	simulateS3();
}

/// <summary>
/// H3世界更新
/// </summary>
void engine3d::UpdateH3()
{
	// H3演算処理
	simulateH3();
}


// 後処理
int engine3d::dispose()
{
	if(disposeFlg)
		return 0;

	// 解放
	menuLgc.Dispose();
	delete[] objs;
	delete[] meshs;
	delete[] meshNames;

	disposeFlg = true;

	return 1;
}



//**** その他 ****//

int engine3d::allocMesh()
{
	meshs = new mesh3d[meshLen];

	return 1;
}


///=============== >>>ループ(双曲空間)<<< ==================
void engine3d::simulateH3()
{
	//射撃オブジェクト更新
	UpdFloatObjsH3();

	// プレイヤーオブジェクト更新
	UpdPlayerObjsH3(nullptr);

	// 相対位置計算
	ClcRelaivePosH3(nullptr);

	// 基objとの相対位置計算 (表示座標)
	ClcCoordinate();
}

///=============== >>>ループ(球面空間)<<< ==================
int engine3d::simulateS3()
{

	///==============

	// 射撃オブジェクト更新
	UpdFloatObjsS3();

	/// パラメータ指定(byダイアログ等)プレイヤー姿勢変更
	setObjPos();

	// プレイヤーオブジェクト更新
	double cmrStd[3] = {};
	UpdPlayerObjsS3(cmrStd);


	///===============^^^^^^^^^~~~~~~~~~~-----------
		//-- 軌跡形成 --//
	for (int h = 0; h < OBJ_QTY; h++) {
		object3d* curObj = objs + h;

		for (int i = 0; i < object3d::PAST_QTY - 1; i++) {
			int idx = h * (object3d::PAST_QTY - 1) * 16 + i * 16;

			markMesh.pts2[idx + 0] = curObj->past[i].x;
			markMesh.pts2[idx + 1] = curObj->past[i].y;
			markMesh.pts2[idx + 2] = curObj->past[i].z;
			markMesh.pts2[idx + 3] = 1.0;
			markMesh.pts2[idx + 4] = 0;
			markMesh.pts2[idx + 5] = (double)i / object3d::PAST_QTY;

			markMesh.pts2[idx + 8] = curObj->past[i + 1].x;
			markMesh.pts2[idx + 9] = curObj->past[i + 1].y;
			markMesh.pts2[idx + 10] = curObj->past[i + 1].z;
			markMesh.pts2[idx + 11] = 1.0;
			markMesh.pts2[idx + 12] = 0;
			markMesh.pts2[idx + 13] = (double)i / object3d::PAST_QTY;

			markMesh.pts2[idx + 6] = markMesh.pts2[idx + 7] =
				markMesh.pts2[idx + 14] = markMesh.pts2[idx + 15] = 0;

		}
	}

	// 相対位置計算
	ClcRelaivePosS3(cmrStd);
	//===============^^^^^^^^^~~~~~~~~~~-----------

	return 0;
}


// 射撃オブジェクト更新 S3
void engine3d::UpdFloatObjsS3()
{
	for (int h = -1; h < objCnt; h++) {//-----------オブジェクトごとの速度更新----------//
		object3d* curObj;
		if (h == -1) curObj = &sun; else curObj = objs + h;
		if (!curObj->used) continue;


		if (obMove) {		//-- 位置,速度,傾きのデータ更新
			if (0.000000001 < abs(curObj->lspX.w))
			{
				///-----------> 位置,速度,基準位置の更新 <-------------
				// .lspX メンバを使用
				//-- 半径1.0として超球面上のUC座標を定義
				pt4 locE = curObj->tudeToEuc(curObj->loc);		// 位置
				pt4 vecT = curObj->tudeToEuc(curObj->lspX.xyz());	// 速度
				pt4 std1 = curObj->tudeToEuc(curObj->std[0]);		// 基準位置1
				pt4 std2 = curObj->tudeToEuc(curObj->std[1]);		// 基準位置2
				if (GRAVITY || curObj->stdRefCnt == stdRefSpan)
				{
					if (GRAVITY) {
						pt4 fVec = curObj->fc2.mns(locE).mtp(adjSpd);	// 追加の力
						vecT = locE.pls(curObj->fc.mns(locE).pls(fVec));	// 合成
					}
					///-- 緯度,経度,深度を0に
					all_tudeRst(&vecT, curObj->loc, 0);	// 速度
					all_tudeRst(&std1, curObj->loc, 0); // 基準1
					all_tudeRst(&std2, curObj->loc, 0); // 基準2

					///-- 進行方向1,2の特定
					double rotOn[2] = {};
					rotOn[0] = atan2(vecT.x, vecT.y);
					rotOn[1] = atan2(pyth2(vecT.x, vecT.y), vecT.z);

					///-- 方向1,2を0に (基準1,2のみ)
					tudeRst(&std1.x, &std1.y, rotOn[0], 0);//-- X-Y 回転
					tudeRst(&std2.x, &std2.y, rotOn[0], 0);
					tudeRst(&std1.y, &std1.z, rotOn[1], 0);//-- Y-Z 回転
					tudeRst(&std2.y, &std2.z, rotOn[1], 0);

					double lspW = (GRAVITY) ? pyth3(vecT.xyz()) : curObj->lspX.w;
					///-- 進行距離分移動(Z-W回転)し、方向1,2へ合わせる(Y-Z, X-Y回転)
					pt4 loc1(1, 0, 0, 0), vec1(1, 0, 0, 0);
					double fwVal = lspW * adjSpd / radius;
					tudeRst(&loc1.z, &loc1.w, fwVal, 1);//-- 位置
					tudeRst(&std1.z, &std1.w, fwVal, 1);//-- 基準1
					tudeRst(&std2.z, &std2.w, fwVal, 1);//-- 基準2
					if (GRAVITY) {
						vec1.z = lspW;
						tudeRst(&vec1.z, &vec1.w, fwVal, 1);
					}
					else
						tudeRst(&vec1.z, &vec1.w, fwVal + 1.0, 1);//-- 速度
					///-- Y-Z 回転
					tudeRst(&loc1.y, &loc1.z, rotOn[1], 1);//-- 基準1
					tudeRst(&vec1.y, &vec1.z, rotOn[1], 1);//-- 基準1
					tudeRst(&std1.y, &std1.z, rotOn[1], 1);//-- 基準1
					tudeRst(&std2.y, &std2.z, rotOn[1], 1);//-- 基準2
					///-- X-Y 回転
					tudeRst(&loc1.x, &loc1.y, rotOn[0], 1);//-- 基準1
					tudeRst(&vec1.x, &vec1.y, rotOn[0], 1);//-- 基準1
					tudeRst(&std1.x, &std1.y, rotOn[0], 1);//-- 基準1
					tudeRst(&std2.x, &std2.y, rotOn[0], 1);//-- 基準2

					///-- 緯度,経度,深度を戻す
					all_tudeRst(&loc1, curObj->loc, 1);	//-- 位置
					all_tudeRst(&vec1, curObj->loc, 1);	//-- 速度
					all_tudeRst(&std1, curObj->loc, 1);	//-- 基準1
					all_tudeRst(&std2, curObj->loc, 1);	//-- 基準2

					///-- 位置,速度,基準位置を上書き (end
					curObj->loc = curObj->eucToTude(loc1);
					curObj->std[0] = curObj->eucToTude(std1);
					curObj->std[1] = curObj->eucToTude(std2);
					if (GRAVITY) {
						curObj->fc = vec1;
						curObj->fc2.asg(0, 0, 0, 0);
					}
					else curObj->lspX.asgPt3(curObj->eucToTude(vec1));

					curObj->stdRefCnt = 0;
				}

				// ▼高速化(?)版 連続使用でstd誤差蓄積
				else
				{
					//-- まず4つの基本軸を算出 (正規化済)
					pt4 normN = locE;
					pt4 normO = normN.mtp(COS_1);

					// 速度ベクトルの方向ベクトル
					pt4 lspXN = vecT.mns(normO).mtp(SIN_1R);

					//o-正規化標準1,2ベクトルの分解 (normN成分は無し)
					double std1xL = pt4::dot(lspXN, std1);
					pt4 std1x = lspXN.mtp(std1xL);
					pt4 std1e = std1.mns(normO).mns(std1x);

					double std2xL = pt4::dot(lspXN, std2);
					pt4 std2x = lspXN.mtp(std2xL);
					pt4 std2e = std2.mns(normO).mns(std2x);

					// 更新
					double lspW = curObj->lspX.w * adjSpd / radius;

					pt2 tmpRt = pt2(0, 1);
					tudeRst(&tmpRt.x, &tmpRt.y, lspW, 1);
					locE = normN.mtp(tmpRt.y).pls(lspXN.mtp(tmpRt.x));

					tmpRt = pt2(SIN_1, COS_1);
					tudeRst(&tmpRt.x, &tmpRt.y, lspW, 1);
					vecT = normN.mtp(tmpRt.y).pls(lspXN.mtp(tmpRt.x));

					tmpRt = pt2(std1xL, COS_1);
					tudeRst(&tmpRt.x, &tmpRt.y, lspW, 1);
					std1 = std1e
						.pls(normN.mtp(tmpRt.y))
						.pls(lspXN.mtp(tmpRt.x));

					tmpRt = pt2(std2xL, COS_1);
					tudeRst(&tmpRt.x, &tmpRt.y, lspW, 1);
					std2 = std2e
						.pls(normN.mtp(tmpRt.y))
						.pls(lspXN.mtp(tmpRt.x));

					///-- 位置,速度,基準位置を上書き (end
					curObj->loc = curObj->eucToTude(locE);
					curObj->std[0] = curObj->eucToTude(std1);
					curObj->std[1] = curObj->eucToTude(std2);
					curObj->lspX.asgPt3(curObj->eucToTude(vecT));
				}

				// std再計算カウンタ
				++curObj->stdRefCnt;
			}
			///----------- 傾きの更新 -------------
			curObj->rot = curObj->rot.pls(curObj->rsp.mtp(adjSpd));

			///-- 軌跡の更新 --
			if (h < BWH_QTY || BWH_QTY + PLR_QTY <= h) {
				for (int i = object3d::PAST_QTY - 1; 0 < i; i--)
					curObj->past[i] = curObj->past[i - 1];

				pt4 loc4 = curObj->tudeToEuc(curObj->loc);
				pt3 tmpt;
				tmpt.x = atan2(loc4.x, loc4.z);		//--方向1
				tmpt.y = atan2(pyth2(loc4.x, loc4.z), loc4.y);	//--方向2
				tmpt.z = curObj->loc.z * radius;	//--距離(長さ)
				curObj->past[0] = tmpt;
			}
		}
	}
}

// 射撃オブジェクト更新 H3
void engine3d::UpdFloatObjsH3()
{
	if (!obMove) return;
	
	//-----------オブジェクトごとの速度更新----------//
	for (int h = BWH_QTY + PLR_QTY; h < OBJ_QTY; h++)
	{
		object3d* curObj = objs + h;
		if (!curObj->used) continue;

		//-- 位置,速度,傾きのデータ更新
		//-----------> 位置,速度,基準位置の更新 <-------------
		pt3 preLoc = curObj->loc;
		// 原点に移動
		curObj->ParallelMove(curObj->loc, false);

		// 速度方向に移動
		double lspEuc = object3d::ClcEucFromHypb(curObj->lspX.w * adjSpd / radius);
		if (lspEuc > abs(0.000000001))
		{
			pt3 drc = curObj->lspX.xyz().norm().mtp(lspEuc);

			// 有効範囲チェック
			if (pyth3(drc) < H3_MAX_RADIUS)
				curObj->ParallelMove(drc, true);
			else
			{
				// プレイヤーの場合範囲内に留める
				if (BWH_QTY <= h && h < BWH_QTY + PLR_QTY)
				{
					if(h3objLoop)
						curObj->DealH3OohObj(h3objLoop);
				}
				else
					curObj->DealH3OohObj(h3objLoop);
			}
		}

		// 元の位置に戻す
		curObj->ParallelMove(preLoc, true);
	}

	for (int h = 0; h < OBJ_QTY; h++)
	{
		///----------- 傾きの更新 -------------
		object3d* curObj = objs + h;
		curObj->rot = curObj->rot.pls(curObj->rsp.mtp(adjSpd));
	}
}

// プレイヤーオブジェクト更新
void engine3d::UpdPlayerObjsS3(double* cmrStd) 
{
	objs[PLR_No].rot.asg(-ope.cmRot.x, ope.cmRot.y, ope.cmRot.z);

	//----
	object3d* curObj = &objs[PLR_No];
	pt4 locE = curObj->tudeToEuc(curObj->loc);
	pt4 std1 = curObj->tudeToEuc(curObj->std[0]);		// 基準位置1
	pt4 std2 = curObj->tudeToEuc(curObj->std[1]);		// 基準位置2
	///-------- 傾きの更新 ----------
	//-- まず4つの基本軸を算出 (正規化済)
	pt4 normN = locE;
	pt4 normO = normN.mtp(COS_1);
	pt4 std1N = std1.mns(normO).mtp(SIN_1R);
	pt4 std2N = std2.mns(normO).mtp(SIN_1R);

	// クロス積を算出 (長さは1.0)
	pt4 sideN = pt4::cross(normN, std1N, std2N);

	if (!CheckTrackedEnable())
	{
		// 視線固定回転
		pt4 tmpN[2];
		pt2 tmpRt = pt2(0, SIN_1);
		tudeRst(&tmpRt.x, &tmpRt.y, curObj->rot.z, 1);
		tmpN[0] = pt4()
			.pls(std2N.mtp(tmpRt.y))
			.pls(sideN.mtp(tmpRt.x))
			.mtp(SIN_1R);
		tmpN[1] = pt4()
			.pls(std2N.mtp(tmpRt.x).mtp(-1))
			.pls(sideN.mtp(tmpRt.y))
			.mtp(SIN_1R);
		std2N = tmpN[0], sideN = tmpN[1];

		// 視線移動回転1
		tmpRt = pt2(0, SIN_1);
		tudeRst(&tmpRt.x, &tmpRt.y, curObj->rot.y, 1);
		tmpN[0] = pt4()
			.pls(std1N.mtp(tmpRt.y))
			.pls(std2N.mtp(tmpRt.x))
			.mtp(SIN_1R);
		tmpN[1] = pt4()
			.pls(std1N.mtp(tmpRt.x).mtp(-1))
			.pls(std2N.mtp(tmpRt.y))
			.mtp(SIN_1R);
		std1N = tmpN[0], std2N = tmpN[1];

		// 視線移動回転2
		tmpRt = pt2(0, SIN_1);
		tudeRst(&tmpRt.x, &tmpRt.y, curObj->rot.x, 1);
		tmpN[0] = pt4()
			.pls(std1N.mtp(tmpRt.y))
			.pls(sideN.mtp(tmpRt.x))
			.mtp(SIN_1R);
		tmpN[1] = pt4()
			.pls(std1N.mtp(tmpRt.x).mtp(-1))
			.pls(sideN.mtp(tmpRt.y))
			.mtp(SIN_1R);
		std1N = tmpN[0], sideN = tmpN[1];
	}
	else
	{
		// 対象オブジェクト方向を向く
		object3d* trgObj = &objs[viewTrackIdx];
		pt4 trgE = object3d::tudeToEuc(trgObj->loc);
		pt4 rotv4 = trgE.mns(normN);
		double rot4Len = pyth4(rotv4);
		if (rot4Len > 0.0000000001)
		{
			double ip = pt4::dot(normN.mtp(-1), rotv4.norm());
			pt4 rotv3 = rotv4.mns(normN.mtp(-1 * rot4Len * ip));
			double rot3Len = pyth4(rotv3);
			if (rot3Len > 0.0000000001)
			{
				double ip2 = pt4::dot(std1N, rotv3.norm());
				pt4 rotvS1 = std1N.mtp(rot3Len * ip2);
				pt4 rotv = rotv3.mns(rotvS1);
				pt4 rotvN = rotv.norm(sideN);

				double rpLen = pt4::dot(std2N, rotvN);
				pt4 std2N_rp = rotvN.mtp(rpLen);
				pt4 std2N_rs = std2N.mns(std2N_rp);


				double rot = atan2(pyth4(rotv), rot3Len * ip2);
				object3d::RotVecs4(&std1N, &rotvN, rot);			// 対象方向へ回転
				std2N = std2N_rs.pls(rotvN.mtp(rpLen));
			}
		}

		// 視線固定回転
		object3d::RotVecs4(&std2N, &sideN, curObj->rot.z);	// 正面固定回転
	}


	///-------- 位置,基準位置の更新 ----------
	pt4 cmLc = pt4(0, ope.cmLoc.y, ope.cmLoc.z, ope.cmLoc.x).mtp(3);
	cmLc.w = pyth3(cmLc.x, cmLc.y, cmLc.z);

	std1 = std1N.mtp(SIN_1).pls(normO);
	std2 = std2N.mtp(SIN_1).pls(normO);

	if (cmLc.w > 0.0000000001)
	{
		// 移動方向ベクトル(正規化)
		pt4 lspXN = std1N.mtp(cmLc.z)
			.pls(std2N.mtp(cmLc.y))
			.pls(sideN.mtp(cmLc.x))
			.mtp(1 / cmLc.w);

		//o-正規化標準1,2ベクトルの分解 (normN成分は無し)
		double std1xL = pt4::dot(lspXN, std1);
		pt4 std1x = lspXN.mtp(std1xL);
		pt4 std1e = std1.mns(normO).mns(std1x);

		double std2xL = pt4::dot(lspXN, std2);
		pt4 std2x = lspXN.mtp(std2xL);
		pt4 std2e = std2.mns(normO).mns(std2x);

		// 更新
		pt2 tmpRt = pt2(0, 1);
		tudeRst(&tmpRt.x, &tmpRt.y, cmLc.w / radius, 1);
		locE = normN.mtp(tmpRt.y).pls(lspXN.mtp(tmpRt.x));

		tmpRt = pt2(std1xL, COS_1);
		tudeRst(&tmpRt.x, &tmpRt.y, cmLc.w / radius, 1);
		std1 = std1e
			.pls(normN.mtp(tmpRt.y))
			.pls(lspXN.mtp(tmpRt.x));

		tmpRt = pt2(std2xL, COS_1);
		tudeRst(&tmpRt.x, &tmpRt.y, cmLc.w / radius, 1);
		std2 = std2e
			.pls(normN.mtp(tmpRt.y))
			.pls(lspXN.mtp(tmpRt.x));
	}

	///-- 位置,基準位置を上書き (end
	curObj->loc = curObj->eucToTude(locE);
	curObj->std[0] = curObj->eucToTude(std1);
	curObj->std[1] = curObj->eucToTude(std2);

	///----------- 更新後のカメラ基準の測定 -------------
	///-- 緯度,経度,深度を0に
	all_tudeRst(&std1, curObj->loc, 0);// 基準1
	all_tudeRst(&std2, curObj->loc, 0);// 基準2
	///-- 基準方向1,2,3の特定
	curObj->clcStd(std1, std2, cmrStd);

	// std修正
	if (curObj->stdRefCnt == stdRefSpan)
	{
		pt4 ntd1 = pt4(COS_1, 0, 0, SIN_1);
		pt4 ntd2 = pt4(COS_1, 0, SIN_1, 0);

		///-- 方向1,2を0に (基準1,2のみ)
		tudeRst(&ntd1.x, &ntd1.y, cmrStd[2], 1);//-- X-Y 回転
		tudeRst(&ntd2.x, &ntd2.y, cmrStd[2], 1);
		tudeRst(&ntd1.y, &ntd1.z, cmrStd[1], 1);//-- Y-Z 回転
		tudeRst(&ntd2.y, &ntd2.z, cmrStd[1], 1);
		tudeRst(&ntd1.x, &ntd1.y, cmrStd[0], 1);//-- X-Y 回転
		tudeRst(&ntd2.x, &ntd2.y, cmrStd[0], 1);

		all_tudeRst(&ntd1, curObj->loc, 1);
		all_tudeRst(&ntd2, curObj->loc, 1);

		curObj->std[0] = object3d::eucToTude(ntd1);
		curObj->std[1] = object3d::eucToTude(ntd2);
		curObj->stdRefCnt = 0;
	}
	// std再計算カウンタ
	++curObj->stdRefCnt;
	
	// 表示情報更新
	double mvCoe = (1000.0 / fps) / radius;
	double rtCoe = (1000.0 / fps);
	cmData.loc.asg(cmLc.z * mvCoe, cmLc.x * mvCoe, cmLc.y * mvCoe);
	cmData.rot.asg(objs[PLR_No].rot.x * rtCoe, objs[PLR_No].rot.y * rtCoe, objs[PLR_No].rot.z * rtCoe);

}

// プレイヤー更新 H3
void engine3d::UpdPlayerObjsH3(double* cmrStd)
{
	//----
	object3d* curObj = &objs[PLR_No];
	curObj->rot.asg(-ope.cmRot.x, ope.cmRot.y, ope.cmRot.z);
	pt3 preLoc = curObj->loc;

	// 原点に移動
	curObj->ParallelMove(curObj->loc, false);

	//---> 新規回転の反映
	// 軸ベクトル定義
	pt3 std1N = curObj->std[0].norm();
	pt3 std2N = curObj->std[1].norm();
	pt3 sideN = pt3::cross(std2N, std1N);

	if (!CheckTrackedEnable())
	{
		// 軸方向の回転
		object3d::RotVecs(&std2N, &sideN, curObj->rot.z);	// 正面固定回転
		object3d::RotVecs(&std1N, &std2N, curObj->rot.y);	// 上下方向回転
		object3d::RotVecs(&std1N, &sideN, curObj->rot.x);	// 左右方向回転
	}
	else
	{
		// 対象オブジェクト方向を向く
		object3d* _trgObj = &objs[viewTrackIdx];
		object3d trgObj(*_trgObj);
		trgObj.ParallelMove(preLoc, false);
		pt3 rotvN = pt3(trgObj.loc.x, trgObj.loc.y, 0).norm(sideN);

		double rpLen = pt3::dot(std2N, rotvN);
		pt3 std2N_rp = rotvN.mtp(rpLen);
		pt3 std2N_rs = std2N.mns(std2N_rp);

		double rot = atan2(pyth2(trgObj.loc.x, trgObj.loc.y), trgObj.loc.z);
		object3d::RotVecs(&std1N, &rotvN, rot);			// 対象方向へ回転
		std2N = std2N_rs.pls(rotvN.mtp(rpLen));

		// 軸方向の回転
		object3d::RotVecs(&std2N, &sideN, curObj->rot.z);	// 正面固定回転
	}

	curObj->std[0] = std1N.mtp(H3_STD_LEN);
	curObj->std[1] = std2N.mtp(H3_STD_LEN);

	///-------- 位置,基準位置の更新 ----------
	pt4 cmLc = pt4(0, ope.cmLoc.y, ope.cmLoc.z, ope.cmLoc.x).mtp(1 / radius);
	cmLc.w = pyth3(cmLc.x, cmLc.y, cmLc.z);
	double eucW = object3d::ClcEucFromHypb(cmLc.w);

	// 有効範囲チェック
	if (cmLc.w > 0.0000000001 && eucW < H3_MAX_RADIUS)
	{
		// 移動方向ベクトル
		pt3 lspX = std1N.mtp(cmLc.z)
			.pls(std2N.mtp(cmLc.y))
			.pls(sideN.mtp(cmLc.x))
			.norm()
			.mtp(eucW);

		curObj->ParallelMove(lspX, true);	// 平行移動
	}

	// 元の位置に戻す
	curObj->ParallelMove(preLoc, true);


	// todo★ 表示情報更新
	//double mvCoe = (1000.0 / fps) / radius;
	//double rtCoe = (1000.0 / fps);
	//cmData.loc.asg(cmLc.z * mvCoe, cmLc.x * mvCoe, cmLc.y * mvCoe);
	//cmData.rot.asg(curObj->rot.x * rtCoe, curObj->rot.y * rtCoe, curObj->rot.z * rtCoe);
}

// VRオブジェクト更新
void engine3d::ClcVRObjectPosS3(VRDeviceOperation devOpe, object3d* curObj, double* cmrStd)
{
	if (devOpe.std[0].isZero())
		return;

	//----
	pt4 locE = curObj->tudeToEuc(curObj->loc);
	pt4 std1 = curObj->tudeToEuc(curObj->std[0]);		// 基準位置1
	pt4 std2 = curObj->tudeToEuc(curObj->std[1]);		// 基準位置2
	///-------- 傾きの更新 ----------
	//-- まず4つの基本軸を算出 (正規化済)
	pt4 normN = locE;
	pt4 normO = normN.mtp(COS_1);
	pt4 std1N = std1.mns(normO).mtp(SIN_1R);
	pt4 std2N = std2.mns(normO).mtp(SIN_1R);
	pt4 sideN = pt4::cross(normN, std1N, std2N);	// (長さは1.0)

	// for objects except the head.
	pt3 std1NDf, std2NDf;

	double rotOn[3];
	curObj->clcStd(devOpe.std[0], devOpe.std[1], rotOn);
	curObj->rot.asg(rotOn[0], rotOn[1], rotOn[2]);
	{
		pt4 tmp;
		pt4 s1x, s1y, s1z;
		pt4 s2x, s2y, s2z;
		// 視線固定回転
		pt4 tmpN[2];
		pt2 tmpRt = pt2(0, 1);
		tudeRst(&tmpRt.x, &tmpRt.y, curObj->rot.z, 1);
		//
		s2y = std2N.mtp(tmpRt.y); 
		s2x = sideN.mtp(tmpRt.x);

		// 視線移動回転1
		tmpRt = pt2(0, 1);
		tudeRst(&tmpRt.x, &tmpRt.y, curObj->rot.y, 1);
		//
		s1z = std1N.mtp(tmpRt.y);
		s1y = std2N.mtp(tmpRt.x);
		tmp = s2y.mtp(tmpRt.y);
		s2z = std1N.mtp(-1 * pt4::dot(std2N, s2y)).mtp(tmpRt.x);
		s2y = tmp;

		// 視線移動回転2
		tmpRt = pt2(0, 1);
		tudeRst(&tmpRt.x, &tmpRt.y, curObj->rot.x, 1);
		//
		tmp = s1y.mtp(tmpRt.y);
		s1x = sideN.mtp(pt4::dot(std2N, s1y)).mtp(tmpRt.x);
		s1y = tmp;
		pt4 s2xy = s2x.pls(s2y);
		pt4 s2xy1 = s2xy.mtp(tmpRt.y);
		double s2xy_yL = pt4::dot(std2N, s2xy);
		double s2xy_xL = pt4::dot(sideN, s2xy);
		pt4 s2xy_y = std2N.mtp(s2xy_yL);
		pt4 s2xy_x = s2xy.mns(s2xy_y);
		pt4 s2xyS = pt4()
			.pls(sideN.mtp(s2xy_yL))
			.pls(std2N.mtp(-1 * s2xy_xL));
		pt4 s2xy2 = s2xyS.mtp(tmpRt.x);


		// set result
		pt4 newStd1N = s1x.pls(s1y).pls(s1z).norm();
		pt4 newStd2N = s2xy1.pls(s2xy2).pls(s2z).norm();
		pt4 newSideN = pt4::cross(normN, std1N, std2N);
		if (cmrStd == nullptr)
		{
			std1NDf = pt3(
				pt4::dot(sideN, newStd1N),
				pt4::dot(std2N, newStd1N), 
				pt4::dot(std1N, newStd1N));
			std2NDf = pt3(
				pt4::dot(sideN, newStd2N),
				pt4::dot(std2N, newStd2N),
				pt4::dot(std1N, newStd2N));
		}
		else
		{
			std1N = newStd1N;
			std2N = newStd2N;
			sideN = newSideN;
			std1NDf = pt3(0, 0, 1);
			std2NDf = pt3(0, 1, 0);
		}
	}
	curObj->rot = pt3();

	///-------- 位置,基準位置の更新 ----------
	pt4 cmLc = pt4(0, devOpe.loc.x, devOpe.loc.y, devOpe.loc.z);
	cmLc.w = pyth3(cmLc.x, cmLc.y, cmLc.z);

	std1 = std1N.mtp(SIN_1).pls(normO);
	std2 = std2N.mtp(SIN_1).pls(normO);

	if (cmLc.w > 0.0000000001)
	{
		// 移動方向ベクトル(正規化)
		pt4 lspXN = std1N.mtp(cmLc.z)
			.pls(std2N.mtp(cmLc.y))
			.pls(sideN.mtp(cmLc.x))
			.mtp(1 / cmLc.w);

		//o-正規化標準1,2ベクトルの分解 (normN成分は無し)
		double std1xL = pt4::dot(lspXN, std1);
		pt4 std1x = lspXN.mtp(std1xL);
		pt4 std1e = std1.mns(normO).mns(std1x);

		double std2xL = pt4::dot(lspXN, std2);
		pt4 std2x = lspXN.mtp(std2xL);
		pt4 std2e = std2.mns(normO).mns(std2x);

		// 更新
		pt2 tmpRt = pt2(0, 1);
		tudeRst(&tmpRt.x, &tmpRt.y, cmLc.w / radius, 1);
		locE = normN.mtp(tmpRt.y).pls(lspXN.mtp(tmpRt.x));

		tmpRt = pt2(std1xL, COS_1);
		tudeRst(&tmpRt.x, &tmpRt.y, cmLc.w / radius, 1);
		std1 = std1e
			.pls(normN.mtp(tmpRt.y))
			.pls(lspXN.mtp(tmpRt.x));

		tmpRt = pt2(std2xL, COS_1);
		tudeRst(&tmpRt.x, &tmpRt.y, cmLc.w / radius, 1);
		std2 = std2e
			.pls(normN.mtp(tmpRt.y))
			.pls(lspXN.mtp(tmpRt.x));

		//
		normN = locE;
		normO = normN.mtp(COS_1);
		std1N = std1.mns(normO).mtp(SIN_1R);
		std2N = std2.mns(normO).mtp(SIN_1R);
		sideN = pt4::cross(normN, std1N, std2N);
		std1 = pt4()
			.pls(std1N.mtp(std1NDf.z))
			.pls(std2N.mtp(std1NDf.y))
			.pls(sideN.mtp(std1NDf.x))
			.mtp(SIN_1)
			.pls(normO);
		std2 = pt4()
			.pls(std1N.mtp(std2NDf.z))
			.pls(std2N.mtp(std2NDf.y))
			.pls(sideN.mtp(std2NDf.x))
			.mtp(SIN_1)
			.pls(normO);
	}

	///-- 位置,基準位置を上書き (end
	curObj->loc = curObj->eucToTude(locE);
	curObj->std[0] = curObj->eucToTude(std1);
	curObj->std[1] = curObj->eucToTude(std2);

	// HMD以外の場合は終了
	if (cmrStd == nullptr)
		return;

	///----------- 更新後のカメラ基準の測定 -------------
	///-- 緯度,経度,深度を0に
	all_tudeRst(&std1, curObj->loc, 0);// 基準1
	all_tudeRst(&std2, curObj->loc, 0);// 基準2
	///-- 基準方向1,2,3の特定
	curObj->clcStd(std1, std2, cmrStd);

	// std修正
	if (curObj->stdRefCnt == stdRefSpan)
	{
		pt4 ntd1 = pt4(COS_1, 0, 0, SIN_1);
		pt4 ntd2 = pt4(COS_1, 0, SIN_1, 0);

		///-- 方向1,2を0に (基準1,2のみ)
		tudeRst(&ntd1.x, &ntd1.y, cmrStd[2], 1);//-- X-Y 回転
		tudeRst(&ntd2.x, &ntd2.y, cmrStd[2], 1);
		tudeRst(&ntd1.y, &ntd1.z, cmrStd[1], 1);//-- Y-Z 回転
		tudeRst(&ntd2.y, &ntd2.z, cmrStd[1], 1);
		tudeRst(&ntd1.x, &ntd1.y, cmrStd[0], 1);//-- X-Y 回転
		tudeRst(&ntd2.x, &ntd2.y, cmrStd[0], 1);

		all_tudeRst(&ntd1, curObj->loc, 1);
		all_tudeRst(&ntd2, curObj->loc, 1);

		curObj->std[0] = object3d::eucToTude(ntd1);
		curObj->std[1] = object3d::eucToTude(ntd2);
		curObj->stdRefCnt = 0;
	}
	// std再計算カウンタ
	++curObj->stdRefCnt;
}


void engine3d::UpdVRObjectsS3(double* cmrStd)
{
	// HMD
	object3d* plrObj = &objs[PLR_No];
	ClcVRObjectPosS3(ope.vrDev[0], plrObj, cmrStd);

	// hands
	for (int i = 0; i < 2; i++)
	{
		vrHand[i].loc = plrObj->loc;
		vrHand[i].std[0] = plrObj->std[0];
		vrHand[i].std[1] = plrObj->std[1];
	}
	ClcVRObjectPosS3(ope.vrDev[1], &vrHand[0], nullptr);
	ClcVRObjectPosS3(ope.vrDev[2], &vrHand[1], nullptr);

	// menu
	vrMenuObj.used = menuLgc.menu.displayed;
	vrMenuObj.loc = vrHand[0].loc;
	vrMenuObj.std[0] = vrHand[0].std[0];
	vrMenuObj.std[1] = vrHand[0].std[1];

}


// 相対位置計算
void engine3d::ClcRelaivePosS3(double* cmrStd)
{
	for (int h = -5; h < objCnt; h++) {	//==============オブジェクトごとの処理==============//

		object3d* curObj = GetObject(h);
		if (!curObj->used) continue;
		pt3 drawLoc;

		///---- ▼自身の回転の反映 ----///

		///---- ▼カメラ位置を考慮して移動 ----///
		///-- 毎度のtude > euc変換, tude0
		pt4 locT = curObj->tudeToEuc(curObj->loc);	//-- 位置
		pt4 std1 = curObj->tudeToEuc(curObj->std[0]);	//-- 基準1
		pt4 std2 = curObj->tudeToEuc(curObj->std[1]);	//-- 基準2
		all_tudeRst(&locT, objs[PLR_No].loc, 0);	//-- 位置
		all_tudeRst(&std1, objs[PLR_No].loc, 0);	//-- 基準1
		all_tudeRst(&std2, objs[PLR_No].loc, 0);	//-- 基準2
		///-- カメラ基準(回転)を考慮して移動	(大円回転
		tudeRst(&locT.x, &locT.y, cmrStd[0], 0);//-- X-Y 回転 (-方向1
		tudeRst(&std1.x, &std1.y, cmrStd[0], 0);
		tudeRst(&std2.x, &std2.y, cmrStd[0], 0);
		tudeRst(&locT.y, &locT.z, cmrStd[1], 0);//-- Y-Z 回転 (-方向2
		tudeRst(&std1.y, &std1.z, cmrStd[1], 0);
		tudeRst(&std2.y, &std2.z, cmrStd[1], 0);
		tudeRst(&locT.x, &locT.y, cmrStd[2], 0);//-- X-Y 回転 (-方向3
		tudeRst(&std1.x, &std1.y, cmrStd[2], 0);
		tudeRst(&std2.x, &std2.y, cmrStd[2], 0);
		//-- 後方カメラなら
		if (ope.cmBack) {
			tudeRst(&locT.x, &locT.z, PIE, 1);//-- X-Y 回転 (-方向3
			tudeRst(&std1.x, &std1.z, PIE, 1);
			tudeRst(&std2.x, &std2.z, PIE, 1);
		}
		// VR
		if (!ope.vrDev[0].std[0].isZero())
		{
			tudeRst(&locT.x, &locT.w, ope.VREysDst / radius, 0);
			tudeRst(&std1.x, &std1.w, ope.VREysDst / radius, 0);
			tudeRst(&std2.x, &std2.w, ope.VREysDst / radius, 0);
		}

		///------ オブジェクトの見かけの座標を計算 ------
		pt3 locR = curObj->eucToTude(locT);
		curObj->locr = locR;	//-- 地図で使用するため格納
		///-- 緯度,経度,深度を0に
		all_tudeRst(&std1, locR, 0);// 基準1
		all_tudeRst(&std2, locR, 0);// 基準2
		///-- オブジェクト基準方向1,2,3の特定
		double objStd[3];
		curObj->clcStd(std1, std2, objStd);

		curObj->objStd.asg(objStd[0], objStd[1], objStd[2]);	//-- 光用 面倒なので
	}
}


// 相対位置計算 H3
void engine3d::ClcRelaivePosH3(double* cmrStd)
{
	object3d* plrObj = &objs[PLR_No];
	pt3 plrLoc = plrObj->loc;

	// プレイヤーstd算出
	object3d plrCpy(*plrObj);	// コピー
	plrCpy.ParallelMove(plrCpy.loc, false);	// 原点に移動
	double rotOn[3];
	plrCpy.clcStd(plrCpy.std[0], plrCpy.std[1], rotOn);


	// 各obj位置ををプレイヤーからの相対位置に
	for (int h = 0; h < OBJ_QTY; h++)
	{
		object3d* curObj = objs + h;

		//プレイヤー中心の平行移動 原点へ
		curObj->ParallelMove(plrLoc, false);
		
		// 有効範囲チェック
		double cLocLen = pyth3(curObj->loc);
		if (cLocLen > H3_MAX_RADIUS)
		{
			// プレイヤーの場合範囲内に留める
			if (BWH_QTY <= h && h < BWH_QTY + PLR_QTY)
			{
				if (h3objLoop)
					curObj->DealH3OohObj(h3objLoop);
				else
				{
					pt3 adjLoc = curObj->loc.mtp(H3_MAX_RADIUS / cLocLen);
					curObj->ParallelMove(curObj->loc, false);
					curObj->ParallelMove(adjLoc, true);
				}
			}
			else
				curObj->DealH3OohObj(h3objLoop);
		}

		// プレイヤーの回転リセット
		tudeRst(&curObj->loc.x, &curObj->loc.y, rotOn[0], 0);
		tudeRst(&curObj->std[0].x, &curObj->std[0].y, rotOn[0], 0);
		tudeRst(&curObj->std[1].x, &curObj->std[1].y, rotOn[0], 0);
		tudeRst(&curObj->lspX.x, &curObj->lspX.y, rotOn[0], 0);

		tudeRst(&curObj->loc.y, &curObj->loc.z, rotOn[1], 0);
		tudeRst(&curObj->std[0].y, &curObj->std[0].z, rotOn[1], 0);
		tudeRst(&curObj->std[1].y, &curObj->std[1].z, rotOn[1], 0);
		tudeRst(&curObj->lspX.y, &curObj->lspX.z, rotOn[1], 0);

		tudeRst(&curObj->loc.x, &curObj->loc.y, rotOn[2], 0);
		tudeRst(&curObj->std[0].x, &curObj->std[0].y, rotOn[2], 0);
		tudeRst(&curObj->std[1].x, &curObj->std[1].y, rotOn[2], 0);
		tudeRst(&curObj->lspX.x, &curObj->lspX.y, rotOn[2], 0);

		//-- 後方カメラなら
		pt3 locT = curObj->loc;
		object3d backObj(*curObj);
		if (ope.cmBack) {
			tudeRst(&backObj.loc.x, &backObj.loc.z, PIE, 1);
			tudeRst(&backObj.std[0].x, &backObj.std[0].z, PIE, 1);
			tudeRst(&backObj.std[1].x, &backObj.std[1].z, PIE, 1);
			locT = backObj.loc;
			backObj.ParallelMove(backObj.loc, false);
		}

		curObj->locr = locT;

		// std算出、調整、保存
		pt3 locOld = curObj->loc;
		curObj->ParallelMove(curObj->loc, false);
		double objStd[3];
		if(ope.cmBack)
			curObj->clcStd(backObj.std[0], backObj.std[1], objStd);
		else
			curObj->clcStd(curObj->std[0], curObj->std[1], objStd);
		curObj->objStd.asg(objStd[0], objStd[1], objStd[2]);

		// std調整
		curObj->OptimStd();
		// 速度vec調整
		curObj->lspX.asgPt3(curObj->lspX.xyz().norm().mtp(H3_STD_LEN));
		
		// 元の位置に戻す
		curObj->ParallelMove(locOld, true);	
	}
}


object3d* engine3d::GetObject(int idx)
{
	switch (idx)
	{
	case -5:
		return &vrMenuObj;
	case -4:
		return &vrHand[0];
	case -3:
		return &vrHand[1];
	case -2:
		return &markObj;
	case -1:
		return &sun;
	default:
		return objs + idx;
	}
}


// 現プレイヤー座標計算
void engine3d::ClcCoordinate()
{
	object3d* baseObj = objs + 0;
	pt3 baseLoc = baseObj->loc;
	object3d* plrObj = &objs[PLR_No];

	if (!baseObj->used)
	{
		cmCo = pt3(0, 0, 0);
		return;
	}

	object3d plrCpy(*plrObj);	// コピー
	object3d baseCpy(*baseObj);	// コピー
	plrCpy.ParallelMove(baseLoc, false);	// 原点に移動
	baseCpy.ParallelMove(baseLoc, false);	// 原点に移動
	double rotOn[3];
	baseCpy.clcStd(baseCpy.std[0], baseCpy.std[1], rotOn);
	// 回転をリセット
	tudeRst(&plrCpy.loc.x, &plrCpy.loc.y, rotOn[0], 0);
	tudeRst(&plrCpy.loc.y, &plrCpy.loc.z, rotOn[1], 0);
	tudeRst(&plrCpy.loc.x, &plrCpy.loc.y, rotOn[2], 0);

	// 結果を格納
	cmCo.x = object3d::ClcHypbFromEuc(pyth3(plrCpy.loc)) * radius;
	cmCo.y = atan2(plrCpy.loc.x, plrCpy.loc.y);
	cmCo.z = atan2(pyth2(plrCpy.loc.x, plrCpy.loc.y), plrCpy.loc.z);
}



//============== 初期化 =================
void engine3d::InitWorld()	// 世界初期化
{
	// プレイヤー座標obj初期化
	cmData.loc.asg(0, 0, 0);
	cmData.rot.asg(0, 0, 0);

	// 世界形状
	if(worldGeo == WorldGeo::SPHERICAL)
		InitS3();
	else if(worldGeo == WorldGeo::HYPERBOLIC)
		InitH3();
}

int engine3d::InitH3()	// 双曲世界用初期化
{
	// todo★ (優先度低) 双曲距離1.0と比較したradiusの正しい値

	// 残弾
	player.ep = ENR_QTY;

	//-- 太陽
	sun.used = false;	// 無効化

	//-- 軌跡オブジェクト
	markObj.objInitH3(meshs + 0);
	markObj.loc = pt3(	//-- 位置
		0,
		0,
		0
	);
	markObj.init_stdH3(false);	//-- 角度標準の設定
	markObj.mesh = &markMesh;
	markObj.draw = 1;
	markObj.used = false;	//-- 有効化


	// 最初に全部オブジェクトを用意
	int h = 0;
	// todo★ 基準線
	for (h; h < BWH_QTY; h++) 
	{
		objs[h].objInitH3(meshs + 0);
		objs[h].draw = 0;
		objs[h].scale = 0.5 * radius;
		objs[h].init_stdH3(0);	//-- std
		objs[h].used = true;	//-- 有効化
	}

	///-- プレイヤー ----------
	for (h; h < BWH_QTY + PLR_QTY; h++)
	{
		objs[h].objInitH3(meshs + 1);

		objs[h].rsp.asg(1 DEG, 0, 0);
		objs[h].used = true;	//-- 有効化
		objs[h].draw = 2;
	}
	// ランダムな位置
	RandLocH3(RandMode::Uniform, ObjType::Player);
	// 一期は中心に
	objs[BWH_QTY].loc = pt3(0, 0, 0);
	objs[BWH_QTY].init_stdH3(0);
	objs[BWH_QTY].lspX.asgPt3(objs[BWH_QTY].std[0]);

	///-- 放出オブジェクト ------
	for (h; h < BWH_QTY + PLR_QTY + ENR_QTY; h++)
	{
		objs[h].objInitH3(meshs + 4);

		objs[h].rot.asg(0, 0 DEG, 0);
		objs[h].rsp.asg(0 DEG, 0, 0);
		objs[h].used = false;	//-- 有効化
		objs[h].draw = 2;
		objs[h].scale = 3;
	}
	// ランダムな位置
	RandLocH3(RandMode::Uniform, ObjType::Energy);

	///-- 共通
	for (h = 0; h < OBJ_QTY; h++) 
	{
		objs[h].markInitS3(radius);
	}


	return 1;
}

int engine3d::InitS3()	// 球面世界用初期化
{
	// 残弾
	player.ep = ENR_QTY;

	//-- 太陽
	sun.objInitS3(meshs + 6);
	sun.loc = pt3(	//-- 位置
		0.0, 0.0, 0.0
	);
	sun.mkLspX_S3(	//-- 速度 
		pt4(SPEED_MAX, 0.8, 1.6, 1.0)
	);
	sun.draw = 2;
	sun.used = false;	//-- 有効化
	sun.init_stdS3(0);//-- 標準の設定

	//-- 軌跡オブジェクト
	markObj.objInitS3(meshs + 0);
	markObj.loc = pt3(	//-- 位置
		0,
		0,
		0
	);
	markObj.init_stdS3(0);	//-- 角度標準の設定
	markObj.mesh = &markMesh;
	markObj.draw = 1;
	markObj.used = false;	//-- 有効化

	// VR hand
	if (vrFlag)
	{
		for (int i = 0; i < 2; i++)
		{
			vrHand[i].draw = 2;
			vrHand[i].objInitS3(meshs + 22);
			vrHand[i].used = true;
		}

		vrMenuObj.draw = 2;
		vrMenuObj.objInitS3(nullptr);
		vrMenuObj.used = true;
		vrMenuMesh.meshInitC((meshLen - 1) + 3);
		vrMenuObj.mesh = &vrMenuMesh;
		vrMenuObj.scale = 4;
	}


	//-- 最初に全オブジェクトを用意
	int h = 0;
	///-- 基準線
	for (h; h < BWH_QTY; h++) {
		objs[h].objInitS3(meshs + 0);
		objs[h].draw = 0;
		objs[h].scale = 0.5 * radius;
		objs[h].init_stdS3(0);	//-- std
		objs[h].used = true;	//-- 有効化
	}
	///-- プレイヤー ----------
	for (h; h < BWH_QTY + PLR_QTY; h++) {
		objs[h].objInitS3(meshs + 1);
		objs[h].loc = pt3(	//-- 位置
			(((double)rand() / RAND_MAX) * 2 - 1) * PIE,
			((double)rand() / RAND_MAX) * PIE,
			((double)rand() / RAND_MAX) * PIE
		);
		objs[h].mkLspX_S3( pt4(	//-- 速度
			0,
			(((double)rand() / RAND_MAX) * 2 - 1) * PIE,
			((double)rand() / RAND_MAX) * PIE,
			((double)rand() / RAND_MAX) * PIE
		));
		objs[h].rsp.asg(1 DEG, 0, 0);
		objs[h].init_stdS3(0);	//-- std
		objs[h].used = true;	//-- 有効化
		objs[h].draw = 2;
	}
	///-- 放出オブジェクト ------
	for (h; h < BWH_QTY + PLR_QTY + ENR_QTY; h++) {
		objs[h].objInitS3(meshs + 4);
		objs[h].loc = pt3(	//-- 位置
			(((double)rand() / RAND_MAX) * 2 - 1) * PIE,
			((double)rand() / RAND_MAX) * PIE,
			((double)rand() / RAND_MAX) * PIE
		);
		objs[h].mkLspX_S3( pt4(	//-- 速度
			SPEED_MAX,
			(((double)rand() / RAND_MAX) * 2 - 1) * PIE,
			((double)rand() / RAND_MAX) * PIE,
			((double)rand() / RAND_MAX) * PIE
		));
		objs[h].rot.asg(0, 1 DEG, 0);
		objs[h].rsp.asg(1 DEG, 0, 0);
		objs[h].init_stdS3(1);	//-- std
		objs[h].used = false;	//-- 有効化
		objs[h].draw = 2;
	}
	// ランダムな位置
	RandLocS3(RandMode::Uniform);

	///-- 共通
	for (h = 0; h < OBJ_QTY; h++) {
		objs[h].markInitS3(radius);
	}

	//--
	return 0;
}

// 全射撃オブジェクトをクリア
void engine3d::ClearFloatObjs()
{
	player.ep = ENR_QTY;
	for (int i = BWH_QTY + PLR_QTY; i < OBJ_QTY; i++) 
	{
		objs[i].used = false;

		if(worldGeo == WorldGeo::HYPERBOLIC)
			objs[i].markInitH3(radius);
		else
			objs[i].markInitS3(radius);
	}
}

void engine3d::RandLoc(engine3d::RandMode mode, int qty)
{
	if (worldGeo == WorldGeo::HYPERBOLIC)
		RandLocH3(mode, ObjType::Energy, qty);
	else
		RandLocS3(mode, qty);
}

// objのランダム配置 (S3)
int engine3d::RandLocS3(engine3d::RandMode mode, int qty) {	

	//-- 放出オブジェクト ------
	int bgn = BWH_QTY + PLR_QTY;
	int end = (qty <= OBJ_QTY) ? qty : OBJ_QTY;
	ClearFloatObjs();

	for (int h = bgn; h < end; h++) {

		if (mode == RandMode::Cluster) {//-- 乱数 (極座標)
			objs[h].loc = pt3(	//-- 位置
				(((double)rand() / RAND_MAX) * 2 - 1) * PIE,
				((double)rand() / RAND_MAX) * PIE,
				((double)rand() / RAND_MAX) * PIE
			);
			objs[h].mkLspX_S3( pt4(	//-- 速度
				SPEED_MAX,
				(((double)rand() / RAND_MAX) * 2 - 1) * PIE,
				((double)rand() / RAND_MAX) * PIE,
				((double)rand() / RAND_MAX) * PIE
			));

		}
		else // 一様乱数
		{
			objs[h].loc = randLocUniS3(0);
			pt3 tmp = randLocUniS3(0);
			objs[h].mkLspX_S3( pt4(SPEED_MAX, tmp.x, tmp.y, tmp.z) );
		}
		objs[h].fc.asg(0, 0, 0, 0);	//必要?
		objs[h].init_stdS3(1);	//-- std
		objs[h].used = true;	//-- 有効化
		objs[h].markInitS3(radius);
	}

	return 0;
}

// objのランダム配置 (H3)
void engine3d::RandLocH3(engine3d::RandMode mode, ObjType oType, int qty)
{
	int bgn;
	int end;
	if (oType == ObjType::Player)
	{
		bgn = BWH_QTY;
		end = BWH_QTY + PLR_QTY;
	}
	else
	{
		bgn = BWH_QTY + PLR_QTY;
		end = (qty <= OBJ_QTY) ? qty : OBJ_QTY;
		ClearFloatObjs();
	}

	double maxRad = object3d::ClcHypbFromEuc(H3_MAX_RADIUS);
	for (int h = bgn; h < end; h++)
	{
		pt3 eucPt;
		if (mode == RandMode::Cluster) // 乱数 (極座標)
		{
			// ランダム位置決定
			double hypDst = ((double)rand() / RAND_MAX) * maxRad;		// 長さ (双曲)
			double dst = object3d::ClcEucFromHypb(hypDst);				// 長さ
			double po1 = ((double)rand() / RAND_MAX) * PIE;				// 極1
			double po2 = (((double)rand() / RAND_MAX) * 2 - 1) * PIE;	// 極2

			eucPt = pt3(0, 0, dst);
			tudeRst(&eucPt.y, &eucPt.z, po1, 1);
			tudeRst(&eucPt.x, &eucPt.y, po2, 1);

			// ランダム速度ベクトル決定
			double pox1 = ((double)rand() / RAND_MAX) * PIE;				// 極1
			double pox2 = (((double)rand() / RAND_MAX) * 2 - 1) * PIE;		// 極2
			pt4 spdDrc = pt4(SPEED_MAX, 0, 0, H3_STD_LEN);
			tudeRst(&spdDrc.y, &spdDrc.z, pox1, 1);
			tudeRst(&spdDrc.x, &spdDrc.y, pox2, 1);
			objs[h].lspX = spdDrc;

		}
		else 
		{
			eucPt = randLocUniH3(maxRad);
			objs[h].lspX.asgPt3(randVec3(H3_STD_LEN));
			objs[h].lspX.w = SPEED_MAX;
		}

		// 結果を反映
		objs[h].ParallelMove(eucPt, true);
		objs[h].used = true;
	}
}

pt3 engine3d::randLocUniS3(int cnt) {	//-- objのランダム配置

	///-- 放出オブジェクト ------

		//-- 一様乱数
	pt4 rnd(
		(((double)rand() / RAND_MAX) * 2 - 1),
		(((double)rand() / RAND_MAX) * 2 - 1),
		(((double)rand() / RAND_MAX) * 2 - 1),
		(((double)rand() / RAND_MAX) * 2 - 1)
	);
	if (
		(pyth4(rnd) > 1.0) && cnt != 100
		) {
		return randLocUniS3(cnt + 1);

	}
	else {
		return object3d::eucToTude(rnd);
	}

}

pt3 engine3d::randLocUniH3(double maxRad)
{
	pt3 rstPt;
	// 双曲球体積
	double volume = 4 * PIE * (0.25 * sinh(2 * maxRad) - 0.5 * maxRad);
	// 双曲球面積算出関数
	auto GetSpArea = [](double r) { return 4 * PIE * powi(sinh(r), 2); };
	// 最遠球面積
	double maxSpArea = GetSpArea(maxRad);

	// 半径を決定
	double eucDst;
	while (true)
	{
		double hypDst = ((double)rand() / RAND_MAX) * maxRad;		// 長さ (双曲)
		double ratio = GetSpArea(hypDst) / maxSpArea;
		if (ratio > ((double)rand() / RAND_MAX))
		{
			eucDst = object3d::ClcEucFromHypb(hypDst);
			break;
		}
	}

	// 球面上分布位置決定
	rstPt = randVec3(eucDst);

	return rstPt;
}


// ランダムベクトル (長さ一定)
pt3 engine3d::randVec3(double r)
{
	pt3 rstPt = pt3(0, 0, r);

	while (true)
	{
		pt3 rnd(
		(((double)rand() / RAND_MAX) * 2 - 1),
			(((double)rand() / RAND_MAX) * 2 - 1),
			(((double)rand() / RAND_MAX) * 2 - 1)
		);

		if (pyth3(rnd) < 1)
		{
			double rot[2];
			rot[0] = atan2(pyth2(rnd.x, rnd.y), rnd.z);
			rot[1] = atan2(rnd.x, rnd.y);

			tudeRst(&rstPt.y, &rstPt.z, rot[0], 1);
			tudeRst(&rstPt.x, &rstPt.y, rot[1], 1);

			break;
		}
	}

	return rstPt;
}

// オブジェクトの姿勢を動的に変更
int engine3d::setObjPos()
{
	if (!mvObjFlg)
		return 0;

	object3d* curObj = objs + PLR_No;


	//-- 半径1.0として超球面上のUC座標を定義
	pt4 locE = curObj->tudeToEuc(curObj->loc);	// 位置
	pt4 vecT = curObj->tudeToEuc(mvObjParam.loc);	// 速度
	pt4 std2 = curObj->tudeToEuc(curObj->std[1]);		// 基準位置2
	pt4 ntd1 = curObj->tudeToEuc(mvObjParam.rot);		// 次の基準1

	// 
	double lspW = asin(pyth4(vecT.mns(locE)) * 0.5) * 2;

	///-- 緯度,経度,深度を0に
	all_tudeRst(&vecT, curObj->loc, 0);	// 速度
	all_tudeRst(&std2, curObj->loc, 0); // 基準2
	all_tudeRst(&ntd1, curObj->loc, 0); // 次準2

	///-- 進行方向1,2の特定
	double rotOn[2] = {};
	rotOn[0] = atan2(vecT.x, vecT.y);
	rotOn[1] = atan2(pyth2(vecT.x, vecT.y), vecT.z);

	///-- 方向1,2を0に (基準2,次準1のみ)
	//-- X-Y 回転
	tudeRst(&std2.x, &std2.y, rotOn[0], 0);
	tudeRst(&ntd1.x, &ntd1.y, rotOn[0], 0);
	//-- Y-Z 回転
	tudeRst(&std2.y, &std2.z, rotOn[1], 0);
	tudeRst(&ntd1.y, &ntd1.z, rotOn[1], 0);

	// ローカルのローカル //
	tudeRst(&ntd1.z, &ntd1.w, lspW, 0);
	//pt4 std1 = ntd1;
	///-- 基準方向1,2,3の特定
	double rotStd[3] = {};
	curObj->clcStd(ntd1, std2, rotStd);
	pt4 xtd1(cos(1.0), 0, 0, sin(1.0));		// WXYZ
	pt4 xtd2(cos(1.0), 0, sin(1.0), 0);		// WXYZ
	tudeRst(&xtd2.x, &xtd2.y, rotStd[2], 1);
	tudeRst(&xtd2.y, &xtd2.z, rotStd[1], 1);
	tudeRst(&xtd2.x, &xtd2.y, rotStd[0], 1);
	tudeRst(&xtd1.y, &xtd1.z, rotStd[1], 1);
	tudeRst(&xtd1.x, &xtd1.y, rotStd[0], 1);
	pt4 std1 = xtd1;
	std2 = xtd2;


	///-- 進行距離分移動(Z-W回転)し、方向1,2へ合わせる(Y-Z, X-Y回転)
	pt4 loc1 = pt4(1, 0, 0, 0), vec1 = pt4(1, 0, 0, 0);
	tudeRst(&loc1.z, &loc1.w, lspW, 1);//-- 位置
	tudeRst(&std1.z, &std1.w, lspW, 1);//-- 基準1
	tudeRst(&std2.z, &std2.w, lspW, 1);//-- 基準2

	tudeRst(&vec1.z, &vec1.w, lspW * 2, 1);//-- 速度
	///-- Y-Z 回転
	tudeRst(&loc1.y, &loc1.z, rotOn[1], 1);//-- 基準1
	tudeRst(&vec1.y, &vec1.z, rotOn[1], 1);//-- 基準1
	tudeRst(&std1.y, &std1.z, rotOn[1], 1);//-- 基準1
	tudeRst(&std2.y, &std2.z, rotOn[1], 1);//-- 基準2
	///-- X-Y 回転
	tudeRst(&loc1.x, &loc1.y, rotOn[0], 1);//-- 基準1
	tudeRst(&vec1.x, &vec1.y, rotOn[0], 1);//-- 基準1
	tudeRst(&std1.x, &std1.y, rotOn[0], 1);//-- 基準1
	tudeRst(&std2.x, &std2.y, rotOn[0], 1);//-- 基準2

	///-- 緯度,経度,深度を戻す
	all_tudeRst(&loc1, curObj->loc, 1);	//-- 位置
	all_tudeRst(&vec1, curObj->loc, 1);	//-- 速度
	all_tudeRst(&std1, curObj->loc, 1);	//-- 基準1
	all_tudeRst(&std2, curObj->loc, 1);	//-- 基準2

	///-- 位置,速度,基準位置を上書き (end
	curObj->loc = curObj->eucToTude(loc1);
	curObj->std[0] = curObj->eucToTude(std1);
	curObj->std[1] = curObj->eucToTude(std2);



	mvObjFlg = false;
	return 1;
}



// 引力・斥力
int engine3d::physics() {


	for (int h = 0; h < objCnt; h++) {	//-- 距離の表示
		if (!objs[h].used) continue;


		for (int i = h + 1; i < objCnt; i++) {	//-- 距離の表示
			if (!objs[i].used) continue;

			if (h < BWH_QTY) {	///-- H & ...
				if (i < BWH_QTY) {	//-- H & H
				}
				else if (i < BWH_QTY + PLR_QTY) {	//-- H & P
				}
				else if (i < BWH_QTY + PLR_QTY + ENR_QTY) {	//-- H & E
				}

			}
			else if (h < BWH_QTY + PLR_QTY) {
				if (i < BWH_QTY + PLR_QTY) {	//-- P & P
				}
				else if (i < BWH_QTY + PLR_QTY + ENR_QTY) {	//-- P & E
				}

			}
			else if (h < OBJ_QTY) {
				if (i < OBJ_QTY) {	//-- E & E

					object3d* eObj1 = objs + h;
					object3d* eObj2 = objs + i;

					///-- 引力
					///-- 物体間の距離
					pt4 e1Loc = eObj1->tudeToEuc(eObj1->loc);
					pt4 e2Loc = eObj1->tudeToEuc(eObj2->loc);

					double unit = radius / 30;///すべての調整完了(途中直径変更も問題なし)
					double scale = pyth4(e1Loc.mns(e2Loc));
					double dstR = asin((scale / 2)) * 2;
					double force = 0.0001 * 1.0 / (dstR * dstR * unit * unit) * (1.0 / sin((PIE - dstR) * 0.5));

					double minR = 0.1 * 30 / radius;
					if (GRAVITY == 2)	//-- 斥力の場合
						force = -2 * force;
					else if (dstR < minR)	//-- 力の最大値
						force = 0.0001 * 1.0 / (minR * minR * unit * unit) * (1.0 / sin((PIE - dstR) * 0.5));
					//force += -4*unit*(minR-dstR)*(1.0/sin((PIE-dstR)*0.5));

					if (scale > pow(0.1, 300)) {//if文はバグ回避
						eObj1->fc2 = eObj1->fc2.pls(e2Loc.mns(e1Loc).mtp(1.0 / scale).mtp(force));
						eObj2->fc2 = eObj2->fc2.pls(e1Loc.mns(e2Loc).mtp(1.0 / scale).mtp(force));
					}
				}
			}
		}
	}


	return 0;
}

void object3d::cnvForce() {
	pt4 lsp4 = tudeToEuc(lspX.xyz());
	pt4 loc4 = tudeToEuc(loc);

	double scale = pyth4(lsp4.mns(loc4));
	double dstR = asin((scale / 2)) * 2;
	double force = lspX.w * (1.0 / sin((PIE - dstR) * 0.5));
	fc = lsp4.mns(loc4).mtp(1 / scale).mtp(force);

}
void engine3d::all_cnvForce() {
	for (int h = 0; h < objCnt; h++)
		if (objs[h].used) objs[h].cnvForce();

}


void engine3d::shoot() 
{
	for (int i = BWH_QTY + PLR_QTY; i < OBJ_QTY; i++) 
	{
		if (!objs[i].used)
			//if (0 < player.ep) //廃止
			{
				objs[i].used = true;

				objs[i].loc = objs[PLR_No].loc;
				objs[i].std[0] = objs[PLR_No].std[0];
				objs[i].std[1] = objs[PLR_No].std[1];
				objs[i].lspX = pt4(0, objs[PLR_No].std[0]);
				objs[i].lspX.w = SPEED_MAX;

				objs[i].cnvForce();
				objs[i].markInitS3(radius);

				player.ep += -1;
				break;
			}
	}
}



//== 操作関係 ==//


int engine3d::inPutMouseMv(double x, double y, int opt)
{
	// ジョイパッドによる上書を無効化
	ope.inputByKey = true;
	// 入力データ初期化
	InitInputParams();

	if (opt==1)
	{
		ope.updRotationParam
		(
			x, y, opt
		);
	}
	else
	{
		ope.updLocationParam
		(
			-1 * x * powi(3.0, ope.speed), 
			y * powi(3.0, ope.speed), 
			opt
		);
	}

	return 1;
}


int engine3d::inPutWheel(double val, int opt)
{
	// ジョイパッドによる上書を無効化
	ope.inputByKey = true;
	// 入力データ初期化
	InitInputParams();

	if (!opt) {
		ope.cmLoc.x = val * powi(3.0, ope.speed);	//前後
	}
	else {
		ope.cmRot.z = val * 0.1;	//-- ねじれ回転
	}

	return 1;
}



// キー入力処理メソッド
int engine3d::inPutKey(int key, int opt)
{
	// ジョイパッドによる上書を無効化
	ope.inputByKey = true;
	// 入力データ初期化
	InitInputParams();

	switch (key)
	{
	case IK::No_1:
		ope.speed = ++ope.speed % 3;
		break;

	case IK::No_2:
		shoot();
		break;

	case IK::No_3:
		PLR_No = BWH_QTY + ((PLR_No - BWH_QTY + 1) % PLR_QTY);
		break;

	case IK::No_4:
		if (mapDir != engine3d::MapDirection::TOP)
			mapDir = engine3d::MapDirection::TOP;
		else
			mapDir = engine3d::MapDirection::FRONT;
		break;

	case IK::No_5:
		if (mapMode != engine3d::MapMode::DUAL)
			mapMode = engine3d::MapMode::DUAL;
		else
			mapMode = engine3d::MapMode::SINGLE;
		break;

	case IK::SPACE:
		obMove = !obMove;
		break;

	case IK::ESCAPE:
		viewTrackIdx = -1;
		break;
	}

	return 1;
}

// 入力値初期化
void engine3d::InitInputParams()
{
	ope.cmLoc = pt3(0.0, 0.0, 0.0);
	ope.cmRot = pt3(0.0, 0.0, 0.0);


	ope.cmBack = false;
}

void engine3d::ChangeBasicObject(int bufIdx, int drawMode, bool sunFlg)
{
	if (bufIdx < 0)
	{
		objs[0].used = false;
		sun.used = false;
	}
	else
	{
		objs[0].mesh = meshs + bufIdx;
		objs[0].draw = drawMode;
		objs[0].used = true;
		sun.used = sunFlg;
	}
}

void engine3d::ChangeThrowObject(int bufIdx)
{
	for (int i = BWH_QTY + PLR_QTY; i < OBJ_QTY; i++)
	{
		objs[i].mesh = meshs + bufIdx;
	}
}

// 内積
double pt4::dot(pt4 a, pt4 b)
{
	return a.x * b.x + a.y * b.y + a.z * b.z + a.w * b.w;
}

// クロス積
pt4 pt4::cross(pt4 a, pt4 b, pt4 c)
{
	return pt4
	(
		+a.x * b.y * c.z + a.y * b.z * c.x + a.z * b.x * c.y
		- a.x * b.z * c.y - a.y * b.x * c.z - a.z * b.y * c.x,

		- a.y * b.z * c.w - a.z * b.w * c.y - a.w * b.y * c.z
		+ a.y * b.w * c.z + a.z * b.y * c.w + a.w * b.z * c.y,

		+ a.x * b.z * c.w + a.z * b.w * c.x + a.w * b.x * c.z
		- a.x * b.w * c.z - a.z * b.x * c.w - a.w * b.z * c.x,

		- a.x * b.y * c.w - a.y * b.w * c.x - a.w * b.x * c.y
		+ a.x * b.w * c.y + a.y * b.x * c.w + a.w * b.y * c.x
	);

}

// オブジェクト有効性チェック
bool engine3d::CheckSelectedEnable(int idx)
{
	if (idx == -1)
		idx = selectedIdx;

	if (idx == -1 || idx >= OBJ_QTY || !objs[idx].used)
		return false;

	return true;
}

// オブジェクト有効性チェック
bool engine3d::CheckTrackedEnable()
{
	if (viewTrackIdx == -1 || viewTrackIdx >= OBJ_QTY || !objs[viewTrackIdx].used)
		return false;

	return true;
}

void engine3d::SetRadius(double radius)
{
	this->radius = radius;
}
double engine3d::GetRadius()
{
	return this->radius;
}

