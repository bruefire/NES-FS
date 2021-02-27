#pragma once
#include "point.h"

struct VRDeviceOperation
{
	pt3 loc;
	pt3 std[2];
	VRDeviceOperation()
	{
		loc = pt3(0, 0, 0);
		std[0] = pt3(0, 0, 0);
		std[1] = pt3(0, 0, 0);
	};
	VRDeviceOperation(pt3 loc, pt3 std1, pt3 std2)
	{
		this->loc = loc;
		std[0] = std1;
		std[1] = std2;
	};
};
