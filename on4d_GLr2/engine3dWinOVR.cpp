#include "engine3dWinOVR.h"



int engine3dWinOVR::init(void** params, HWND(*initStdWndFunc)(HINSTANCE hCurInst, int nCmdShow), bool(*loopFunc)(MSG*))
{
	HINSTANCE hInst = *((HINSTANCE*)params[0]);
	int nCmdShow = *((int*)params[1]);
	this->hInst = hInst;
	this->nCmdShow = nCmdShow;
	this->initWndFunc = initStdWndFunc;
	this->wndProcFunc = loopFunc;

	// 基底クラスのinit
	engine3d::init();

	// VR, ウィンドウ初期化
	vrm.Init(hInst);
	// pythonインタラクティブ初期化
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
	engine3d::update();

	GL_SwapBuffer();
	if (worldGeo == WorldGeo::SPHERICAL)
	{
		simulateS3GL();
	}
	else if (worldGeo == WorldGeo::HYPERBOLIC)
	{
		SimulateH3GL();
	}

	return 0;
}

int engine3dWinOVR::dispose()
{
	vrm.Dispose();

	return 0;
}

int engine3dWinOVR::GL_CreateContextPls()
{
	// transfer initialization to VR_Manager.

	return 1;
}

int engine3dWinOVR::GL_DeleteContextPls()
{
	// transfer initialization to VR_Manager.

	return 1;
}

int engine3dWinOVR::GL_SwapBuffer()
{
	return 1;
}
