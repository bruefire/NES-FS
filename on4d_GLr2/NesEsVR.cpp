#include "NesEsVR.h"
#include "engine3dWinOVR.h"
using WorldGeo = engine3d::WorldGeo;



NesEsVR::NesEsVR(engine3dWinOVR* owner)
{
	this->owner = owner;
}

void NesEsVR::initGlScnene()
{
	owner->GL_InitScene();

	//-- ���b�V���]��
	for (int i = 0; i < owner->meshLen; i++)
	{
		owner->MakeCommonVBO(i);
	}
}

void NesEsVR::updateSceneLgc()
{
	cmrStd[0] = cmrStd[1] = cmrStd[2] = 0;

	owner->engine3d::UpdFloatObjsS3();
	owner->PrepareInParamForNext();

	// VR HMD�̈ړ�
	if (!owner->ope.VRStd[0].isZero())
		owner->ClcVRPlayerPosS3(cmrStd);

}

void NesEsVR::updateGlScene()
{
	if (owner->worldGeo == WorldGeo::SPHERICAL)
	{
		// ���Έʒu�v�Z
		owner->ClcRelaivePosS3(cmrStd);	// �������Ή�
		owner->simulateS3GL();
	}
}


void NesEsVR::disposeGlScene()
{
	owner->GL_DisposeScene();
}

void NesEsVR::SendPose(pt3 loc, pt3 std[2], double eyeDst)
{
	owner->ope.VRLoc = loc;
	owner->ope.VRStd[0] = std[0];
	owner->ope.VRStd[1] = std[1];
	owner->ope.VREysDst = eyeDst;
}
