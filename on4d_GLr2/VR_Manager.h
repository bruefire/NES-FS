#pragma once
#include <Windows.h>

#include "OVR_CAPI_GL.h"
#if defined(_WIN32)
#include <dxgi.h> // for GetDefaultAdapterLuid
#pragma comment(lib, "dxgi.lib")
#endif
#include "point.h"


class VR_Manager
{
private:
	HINSTANCE hInst;
	void* owner;

public:
	virtual void initGlScnene() = 0;
	virtual void updateSceneLgc() = 0;
	virtual void updateGlScene() = 0;
	virtual void disposeGlScene() = 0;
	virtual void SendPose(pt3 loc, pt3 std[2], double eyeDst) = 0;

	VR_Manager();
	~VR_Manager();

	void Init(HINSTANCE hInst);
	void Start();
	void Dispose();
	bool MainLoop(bool retryCreate);
	static ovrGraphicsLuid VR_Manager::GetDefaultAdapterLuid();
	static int Compare(const ovrGraphicsLuid& lhs, const ovrGraphicsLuid& rhs);
};

bool NesEsVRmainLoop(bool);
