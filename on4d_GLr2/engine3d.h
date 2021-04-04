#pragma once

#include <stdint.h>
#include <string>
#include "constants.h"
#include "geometry.h"
#include "player.h"
#include "object.h"
#include "operation.h"
#include "MenuLgc.h"


class engine3d {
public:

	// 三角関数多用対策
	const double COS_1;
	const double SIN_1;
	const double SIN_1R;

	
	const double H3_STD_LEN;	// クラインstd長 (EUC長)
	double H3_MAX_RADIUS;		// クライン有効半径 (EUC長)
	double H3_REF_RADIUS;		// クライン鏡映半径 (EUC長)
	bool h3objLoop;

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
	empObj cmData;		//if(on4D)地図で使用
	pt3 cmCo;			// 基objとの相対座標 H3で使用
	int selectedIdx;	// 選択オブジェクト

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

	bool vrFlag;
	object3d vrHand[2];	// hand for VR
	object3d vrMenuObj;
	mesh3d vrMenuMesh;

	Player player;	//--プレイヤー

	double SPEED_MAX;

	// 操作関係
	Operation ope;
	bool obMove;
	int PLR_No;
	bool useJoyPad;
	int viewTrackIdx;	// 観測対象オブジェクト

	bool mvObjFlg;		// obj位置変更フラグ
	struct MvObjParam	// obj位置変更パラメータ
	{
		pt3 loc;
		pt3 rot;
	}
	mvObjParam;

	MenuLgc menuLgc;

	// メソッド //
	void InitWorld();				// 世界初期化(共通)
	int InitS3();					// 球面世界初期化
	int InitH3();					// 双曲世界初期化
	int simulateS3();
	void simulateH3();
	void UpdFloatObjsS3();			// 射撃オブジェクト更新 S3
	void UpdFloatObjsH3();			// 射撃オブジェクト更新 H3
	void UpdPlayerObjsS3(double*);	// プレイヤー更新S3
	void UpdPlayerObjsH3(double*);	// プレイヤー更新H3
	void ClcRelaivePosS3(double*);	// 相対位置計算S3
	void ClcRelaivePosH3(double*);	// 相対位置計算H3
	void ClcVRObjectPosS3(VRDeviceOperation devOpe, object3d* curObj, double* cmrStd);
	void ClcVRObjectPosH3(VRDeviceOperation devOpe, object3d* curObj, bool isHmd);
	void UpdVRObjectsS3(double* cmrSts);
	void UpdVRObjectsH3(double* cmrSts);
	void ClcCoordinate();
	object3d* GetObject(int idx);
	int physics();
	bool CheckSelectedEnable(int idx = -1);
	bool CheckTrackedEnable();
	void SetRadius(double);
	double GetRadius();

	enum class RandMode
	{
		Cluster,
		Uniform
	};
	enum class ObjType
	{
		Player,
		Energy
	};
	void RandLoc(engine3d::RandMode mode, int qty=100);
	int RandLocS3(RandMode, int qty=100);
	void RandLocH3(RandMode, ObjType, int qty=100);
	pt3 randLocUniS3(int);
	pt3 randLocUniH3(double);
	pt3 randVec3(double);
	void all_cnvForce();	// 速度ベクトルから変換(存在する全て)
	int setObjPos();		// オブジェクトの位置・回転を変更
	virtual int allocMesh();
	void shoot();
	void InitInputParams();	// 入力値初期化
	void PrepareInParamForNext();

	// 不要？
	void adjW(pt4* pt2, pt4* pt1);

	engine3d();
	~engine3d();

	// 基本3メソッド
	int init();
	int update();
	int dispose();

	enum class WorldGeo
	{
		SPHERICAL,
		HYPERBOLIC
	}
	worldGeo;
	bool UpdateS3();		// S3更新
	bool UpdateH3();		// H3更新
	void ClearFloatObjs();

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
		SPACE,
		ESCAPE
	};
	int inPutKey(int key, int opt);
	void ChangeBasicObject(int, int, bool = false);
	void ChangeThrowObject(int);

};

