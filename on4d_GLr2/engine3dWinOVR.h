#pragma once
#include "engine3dWin.h"


class engine3dWinOVR : public engine3dWin
{
public:
	int init(
		void** params,
		HWND(*initStdWndFunc)(HINSTANCE hCurInst, int nCmdShow),
		bool (*loopFunc)(MSG*));
	int start();
	int update();
	int dispose();

	// openGL overrides
	int GL_CreateContextPls() override;
	int GL_DeleteContextPls() override;
	int GL_SwapBuffer() override;
};
