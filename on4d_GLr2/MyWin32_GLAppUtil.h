#pragma once
#include <Windows.h>
#include "../OVR/Common/Win32_GLAppUtil.h"

struct MyOGL : public OGL
{
	void InitWindow(HINSTANCE hInst, LPCWSTR title, WNDPROC wndProc);
};
