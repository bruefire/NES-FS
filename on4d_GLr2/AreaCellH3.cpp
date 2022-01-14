#include "AreaCellH3.h"



AreaCellH3::AreaCellH3(int x, int y, int z)
	: coord(x, y, z)
{
}

AreaCellH3::AreaCellH3(pt3i co)
	: coord(co.x, co.y, co.z)
{}
