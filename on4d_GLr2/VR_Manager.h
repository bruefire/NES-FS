#pragma once
#include <Windows.h>

#include "OVR_CAPI_GL.h"
#if defined(_WIN32)
#include <dxgi.h> // for GetDefaultAdapterLuid
#pragma comment(lib, "dxgi.lib")
#endif


class VR_Manager
{
private:
	HINSTANCE hInst;

public:
	virtual void initGlScnene() = 0;
	virtual void updateGlScene() = 0;
	virtual void disposeGlScene() = 0;

	VR_Manager();
	~VR_Manager();

	void Init(HINSTANCE hInst);
	void Start();
	void Dispose();
	static bool MainLoop(bool retryCreate);
	static ovrGraphicsLuid VR_Manager::GetDefaultAdapterLuid();
	static int Compare(const ovrGraphicsLuid& lhs, const ovrGraphicsLuid& rhs);
};
