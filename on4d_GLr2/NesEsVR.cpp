#include "NesEsVR.h"
#include "engine3dWinOVR.h"
using WorldGeo = engine3d::WorldGeo;



NesEsVR::NesEsVR(engine3dWinOVR* owner)
{
	this->owner = owner;
}

void NesEsVR::initGlScnene(double w, double h, double fovX)
{
	owner->GL_InitScene();
	glCullFace(GL_FRONT);
	owner->WIDTH = w;
	owner->HEIGHT = h;
	owner->CR_RANGE_X = atan(fovX) * 2 / PIE * 180;
	owner->CR_RANGE_Y = owner->clcRangeY(owner->CR_RANGE_X);

	//-- メッシュ転送
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

	// VR HMDの移動
	if (!owner->ope.VRStd[0].isZero())
		owner->ClcVRPlayerPosS3(cmrStd);

}

void NesEsVR::updateGlScene()
{
	if (owner->worldGeo == WorldGeo::SPHERICAL)
	{
		// 相対位置計算
		owner->ClcRelaivePosS3(cmrStd);	// 視差未対応
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
