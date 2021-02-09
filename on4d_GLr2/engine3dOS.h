#pragma once
#include <Windows.h>
#include "engine3dGL.h"


class engine3dOS : public engine3dGL
{
public:
	HWND(*initWndFunc)(HINSTANCE hCurInst, int nCmdShow);
	bool (*wndProcFunc)(MSG*);

	virtual int init(void** params, HWND(HINSTANCE hCurInst, int nCmdShow), bool(MSG*)) = 0;
	virtual int start() = 0;
	virtual int update() = 0;
	virtual int dispose() = 0;
};
