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
	: BWH_QTY(1)	// ��
	, PLR_QTY(10)	// �v���C���[
	, ENR_QTY(100)	// 覐�()
	, OBJ_QTY(BWH_QTY + PLR_QTY + ENR_QTY)
	, EFE_QTY(1)	// �G�t�F�N�g(����/���݊֌W�Ȃ�)
	, ATK_QTY(1)	// ��ڲԂƓG�̍U��
	, ALL_QTY(OBJ_QTY + EFE_QTY + ATK_QTY)

	, obMove(true)
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
{
	adjTime[0] = adjTime[1] = 0;
	

	// �u�Ԉړ�
	mvObjFlg = false;
	mvObjParam.loc = pt3(0, 0, 0);
	mvObjParam.rot = pt3(0, 0, 0);

}
engine3d::~engine3d() {
}


//**** ��{3���\�b�h ****//

// ������
int engine3d::init()
{
	int rtnVal = 1;

	//-- �v���C���[�󂫔ԍ���ݒ� (�����_��)
	PLR_No = BWH_QTY + (time(NULL) % PLR_QTY);	

	// ���b�V����`
	meshLen = 21;
	meshNames = new char*[meshLen];
	meshNames[0] = "wLines", 
	meshNames[1] = "player", 
	meshNames[2] = "earth", 
	meshNames[3] = "wLines3",
	meshNames[4] = "cube8", 
	meshNames[5] = "horse", 
	meshNames[6] = "sun", 
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

	///-- ���`���b�V��
	allocMesh();
	try
	{
		for(int i=0;i<meshLen;i++)//-- ���b�V���쐬
		{	
			if(!meshs[i].meshInit( meshNames[i], i+1, 0))
				rtnVal = 0;
		}
	}
	// dispose���ďI���Ɍ�����
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

	///-- �����I�u�W�F�N�g�쐬
	objs = new object3d[ OBJ_QTY ];
	for (int i = 0; i < OBJ_QTY; i++) 
	{
		objs[i].owner = this;
	}
	objCnt = OBJ_QTY;
		

	init_on4();

	// �}�b�v
	mapMeshLen = 2;
	mapMesh[0] = meshs + 15;
	mapMesh[1] = meshs + 16;


	return rtnVal;
}


// �t���[���P�ʂ̍X�V
int engine3d::update()
{


	////-----------------time
	// �v���I��������ۑ�
	long long newTime = duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();
	adjTime[1] = (newTime < adjTime[0]) ? 0 : newTime - adjTime[0];

	if (adjTime[1] < fps)
		this_thread::sleep_for(milliseconds(fps - adjTime[1]));
	
	adjTime[0] = duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();

	////-----------------time

	loop = (loop+1) % INT_MAX;//std::numeric_limits<int>::max(); <- �����ꂱ�����ɕύX

	// S3���Z����
	if(GRAVITY && obMove) physics();
	simulateS3();

	ope.inputByKey = false;

	return 1;
};


// �㏈��
int engine3d::dispose()
{
	if(disposeFlg)
		return 0;

	// ���
	delete[] objs;
	delete[] meshs;
	delete[] meshNames;

	disposeFlg = true;

	return 1;
}



//**** ���̑� ****//

int engine3d::allocMesh()
{
	meshs = new mesh3d[meshLen];

	return 1;
}




///=============== >>>���[�v(���C��)<<< ==================
int engine3d::simulateS3()
{

	///==============

	// �ˌ��I�u�W�F�N�g�X�V
	UpdFloatObjs();

	/// �p�����[�^�w��(by�_�C�A���O��)�v���C���[�p���ύX
	setObjPos();

	// �v���C���[�I�u�W�F�N�g�X�V
	double cmrStd[3] = {};
	UpdPlayerObjs(cmrStd);


	///===============^^^^^^^^^~~~~~~~~~~-----------
		//-- �O�Ռ`�� --//
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

	// ���Έʒu�v�Z
	ClcRelaivePos(cmrStd);
	//===============^^^^^^^^^~~~~~~~~~~-----------

	if(!useJoyPad)
		ope.ClearLocRotParam();

	ope.chgMapStateOld = ope.chgMapState;

	return 0;
}


// �ˌ��I�u�W�F�N�g�X�V
void engine3d::UpdFloatObjs()
{
	for (int h = -1; h < objCnt; h++) {//-----------�I�u�W�F�N�g���Ƃ̑��x�X�V----------//
		object3d* curObj;
		if (h == -1) curObj = &sun; else curObj = objs + h;
		if (!curObj->used) continue;


		if (obMove) {		//-- �ʒu,���x,�X���̃f�[�^�X�V
			if (0.000000001 < abs(curObj->lspX.w))
			{
				///-----------> �ʒu,���x,��ʒu�̍X�V <-------------
				// .lspX �����o���g�p
				//-- ���a1.0�Ƃ��Ē����ʏ��UC���W���`
				pt4 locE = curObj->tudeToEuc(curObj->loc);		// �ʒu
				pt4 vecT = curObj->tudeToEuc(curObj->lspX.xyz());	// ���x
				pt4 std1 = curObj->tudeToEuc(curObj->std[0]);		// ��ʒu1
				pt4 std2 = curObj->tudeToEuc(curObj->std[1]);		// ��ʒu2
				if (GRAVITY || curObj->stdRefCnt == stdRefSpan)
				{
					if (GRAVITY) {
						pt4 fVec = curObj->fc2.mns(locE).mtp(adjSpd);	// �ǉ��̗�
						vecT = locE.pls(curObj->fc.mns(locE).pls(fVec));	// ����
					}
					///-- �ܓx,�o�x,�[�x��0��
					all_tudeRst(&vecT, curObj->loc, 0);	// ���x
					all_tudeRst(&std1, curObj->loc, 0); // �1
					all_tudeRst(&std2, curObj->loc, 0); // �2

					///-- �i�s����1,2�̓���
					double rotOn[2] = {};
					rotOn[0] = atan2(vecT.x, vecT.y);
					rotOn[1] = atan2(pyth2(vecT.x, vecT.y), vecT.z);

					///-- ����1,2��0�� (�1,2�̂�)
					tudeRst(&std1.x, &std1.y, rotOn[0], 0);//-- X-Y ��]
					tudeRst(&std2.x, &std2.y, rotOn[0], 0);
					tudeRst(&std1.y, &std1.z, rotOn[1], 0);//-- Y-Z ��]
					tudeRst(&std2.y, &std2.z, rotOn[1], 0);

					double lspW = (GRAVITY) ? pyth3(vecT.xyz()) : curObj->lspX.w;
					///-- �i�s�������ړ�(Z-W��])���A����1,2�֍��킹��(Y-Z, X-Y��])
					pt4 loc1(1, 0, 0, 0), vec1(1, 0, 0, 0);
					double fwVal = lspW * adjSpd / radius;
					tudeRst(&loc1.z, &loc1.w, fwVal, 1);//-- �ʒu
					tudeRst(&std1.z, &std1.w, fwVal, 1);//-- �1
					tudeRst(&std2.z, &std2.w, fwVal, 1);//-- �2
					if (GRAVITY) {
						vec1.z = lspW;
						tudeRst(&vec1.z, &vec1.w, fwVal, 1);
					}
					else
						tudeRst(&vec1.z, &vec1.w, fwVal + 1.0, 1);//-- ���x
					///-- Y-Z ��]
					tudeRst(&loc1.y, &loc1.z, rotOn[1], 1);//-- �1
					tudeRst(&vec1.y, &vec1.z, rotOn[1], 1);//-- �1
					tudeRst(&std1.y, &std1.z, rotOn[1], 1);//-- �1
					tudeRst(&std2.y, &std2.z, rotOn[1], 1);//-- �2
					///-- X-Y ��]
					tudeRst(&loc1.x, &loc1.y, rotOn[0], 1);//-- �1
					tudeRst(&vec1.x, &vec1.y, rotOn[0], 1);//-- �1
					tudeRst(&std1.x, &std1.y, rotOn[0], 1);//-- �1
					tudeRst(&std2.x, &std2.y, rotOn[0], 1);//-- �2

					///-- �ܓx,�o�x,�[�x��߂�
					all_tudeRst(&loc1, curObj->loc, 1);	//-- �ʒu
					all_tudeRst(&vec1, curObj->loc, 1);	//-- ���x
					all_tudeRst(&std1, curObj->loc, 1);	//-- �1
					all_tudeRst(&std2, curObj->loc, 1);	//-- �2

					///-- �ʒu,���x,��ʒu���㏑�� (end
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

				// ��������(?)�� �A���g�p��std�덷�~��
				else
				{
					//-- �܂�4�̊�{�����Z�o (���K����)
					pt4 normN = locE;
					pt4 normO = normN.mtp(COS_1);

					// ���x�x�N�g���̕����x�N�g��
					pt4 lspXN = vecT.mns(normO).mtp(SIN_1R);

					//o-���K���W��1,2�x�N�g���̕��� (normN�����͖���)
					double std1xL = pt4::dot(lspXN, std1);
					pt4 std1x = lspXN.mtp(std1xL);
					pt4 std1e = std1.mns(normO).mns(std1x);

					double std2xL = pt4::dot(lspXN, std2);
					pt4 std2x = lspXN.mtp(std2xL);
					pt4 std2e = std2.mns(normO).mns(std2x);

					// �X�V
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

					///-- �ʒu,���x,��ʒu���㏑�� (end
					curObj->loc = curObj->eucToTude(locE);
					curObj->std[0] = curObj->eucToTude(std1);
					curObj->std[1] = curObj->eucToTude(std2);
					curObj->lspX.asgPt3(curObj->eucToTude(vecT));
				}

				// std�Čv�Z�J�E���^
				++curObj->stdRefCnt;
			}
			///----------- �X���̍X�V -------------
			curObj->rot = curObj->rot.pls(curObj->rsp.mtp(adjSpd));

			///-- �O�Ղ̍X�V --
			if (h < BWH_QTY || BWH_QTY + PLR_QTY <= h) {
				for (int i = object3d::PAST_QTY - 1; 0 < i; i--)
					curObj->past[i] = curObj->past[i - 1];

				pt4 loc4 = curObj->tudeToEuc(curObj->loc);
				pt3 tmpt;
				tmpt.x = atan2(loc4.x, loc4.z);		//--����1
				tmpt.y = atan2(pyth2(loc4.x, loc4.z), loc4.y);	//--����2
				tmpt.z = curObj->loc.z * radius;	//--����(����)
				curObj->past[0] = tmpt;
			}
		}
	}
}

// �v���C���[�I�u�W�F�N�g�X�V
void engine3d::UpdPlayerObjs(double* cmrStd) 
{
	objs[PLR_No].rot.asg(-ope.cmRot.x, ope.cmRot.y, ope.cmRot.z);

	//----
	object3d* curObj = &objs[PLR_No];
	pt4 locE = curObj->tudeToEuc(curObj->loc);
	pt4 std1 = curObj->tudeToEuc(curObj->std[0]);		// ��ʒu1
	pt4 std2 = curObj->tudeToEuc(curObj->std[1]);		// ��ʒu2
	///-------- �X���̍X�V ----------
	//-- �܂�4�̊�{�����Z�o (���K����)
	pt4 normN = locE;
	pt4 normO = normN.mtp(COS_1);
	pt4 std1N = std1.mns(normO).mtp(SIN_1R);
	pt4 std2N = std2.mns(normO).mtp(SIN_1R);

	// �N���X�ς��Z�o (������1.0)
	pt4 sideN = pt4::cross(normN, std1N, std2N);

	// �����Œ��]
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

	// �����ړ���]1
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

	// �����ړ���]2
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

	///-------- �ʒu,��ʒu�̍X�V ----------
	pt4 cmLc = pt4(0, ope.cmLoc.y, ope.cmLoc.z, ope.cmLoc.x).mtp(3);
	cmLc.w = pyth3(cmLc.x, cmLc.y, cmLc.z);

	std1 = std1N.mtp(SIN_1).pls(normO);
	std2 = std2N.mtp(SIN_1).pls(normO);

	if (cmLc.w > 0.0000000001)
	{
		// �ړ������x�N�g��(���K��)
		pt4 lspXN = std1N.mtp(cmLc.z)
			.pls(std2N.mtp(cmLc.y))
			.pls(sideN.mtp(cmLc.x))
			.mtp(1 / cmLc.w);

		//o-���K���W��1,2�x�N�g���̕��� (normN�����͖���)
		double std1xL = pt4::dot(lspXN, std1);
		pt4 std1x = lspXN.mtp(std1xL);
		pt4 std1e = std1.mns(normO).mns(std1x);

		double std2xL = pt4::dot(lspXN, std2);
		pt4 std2x = lspXN.mtp(std2xL);
		pt4 std2e = std2.mns(normO).mns(std2x);

		// �X�V
		tmpRt = pt2(0, 1);
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

	///-- �ʒu,��ʒu���㏑�� (end
	curObj->loc = curObj->eucToTude(locE);
	curObj->std[0] = curObj->eucToTude(std1);
	curObj->std[1] = curObj->eucToTude(std2);

	///----------- �X�V��̃J������̑��� -------------
	///-- �ܓx,�o�x,�[�x��0��
	all_tudeRst(&std1, curObj->loc, 0);// �1
	all_tudeRst(&std2, curObj->loc, 0);// �2
	///-- �����1,2,3�̓���
	curObj->clcStd(std1, std2, cmrStd);

	// std�C��
	if (curObj->stdRefCnt == stdRefSpan)
	{
		pt4 ntd1 = pt4(COS_1, 0, 0, SIN_1);
		pt4 ntd2 = pt4(COS_1, 0, SIN_1, 0);

		///-- ����1,2��0�� (�1,2�̂�)
		tudeRst(&ntd1.x, &ntd1.y, cmrStd[2], 1);//-- X-Y ��]
		tudeRst(&ntd2.x, &ntd2.y, cmrStd[2], 1);
		tudeRst(&ntd1.y, &ntd1.z, cmrStd[1], 1);//-- Y-Z ��]
		tudeRst(&ntd2.y, &ntd2.z, cmrStd[1], 1);
		tudeRst(&ntd1.x, &ntd1.y, cmrStd[0], 1);//-- X-Y ��]
		tudeRst(&ntd2.x, &ntd2.y, cmrStd[0], 1);

		all_tudeRst(&ntd1, curObj->loc, 1);
		all_tudeRst(&ntd2, curObj->loc, 1);

		curObj->std[0] = object3d::eucToTude(ntd1);
		curObj->std[1] = object3d::eucToTude(ntd2);
		curObj->stdRefCnt = 0;
	}
	// std�Čv�Z�J�E���^
	++curObj->stdRefCnt;
	
	double mvCoe = (1000.0 / fps) / radius;
	double rtCoe = (1000.0 / fps);
	cmData.loc.asg(cmLc.z * mvCoe, cmLc.x * mvCoe, cmLc.y * mvCoe);
	cmData.rot.asg(objs[PLR_No].rot.x * rtCoe, objs[PLR_No].rot.y * rtCoe, objs[PLR_No].rot.z * rtCoe);

}

// ���Έʒu�v�Z
void engine3d::ClcRelaivePos(double* cmrStd)
{
	for (int h = -2; h < objCnt; h++) {	//==============�I�u�W�F�N�g���Ƃ̏���==============//

		object3d* curObj;
		if (h == -2) curObj = &markObj; else if (h == -1) curObj = &sun; else curObj = objs + h;
		if (!curObj->used) continue;
		pt3 drawLoc;

		///---- �����g�̉�]�̔��f ----///

		///---- ���J�����ʒu���l�����Ĉړ� ----///
		///-- ���x��tude > euc�ϊ�, tude0
		pt4 locT = curObj->tudeToEuc(curObj->loc);	//-- �ʒu
		pt4 std1 = curObj->tudeToEuc(curObj->std[0]);	//-- �1
		pt4 std2 = curObj->tudeToEuc(curObj->std[1]);	//-- �2
		all_tudeRst(&locT, objs[PLR_No].loc, 0);	//-- �ʒu
		all_tudeRst(&std1, objs[PLR_No].loc, 0);	//-- �1
		all_tudeRst(&std2, objs[PLR_No].loc, 0);	//-- �2
		///-- �J�����(��])���l�����Ĉړ�	(��~��]
		tudeRst(&locT.x, &locT.y, cmrStd[0], 0);//-- X-Y ��] (-����1
		tudeRst(&std1.x, &std1.y, cmrStd[0], 0);
		tudeRst(&std2.x, &std2.y, cmrStd[0], 0);
		tudeRst(&locT.y, &locT.z, cmrStd[1], 0);//-- Y-Z ��] (-����2
		tudeRst(&std1.y, &std1.z, cmrStd[1], 0);
		tudeRst(&std2.y, &std2.z, cmrStd[1], 0);
		tudeRst(&locT.x, &locT.y, cmrStd[2], 0);//-- X-Y ��] (-����3
		tudeRst(&std1.x, &std1.y, cmrStd[2], 0);
		tudeRst(&std2.x, &std2.y, cmrStd[2], 0);
		//-- ����J�����Ȃ�
		if (ope.cmBack) {
			tudeRst(&locT.x, &locT.z, PIE, 1);//-- X-Y ��] (-����3
			tudeRst(&std1.x, &std1.z, PIE, 1);
			tudeRst(&std2.x, &std2.z, PIE, 1);
		}

		///------ �I�u�W�F�N�g�̌������̍��W���v�Z ------
		pt3 locR = curObj->eucToTude(locT);
		curObj->locr = locR;	//-- �n�}�Ŏg�p���邽�ߊi�[
		///-- �ܓx,�o�x,�[�x��0��
		all_tudeRst(&std1, locR, 0);// �1
		all_tudeRst(&std2, locR, 0);// �2
		///-- �I�u�W�F�N�g�����1,2,3�̓���
		double objStd[3];
		curObj->clcStd(std1, std2, objStd);

		curObj->objStd.asg(objStd[0], objStd[1], objStd[2]);	//-- ���p �ʓ|�Ȃ̂�
	}
}



///============== ������ =================
int engine3d::init_on4() {
	srand(time(NULL));

	// �c�e
	player.ep = ENR_QTY;

	//-- �t���[�����[�g
	fps = STD_PMSEC;
	adjSpd = 1.0;

	cmData.loc.asg(0, 0, 0);
	cmData.rot.asg(0, 0, 0);

	//-- ���z
	sun.objInit(meshs + 6);
	sun.loc = pt3(	//-- �ʒu
		0.0, 0.0, 0.0
	);
	sun.mkLspX(	//-- ���x 
		pt4(SPEED_MAX, 0.8, 1.6, 1.0)
	);
	sun.draw = 2;
	sun.used = false;	//-- �L����
	sun.init_std(0);//-- �W���̐ݒ�

	//-- �O�ՃI�u�W�F�N�g
	markMesh.meshInitB((OBJ_QTY) * (object3d::PAST_QTY - 1), (meshLen - 1) + 2);
	markObj.objInit(meshs + 0);
	markObj.loc = pt3(	//-- �ʒu
		0,
		0,
		0
	);
	markObj.init_std(0);	//-- �p�x�W���̐ݒ�
	markObj.mesh = &markMesh;
	markObj.draw = 1;
	markObj.used = false;	//-- �L����


	//-- �ŏ��ɑS�I�u�W�F�N�g��p�ӂ��Ă��܂�
	int h = 0;
	///-- ���
	for (h; h < BWH_QTY; h++) {
		objs[h].objInit(meshs + 0);
		objs[h].draw = 0;
		objs[h].scale = 0.5 * radius;
		objs[h].init_std(0);	//-- std
		objs[h].used = true;	//-- �L����
	}
	///-- �v���C���[ ----------
	for (h; h < BWH_QTY + PLR_QTY; h++) {
		objs[h].objInit(meshs + 1);
		objs[h].loc = pt3(	//-- �ʒu
			(((double)rand() / RAND_MAX) * 2 - 1) * PIE,
			((double)rand() / RAND_MAX) * PIE,
			((double)rand() / RAND_MAX) * PIE
		);
		objs[h].mkLspX( pt4(	//-- ���x
			0,
			(((double)rand() / RAND_MAX) * 2 - 1) * PIE,
			((double)rand() / RAND_MAX) * PIE,
			((double)rand() / RAND_MAX) * PIE
		));
		objs[h].rsp.asg(0, 0, 1 DEG);
		objs[h].init_std(0);	//-- std
		objs[h].used = true;	//-- �L����
		objs[h].draw = 2;
	}
	///-- ���o�I�u�W�F�N�g ------
	for (h; h < BWH_QTY + PLR_QTY + ENR_QTY; h++) {
		objs[h].objInit(meshs + 4);
		objs[h].loc = pt3(	//-- �ʒu
			(((double)rand() / RAND_MAX) * 2 - 1) * PIE,
			((double)rand() / RAND_MAX) * PIE,
			((double)rand() / RAND_MAX) * PIE
		);
		objs[h].mkLspX( pt4(	//-- ���x
			SPEED_MAX,
			(((double)rand() / RAND_MAX) * 2 - 1) * PIE,
			((double)rand() / RAND_MAX) * PIE,
			((double)rand() / RAND_MAX) * PIE
		));
		objs[h].rot.asg(0, 1 DEG, 0);
		objs[h].rsp.asg(1 DEG, 0, 0);
		objs[h].init_std(1);	//-- std
		objs[h].used = false;	//-- �L����
		objs[h].draw = 2;
	}

	///-- ����
	for (h = 0; h < OBJ_QTY; h++) {
		objs[h].markInit(radius);
	}

	//--
	return 0;
}

int engine3d::randLoc(int mode) {	//-- obj�̃����_���z�u

	///-- ���o�I�u�W�F�N�g ------
	for (int h = BWH_QTY + PLR_QTY; h < OBJ_QTY; h++) {

		if (mode) {//-- �W���^����
			objs[h].loc = pt3(	//-- �ʒu
				(((double)rand() / RAND_MAX) * 2 - 1) * PIE,
				((double)rand() / RAND_MAX) * PIE,
				((double)rand() / RAND_MAX) * PIE
			);
			objs[h].mkLspX( pt4(	//-- ���x
				SPEED_MAX,
				(((double)rand() / RAND_MAX) * 2 - 1) * PIE,
				((double)rand() / RAND_MAX) * PIE,
				((double)rand() / RAND_MAX) * PIE
			));

		}
		else {
			objs[h].loc = randLoc2(0);
			pt3 tmp = randLoc2(0);
			objs[h].mkLspX( pt4(SPEED_MAX, tmp.x, tmp.y, tmp.z) );
		}
		objs[h].fc.asg(0, 0, 0, 0);	//�K�v?
		objs[h].init_std(1);	//-- std
		objs[h].used = true;	//-- �L����
		objs[h].markInit(radius);
	}

	return 0;
}

pt3 engine3d::randLoc2(int cnt) {	//-- obj�̃����_���z�u

	///-- ���o�I�u�W�F�N�g ------

		//-- ��l����
	pt4 rnd(
		(((double)rand() / RAND_MAX) * 2 - 1),
		(((double)rand() / RAND_MAX) * 2 - 1),
		(((double)rand() / RAND_MAX) * 2 - 1),
		(((double)rand() / RAND_MAX) * 2 - 1)
	);
	if (
		(pyth4(rnd) > 1.0) && cnt != 100
		) {
		return randLoc2(cnt + 1);

	}
	else {
		return object3d::eucToTude(rnd);
	}

}

// �I�u�W�F�N�g�̎p���𓮓I�ɕύX
int engine3d::setObjPos()
{
	if (!mvObjFlg)
		return 0;

	object3d* curObj = objs + PLR_No;


	//-- ���a1.0�Ƃ��Ē����ʏ��UC���W���`
	pt4 locE = curObj->tudeToEuc(curObj->loc);	// �ʒu
	pt4 vecT = curObj->tudeToEuc(mvObjParam.loc);	// ���x
	pt4 std2 = curObj->tudeToEuc(curObj->std[1]);		// ��ʒu2
	pt4 ntd1 = curObj->tudeToEuc(mvObjParam.rot);		// ���̊1

	// 
	double lspW = asin(pyth4(vecT.mns(locE)) * 0.5) * 2;

	///-- �ܓx,�o�x,�[�x��0��
	all_tudeRst(&vecT, curObj->loc, 0);	// ���x
	all_tudeRst(&std2, curObj->loc, 0); // �2
	all_tudeRst(&ntd1, curObj->loc, 0); // ����2

	///-- �i�s����1,2�̓���
	double rotOn[2] = {};
	rotOn[0] = atan2(vecT.x, vecT.y);
	rotOn[1] = atan2(pyth2(vecT.x, vecT.y), vecT.z);

	///-- ����1,2��0�� (�2,����1�̂�)
	//-- X-Y ��]
	tudeRst(&std2.x, &std2.y, rotOn[0], 0);
	tudeRst(&ntd1.x, &ntd1.y, rotOn[0], 0);
	//-- Y-Z ��]
	tudeRst(&std2.y, &std2.z, rotOn[1], 0);
	tudeRst(&ntd1.y, &ntd1.z, rotOn[1], 0);

	// ���[�J���̃��[�J�� //
	tudeRst(&ntd1.z, &ntd1.w, lspW, 0);
	//pt4 std1 = ntd1;
	///-- �����1,2,3�̓���
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


	///-- �i�s�������ړ�(Z-W��])���A����1,2�֍��킹��(Y-Z, X-Y��])
	pt4 loc1 = pt4(1, 0, 0, 0), vec1 = pt4(1, 0, 0, 0);
	tudeRst(&loc1.z, &loc1.w, lspW, 1);//-- �ʒu
	tudeRst(&std1.z, &std1.w, lspW, 1);//-- �1
	tudeRst(&std2.z, &std2.w, lspW, 1);//-- �2

	tudeRst(&vec1.z, &vec1.w, lspW * 2, 1);//-- ���x
	///-- Y-Z ��]
	tudeRst(&loc1.y, &loc1.z, rotOn[1], 1);//-- �1
	tudeRst(&vec1.y, &vec1.z, rotOn[1], 1);//-- �1
	tudeRst(&std1.y, &std1.z, rotOn[1], 1);//-- �1
	tudeRst(&std2.y, &std2.z, rotOn[1], 1);//-- �2
	///-- X-Y ��]
	tudeRst(&loc1.x, &loc1.y, rotOn[0], 1);//-- �1
	tudeRst(&vec1.x, &vec1.y, rotOn[0], 1);//-- �1
	tudeRst(&std1.x, &std1.y, rotOn[0], 1);//-- �1
	tudeRst(&std2.x, &std2.y, rotOn[0], 1);//-- �2

	///-- �ܓx,�o�x,�[�x��߂�
	all_tudeRst(&loc1, curObj->loc, 1);	//-- �ʒu
	all_tudeRst(&vec1, curObj->loc, 1);	//-- ���x
	all_tudeRst(&std1, curObj->loc, 1);	//-- �1
	all_tudeRst(&std2, curObj->loc, 1);	//-- �2

	///-- �ʒu,���x,��ʒu���㏑�� (end
	curObj->loc = curObj->eucToTude(loc1);
	curObj->std[0] = curObj->eucToTude(std1);
	curObj->std[1] = curObj->eucToTude(std2);



	mvObjFlg = false;
	return 1;
}



// ���́E�˗�
int engine3d::physics() {


	for (int h = 0; h < objCnt; h++) {	//-- �����̕\��
		if (!objs[h].used) continue;


		for (int i = h + 1; i < objCnt; i++) {	//-- �����̕\��
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

					///-- ����
					///-- ���̊Ԃ̋���
					pt4 e1Loc = eObj1->tudeToEuc(eObj1->loc);
					pt4 e2Loc = eObj1->tudeToEuc(eObj2->loc);

					double unit = radius / 30;///���ׂĂ̒�������(�r�����a�ύX�����Ȃ�)
					double scale = pyth4(e1Loc.mns(e2Loc));
					double dstR = asin((scale / 2)) * 2;
					double force = 0.0001 * 1.0 / (dstR * dstR * unit * unit) * (1.0 / sin((PIE - dstR) * 0.5));

					double minR = 0.1 * 30 / radius;
					if (GRAVITY == 2)	//-- �˗͂̏ꍇ
						force = -2 * force;
					else if (dstR < minR)	//-- �͂̍ő�l
						force = 0.0001 * 1.0 / (minR * minR * unit * unit) * (1.0 / sin((PIE - dstR) * 0.5));
					//force += -4*unit*(minR-dstR)*(1.0/sin((PIE-dstR)*0.5));

					if (scale > pow(0.1, 300)) {//if���̓o�O���
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
			if (0 < player.ep)
			{
				objs[i].used = true;

				objs[i].loc = objs[PLR_No].loc;
				objs[i].std[0] = objs[PLR_No].std[0];
				objs[i].std[1] = objs[PLR_No].std[1];
				objs[i].lspX = pt4(0, objs[PLR_No].std[0]);
				objs[i].lspX.w = SPEED_MAX;

				objs[i].cnvForce();
				objs[i].markInit(radius);

				player.ep += -1;
				break;
			}
	}
}



//== ����֌W ==//


int engine3d::inPutMouseMv(double x, double y, int opt)
{
	// �W���C�p�b�h�ɂ��㏑�𖳌���
	ope.inputByKey = true;
	// ���̓f�[�^������
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
	// �W���C�p�b�h�ɂ��㏑�𖳌���
	ope.inputByKey = true;
	// ���̓f�[�^������
	InitInputParams();

	if (!opt) {
		ope.cmLoc.x = val * powi(3.0, ope.speed);	//�O��
	}
	else {
		ope.cmRot.z = val * 0.1;	//-- �˂����]
	}

	return 1;
}



// �L�[���͏������\�b�h
int engine3d::inPutKey(int key, int opt)
{
	// �W���C�p�b�h�ɂ��㏑�𖳌���
	ope.inputByKey = true;
	// ���̓f�[�^������
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
	}

	return 1;
}

// ���͒l������
void engine3d::InitInputParams()
{
	ope.cmLoc = pt3(0.0, 0.0, 0.0);
	ope.cmRot = pt3(0.0, 0.0, 0.0);


	ope.cmBack = false;
}

// ����
double pt4::dot(pt4 a, pt4 b)
{
	return a.x * b.x + a.y * b.y + a.z * b.z + a.w * b.w;
}

// �N���X��
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

