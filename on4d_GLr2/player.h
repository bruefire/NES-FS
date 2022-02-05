#pragma once
#include "point.h"


class object3d;

class Player {
public:
	double hp;		
	int ep;			
	//double sp;		
	//int score;
	object3d* holdedObj;
	pt3 holdedPreLoc;
	pt3i holdedPreArea;

	Player();
};
