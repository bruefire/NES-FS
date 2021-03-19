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

	struct Ope
	{
		bool buttonA_raw;
		bool buttonB_raw;
		bool buttonX_raw;

		const double StickStateExTH = 0.5;
		const double StickStateNtTH = 0.25;
		enum class StickVState
		{
			Up,
			Neutral,
			Down
		};
		StickVState lStickVState;
		StickVState rStickVState;
		enum class StickHState
		{
			Left,
			Neutral,
			Right
		};
		StickHState lStickHState;
		StickHState rStickHState;

		Ope()
			: buttonA_raw(false)
			, buttonX_raw(false)
			, lStickVState(StickVState::Neutral)
			, rStickVState(StickVState::Neutral)
			, lStickHState(StickHState::Neutral)
			, rStickHState(StickHState::Neutral)
		{}
	} 
	ope;

	void initGlScnene(double w, double h, double fovL, double fovR, double fovT, double fovD) override;
	void updateSceneLgc() override;
	void updateGlScene(Eye) override;
	void disposeGlScene() override;
	void DeviceInputProcedure(ovrInputState, ovrControllerType) override;
	void SendPose(VRDeviceOperation dev[3], double eyeDst) override;
};

