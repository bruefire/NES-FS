#include <stdint.h>
#include <string.h>
#include <String>
#include <iostream>
#include <Complex>
#include <algorithm>

#include "constants.h"
#include "engine3d.h"
#include "S3ErrorManager.h"
#include "functions.h"

using namespace std;


mesh3d::~mesh3d(){
	for(int i=0; i<faceLen; i++){	///=====面ごとの処理
		delete[] faces[i].pts;
		delete[] faces[i].txs;
	}
	pLen;
	delete[] faces;
	delete[] pts;
	delete[] txs;
	delete[] lines;
	delete[] pts2;
	delete[] ptsGl;
	delete[] pts0;
	//if(texJD)			// 派生クラスで解放
	//	stbi_image_free( bmp );
	objNameS = "";
  
}
poly::~poly(){
	for(uint32_t i=0; i<objLen; i++){	///=====面ごとの処理
		delete[] objs[i].faces;
	}
	delete[] objs;
	delete[] pts;
	delete[] lins;
 
}

void mesh3d::Init()
{
	coorType = COOR::POLAR;
	setNull();
}
mesh3d::mesh3d()
{
	Init();
}

int object3d::PAST_QTY = 75;
object3d::object3d()
	: stdRefCnt(1)
	, copyFlg(false)
{	//-- OBJ_コンストラクタ
	used = false;
	draw = 2;
	past = new pt3[PAST_QTY];

}
object3d::object3d(engine3d* owner)
	: object3d()
{	//-- OBJ_コンストラクタ
	this->owner = owner;
}
object3d::~object3d()
{
	if (copyFlg)
		return;

	delete[] past;
}
object3d::object3d(const object3d& obj)
{
	*this = obj;

	copyFlg = true;
	past = nullptr;
}

// std調整 (事前にlocの原点移動が必要)
void object3d::OptimStd()
{
	// std調整
	double rotOn[3];
	clcStd(std[0], std[1], rotOn);
	pt3 ntd1 = pt3(0, 0, owner->H3_STD_LEN);
	pt3 ntd2 = pt3(0, owner->H3_STD_LEN, 0);
	tudeRst(&ntd2.x, &ntd2.y, rotOn[2], 1);
	tudeRst(&ntd1.y, &ntd1.z, rotOn[1], 1);
	tudeRst(&ntd2.y, &ntd2.z, rotOn[1], 1);
	tudeRst(&ntd1.x, &ntd1.y, rotOn[0], 1);
	tudeRst(&ntd2.x, &ntd2.y, rotOn[0], 1);
	std[0] = ntd1;
	std[1] = ntd2;
}

// 鏡映 (H3)
// dstPts: 移動方向ベクトル (原点から離れた点を指定する)
object3d object3d::ReflectionH3(pt3 dst, pt3 ctr)
{

	// 鏡映用球面上の点 src, dst
	pt4 ctrR = pt4(pyth3OS(ctr), ctr.x, ctr.y, ctr.z);
	pt4 dstR = pt4(pyth3OS(dst), dst.x, dst.y, dst.z);

	// locR, dstRを通りクライン球面に接する直線
	// 切片、傾き算出
	pt4 ldDif = ctrR.mns(dstR);
	double slopeX = ldDif.x / ldDif.w;
	double slopeY = ldDif.y / ldDif.w;
	double slopeZ = ldDif.z / ldDif.w;
	double segmX = ctrR.x - ctrR.w * slopeX;
	double segmY = ctrR.y - ctrR.w * slopeY;
	double segmZ = ctrR.z - ctrR.w * slopeZ;

	// 各切片を成分とした点が接地点
	pt4 grdPt = pt4(0, segmX, segmY, segmZ);


	// 鏡映結果を算出
	auto ClcReflected = [](pt4 grdPt, pt3 trg) 
	{
		// 鏡映用球面上の点 std1, std2
		pt4 trgPt = pt4(pyth3OS(trg), trg.x, trg.y, trg.z);

		// 球面原点からの垂線ベクトル (接点)
		pt4 trgToGrd = grdPt.mns(trgPt);
		double ip = pt4::dot(trgToGrd.norm(pt4(0, 0, 0, 0), 0), trgPt.norm(pt4(0, 0, 0, 0), 0));
		double ttgRate = pyth4(trgPt) / pyth4(trgToGrd);
		pt4 ttgNorm = trgToGrd.mtp(ttgRate).mtp(ip);

		// 結果
		pt4 result = trgPt.mns(ttgNorm).mns(ttgNorm);

		return result; 
	};
	pt4 ntd1R = ClcReflected(grdPt, std[0]);
	pt4 ntd2R = ClcReflected(grdPt, std[1]);
	pt4 nspXR = ClcReflected(grdPt, lspX.xyz());
	pt4 nLocR = ClcReflected(grdPt, loc);


	// 結果を格納
	object3d moved(*this); // コピー
	moved.loc	 = nLocR.xyz();
	moved.std[0] = ntd1R.xyz();
	moved.std[1] = ntd2R.xyz();
	moved.lspX	 = pt4(lspX.w, nspXR.x, nspXR.y, nspXR.z);

	return moved;
}

/// <summary>
/// H3 平行移動 鏡映2回 (原点-任意点間の移動限定)
/// </summary>
void object3d::ParallelMove(pt3 tLoc, bool mode)
{
	double tLocPh = pyth3(tLoc);
	float refBrRatio = 0.8;
	pt3 refVec;

	if (tLocPh < 0.001)
		refVec = pt3(0.0, 0.0, owner->H3_REF_RADIUS);
	else if (tLocPh < owner->H3_REF_RADIUS * refBrRatio)
		refVec = tLoc.mtp(owner->H3_REF_RADIUS / tLocPh);
	else
		refVec = tLoc.mtp(owner->H3_REF_RADIUS / tLocPh * 0.5);

	pt3 bgnPt, endPt;
	if (mode){
		bgnPt = pt3(0, 0, 0);
		endPt = tLoc;
	} else {
		bgnPt = tLoc;
		endPt = pt3(0, 0, 0);
	}

	object3d mrr = ReflectionH3(refVec, bgnPt);
	object3d rst = mrr.ReflectionH3(endPt, refVec);
	
	// 結果反映
	loc    = rst.loc;
	std[0] = rst.std[0];
	std[1] = rst.std[1];
	lspX   = rst.lspX;
}

void object3d::DealH3OohObj(bool loopFlg)
{
	if (loopFlg)
	{
		// 反対側へ移動
		double nmlz = owner->H3_MAX_RADIUS / pyth3(loc) * 0.9999999999;

		pt3 lLoc = loc.mtp(nmlz).mtp(-1);
		ParallelMove(loc, false);
		ParallelMove(lLoc, true);
	}
	else
		used = false;
}

void object3d::TrackObjDirection(object3d* trgObj)
{
	if (owner->worldGeo == engine3d::WorldGeo::SPHERICAL)
	{
		pt4 locE = tudeToEuc(loc);
		pt4 std1 = tudeToEuc(std[0]);		// 基準位置1
		pt4 std2 = tudeToEuc(std[1]);		// 基準位置2
		///-------- 傾きの更新 ----------
		//-- まず4つの基本軸を算出 (正規化済)
		pt4 normN = locE;
		pt4 normO = normN.mtp(owner->COS_1);
		pt4 std1N = std1.mns(normO).mtp(owner->SIN_1R);
		pt4 std2N = std2.mns(normO).mtp(owner->SIN_1R);

		// クロス積を算出 (長さは1.0)
		pt4 sideN = pt4::cross(normN, std1N, std2N);

		// 対象オブジェクト方向を向く
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

		///-- 位置,基準位置を上書き (end
		std1 = std1N.mtp(owner->SIN_1).pls(normO);
		std2 = std2N.mtp(owner->SIN_1).pls(normO);
		std[0] = eucToTude(std1);
		std[1] = eucToTude(std2);
	}
	else if (owner->worldGeo == engine3d::WorldGeo::HYPERBOLIC)
	{
		pt3 preLoc = this->loc;
		// 原点に移動
		this->ParallelMove(preLoc, false);

		// 軸ベクトル定義
		pt3 std1N = this->std[0].norm();
		pt3 std2N = this->std[1].norm();
		pt3 sideN = pt3::cross(std2N, std1N);

		// 対象オブジェクト方向を向く
		object3d trgCpy(*trgObj);
		trgCpy.ParallelMove(preLoc, false);
		pt3 tNorm = trgCpy.loc.norm();
		double ip = pt3::dot(std1N, tNorm);
		pt3 tStd1 = std1N.mtp(ip);
		pt3 tSide = tNorm.mns(tStd1);
		pt3 rotvN = tSide.norm(sideN);

		double rpLen = pt3::dot(std2N, rotvN);
		pt3 std2N_rp = rotvN.mtp(rpLen);
		pt3 std2N_rs = std2N.mns(std2N_rp);

		double rot = atan2(pyth3(tSide), ip);
		object3d::RotVecs(&std1N, &rotvN, rot);			// 対象方向へ回転
		std2N = std2N_rs.pls(rotvN.mtp(rpLen));

		this->std[0] = std1N.mtp(owner->H3_STD_LEN);
		this->std[1] = std2N.mtp(owner->H3_STD_LEN);

		// 元の位置に戻す
		this->ParallelMove(preLoc, true);
	}
}

double object3d::GetDistance(object3d* trgObj)
{
	if (owner->worldGeo == engine3d::WorldGeo::SPHERICAL)
	{
		pt4 pLoc = object3d::tudeToEuc(this->loc);
		pt4 eLoc = object3d::tudeToEuc(trgObj->loc);
		double val = asin((pyth4(pLoc.mns(eLoc)) * 0.5)) * 2 * owner->radius;

		return val;
	}
	else if (owner->worldGeo == engine3d::WorldGeo::HYPERBOLIC)
	{
		pt3 preLoc = this->loc;
		// 原点に移動
		this->ParallelMove(preLoc, false);
		object3d trgCpy(*trgObj);
		trgCpy.ParallelMove(preLoc, false);

		double dst = object3d::ClcHypbFromEuc(pyth3(trgCpy.loc)) * owner->radius;

		// 元の位置に戻す
		this->ParallelMove(preLoc, true);

		return dst;
	}
}

// 相対的に位置再設定
bool object3d::SetLocRelativeH3(object3d* trgObj, pt3 nLoc, double dst)
{
	if (!trgObj->used)
		return false;

	// 位置再設定
	object3d trgCpy(*trgObj);
	pt3 preLoc = trgCpy.loc;
	trgCpy.ParallelMove(preLoc, false);

	object3d reObj(owner);
	reObj.objInitH3(nullptr);
	reObj.std[0] = trgCpy.std[0];
	reObj.std[1] = trgCpy.std[1];
	reObj.lspX.asgPt3(reObj.std[0]);
	reObj.lspX.w = owner->SPEED_MAX;

	pt3 std1N = trgCpy.std[0].norm();
	pt3 std2N = trgCpy.std[1].norm();
	pt3 sideN = pt3::cross(std2N, std1N);
	pt3 nLocK = pt3(0, 0, 0)
		.pls(std1N.mtp(nLoc.z))
		.pls(std2N.mtp(nLoc.y))
		.pls(sideN.mtp(nLoc.x))
		.norm()
		.mtp(object3d::ClcEucFromHypb(dst / owner->radius));

	reObj.ParallelMove(nLocK, true);
	reObj.ParallelMove(preLoc, true);

	// 有効範囲チェック
	double rstDst = pyth3(reObj.loc);
	if (rstDst > owner->H3_MAX_RADIUS || isnan(rstDst))
		return true;

	// 結果を反映
	this->used = true;
	this->loc = reObj.loc;
	this->std[0] = reObj.std[0];
	this->std[1] = reObj.std[1];
	this->lspX = reObj.lspX;

	return true;
}

// 相対的に位置再設定
bool object3d::SetLocRelativeS3(object3d* trgObj, pt3 nLoc, double dst)
{
	if (!trgObj->used)
		return false;

	// move obj to the same position with target
	loc = trgObj->loc;
	std[0] = trgObj->std[0];
	std[1] = trgObj->std[1];

	// prepare local axes etc
	pt4 locE = trgObj->tudeToEuc(trgObj->loc);
	pt4 std1 = trgObj->tudeToEuc(trgObj->std[0]);		// 基準位置1
	pt4 std2 = trgObj->tudeToEuc(trgObj->std[1]);		// 基準位置2
	pt4 normN = locE;
	pt4 normO = normN.mtp(owner->COS_1);
	pt4 std1N = std1.mns(normO).mtp(owner->SIN_1R);
	pt4 std2N = std2.mns(normO).mtp(owner->SIN_1R);
	pt4 sideN = pt4::cross(normN, std1N, std2N);

	pt4 dstN = pt4(0, 0, 0, 0)
		.pls(std1N.mtp(nLoc.z))
		.pls(std2N.mtp(nLoc.y))
		.pls(sideN.mtp(nLoc.x))
		.norm(pt4(std1N));

	// update loc
	pt2 tmpRt = pt2(0, 1);
	tudeRst(&tmpRt.x, &tmpRt.y, dst / owner->radius, 1);
	locE = pt4()
		.pls(normN.mtp(tmpRt.y))
		.pls(dstN.mtp(tmpRt.x));

	// update std1
	double ipDS1 = pt4::dot(std1, dstN);
	pt4 tDS1 = dstN.mtp(ipDS1);
	pt4 tRS1 = std1.mns(normO).mns(tDS1);

	tmpRt = pt2(pyth4(tDS1), owner->COS_1);
	tudeRst(&tmpRt.x, &tmpRt.y, dst / owner->radius, 1);
	std1 = pt4()
		.pls(normN.mtp(tmpRt.y))
		.pls(dstN.mtp(tmpRt.x))
		.pls(tRS1);

	// update std2
	double ipDS2 = pt4::dot(std2, dstN);
	pt4 tDS2 = dstN.mtp(ipDS2);
	pt4 tRS2 = std2.mns(normO).mns(tDS2);

	tmpRt = pt2(pyth4(tDS2), owner->COS_1);
	tudeRst(&tmpRt.x, &tmpRt.y, dst / owner->radius, 1);
	std2 = pt4()
		.pls(normN.mtp(tmpRt.y))
		.pls(dstN.mtp(tmpRt.x))
		.pls(tRS2);

	// set results
	this->used = true;
	this->loc = eucToTude(locE);
	this->std[0] = eucToTude(std1);
	this->std[1] = eucToTude(std2);
	this->lspX.asgPt3(std[0]);



	return true;
}

// 相対的に角度再設定
bool object3d::SetRotRelative(pt3 nRot)
{
	pt3 preLoc = loc;
	ParallelMove(preLoc, false);

	//---> 新規回転の反映
	// 軸ベクトル定義
	pt3 std1N = std[0].norm();
	pt3 std2N = std[1].norm();
	pt3 sideN = pt3::cross(std2N, std1N);

	// 軸方向の回転
	object3d::RotVecs(&std2N, &sideN, nRot.z);	// 正面固定回転
	object3d::RotVecs(&std1N, &std2N, nRot.y);	// 上下方向回転
	object3d::RotVecs(&std1N, &sideN, nRot.x);	// 左右方向回転

	// 結果の反映
	std[0] = std1N.mtp(owner->H3_STD_LEN);
	std[1] = std2N.mtp(owner->H3_STD_LEN);

	ParallelMove(preLoc, true);

	return true;
}

// 相対的に角度再設定
bool object3d::SetRotRelativeS3(pt3 nRot)
{
	// prepare local axes etc
	pt4 locE = tudeToEuc(loc);
	pt4 std1 = tudeToEuc(std[0]);		// 基準位置1
	pt4 std2 = tudeToEuc(std[1]);		// 基準位置2
	pt4 normN = locE;
	pt4 normO = normN.mtp(owner->COS_1);
	pt4 std1N = std1.mns(normO).mtp(owner->SIN_1R);
	pt4 std2N = std2.mns(normO).mtp(owner->SIN_1R);
	pt4 sideN = pt4::cross(normN, std1N, std2N);

	// rotate local axes
	object3d::RotVecs4(&std2N, &sideN, nRot.z);	// 正面固定回転
	object3d::RotVecs4(&std1N, &std2N, nRot.y);	// 上下方向回転
	object3d::RotVecs4(&std1N, &sideN, nRot.x);	// 左右方向回転

	// set results
	std1 = normO.pls(std1N.mtp(owner->SIN_1));
	std2 = normO.pls(std2N.mtp(owner->SIN_1));
	std[0] = eucToTude(std1);
	std[1] = eucToTude(std2);


	return true;
}

// スケール更新
bool object3d::SetScale(double scale)
{
	this->scale = scale;
	return true;
}
double object3d::GetScale()
{
	return this->scale;
}
// 速度更新
bool object3d::SetVelocity(double veloc)
{
	this->lspX.w = veloc;
	return true;
}
double object3d::GetVelocity()
{
	return this->lspX.w;
}
// update rsp
bool object3d::SetRsp(pt3 pts)
{
	this->rsp = pts;
	return true;
}
pt3 object3d::GetRsp()
{
	return this->rsp;
}

// std更新
void object3d::RotVecs(pt3* vec1, pt3* vec2, double rot, double len)
{
	pt3 tmpN[2];
	pt2 tmpRt = pt2(0, len);

	tudeRst(&tmpRt.x, &tmpRt.y, rot, 1);

	tmpN[0] = pt3()
		.pls(vec1->mtp(tmpRt.y))
		.pls(vec2->mtp(tmpRt.x));
	tmpN[1] = pt3()
		.pls(vec1->mtp(tmpRt.x).mtp(-1))
		.pls(vec2->mtp(tmpRt.y));

	*vec1 = tmpN[0];
	*vec2 = tmpN[1];
}

// std更新
void object3d::RotVecs4(pt4* vec1, pt4* vec2, double rot, double len)
{
	pt4 tmpN[2];
	pt2 tmpRt = pt2(0, len);

	tudeRst(&tmpRt.x, &tmpRt.y, rot, 1);

	tmpN[0] = pt4()
		.pls(vec1->mtp(tmpRt.y))
		.pls(vec2->mtp(tmpRt.x));
	tmpN[1] = pt4()
		.pls(vec1->mtp(tmpRt.x).mtp(-1))
		.pls(vec2->mtp(tmpRt.y));

	*vec1 = tmpN[0];
	*vec2 = tmpN[1];
}

// ToDo★: 標準の初期設定 H3
void object3d::init_stdH3(bool randSW)
{
	// 2つの基準ベクトル
	pt3 std1 = pt3(0, 0, owner->H3_STD_LEN);
	pt3 std2 = pt3(0, owner->H3_STD_LEN, 0);

	// ToDo★: ランダムstd
	if (randSW)
	{
		double rotOn[2] = {
			((double)rand() / RAND_MAX) * PIE * 2 - PIE,
			((double)rand() / RAND_MAX) * PIE
		};
		tudeRst(&std1.y, &std1.z, rotOn[0], 1);
		tudeRst(&std2.y, &std2.z, rotOn[0], 1);
		tudeRst(&std1.x, &std1.y, rotOn[1], 1);
		tudeRst(&std2.x, &std2.y, rotOn[1], 1);
	}
	pt3 tLoc = loc;
	loc = pt3(0,0,0);
	std[0] = std1;
	std[1] = std2;

	// 平行移動
	ParallelMove(tLoc, true);
}

void object3d::init_stdS3(bool randSW){	//-- 標準の初期設定 S3
	//-- 2つの基準ベクトル
	pt4 std1 = pt4( owner->COS_1,0,0,owner->SIN_1 );// WXYZ
	pt4 std2 = pt4( owner->COS_1,0,owner->SIN_1,0 );// WXYZ

	if(randSW){//todo★ ランダムなstd
		double rotOn[2] = {
			( (double)rand() / RAND_MAX )*PIE*2 - PIE,
			( (double)rand() / RAND_MAX )*PIE
		};
		tudeRst(&std1.y, &std1.z, rotOn[0], 1);
		tudeRst(&std2.y, &std2.z, rotOn[0], 1);
		tudeRst(&std1.x, &std1.y, rotOn[1], 1);
		tudeRst(&std2.x, &std2.y, rotOn[1], 1);
	}

	//-- 緯度,経度,深度に合わせる
	all_tudeRst(&std1, loc, 1);
	all_tudeRst(&std2, loc, 1);
		
	//-- 基準1を設定
	std[0].x = atan2(std1.x, std1.y);
	std[0].y = atan2(pyth2(std1.x,std1.y), std1.z);	
	std[0].z = atan2(pyth3(std1.x,std1.y,std1.z), std1.w);
	//-- 基準2を設定
	std[1].x = atan2(std2.x, std2.y);
	std[1].y = atan2(pyth2(std2.x,std2.y), std2.z);	
	std[1].z = atan2(pyth3(std2.x,std2.y,std2.z), std2.w);
}

// 二重開放防止
int mesh3d::setNull()
{
	faceLen = 0;
	pts = nullptr;
	faces = nullptr;
	lines = nullptr;
	txs = nullptr;
	bmp = nullptr;
	pts2 = nullptr;
	ptsGl = nullptr;
	pts0 = nullptr;

	return 1;
}

// 軌跡専用
int mesh3d::meshInitB(int qty, uint32_t texNo)
{
	pts2 = new float[qty*16];
	
	faceLen = 0;
	lLen = qty;
	texJD = false;
	this->texNo = texNo;

	return 0;
}
int mesh3d::meshInit(std::string objName, uint32_t texNo, int md)
{
	texJD = false;

	//ファイルオープン(作成)
	FILE *fp, *fpM;
	char tmpStr[1024];
	char cmpStr[256];
	char mtlStr[256];
	mtlStr[0] = 0x00;
	char *tp;
	uint32_t vQty = 0, fQty = 0, lQty = 0, vtQty = 0;
	objNameS = objName;
	string obj0 = objName;
	if(!md) obj0 = "obj\\" + obj0;
	string texName = "";
	bool pcFlg = false;

	
	
	//if((fp = _fsopen(&(obj0+".obj")[0], "r", _SH_DENYWR)) == NULL)	// ｸﾛﾌﾟﾗ化のため排他処理は犠牲に
	if((fp = fopen(&(obj0 + ".obj")[0], "r")) == NULL)
	{
		throw S3ErrorManager(".objファイルが見つかりません");
	}
	///まずv, f, l, vtをカウント, mtlファイル名確認 (roop1)
	while(fgets(tmpStr, 1024, fp) != NULL){
		if(tmpStr[0]=='v' && tmpStr[1]==' ') ++vQty;
		else if(tmpStr[0]=='l' && tmpStr[1]==' ') ++lQty;
		else if(tmpStr[0]=='v' && tmpStr[1]=='t') ++vtQty;
		else if(tmpStr[0]=='f'){
			tp = strtok(tmpStr+2, " ");
			while((tp=strtok(NULL, " ")) != NULL) ++fQty;
			--fQty;//調整

		}else{
			string getStr(tmpStr);
			if (getStr.substr(0, 6) == "mtllib") strcpy(mtlStr, tmpStr + 7);
			else if (getStr == "# on4D object\n") pcFlg = true;
			else if (getStr == "# not convert to polar coordinates\n") coorType = COOR::Cartesian;
		}
	}
	///マテリアルファイル (マテルroop1)
	int mtlQty = 0, mtlSum = 0;
	if(mtlStr[0]!=0){
		//if ((fpM = _fsopen(&(obj0 + ".mtl")[0], "r", _SH_DENYWR)) == NULL)	// ｸﾛﾌﾟﾗ化のため排他処理は犠牲に
		if((fpM = fopen(&(obj0+".mtl")[0], "r")) == NULL)
		{
			throw S3ErrorManager(".mtlファイルが見つかりません");
		}
		while(fgets(tmpStr, 1024, fpM) != NULL){
			memcpy(cmpStr, tmpStr, 6);
			cmpStr[6] = 0x00;
			if(strcmp(cmpStr, "newmtl")==0) ++mtlQty;
			else if(strcmp(cmpStr, "map_Kd")==0) texName = string(tmpStr+7);	//１つのみ
		}
	}
	///動的確保
	faces = new face3[fQty]; faceLen = fQty;
	pts = new pt3[vQty]; pLen = vQty;
	pts0 = new pt3[vQty];	//-- 一時的euc4座標
	txs = new pt2[vtQty]; txLen = vtQty;
	lines = new pt2i[lQty]; lLen = lQty;
	char *mtlNames = new char[256*mtlQty];
	char *mtlConts = new char[256*mtlQty];
	char *mtlContsT = new char[256*mtlQty];
	mtlSum = mtlQty;
	vQty = fQty = lQty = vtQty = 0;
	mtlQty = -1;

	/// (マテルroop2)
	uint32_t mtlPt = 0;
	bool mtlBgn = false;
	if(mtlStr[0]!=0){
		fseek(fpM, 0, SEEK_SET);
		while(fgets(tmpStr, 1024, fpM) != NULL){
			memcpy(cmpStr, tmpStr, 6);
			cmpStr[6] = 0x00;
			if(strcmp(cmpStr, "newmtl")==0){
				if(mtlBgn) mtlConts[mtlQty*256+mtlPt-1] = 0x00;	//終端文字
				++mtlQty;
				strcpy(mtlNames+mtlQty*256, tmpStr+7);
				mtlBgn = true;
				mtlPt = 0;

			}else if(mtlBgn){
				strcpy(mtlConts+mtlQty*256+mtlPt, tmpStr);
				mtlPt += strlen(tmpStr)+1;
				mtlConts[mtlQty*256+mtlPt-1] = '$';	//区切り文字
			}
		}
		if(mtlBgn) mtlConts[mtlQty*256+mtlPt-1] = 0x00;	//終端文字
	}
	///暫定色設定
	//cSet(200, 200, 200);
	///頂点, 辺, tex頂点データの格納 (roop2)
	fseek(fp, 0, SEEK_SET);
	while(fgets(tmpStr, 1024, fp) != NULL){
		if(tmpStr[0]=='v' && tmpStr[1]==' '){
			if(pcFlg){
				tp = strtok(tmpStr+2, " ");
				pts[vQty].x = strtod(tp, NULL);
				tp = strtok(NULL, " ");
				pts[vQty].y = strtod(tp, NULL);
				tp = strtok(NULL, " ");
				pts[vQty].z = strtod(tp, NULL);
			}else{
				tp = strtok(tmpStr+2, " ");
				pts[vQty].x = strtod(tp, NULL);
				tp = strtok(NULL, " ");
				pts[vQty].z = strtod(tp, NULL);
				tp = strtok(NULL, " ");
				pts[vQty].y = strtod(tp, NULL)*-1;
			}
			++vQty;

		}else if(tmpStr[0]=='l' && tmpStr[1]==' '){
			tp = strtok(tmpStr+2, " ");
			lines[lQty].x = strtol(tp, NULL, 10)-1;
			tp = strtok(NULL, " ");
			lines[lQty].y = strtol(tp, NULL, 10)-1;
			++lQty;

		}else if(tmpStr[0]=='v' && tmpStr[1]=='t'){
			tp = strtok(tmpStr+3, " ");
			txs[vtQty].x = strtod(tp, NULL);
			tp = strtok(NULL, " ");
			txs[vtQty].y = 1-strtod(tp, NULL);
			++vtQty;
		}
	}
	
	///面データの格納 (roop3)
	int mtlIdx = -1;
	fseek(fp, 0, SEEK_SET);
	while(fgets(tmpStr, 1024, fp) != NULL){
		memcpy(mtlContsT, mtlConts, 256*mtlSum);
		memcpy(cmpStr, tmpStr, 6);
		cmpStr[6] = 0x00;
		if(strcmp(cmpStr, "usemtl")==0)
			for(int i=0;i<mtlSum;i++){ if(strcmp(mtlNames+i*256, tmpStr+7)==0) mtlIdx = i; }
		else if(tmpStr[0]=='f'){
			///マテルKdを探す
			bool bgnJD = false, kdJD = false;
			char *tpM;
			while(mtlIdx!=-1){
				if(!bgnJD)tpM = strtok(mtlContsT+mtlIdx*256, "$");
				else tpM = strtok(NULL, "$");
				if(tpM==NULL) break;
				else if(*tpM=='K' && *(tpM+1)=='d'){ kdJD = true; break; }
				else bgnJD = true;//MessageBox(preWnd, tpM, TEXT("a"), MB_OK);
			}
			char rgb[3];
			if(kdJD){
				tpM = strtok(tpM+3, " ");
				rgb[0] = (uint8_t)(atof(tpM)*255);
				tpM = strtok(NULL, " ");
				rgb[1] = (uint8_t)(atof(tpM)*255);
				tpM = strtok(NULL, " ");
				rgb[2] = (uint8_t)(atof(tpM)*255);
				faces[fQty].cSet(rgb[0], rgb[1], rgb[2]);
			}else{
				faces[fQty].cSet(200, 200, 200);
			}
			
			///----///
			faces[fQty].pts = new pt3*[3];
			faces[fQty].txs = new pt2*[3];
			pINT strMax = (pINT)tmpStr + strlen(tmpStr) +1;	//-- 文字列終端のアドレス値
			tp = strtok(tmpStr+2, " ");
			char* svTok = tp +strlen(tp) +1;
			string tp0 = string(tp);
			string tp0_ = tp0;
			
			faces[fQty].pts[0] = &pts[atoi( strtok(tp, "/") )-1];
			faces[fQty].txs[0] = &txs[atoi( strtok(NULL, "/") )-1];

			tp = strtok(svTok, " ");
			svTok = tp +strlen(tp) +1;
			faces[fQty].pts[1] = &pts[atoi( strtok(tp, "/") )-1];
			faces[fQty].txs[1] = &txs[atoi( strtok(NULL, "/") )-1];
			
			tp = strtok(svTok, " ");	//	cout << svTok << endl;
			svTok = tp +strlen(tp) +1;	//	cout << (int)(svTok)[0] << endl;
			string preTp = string(tp);
			faces[fQty].pts[2] = &pts[atoi( strtok(tp, "/") )-1];
			faces[fQty].txs[2] = &txs[atoi( strtok(NULL, "/") )-1];

			
			while( (pINT)svTok < strMax ){
				tp = strtok(svTok, " ");
				++fQty;
				svTok = tp +strlen(tp) +1;
				string curTp = string(tp);
				
				faces[fQty].pts = new pt3*[3];
				faces[fQty].txs = new pt2*[3];
				faces[fQty].pts[0] = &pts[atoi( strtok(&tp0[0], "/") )-1];
				faces[fQty].txs[0] = &txs[atoi( strtok(NULL, "/") )-1];
				faces[fQty].pts[1] = &pts[atoi( strtok(&preTp[0], "/") )-1];
				faces[fQty].txs[1] = &txs[atoi( strtok(NULL, "/") )-1];
				faces[fQty].pts[2] = &pts[atoi( strtok(tp, "/") )-1];
				faces[fQty].txs[2] = &txs[atoi( strtok(NULL, "/") )-1];

				tp0 = tp0_;
				preTp = curTp;
				if(kdJD) faces[fQty].cSet(rgb[0], rgb[1], rgb[2]);
				else faces[fQty].cSet(200, 200, 200);
			};
			++fQty;
		}
	}
	fclose(fp);
	if(mtlStr[0]!=0) fclose(fpM);
	delete[] mtlNames;
	delete[] mtlConts;
	delete[] mtlContsT;


	///-- テクスチャロード & 設定 ( 1ｵﾌﾞｼﾞｪにつき1ﾃｸｽ. とりあえず
	this->texNo = texNo;
	this->texName = texName;
	this->extObjFlg = md;

	if(0 < texName.length() && 0<faceLen){
		texJD = true;

	}else
		texJD = false;
	

	///--

  //-- 超球表面空間 (緯度・軽度・深度)--//
	if(pcFlg){
		for(int i=0;i<pLen;i++){
			pts[i].z *= 2.0;
		}
	}
	else if (coorType == COOR::Cartesian) {
	}
	else{
		double tooLong = -1;
		for(int i=0;i<pLen;i++){
			pts0[i] = pts[i];
			double oLen = pyth3(pts[i]);
			pt3 tmpt;
			tmpt.x = atan2(pts[i].x, pts[i].y);		//--方向1
			tmpt.y = atan2(pyth2(pts[i].x,pts[i].y), pts[i].z);	//--方向2
			tmpt.z = oLen;	//--距離(長さ)
			pts[i] = tmpt;
			//if(OBJ_LEN_MAX < oLen)
			//	if(tooLong<oLen) tooLong = oLen;
		}
		//if(0<tooLong && md){	//-- 長さオーバーなら
		//	double fixRate = OBJ_LEN_MAX / tooLong;
		//	for(int i=0;i<pLen;i++) pts[i].z *= fixRate;
		//	MessageBox(preWnd, "リサイズしました", "オブジェクト情報", MB_OK);
		//}
	}


  ///----
 

  return 1;
}


void object3d::objInitH3(mesh3d* pForm)
{
	mesh = pForm;
	//-- モデルメッシュからコピー



	///--------
	///-----------///
	//alfa = 0.4;
	scale = 1;
	//------

	ctr.asg(0, 0, 0);
	loc.asg(0, 0, 0);	//
	rot.asg(0, 0, 0);
	lspX.asg(0, 0, 0, 0);
	rsp.asg(0 DEG, 0 DEG, 0 DEG);
	fc.asg(0, 0, 0, 0);
	fc2.asg(0, 0, 0, 0);
	ssp = 0.0;		// スケール速度

	init_stdH3(false);	//-- 角度標準の設定
	mkLspX_H3(pt4(0, std[0].x, std[0].y, std[0].z));

	polObj = false;
	//used = true;

}

void object3d::objInitS3(mesh3d* pForm)
{
  mesh = pForm;
  //-- モデルメッシュからコピー

	
  
  ///--------
  ///-----------///
  //alfa = 0.4;
  scale = 1;
  //------

	ctr.asg(0, 0, 0);
	loc.asg(0, 0, PIE);	//-- (緯度, 経度, 深度)
	pt4 cPt4 = pt4( 0, 0*PIE, 0*PIE, 0.2*PIE );
	mkLspX_S3(cPt4);
	rot.asg(0,0,0);
	rsp.asg(0 DEG, 0 DEG,0 DEG);
	fc.asg(0,0,0,0);
	fc2.asg(0,0,0,0);
	ssp = 0.0;		// スケール速度
  
  init_stdS3(0);	//-- 角度標準の設定

  polObj = false;
  //used = true;

}

// 軌跡初期化 S3
void object3d::markInitS3(double radius)
{
	pt4 loc4 = tudeToEuc( loc );
	pt3 tmpt;
	tmpt.x = atan2(loc4.x, loc4.z);		//--方向1
	tmpt.y = atan2(pyth2(loc4.x,loc4.z), loc4.y);	//--方向2
	tmpt.z = loc.z*radius;	//--距離(長さ)

	for(int i=0;i<PAST_QTY;i++) past[i] = tmpt;
}

// 軌跡初期化 H3
void object3d::markInitH3(double radius)
{
	//double dst = ClcHypbFromEuc(pyth3(loc));
	//pt3 tmpt;
	//tmpt.x = atan2(loc.x, loc.z);		//--方向1
	//tmpt.y = atan2(pyth2(loc.x, loc.z), loc.y);	//--方向2
	//tmpt.z = dst * radius;	//--距離(長さ)

	//for (int i = 0; i < PAST_QTY; i++) past[i] = tmpt;
}


poly::poly(){
}
void poly::polyInit(int form){


 int polForm = form;

 if(polForm==1){	//------正五胞体-------//
	objs = new object4d[5]; objLen = 5;
	for(uint32_t i=0;i<objLen;i++){
		objs[i].faces = new face4[4];
		objs[i].faceLen = 4; objs[i].cSet(222,222,111);
	}
	pts = new pt4[5];
	pt4 pt[5] = { pt4(-1, -1/sqrt(3.0), -1/sqrt(6.0), -1/sqrt(10.0)),
				  pt4(1 ,-1/sqrt(3.0), -1/sqrt(6.0), -1/sqrt(10.0)),
				  pt4(0, 2/sqrt(3.0), -1/sqrt(6.0), -1/sqrt(10.0)),
				  pt4(0, 0, sqrt(1.5), -1/sqrt(10.0)),
				  pt4(0, 0, 0, 4/sqrt(10.0)) };
	pSet(pt, 5);
	pLen = 5;
	
	objs[0].faces[0].fSet(0, 2, 1);	//面設定
	objs[0].faces[1].fSet(0, 1, 3);
	objs[0].faces[2].fSet(1, 2, 3);
	objs[0].faces[3].fSet(0, 3, 2);
	objs[1].faces[0].fSet(0, 4, 3);	//面設定
	objs[1].faces[1].fSet(0, 3, 1);
	objs[1].faces[2].fSet(1, 3, 4);
	objs[1].faces[3].fSet(0, 1, 4);
	objs[2].faces[0].fSet(1, 4, 3);	//面設定
	objs[2].faces[1].fSet(1, 3, 2);
	objs[2].faces[2].fSet(2, 3, 4);
	objs[2].faces[3].fSet(1, 2, 4);
	objs[3].faces[0].fSet(0, 3, 4);	//面設定
	objs[3].faces[1].fSet(4, 3, 2);
	objs[3].faces[2].fSet(0, 2, 3);
	objs[3].faces[3].fSet(4, 2, 0);
	objs[4].faces[0].fSet(2, 0, 1);	//面設定
	objs[4].faces[1].fSet(4, 1, 0);
	objs[4].faces[2].fSet(4, 2, 1);
	objs[4].faces[3].fSet(4, 0, 2);

	lins = new uint32_t[20];
	linLen = 20;
	uint32_t tln[20] = {
		0, 1,
		1, 2,
		2, 0,
		3, 0,
		3, 1,
		3, 2,
		3, 4,
		4, 0,
		4, 1,
		4, 2
	};
	lSet(lins, tln, 20);

 }else if(polForm==3){	//------ローポリハイパースフィア-------//

	objs = new object4d[128]; objLen = 128;
	uint32_t i = 0;
	for(;i<8;i++){
		objs[i].faces = new face4[4];
		objs[i].faceLen = 4; objs[i].cSet(222,222,111);
	}
	for(;i<24;i++){
		objs[i].faces = new face4[5+1];
		objs[i].faceLen = 5+1; objs[i].cSet(222,222,111);
	}
	for(;i<32;i++){
		objs[i].faces = new face4[4];
		objs[i].faceLen = 4; objs[i].cSet(222,222,111);
	}
	//--
	for(;i<40;i++){
		objs[i].faces = new face4[5+3];
		objs[i].faceLen = 5+3; objs[i].cSet(222,222,111);
	}
	for(;i<56;i++){
		objs[i].faces = new face4[6*2];
		objs[i].faceLen = 6*2; objs[i].cSet(222,222,111);
	}
	for(;i<72;i++){
		objs[i].faces = new face4[5+3];
		objs[i].faceLen = 5+3; objs[i].cSet(222,222,111);
	}
	for(;i<88;i++){
		objs[i].faces = new face4[6*2];
		objs[i].faceLen = 6*2; objs[i].cSet(222,222,111);
	}
	for(;i<96;i++){
		objs[i].faces = new face4[5+3];
		objs[i].faceLen = 5+3; objs[i].cSet(222,222,111);
	}
	//-
	for(;i<104;i++){
		objs[i].faces = new face4[4];
		objs[i].faceLen = 4; objs[i].cSet(222,222,111);
	}
	for(;i<120;i++){
		objs[i].faces = new face4[5+1];
		objs[i].faceLen = 5+1; objs[i].cSet(222,222,111);
	}
	for(;i<128;i++){
		objs[i].faces = new face4[4];
		objs[i].faceLen = 4; objs[i].cSet(222,222,111);
	}
	///
	//===頂点設定 ▼
	pts = new pt4[80];
	pLen = 80;
	double dRot[3] = {0,0,0};	//-- {x_y, xy_z, xyz_w};
	
	pt4 tmpt;
	pSet(0, 0,0,0,1);		//-- no.0
	for(int k=0;k<3;k++){	  ///-- xyz_w軸回転
		dRot[2] += PIE/4;		//-- xyz_w+
		tmpt.w = 1 *cos(dRot[2]);
		tmpt.z = 1 *1 *sin(dRot[2]);
		pSet(1+k*26, 0,0,tmpt.z,tmpt.w);
		for(int j=0;j<3;j++){	  ///-- xy_z軸回転
			dRot[1] += PIE/4;		//-- xy_z+
			tmpt.z = 1 *cos(dRot[1]) *sin(dRot[2]);
			int ptNo = 2 + k*26 + j*8;
			for(int i=ptNo;i<ptNo+8;i++){	  ///-- x_y軸回転
				tmpt.x = 1 *cos(dRot[0]) *sin(dRot[1]) *sin(dRot[2]);
				tmpt.y = 1 *sin(dRot[0]) *sin(dRot[1]) *sin(dRot[2]);
				pSet(i, tmpt.x,tmpt.y,tmpt.z,tmpt.w);
				dRot[0] += PIE/4;		//-- x_y+
			}
			dRot[0] = 0;		//-- x_y+
		}
		tmpt.z = 1 *(-1) *sin(dRot[2]);
		pSet((k+1)*26, 0,0,tmpt.z,tmpt.w);
		dRot[1] = 0;		//-- xy_z+
	}
	pSet(79, 0,0,0,-1);		//-- no.79 (最後)
	///===面設定 ▼
	uint32_t pIdx[30];
	for(int i=0;i<8;i++)
		tetraPts(2+i, 1, 2+(1+i)%8, 0, pIdx), objs[i].fSet(pIdx, 4);//
	
	for(int i=0;i<8;i++){
		piraPts(2+i,  2+(1+i)%8,  10+(1+i)%8, 10+i, 0, pIdx); objs[i+8].fSet(pIdx, 5);
		piraPts(10+i, 10+(1+i)%8, 18+(1+i)%8, 18+i, 0, pIdx); objs[i+16].fSet(pIdx, 5);
	}
	for(int i=0;i<8;i++)
		tetraPts(18+i, 0, 18+(1+i)%8, 26, pIdx), objs[i+24].fSet(pIdx, 4);
	
	//--
	for(int i=0;i<8;i++){
		prismPts(28+i, 27, 28+(1+i)%8, pIdx, -26); objs[i+32].fSet(pIdx, 5); 
	}
	for(int i=0;i<8;i++){
		cubePts(28+i, 28+(1+i)%8, 36+(1+i)%8, 36+i, pIdx, -26); objs[i+40].fSet(pIdx, 6);
		cubePts(36+i, 36+(1+i)%8, 44+(1+i)%8, 44+i, pIdx, -26); objs[i+48].fSet(pIdx, 6);
	}
	for(int i=0;i<8;i++){
		prismPts(44+i, 44+(1+i)%8, 52, pIdx, -26); objs[i+56].fSet(pIdx, 5);
	}
	//中間
	for(int i=0;i<8;i++){
		prismPts(54+i, 53, 54+(1+i)%8, pIdx, -26); objs[i+64].fSet(pIdx, 5); 
	}
	for(int i=0;i<8;i++){
		cubePts(54+i, 54+(1+i)%8, 62+(1+i)%8, 62+i, pIdx, -26); objs[i+72].fSet(pIdx, 6);
		cubePts(62+i, 62+(1+i)%8, 70+(1+i)%8, 70+i, pIdx, -26); objs[i+80].fSet(pIdx, 6);
	}
	for(int i=0;i<8;i++){
		prismPts(70+i, 70+(1+i)%8, 78, pIdx, -26); objs[i+88].fSet(pIdx, 5);
	}
	//-
	for(int i=0;i<8;i++){
		tetraPts(70+i, 78, 70+(1+i)%8, 79, pIdx); objs[i+96].fSet(pIdx, 4);
	}
	for(int i=0;i<8;i++){
		piraPts(70+i, 70+(1+i)%8, 62+(1+i)%8, 62+i, 79, pIdx); objs[i+104].fSet(pIdx, 5);
		piraPts(62+i, 62+(1+i)%8, 54+(1+i)%8, 54+i, 79, pIdx); objs[i+112].fSet(pIdx, 5);
	}
	for(int i=0;i<8;i++){
		tetraPts(54+i, 54+(1+i)%8, 53, 79, pIdx); objs[i+120].fSet(pIdx, 4);
	}


	///===辺設定 ▼
	lins = new uint32_t[272*2]; linLen = 272*2; //-- 辺の数
	int use = 0;

	for(int i=0;i<26;i++) lins[i*2+use] = 1+i, lins[i*2+use+1] = 0; use += 26*2;
	for(int i=0;i<26;i++) lins[i*2+use] = 1+i, lins[i*2+use+1] = 1+i+26; use += 26*2;
	for(int i=0;i<26;i++) lins[i*2+use] = 27+i, lins[i*2+use+1] = 27+i+26; use += 26*2;
	for(int i=0;i<26;i++) lins[i*2+use] = 53+i, lins[i*2+use+1] = 79; use += 26*2;
	//----//
	for(int i=0;i<8;i++) lins[i*2+use] = 2+i,  lins[i*2+use+1] = 3+(i%7);  use += 8*2;
	for(int i=0;i<8;i++) lins[i*2+use] = 10+i, lins[i*2+use+1] = 11+(i%7); use += 8*2;
	for(int i=0;i<8;i++) lins[i*2+use] = 18+i, lins[i*2+use+1] = 19+(i%7); use += 8*2;
	
	for(int i=0;i<8;i++)  lins[i*2+use] = 1, lins[i*2+use+1] = 2+i; use += 8*2;
	for(int i=0;i<16;i++) lins[i*2+use] = 2+i, lins[i*2+use+1] = 10+i;  use += 16*2;
	for(int i=0;i<8;i++)  lins[i*2+use] = 18+i, lins[i*2+use+1] = 26; use += 8*2;
	//--
	for(int i=0;i<8;i++) lins[i*2+use] = 28+i, lins[i*2+use+1] = 29+(i%7);  use += 8*2;
	for(int i=0;i<8;i++) lins[i*2+use] = 36+i, lins[i*2+use+1] = 37+(i%7); use += 8*2;
	for(int i=0;i<8;i++) lins[i*2+use] = 44+i, lins[i*2+use+1] = 45+(i%7); use += 8*2;
	
	for(int i=0;i<8;i++)  lins[i*2+use] = 27, lins[i*2+use+1] = 28+i; use += 8*2;
	for(int i=0;i<16;i++) lins[i*2+use] = 28+i, lins[i*2+use+1] = 36+i;  use += 16*2;
	for(int i=0;i<8;i++)  lins[i*2+use] = 44+i, lins[i*2+use+1] = 52; use += 8*2;
	//--
	for(int i=0;i<8;i++) lins[i*2+use] = 54+i, lins[i*2+use+1] = 55+(i%7);  use += 8*2;
	for(int i=0;i<8;i++) lins[i*2+use] = 62+i, lins[i*2+use+1] = 63+(i%7); use += 8*2;
	for(int i=0;i<8;i++) lins[i*2+use] = 70+i, lins[i*2+use+1] = 71+(i%7); use += 8*2;
	
	for(int i=0;i<8;i++)  lins[i*2+use] = 53, lins[i*2+use+1] = 54+i; use += 8*2;
	for(int i=0;i<16;i++) lins[i*2+use] = 54+i, lins[i*2+use+1] = 55+i;  use += 16*2;
	for(int i=0;i<8;i++)  lins[i*2+use] = 70+i, lins[i*2+use+1] = 79; use += 8*2;

	//if(PHYSICS_4D){
	//	scale = 0.2;
	//	for(int i=0;i<pLen;i++) pts[i] = pts[i].mtp(scale);	//スケール変換
	//}
pts[0].z;

 }else if(polForm==2){	//-------ハイパーキューブ----------//

	objs = new object4d[8]; objLen = 8;
	for(uint32_t i=0;i<objLen;i++){
		objs[i].faces = new face4[6*2];	
		objs[i].faceLen = 6*2; objs[i].cSet(250,230,150);
	}
	pts = new pt4[16];
	pt4 pt[16] = {pt4(-1,-1,-1,-1),	//===頂点設定 ▼
				  pt4( 1,-1,-1,-1),
				  pt4(-1, 1,-1,-1),
				  pt4( 1, 1,-1,-1),
				  pt4(-1,-1, 1,-1),
				  pt4( 1,-1, 1,-1),
				  pt4(-1, 1, 1,-1),
				  pt4( 1, 1, 1,-1),
				  pt4(-1,-1,-1, 1),
				  pt4( 1,-1,-1, 1),
				  pt4(-1, 1,-1, 1),
				  pt4( 1, 1,-1, 1),
				  pt4(-1,-1, 1, 1),
				  pt4( 1,-1, 1, 1),
				  pt4(-1, 1, 1, 1),
				  pt4( 1, 1, 1, 1)};
	pSet(pt, 16);
	pLen = 16;
	///
	uint32_t pIdx[30];
	cubePts(0, 2, 3, 1, pIdx, 8); objs[0].fSet(pIdx, 6); //===面設定 ▼
	cubePts(1, 3, 7, 5, pIdx, 8); objs[1].fSet(pIdx, 6);
	cubePts(5, 7, 6, 4, pIdx, 8); objs[2].fSet(pIdx, 6);
	cubePts(4, 6, 2, 0, pIdx, 8); objs[3].fSet(pIdx, 6);
	cubePts(4, 0, 1, 5, pIdx, 8); objs[4].fSet(pIdx, 6);
	cubePts(7, 3, 2, 6, pIdx, 8); objs[5].fSet(pIdx, 6);
	cubePts(0, 4, 5, 1, pIdx, 2); objs[6].fSet(pIdx, 6);
	cubePts(12,8, 9,13, pIdx, 2); objs[7].fSet(pIdx, 6); //===面設定 ▲
	///

	lins = new uint32_t[32*2]; linLen = 32*2; //===辺設定 ▼
	int c = -1;
	for(uint32_t i=0;i<8;i++){ lins[++c] = 2*i; lins[++c] = 2*i+1; }
	for(uint32_t i=0;i<12;i++)if(i<4||8<=i){ lins[++c] = i; lins[++c] = i+4; }
	for(uint32_t i=0;i<14;i++)if(i%4<2){ lins[++c] = i; lins[++c] = i+2; }
	for(uint32_t i=0;i<8;i++){ lins[++c] = i; lins[++c] = i+8; }

 //}else if(polForm<0){
	//objs = new object4d[1]; objLen = 1;
	//for(uint32_t i=0;i<objLen;i++){
	//	objs[i].faces = new face4[6*2];	
	//	objs[i].faceLen = 6*2; objs[i].cSet(250,230,150);
	//}

 }

 ctr.asg(0,0,0,0);
 loc.asg(5,0,0,0);	//-- 超級衝突modeなら必ずloc.asg(2,0,0,0);
 rot.asg(0,12 DEG,0,0,0,0);
 lsp.asg(0,0,0,0);//-0.1,-0.2,0);
 rsp.asg(0 DEG,0 DEG,0, 0 DEG,0 DEG,0 DEG);
}
///▼ 2D関係 ====================
pt2::pt2() {}
pt2::pt2(double x, double y)
{
	this->x = x; this->y = y;
}
pt2 pt2::mtp(double mt) { pt2 tmpt(x * mt, y * mt); return tmpt; };
pt2i::pt2i() {}
pt2i::pt2i(int x, int y)
{
	this->x = x; this->y = y;
}

///▼ 3D関係 ====================
void object3d::pSet(pt3* pt, uint32_t len){ for(uint32_t i=0;i<len;i++) mesh->pts[i] = pt[i]; }	//オブジェクトのpt設定

void pt3::asg(double nx, double ny, double nz){ x = nx, y = ny, z = nz; } //代入
void pt3::asg2(pt3 xyz){ x = xyz.x, y = xyz.y, z = xyz.z; }	 //代入
pt3 pt3::pls(pt3 pts){ pts.x+=x, pts.y+=y, pts.z+=z; return pts; }
pt3 pt3::mns(pt3 pts){ pts.x = x-pts.x, pts.y = y-pts.y, pts.z = z-pts.z; return pts; }
pt3 pt3::mtp(double mt){ pt3 pt; pt.x = x*mt, pt.y = y*mt, pt.z = z*mt; return pt; }	//乗算
pt3 pt3::mtp(pt3 mt){ mt.x *= x, mt.y *= y, mt.z *= z; return mt; }	//乗算
// 長さ正規化
pt3 pt3::norm(pt3 errPt, double lim)
{
	double len = pyth3(*this);
	return (len <= lim) ? errPt : this->mtp(1 / len);
}
// 内積
double pt3::dot(pt3 a, pt3 b)
{
	return a.x * b.x + a.y * b.y + a.z * b.z;
}
// クロス積
pt3 pt3::cross(pt3 a, pt3 b)
{
	return pt3
	(
		a.y * b.z - a.z * b.y,
		a.z * b.x - a.x * b.z,
		a.x * b.y - a.y * b.x
	);
}
pt3::pt3() :x(0), y(0), z(0) {}
pt3::pt3(double x, double y, double z)
{
	this->x = x; this->y = y; this->z = z;
}

void face3::cSet(uint8_t r, uint8_t g, uint8_t b){ col3 tCol = { r, g, b }; col = tCol; }//色設定
void face3::fSet(pt3* p1, pt3* p2, pt3* p3){ pts[0] = p1; pts[1] = p2; pts[2] = p3; }//頂点一括設定



/// ▼4D関係 =======================
void poly::pSet(pt4* pt, uint32_t len){ for(uint32_t i=0;i<len;i++) pts[i] = pt[i]; }	//オブジェクトのpt設定
void poly::pSet(uint32_t num, double tx, double ty, double tz, double tw){		//オブジェクトのpt設定
	pt4 pt = pt4(tw, tx, ty, tz);
	pts[num] = pt;
}

void object4d::cSet(uint8_t r, uint8_t g, uint8_t b){//一括色設定
	col3 tCol = { r, g, b };
	for(uint32_t i=0;i<faceLen;i++) faces[i].col = tCol;
}
void pt4::asg(double nw, double nx, double ny, double nz){ w = nw, x = nx, y = ny, z = nz; } //代入4
void pt4::asg2(pt4 xyz){ w = xyz.w, x = xyz.x, y = xyz.y, z = xyz.z; }	 //代入4
pt4 pt4::pls(pt4 pts){ pts.w+=w, pts.x+=x, pts.y+=y, pts.z+=z; return pts; }
pt4 pt4::pls(double pW, double pX, double pY, double pZ){ pt4 pts = pt4(pW+w,pX+x,pY+y,pZ+z); return pts; }
pt4 pt4::mns(pt4 pts){ pts.w = w-pts.w, pts.x = x-pts.x, pts.y = y-pts.y, pts.z = z-pts.z; return pts; }
pt4 pt4::mtp(double mt){ pt4 pt; pt.w = w*mt, pt.x = x*mt, pt.y = y*mt, pt.z = z*mt; return pt; }	//乗算
pt4 pt4::mtp(pt4 mt){ mt.w *= w, mt.x *= x, mt.y *= y, mt.z *= z; return mt; }	//乗算
pt6 pt6::pls(double Pxy, double Pyz, double Pxz, double Pwx, double Pwy, double Pwz)
{
	pt6 pts = {Pxy+xy, Pyz+yz, Pxz+xz, Pwx+wx, Pwy+wy, Pwz+wz}; return pts;
}
void pt6::asg(double nxy, double nyz, double nxz, double nwx, double nwy, double nwz)
{ xy=nxy, yz=nyz, xz=nxz, wx=nwx, wy=nwy, wz=nwz; } //代入6
void pt6::asg2(pt6 xyz){ xy=xyz.xy, yz=xyz.yz, xz=xyz.xz, wx=xyz.wx, wy=xyz.wy, wz=xyz.wz; }	 //代入6
pt6 pt6::pls(pt6 pts){ pts.wx+=wx, pts.wy+=wy, pts.wz+=wz, pts.xy+=xy, pts.yz+=yz, pts.xz+=xz; return pts; }

void face4::fSet(uint32_t p1, uint32_t p2, uint32_t p3){ pts[0] = p1; pts[1] = p2; pts[2] = p3; }//頂点一括設定
void poly::lSet(uint32_t* ls1, uint32_t* ls2, uint32_t len){ for(uint32_t i=0;i<len;i++) ls1[i] = ls2[i]; }//頂点一括設定
void object4d::fSet(uint32_t* pt, uint32_t Len)
{
	uint32_t pCnt = 0;
	for(uint32_t i=0;i<Len;i++){
		for(uint32_t j=0;j<3;j++) faces[i].pts[j] = pt[pCnt+1+j];
		if(pt[pCnt]){ ++i; ++Len; for(uint32_t j=0;j<3;j++) faces[i].pts[j] = pt[pCnt+1+(2+j)%4]; }
		pCnt += 4+pt[pCnt];
	}
}
pt3 pt4::xyz(){
	return pt3(x,y,z);
}
// 長さ正規化
pt4 pt4::norm(pt4 errOpt, double lim)
{
	double len = pyth4(*this);
	return (len <= lim) ? errOpt : this->mtp(1 / len);
}
void pt4::asgPt3(pt3 pts3){ x = pts3.x, y = pts3.y, z = pts3.z; }
pt4::pt4()
{ 
	w = 0; x = 0; y = 0; z = 0; 
}
pt4::pt4(double w, double x, double y, double z)
{
	this->w = w; this->x = x; this->y = y; this->z = z;
}
pt4::pt4(double w, pt3 xyz)
{
	this->w = w; this->x = xyz.x; this->y = xyz.y, this->z = xyz.z;
}

void engine3d::adjW(pt4* pt2, pt4* pt1)
{
	if((*pt2).w-(*pt1).w==0.0) (*pt2).w += pow(0.1, 100);	///とりあえす
	int t=0;pols[0].objs[4].faces[0];++t;--t;
	if(abs(((*pt2).x-(*pt1).x)/((*pt2).w-(*pt1).w))>pow(10, ADJ_VAL+1))
		(*pt2).w += ((*pt2).x-(*pt1).x) * pow(0.1, ADJ_VAL);	///とりあえす2
	if(abs(((*pt2).y-(*pt1).y)/((*pt2).w-(*pt1).w))>pow(10, ADJ_VAL+1))
		(*pt2).w += ((*pt2).y-(*pt1).y) * pow(0.1, ADJ_VAL);	///とりあえす2
	if(abs(((*pt2).z-(*pt1).z)/((*pt2).w-(*pt1).w))>pow(10, ADJ_VAL+1))
		(*pt2).w += ((*pt2).z-(*pt1).z) * pow(0.1, ADJ_VAL);	///とりあえす2
}
/// ▼on4D関係 ===================


pt4 object3d::tudeToEuc(pt3 locT){	// [緯,経,深]座標を[XYZ]W座標に変換
	double sinZ = sin(locT.z);
	double sinY = sin(locT.y);

	pt4 vecT = pt4( cos(locT.z),		//W
				sinZ * sinY * sin(locT.x),//X
				sinZ * sinY * cos(locT.x),//Y
				sinZ * cos(locT.y) );//Z

	return vecT;
}
pt3 object3d::eucToTude(pt4 vecT){	// [XYZ]W座標を[緯,経,深]座標に変換
	pt3 locT;
	locT.x = atan2(vecT.x, vecT.y);			
	locT.y = atan2(pyth2(vecT.x,vecT.y), vecT.z);	
	locT.z = atan2(pyth3(vecT.x,vecT.y,vecT.z), vecT.w);

	return locT;
}

// Euc距離から双曲距離に変換 (原点-任意点間)
double object3d::ClcHypbFromEuc(double dst)
{
	//double dstPh = dst * dst;
	//return acosh(1 + ((2 * dstPh) / (1 - dstPh)));
	return atanh(dst);
}

// 双曲距離からEuc距離に変換 (原点-任意点間)
double object3d::ClcEucFromHypb(double dst)
{
	//double dstSrc = cosh(dst);
	//return sqrt((dstSrc - 1) / (1 + dstSrc));
	return tanh(dst);
}

// 移動用ベクトル作成 S3
void object3d::mkLspX_S3(pt4 vec) 
{
	pt4 locE = tudeToEuc(loc);		// 位置
	pt4 vecT = tudeToEuc(vec.xyz());		// 速度
	all_tudeRst(&vecT, loc, 0);

	double vecRt = vecT.w;
	if (abs(vecRt) > 0.999999999)
	{
		vecT = pt4(0,0,0,1);
		vecRt = 0;
	}
	all_tudeRst(&vecT, loc, 1);

	pt4 vecTm = vecT.mns( locE.mtp(vecRt) );
	pt4 lspXE = vecTm.mtp(1.0 / pyth4(vecTm)).mtp( owner->SIN_1 )
		.pls( locE.mtp(owner->COS_1) );

	lspX.asgPt3(eucToTude(lspXE));
	lspX.w = vec.w;
}

// 移動用ベクトル作成 H3
void object3d::mkLspX_H3(pt4 vec)
{
	lspX = vec;
}

//void object3d::clcStd(double* rotStd)
//{
//	pt4 std1 = tudeToEuc(std[0]);
//	pt4 std2 = tudeToEuc(std[1]);
//
//	clcStd(std1, std2, rotStd);
//}

void object3d::clcStd(pt3 std1, pt3 std2, double* rotStd) {///-- 基準方向1,2,3の特定

	pt3 tVec = std2;
	rotStd[0] = atan2(std1.x, std1.y);					//-方向1の特定
	rotStd[1] = atan2(pyth2(std1.x, std1.y), std1.z);	//-方向2の特定
	///-- 3特定の際、基準2cpyの基準方向1,2を0に
	tudeRst(&tVec.x, &tVec.y, rotStd[0], 0);//-- X-Y 回転 (基準2cpy
	tudeRst(&tVec.y, &tVec.z, rotStd[1], 0);//-- Y-Z 回転 (基準2cpy
	rotStd[2] = atan2(tVec.x, tVec.y);	//-方向3の特定
}

void object3d::clcStd(pt4 std1, pt4 std2, double* rotStd){///-- 基準方向1,2,3の特定 (上と同じ。pt4である必要はなかった)

	pt4 tVec = std2;
	rotStd[0] = atan2(std1.x, std1.y);					//-方向1の特定
	rotStd[1] = atan2(pyth2(std1.x, std1.y), std1.z);	//-方向2の特定
	///-- 3特定の際、基準2cpyの基準方向1,2を0に
	tudeRst(&tVec.x, &tVec.y, rotStd[0], 0);//-- X-Y 回転 (基準2cpy
	tudeRst(&tVec.y, &tVec.z, rotStd[1], 0);//-- Y-Z 回転 (基準2cpy
	rotStd[2] = atan2(tVec.x, tVec.y);	//-方向3の特定
}
