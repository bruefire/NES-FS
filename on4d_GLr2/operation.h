// ユーザによる入力(キー, ジョイパッドetc)を保持する
#pragma once

#include "point.h"
#include "VRDeviceOperation.h"
#include "MenuLgc.h"


class Operation
{
public:

	pt3 cmLoc;
	pt3 cmRot;
	bool cmBack;
	int speed;

	MenuLgc::INPUT menuAction;

	VRDeviceOperation vrDev[3];		// 0: HMD, 1: left hand, 2: right hand
	double VREysDst;

	int chgMapStateOld;
	int chgMapState;
	bool inputByKey;	// キーによる入力

	enum class ClickState
	{
		None,
		Right,
		Left
	} clickState;
	pt2i clickCoord;

	int updLocationParam(double x, double y, int opt);
	int updRotationParam(double x, double y, int opt);
	void ClearUnkeepedParam();

	Operation();
};

