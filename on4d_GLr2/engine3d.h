#pragma once

#include <stdint.h>
#include <string>
#include "constants.h"
#include "geometry.h"
#include "player.h"
#include "object.h"
#include "operation.h"


class engine3d {
public:

	// �O�p�֐����p�΍�
	const double COS_1;
	const double SIN_1;
	const double SIN_1R;

	
	const double H3_STD_LEN;	// �|�A���J��std�� (EUC��)
	double H3_MAX_RADIUS;		// �|�A���J���L�����a (EUC��)
	double H3_REF_RADIUS;		// �|�A���J�����f���a (EUC��)

	enum CLS_TYPE
	{
		LOGIC,
		GRAPHIC
	};
	CLS_TYPE clsType;
	bool disposeFlg;

	// ���Ԑ���n
	long long adjTime[2];
	double adjSpd;	//-- VFR�̏ꍇ��(��)���x�␳�W��
	int fps;		//-- 1���[�v�̎���
	int GRAVITY;
	const int stdRefSpan;	// std�Čv�Z�X�p��
	int loop;
	int STD_PMSEC;
	int MAX_PMSEC;
	int MIN_PMSEC;

	///--- �I�u�W�F�N�g�֌W
	//====�ő�obj��====
	const int BWH_QTY;	// ��
	const int PLR_QTY;	// �v���C���[
	const int ENR_QTY;	// 覐�()
	const int OBJ_QTY;
	const int EFE_QTY;	// �G�t�F�N�g(����/���݊֌W�Ȃ�)
	const int ATK_QTY;	// ��ڲԂƓG�̍U��
	const int ALL_QTY;

	int meshLen;	// ���b�V���̐�
	char** meshNames;

	mesh3d* meshs;
	object3d* objs;		//3d�I�u�W�F�N�g�Q
	poly* pols;			//4d�I�u�W�F�N�g�Q
	uint32_t polCnt;	//�|���̐�
	uint32_t pol_pLen;	//�S�Ẵ|�����_�̑���

	int objCnt;			//�I�u�W�F�N�g�̐�
	empObj* eObjs;		//���I�u�W�F�N�g�Q
	uint32_t eObjCnt;	//���I�u�W�F�N�g�̐�
	double radius;		//if(ON4D)�������a
	double radius_min;
	// OBJ_LEN_MAX 0.499*PIE*RADIUS_MIN
	empObj cmData;	//if(on4D)�n�}�Ŏg�p

	// Map
	int mapMeshLen;
	mesh3d* mapMesh[2];	// �}�b�v (���[�N���b�h)
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

	object3d sun;		//if(on4D)���z
	object3d markObj;	//�O��s
	mesh3d markMesh;	//�O��s

	Player player;	//--�v���C���[

	double SPEED_MAX;

	// ����֌W
	Operation ope;
	bool obMove;
	int PLR_No;
	bool useJoyPad;

	bool mvObjFlg;		// obj�ʒu�ύX�t���O
	struct MvObjParam	// obj�ʒu�ύX�p�����[�^
	{
		pt3 loc;
		pt3 rot;
	}
	mvObjParam;

	// ���\�b�h //
	void InitWorld();				// ���E������(����)
	int InitS3();					// ���ʐ��E������
	int InitH3();					// �o�Ȑ��E������
	int simulateS3();
	void simulateH3();
	void UpdFloatObjsS3();			// �ˌ��I�u�W�F�N�g�X�V S3
	void UpdFloatObjsH3();			// �ˌ��I�u�W�F�N�g�X�V H3
	void UpdPlayerObjs(double*);	// �v���C���[�I�u�W�F�N�g�X�V
	void ClcRelaivePos(double*);	// ���Έʒu�v�Z
	int physics();

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
	int RandLocS3(RandMode);
	int RandLocH3(RandMode, ObjType);
	pt3 randLoc2(int);
	void all_cnvForce();	// ���x�x�N�g������ϊ�(���݂���S��)
	int setObjPos();		// �I�u�W�F�N�g�̈ʒu�E��]��ύX
	virtual int allocMesh();
	void shoot();
	void InitInputParams();	// ���͒l������

	// �s�v�H
	void adjW(pt4* pt2, pt4* pt1);

	engine3d();
	~engine3d();

	// ��{3���\�b�h
	int init();
	int update();
	int dispose();

	enum class WorldGeo
	{
		SPHERICAL,
		HYPERBOLIC
	}
	worldGeo;
	void UpdateS3();		// S3�X�V
	void UpdateH3();		// H3�X�V
	void DisableShootObjs();

	// ����֌W
	int inPutMouseMv(double x, double y, int opt); // �󂯎��l�͐��K���ς̂���
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

};

