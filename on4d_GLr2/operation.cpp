#include "operation.h"


Operation::Operation()
	: OBJ_HOLD_RANGE(1.0)
	, cmBack(false)
	, speed(0)
	, chgMapStateOld(0)
	, chgMapState(0)
	, inputByKey(false)
	, clickState(Operation::ClickState::None)
	, clickCoord(pt2i(-1, -1))
	, VREysDst(0.0)
	, menuAction(MenuLgc::INPUT::NONE)
{
	cmLoc = pt3(0, 0, 0);
	cmRot = pt3(0, 0, 0);
}

int Operation::updRotationParam(double x, double y, int opt)
{
	cmRot.x = x;
	cmRot.y = y;

	return 1;
}


int Operation::updLocationParam(double x, double y, int opt)
{
	cmLoc.y = x;
	cmLoc.z = y;

	return 1;
}

void Operation::ClearUnkeepedParam()
{
	cmLoc = pt3(0, 0, 0);
	cmRot = pt3(0, 0, 0);
	menuAction = MenuLgc::INPUT::NONE;
}

