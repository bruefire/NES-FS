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

	bool isLazyLoaded;

	enum class Symmetric	// for object size saving
	{
		None,
		X_Symm,
		Y_Symm,
		Z_Symm,
		XY_Symm,
		YZ_Symm,
		XZ_Symm,
		XYZ_Symm
	}
	symmType;

	int meshInit(std::string, uint32_t, int);
	int meshInitB(int, uint32_t);
	void meshInitC(int);
	int setNull();
	void Init();
	void CartesianToPolar();

	mesh3d();
	~mesh3d();
};
