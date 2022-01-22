#pragma once

#include <stdint.h>
#include <string>
#include "constants.h"
#include "geometry.h"



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
	pt3i area;

	int drawFace(uint32_t* bmp);
	void pSet(pt3*, uint32_t);

	static pt4 tudeToEuc(pt3);			// S3 [��,�o,�[]���W��[XYZW]���W�ɕϊ�
	static pt3 eucToTude(pt4);			// S3 [XYZW]���W��[��,�o,�[]���W�ɕϊ�
	static double ClcHypbFromEuc(double);	// H3 �o�ȋ������Z�o
	static double ClcEucFromHypb(double);	// H3 �o�ȋɍ��W��EUC[X,Y,Z]���W�ɕϊ�
	void mkLspX_S3(pt4);		// S3�p
	void mkLspX_H3(pt4);	// H3�p
	bool SetLocRelativeS3(object3d* trgObj, pt3 nLoc, double dst);	// S3
	bool SetLocRelativeH3(object3d* trgObj, pt3 nLoc, double dst);	// H3
	bool SetLocS3(pt3 nLoc);	
	bool SetLocH3(pt3 nLoc);	// H3
	bool SetRotRelative(pt3 nRot);	// H3
	bool SetRotRelativeS3(pt3 nRot);	// S3
	bool SetScale(double);
	double GetScale();
	bool SetVelocity(double);
	double GetVelocity();
	bool SetRsp(pt3);
	pt3 GetRsp();
	static void RotVecs(pt3* vec1, pt3* vec2, double rot, double len = 1);
	static void RotVecs4(pt4* vec1, pt4* vec2, double rot, double len = 1);

	void clcStd(pt3, pt3, double*);
	void clcStd(pt4, pt4, double*);
	//void clcStd(double*);
	void markInitS3(double);			// �O�Տ�����
	void markInitH3(double);			// �O�Տ�����
	void cnvForce();	// ���x�x�N�g������ϊ�

	char draw;	//-- 0:�|�C���g, 1:���C��, 2:�T�[�t�F�X. 
	bool used;
	void objInitS3(int);
	void objInitH3(int);
	void init_stdS3(bool);
	void init_stdH3(bool);
	void OptimStd();
	object3d ReflectionH3(pt3, pt3);	// ���s�ړ� (H3)
	object3d ReflectionH3(pt4, pt4);	// ���s�ړ� (H3)
	void ParallelMove(pt3, bool);
	void DealH3OohObj(bool);

	
	static pt3 Klein2PoinCoord(pt3 tmpPt);
	static pt3 Poin2KleinCoord(pt3 tmpPt);
	static pt4 object3d::MapFromFlat2Sphere(pt3);
	static pt3 object3d::MapFromSphere2Flat(pt4);
	static pt3 object3d::Klein2HalfSpace(pt3);
	static pt3 object3d::HalfSpace2Klein(pt3);
	void object3d::Klein2HalfSpace(const object3d*, pt3* = nullptr);
	object3d object3d::HalfSpace2Klein(object3d* = nullptr);
	bool TrackObjDirection(object3d*);
	bool TrackObjDirectionHead(object3d*);
	double GetDistance(object3d*);
	void SetMesh(int idx);

	bool copyFlg;	// �錾�A�����A�߂�l����̍쐬
	object3d();
	object3d(engine3d*);
	object3d(const object3d&);
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


