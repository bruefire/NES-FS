#pragma once
#include "VR_Manager.h"


class engine3dWinOVR;
class NesEsVR : public VR_Manager
{
private:
	engine3dWinOVR* owner;

	void initGlScnene() override;
	void updateGlScene() override;
	void disposeGlScene() override;
};

