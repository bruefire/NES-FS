#include "engine3dWinOVR.h"



int engine3dWinOVR::init(void** params, HWND(*initStdWndFunc)(HINSTANCE hCurInst, int nCmdShow), bool(*loopFunc)(MSG*))
{
	HINSTANCE hInst = *((HINSTANCE*)params[0]);
	int nCmdShow = *((int*)params[1]);
	this->hInst = hInst;
	this->nCmdShow = nCmdShow;
	this->initWndFunc = initStdWndFunc;
	this->wndProcFunc = loopFunc;

	// VR, ウィンドウ初期化
	vrm.Init(hInst);

	pyInter.Init(this);
	timeBeginPeriod(1);

	return 1;
}

int engine3dWinOVR::start()
{
	vrm.Start();

	return 0;
}

int engine3dWinOVR::update()
{
	return 0;
}

int engine3dWinOVR::dispose()
{
	return 0;
}
