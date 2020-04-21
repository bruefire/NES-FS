#pragma once

#include <stdint.h>
#include <string>
#include "constants.h"



struct col3 { uint8_t R, G, B; };

class pt2 {
public:
	double x;
	double y;
	pt2 mtp(double);

	pt2();
	pt2(double, double);
};
class pt2i {
public:
	int x;
	int y;
};
class pt3 {
public:
	double x;
	double y;
	double z;
	void asg(double x, double y, double z);
	void asg2(pt3 xyz);
	pt3 pls(pt3 pts);
	pt3 mns(pt3 pts);
	pt3 mtp(double pts);
	pt3 mtp(pt3 pts);

	pt3();
	pt3(double, double, double);
};
class pt4 {
public:
	double w;
	double x;
	double y;
	double z;
	void asg(double w, double x, double y, double z);
	void asg2(pt4 wxyz);
	pt4 pls(pt4 pts);
	pt4 pls(double, double, double, double);
	pt4 mns(pt4 pts);
	pt4 mtp(double pts);
	pt4 mtp(pt4 pts);
	pt3 xyz();
	void asgPt3(pt3);

	static double dot(pt4, pt4);		// 内積
	static pt4 cross(pt4, pt4, pt4);	// 外積(クロス積)

	pt4();
	pt4(double, double, double, double);
	pt4(double, pt3);
};
class pt6 {
public:
	double xy, yz, xz, wx, wy, wz;
	void asg(double, double, double, double, double, double);
	void asg2(pt6 wxyz);
	pt6 pls(pt6 pts);
	pt6 pls(double, double, double, double, double, double);
};

class Rect
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

	mesh3d();
	~mesh3d();
};

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

	static pt4 tudeToEuc(pt3);			//if(on4D) [緯,経,深]座標を[XYZW]座標に変換
	static pt3 eucToTude(pt4);			//if(on4D) [XYZW]座標を[緯,経,深]座標に変換
	void mkLspX(pt4);

	void clcStd(pt4, pt4, double*);
	void markInit(double);			// 軌跡初期化
	void cnvForce();	// 速度ベクトルから変換

	char draw;	//-- 0:ポイント, 1:ライン, 2:サーフェス. 
	bool used;
	void objInit(mesh3d*);
	void init_std(bool);
	object3d();
	object3d(engine3d*);
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

class plyr {
public:
	double hp;	//-- 体力
	int ep;	//-- エナジーポイント
	double sp;	//-- スペシャルパワー
	int score;	//-- スコア
	plyr();
};

class engine3d {
public:

	// 三角関数多用対策
	const double COS_1;
	const double SIN_1;
	const double SIN_1R;

	enum CLS_TYPE
	{
		LOGIC,
		GRAPHIC
	};
	CLS_TYPE clsType;
	bool disposeFlg;

	// 時間制御系
	long long adjTime[2];
	double adjSpd;	//-- VFRの場合の(加)速度補正係数
	int fps;		//-- 1ループの時間
	bool vfrFlg;	//-- VFRであるか
	int GRAVITY;
	const int stdRefSpan;	// std再計算スパン
	int loop;
	int STD_PMSEC;
	int MAX_PMSEC;
	int MIN_PMSEC;

	///--- オブジェクト関係
	//====最大obj数====
	const int BWH_QTY;	// 軸
	const int PLR_QTY;	// プレイヤー
	const int ENR_QTY;	// 隕石()
	const int OBJ_QTY;
	const int EFE_QTY;	// エフェクト(飾り/相互関係なし)
	const int ATK_QTY;	// ﾌﾟﾚｲﾔと敵の攻撃
	const int ALL_QTY;

	int meshLen;	// メッシュの数
	char** meshNames;

	mesh3d* meshs;
	object3d* objs;		//3dオブジェクト群
	poly* pols;			//4dオブジェクト群
	uint32_t polCnt;	//ポリの数
	uint32_t pol_pLen;	//全てのポリ頂点の総数

	int objCnt;			//オブジェクトの数
	empObj* eObjs;		//虚オブジェクト群
	uint32_t eObjCnt;	//虚オブジェクトの数
	double radius;		//if(ON4D)超球半径
	double radius_min;
	// OBJ_LEN_MAX 0.499*PIE*RADIUS_MIN
	empObj cmData;	//if(on4D)地図で使用

	// Map
	int mapMeshLen;
	mesh3d* mapMesh[2];	// マップ (ユークリッド)
	enum class MapDirection 
	{
		 FRONT,
		 TOP
	} mapDir;
	enum class MapMode
	{
		SINGLE,
		DUAL
	} mapMode;

	object3d sun;		//if(on4D)太陽
	object3d markObj;	//軌跡s
	mesh3d markMesh;	//軌跡s

	plyr player;	//--プレイヤー

	double SPEED_MAX;

	// 操作関係
	bool cmBack;
	int speed1;
	bool obMove;
	int PLR_No;
	double cm_loc[3];
	double preCm_rotX;
	double preCm_rotY;
	double preCm_rotZ;
	int chgMapStateOld;
	int chgMapState;
	bool inputByKey;	// キーによる入力
	bool useJoyPad;

	bool mvObjFlg;		// obj位置変更フラグ
	struct MvObjParam	// obj位置変更パラメータ
	{
		pt3 loc;
		pt3 rot;
	}
	mvObjParam;

	// メソッド //
	int init_on4();					// 初期化
	int simulateS3();
	void UpdFloatObjs();			// 射撃オブジェクト更新
	void UpdPlayerObjs(double*);	// プレイヤーオブジェクト更新
	void ClcRelaivePos(double*);	// 相対位置計算
	int physics();
	int randLoc(int);
	pt3 randLoc2(int);
	void all_cnvForce();	// 速度ベクトルから変換(存在する全て)
	int setObjPos();		// オブジェクトの位置・回転を変更
	virtual int allocMesh();
	void shoot();
	void InitInputParams();	// 入力値初期化

	// 不要？
	void adjW(pt4* pt2, pt4* pt1);

	engine3d();
	~engine3d();

	// 基本3メソッド
	int init();
	int update();
	int dispose();

	// 操作関係
	int inPutMouseMv(double x, double y, int opt); // 受け取る値は正規化済のもの
	int inPutWheel(double val, int opt);
	enum IK
	{
		No_1,
		No_2,
		No_3,
		No_4,
		No_5,
		SPACE
	};
	int inPutKey(int key, int opt);

	int updLocationParam(double x, double y, int opt);
	int updRotationParam(double x, double y, int opt);
	void ClearLocRotParam();

};

