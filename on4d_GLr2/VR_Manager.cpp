#include <Windows.h>
#include "../OVR/Common/Win32_GLAppUtil.h"
#include "VR_Manager.h"



VR_Manager::VR_Manager()
{
}
VR_Manager::~VR_Manager()
{
}

void VR_Manager::Init(HINSTANCE hInst)
{
	this->hInst = hInst;

	// Initializes LibOVR, and the Rift
	ovrInitParams initParams = { ovrInit_RequestVersion | ovrInit_FocusAware, OVR_MINOR_VERSION, NULL, 0, 0 };
	ovrResult result = ovr_Initialize(&initParams);
	VALIDATE(OVR_SUCCESS(result), "Failed to initialize libOVR.");

	// use the window for VR.
	VALIDATE(Platform.InitWindow(this->hInst, L"Oculus Room Tiny (GL)"), "Failed to open window.");
}

void VR_Manager::Dispose()
{
	ovr_Shutdown();
}
