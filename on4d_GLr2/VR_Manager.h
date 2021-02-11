#pragma once
#include <Windows.h>

#if defined(_WIN32)
#include <dxgi.h> // for GetDefaultAdapterLuid
#pragma comment(lib, "dxgi.lib")
#endif
#include "../OVR/Common/Win32_GLAppUtil.h"
#include "OculusTextureBuffer.h"


class VR_Manager
{
private:
	HINSTANCE hInst;

public:
	VR_Manager();
	~VR_Manager();

	void Init(HINSTANCE hInst);
	void Start();
	void Dispose();
	static bool MainLoop(bool retryCreate);
	static ovrGraphicsLuid VR_Manager::GetDefaultAdapterLuid();
	static int Compare(const ovrGraphicsLuid& lhs, const ovrGraphicsLuid& rhs);
};
