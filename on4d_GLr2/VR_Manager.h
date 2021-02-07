#pragma once
#include <Windows.h>


class VR_Manager
{
private:
	HINSTANCE hInst;

public:
	VR_Manager();
	~VR_Manager();

	void Init(HINSTANCE hInst);
	void Dispose();
};
