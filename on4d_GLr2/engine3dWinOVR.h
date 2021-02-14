#pragma once
#include "NesEsVR.h"
#include "engine3dWin.h"


class engine3dWinOVR : public engine3dWin
{
private:
	NesEsVR vrm;

public:
	int init(
		void** params,
		HWND(*initStdWndFunc)(HINSTANCE hCurInst, int nCmdShow),
		bool (*loopFunc)(MSG*)) override;
	int start() override;
	int update() override;
	int dispose() override;

	// openGL overrides
	int GL_CreateContextPls() override;
	int GL_DeleteContextPls() override;
	int GL_SwapBuffer() override;
};
