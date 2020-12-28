// ���[�U�ɂ�����(�L�[, �W���C�p�b�hetc)��ێ�����
#pragma once

#include "point.h"


class Operation
{
public:

	pt3 cmLoc;
	pt3 cmRot;
	bool cmBack;
	int speed;

	int chgMapStateOld;
	int chgMapState;
	bool inputByKey;	// �L�[�ɂ�����

	enum class ClickState
	{
		None,
		Right,
		Left
	} clickState;
	pt2i clickCoord;

	int updLocationParam(double x, double y, int opt);
	int updRotationParam(double x, double y, int opt);
	void ClearLocRotParam();

	Operation();
};

