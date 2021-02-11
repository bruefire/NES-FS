#pragma once

#include <stdint.h>
#include <string>
#include "constants.h"
#include "point.h"


struct col3 { uint8_t R, G, B; };

class RectAngle
{
public:
	pt2 sz;
	double l;
	double t;
	double r;
	double b;
};


class face3 {
public:
	pt3** pts;	//頂点群(pt3)を含んだ配列へのポインタ
	pt2** txs;	//ﾃｸｽﾁｬ頂点群(pt2)を含んだ配列へのポインタ
	col3 col;	//カラー

	bool alfaJD;		//αを使用するか
	short alfaPt[16];		//スクリーン上のポイント群
	double alfaDst[8];		//αポイントまでの距離

	void fSet(pt3*, pt3*, pt3*); //頂点一括設定
	void cSet(uint8_t, uint8_t, uint8_t); //色設定
};
class face4 {
public:
	uint32_t pts[3];	//頂点群(pt3)を含んだ配列へのインデックス
	col3 col;	//カラー
	void fSet(uint32_t, uint32_t, uint32_t); //頂点一括設定
	void cSet(uint8_t, uint8_t, uint8_t); //色設定
};

class mesh3d {	//-- メッシュ
public:
	// 座標タイプ
	enum COOR {
		POLAR,		// 極座標
		Cartesian	// 直交座標
	};
	COOR coorType;

	pt3* pts;	//頂点群(pt3)を含んだ配列へのポインタ
	face3* faces;	//面群(face3)を含んだ配列へのポインタ
	pt2i* lines;	//線群(pt2)インデックス
	uint32_t pLen;		//頂点の数
	uint32_t faceLen;	//面の数
	uint32_t lLen;		//線の数

	pt2* txs;			//ﾃｸｽﾁｬ頂点を含んだ配列
	int texWH[2];		//テクスチャWhith, Height
	bool texJD;			//テクスチャを使用するか
	uint32_t txLen;			//ﾃｸｽﾁｬ頂点の数
	uint8_t* bmp;
	int texNo;			//テクスチャ番号(バッファ番号)
	std::string texName;	//テクスチャ名
	int extObjFlg;		// 外部objファイルの指定
	int bufLen;

	// OpenGL関係
	float* pts2;	// 頂点群, 加えて色情報
	int* ptsGl;		// 頂点群インデックス(GL描画用)
	pt3* pts0;
	std::string objNameS;
	//uint32_t vtxBuf;	// 頂点バッファ番号
	//uint32_t texBuf;	// テクスチャバッファ番号

	int meshInit(std::string, uint32_t, int);
	int meshInitB(int, uint32_t);
	int setNull();
	void Init();

	mesh3d();
	~mesh3d();
};
