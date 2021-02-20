#pragma once
#include "point.h"
#include "VR_Manager.h"


class engine3dWinOVR;
class NesEsVR : public VR_Manager
{
public:
	NesEsVR(engine3dWinOVR*);

private:
	engine3dWinOVR* owner;
	double cmrStd[3];

	void initGlScnene() override;
	void updateSceneLgc() override;
	void updateGlScene() override;
	void disposeGlScene() override;
	void SendPose(pt3 loc, pt3 std[2], double eyeDst) override;
};

