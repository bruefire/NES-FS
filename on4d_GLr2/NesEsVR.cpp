#include <math.h>
#include "NesEsVR.h"
#include "engine3dWinOVR.h"
#include "functions.h"
using WorldGeo = engine3d::WorldGeo;



NesEsVR::NesEsVR(engine3dWinOVR* owner)
{
	this->owner = owner;
}

void NesEsVR::initGlScnene(double w, double h, double fovL, double fovR, double fovT, double fovD)
{
	owner->GL_InitScene();
	glCullFace(GL_FRONT);
	owner->WIDTH = w;
	owner->HEIGHT = h;
	owner->CR_RANGE_X = atan(fovL) * 2 / PIE * 180;
	owner->CR_RANGE_R = atan(fovR) * 2 / PIE * 180;
	owner->CR_RANGE_Y = atan(fovT) * 2 / PIE * 180;
	owner->CR_RANGE_D = atan(fovD) * 2 / PIE * 180;

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

	// VR HMD, handの移動
	owner->UpdPlayerObjsS3(cmrStd);	// cmrStd is unused.
	owner->UpdVRObjectsS3(cmrStd);

	owner->PrepareInParamForNext();

}

void NesEsVR::updateGlScene(Eye eye)
{
	double rMax = fmax(owner->CR_RANGE_X, owner->CR_RANGE_R);
	double rMin = fmin(owner->CR_RANGE_X, owner->CR_RANGE_R);

	if (eye == Eye::Left)
	{
		owner->CR_RANGE_X = rMax;
		owner->CR_RANGE_R = rMin;
	}
	else
	{
		owner->CR_RANGE_X = rMin;
		owner->CR_RANGE_R = rMax;
	}

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

void NesEsVR::SendPose(VRDeviceOperation ope[3], double eyeDst)
{
	for (int i = 0; i < 3; i++)
		owner->ope.vrDev[i] = ope[i];

	owner->ope.VREysDst = eyeDst;
}

/// <summary>
/// process input data from device(controller).
/// </summary>
void NesEsVR::DeviceInputProcedure(ovrInputState state, ovrControllerType type)
{
	// 既にキー操作済なら退出
	if (owner->ope.inputByKey)
		return;

	switch (type)
	{
	case ovrControllerType::ovrControllerType_LTouch:
	{
		pt3 gLoc = pt3(state.IndexTriggerRaw[0],	// 前後
			state.ThumbstickRaw[0].x,	// 左右
			state.ThumbstickRaw[0].y)	// 上下
			.mtp(0.32767 * owner->adjSpd * powi(3.0, owner->ope.speed));	// 係数

		// 位置
		double lLim = 0.05 * owner->adjSpd * powi(3.0, owner->ope.speed);
		owner->ope.cmLoc.x = (lLim < abs(gLoc.x)) ? gLoc.x : 0.0;
		owner->ope.cmLoc.y = (lLim < abs(gLoc.y)) ? gLoc.y : 0.0;
		owner->ope.cmLoc.z = (lLim < abs(gLoc.z)) ? gLoc.z : 0.0;

		//// マップ
		//owner->ope.chgMapState = JoyInfoEx.dwPOV;
		//if (owner->ope.chgMapState != owner->ope.chgMapStateOld)
		//{
		//	if (owner->ope.chgMapState == JOY_POVFORWARD)
		//	{
		//		owner->inPutKey(owner->IK::No_4, NULL); break;
		//	}
		//	else if (owner->ope.chgMapState == JOY_POVLEFT)
		//	{
		//		owner->inPutKey(owner->IK::No_5, NULL); break;
		//	}
		//}

		//// 後ろを見る
		//owner->ope.cmBack = (JoyInfoEx.dwButtons & JOY_BUTTON10) ? true : false;
		break;
	}

	case ovrControllerType::ovrControllerType_RTouch:
	{
		pt2 gRot = pt2(
			-state.ThumbstickRaw[1].x,	//左右
			state.ThumbstickRaw[1].y)	//上下
			.mtp(0.32767 * 0.3);

		// 回転
		owner->ope.cmRot.x = (0.02 < abs(gRot.x)) ? gRot.x * owner->adjSpd : 0.0;
		owner->ope.cmRot.y = (0.02 < abs(gRot.y)) ? gRot.y * owner->adjSpd : 0.0;
		//if (JoyInfoEx.dwButtons == 16)
		//	owner->ope.cmRot.z = owner->adjSpd * -0.1;
		//else if (JoyInfoEx.dwButtons == 32)
		//	owner->ope.cmRot.z = owner->adjSpd * 0.1;
		//else
		//	owner->ope.cmRot.z = 0.0;

		break;
	}
	}
}
