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

	static double dot(pt4, pt4);		// ����
	static pt4 cross(pt4, pt4, pt4);	// �O��(�N���X��)

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
	pt3** pts;	//���_�Q(pt3)���܂񂾔z��ւ̃|�C���^
	pt2** txs;	//ø������_�Q(pt2)���܂񂾔z��ւ̃|�C���^
	col3 col;	//�J���[

	bool alfaJD;		//�����g�p���邩
	short alfaPt[16];		//�X�N���[����̃|�C���g�Q
	double alfaDst[8];		//���|�C���g�܂ł̋���

	void fSet(pt3*, pt3*, pt3*); //���_�ꊇ�ݒ�
	void cSet(uint8_t, uint8_t, uint8_t); //�F�ݒ�
};
class face4 {
public:
	uint32_t pts[3];	//���_�Q(pt3)���܂񂾔z��ւ̃C���f�b�N�X
	col3 col;	//�J���[
	void fSet(uint32_t, uint32_t, uint32_t); //���_�ꊇ�ݒ�
	void cSet(uint8_t, uint8_t, uint8_t); //�F�ݒ�
};

class mesh3d {	//-- ���b�V��
public:
	// ���W�^�C�v
	enum COOR {
		POLAR,		// �ɍ��W
		Cartesian	// �������W
	};
	COOR coorType;

	pt3* pts;	//���_�Q(pt3)���܂񂾔z��ւ̃|�C���^
	face3* faces;	//�ʌQ(face3)���܂񂾔z��ւ̃|�C���^
	pt2i* lines;	//���Q(pt2)�C���f�b�N�X
	uint32_t pLen;		//���_�̐�
	uint32_t faceLen;	//�ʂ̐�
	uint32_t lLen;		//���̐�

	pt2* txs;			//ø������_���܂񂾔z��
	int texWH[2];		//�e�N�X�`��Whith, Height
	bool texJD;			//�e�N�X�`�����g�p���邩
	uint32_t txLen;			//ø������_�̐�
	uint8_t* bmp;
	int texNo;			//�e�N�X�`���ԍ�(�o�b�t�@�ԍ�)
	std::string texName;	//�e�N�X�`����
	int extObjFlg;		// �O��obj�t�@�C���̎w��
	int bufLen;

	// OpenGL�֌W
	float* pts2;	// ���_�Q, �����ĐF���
	int* ptsGl;		// ���_�Q�C���f�b�N�X(GL�`��p)
	pt3* pts0;
	std::string objNameS;
	//uint32_t vtxBuf;	// ���_�o�b�t�@�ԍ�
	//uint32_t texBuf;	// �e�N�X�`���o�b�t�@�ԍ�

	int meshInit(std::string, uint32_t, int);
	int meshInitB(int, uint32_t);
	int setNull();

	mesh3d();
	~mesh3d();
};

class engine3d;
class object3d {	//=====�I�u�W�F�N�g
public:

	engine3d* owner;
	mesh3d* mesh;	//-- �g�p���郁�b�V��
	bool texJD;
	bool polObj;	//3D�X�N���[���ł��邩
	char* pCll;		//(4D�Փ˂̂�) �Փ�

	double alfa;	//�����x
	double scale;	//�X�P�[��
	static int PAST_QTY;
	pt3* past;	//�O��

	pt3 ctr;		//���S(�d�S)
	pt3 loc;		//�ʒu
	pt3 rot;		//�X��
	pt3 lsp;		//�������x
	pt3 rsp;		//��]���x
	pt4 lspX;		//if(on4D) �ʒu  (�ܓx,�o�x,�[�x,���x)
	pt3 std[2];		//if(on4D) ��x�N�g�� (���_���]�̊�ƂȂ�)
	pt3 locr;		//if(on4D) �����ڂ̈ʒu (�n�}�Ŏg�p)
	pt3 objStd;		//if(on4D) �����ڂ�std?
	pt4 fc;			//��
	pt4 fc2;		//�ǉ��̗�
	double ssp;		// �X�P�[���̕ύX���x
	int stdRefCnt;	// std�Čv�Z�܂ł̃J�E���^

	int drawFace(uint32_t* bmp);
	void pSet(pt3*, uint32_t);

	static pt4 tudeToEuc(pt3);			//if(on4D) [��,�o,�[]���W��[XYZW]���W�ɕϊ�
	static pt3 eucToTude(pt4);			//if(on4D) [XYZW]���W��[��,�o,�[]���W�ɕϊ�
	void mkLspX(pt4);

	void clcStd(pt4, pt4, double*);
	void markInit(double);			// �O�Տ�����
	void cnvForce();	// ���x�x�N�g������ϊ�

	char draw;	//-- 0:�|�C���g, 1:���C��, 2:�T�[�t�F�X. 
	bool used;
	void objInit(mesh3d*);
	void init_std(bool);
	object3d();
	object3d(engine3d*);
	~object3d();
};
class object4d {	//=====�I�u�W�F�N�g4
public:
	pt4* pts;	//���_�Q(pt4)���܂񂾔z��ւ̃|�C���^
	face4* faces;	//�ʌQ(face3)���܂񂾔z�� (�ʂɕ\��������ꍇ)
	face4** facesLP;//�ʌQ(face3)���܂񂾔z��ւ̃|�C���^ (�Ȃ��ꍇ)
	uint32_t faceLen;	//�ʂ̐�

	int drawFace(uint32_t* bmp);
	void pSet(pt4*, uint32_t);
	void fSet(uint32_t*, uint32_t);
	void cSet(uint8_t, uint8_t, uint8_t); //�F�ݒ�
};
class poly {	//=====�E��
public:
	pt4* pts;	//���_�Q(pt3)���܂񂾔z��
	uint32_t* lins;	//�ӌQ(pt4)���܂񂾔z��ւ̃C���f�b�N�X
	face4* faces;	//���_�Q(pt3)���܂񂾔z�� (�ʂɕ\�����Ȃ��ꍇ)
	object4d* objs;	//���̌Q(face3)���܂񂾔z��
	int pLen;	//���_�̐�
	int linLen;	//�ӂ̐�
	uint32_t objLen;	//���̂̐�

	double alfa;	//�����x
	double scale;	//�X�P�[��

	pt4 ctr;		//���S(�d�S)
	pt4 loc;		//�ʒu
	pt6 rot;		//�X��
	pt4 lsp;		//�������x
	pt6 rsp;		//��]���x

	int drawFace(uint32_t* bmp);
	void pSet(pt4*, uint32_t);
	void pSet(uint32_t, double, double, double, double);
	void lSet(uint32_t*, uint32_t*, uint32_t len);
	void polyInit(int mode);
	void polyInit(int mode, bool phys);
	poly();
	~poly();
};

class empObj {	//======���̂̂Ȃ��I�u�W�F�N�g
public:
	pt3 loc;
	pt3 rot;
};
class empObj4 {	//======���̂̂Ȃ��I�u�W�F�N�g
public:
	pt4 loc;
	pt6 rot;
};

class plyr {
public:
	double hp;	//-- �̗�
	int ep;	//-- �G�i�W�[�|�C���g
	double sp;	//-- �X�y�V�����p���[
	int score;	//-- �X�R�A
	plyr();
};

class engine3d {
public:

	// �O�p�֐����p�΍�
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

	// ���Ԑ���n
	long long adjTime[2];
	double adjSpd;	//-- VFR�̏ꍇ��(��)���x�␳�W��
	int fps;		//-- 1���[�v�̎���
	bool vfrFlg;	//-- VFR�ł��邩
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

	plyr player;	//--�v���C���[

	double SPEED_MAX;

	// ����֌W
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
	bool inputByKey;	// �L�[�ɂ�����
	bool useJoyPad;

	bool mvObjFlg;		// obj�ʒu�ύX�t���O
	struct MvObjParam	// obj�ʒu�ύX�p�����[�^
	{
		pt3 loc;
		pt3 rot;
	}
	mvObjParam;

	// ���\�b�h //
	int init_on4();					// ������
	int simulateS3();
	void UpdFloatObjs();			// �ˌ��I�u�W�F�N�g�X�V
	void UpdPlayerObjs(double*);	// �v���C���[�I�u�W�F�N�g�X�V
	void ClcRelaivePos(double*);	// ���Έʒu�v�Z
	int physics();
	int randLoc(int);
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

	int updLocationParam(double x, double y, int opt);
	int updRotationParam(double x, double y, int opt);
	void ClearLocRotParam();

};

