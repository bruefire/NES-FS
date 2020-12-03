#pragma once

#include <stdint.h>
#include <string>
#include "constants.h"
#include "geometry.h"



class engine3d;
class object3d {	//=====オブジェクト
public:

	engine3d* owner;
	mesh3d* mesh;	//-- 使用するメッシュ
	bool texJD;
	bool polObj;	//3Dスクリーンであるか
	char* pCll;		//(4D衝突のみ) 衝突

	double alfa;	//透明度
	double scale;	//スケール
	static int PAST_QTY;
	pt3* past;	//軌跡

	pt3 ctr;		//中心(重心)
	pt3 loc;		//位置
	pt3 rot;		//傾き
	pt3 lsp;		//直線速度
	pt3 rsp;		//回転速度
	pt4 lspX;		//if(on4D) 位置  (緯度,経度,深度,速度)
	pt3 std[2];		//if(on4D) 基準ベクトル (頂点や回転の基準となる)
	pt3 locr;		//if(on4D) 見た目の位置 (地図で使用)
	pt3 objStd;		//if(on4D) 見た目のstd?
	pt4 fc;			//力
	pt4 fc2;		//追加の力
	double ssp;		// スケールの変更速度
	int stdRefCnt;	// std再計算までのカウンタ

	int drawFace(uint32_t* bmp);
	void pSet(pt3*, uint32_t);

	static pt4 tudeToEuc(pt3);			// S3 [緯,経,深]座標を[XYZW]座標に変換
	static pt3 eucToTude(pt4);			// S3 [XYZW]座標を[緯,経,深]座標に変換
	static double ClcHypbFromEuc(double);	// H3 双曲距離を算出
	static double ClcEucFromHypb(double);	// H3 双曲極座標をEUC[X,Y,Z]座標に変換
	void mkLspX_S3(pt4);		// S3用
	void mkLspX_H3(pt4);	// H3用

	void clcStd(pt3, pt3, double*);
	void clcStd(pt4, pt4, double*);
	//void clcStd(double*);
	void markInitS3(double);			// 軌跡初期化
	void markInitH3(double);			// 軌跡初期化
	void cnvForce();	// 速度ベクトルから変換

	char draw;	//-- 0:ポイント, 1:ライン, 2:サーフェス. 
	bool used;
	void objInitS3(mesh3d*);
	void objInitH3(mesh3d*);
	void init_stdS3(bool);
	void init_stdH3(bool);
	void OptimStd();
	object3d ReflectionH3(pt3, pt3);	// 平行移動 (H3)
	void ParallelMove(pt3, bool);
	void DealH3OohObj(bool);

	bool copyFlg;	// 宣言、引数、戻り値からの作成
	object3d();
	object3d(engine3d*);
	object3d(const object3d&);
	~object3d();
};

class object4d {	//=====オブジェクト4
public:
	pt4* pts;	//頂点群(pt4)を含んだ配列へのポインタ
	face4* faces;	//面群(face3)を含んだ配列 (面に表裏がある場合)
	face4** facesLP;//面群(face3)を含んだ配列へのポインタ (ない場合)
	uint32_t faceLen;	//面の数

	int drawFace(uint32_t* bmp);
	void pSet(pt4*, uint32_t);
	void fSet(uint32_t*, uint32_t);
	void cSet(uint8_t, uint8_t, uint8_t); //色設定
};

class poly {	//=====胞体
public:
	pt4* pts;	//頂点群(pt3)を含んだ配列
	uint32_t* lins;	//辺群(pt4)を含んだ配列へのインデックス
	face4* faces;	//頂点群(pt3)を含んだ配列 (面に表裏がない場合)
	object4d* objs;	//立体群(face3)を含んだ配列
	int pLen;	//頂点の数
	int linLen;	//辺の数
	uint32_t objLen;	//立体の数

	double alfa;	//透明度
	double scale;	//スケール

	pt4 ctr;		//中心(重心)
	pt4 loc;		//位置
	pt6 rot;		//傾き
	pt4 lsp;		//直線速度
	pt6 rsp;		//回転速度

	int drawFace(uint32_t* bmp);
	void pSet(pt4*, uint32_t);
	void pSet(uint32_t, double, double, double, double);
	void lSet(uint32_t*, uint32_t*, uint32_t len);
	void polyInit(int mode);
	void polyInit(int mode, bool phys);
	poly();
	~poly();
};

class empObj {	//======実体のないオブジェクト
public:
	pt3 loc;
	pt3 rot;
};
class empObj4 {	//======実体のないオブジェクト
public:
	pt4 loc;
	pt6 rot;
};


