#pragma once

#include <Windows.h>
#include <string>
#include <glew.h>
#include <GL/gl.h>
#include "constants.h"
#include "engine3dGL.h"



class engine3dWin : public engine3dGL
{
public:
	HWND preWnd;
	HDC hdc;
	HGLRC hGLRC;
	HDC hMemDC;
	HBITMAP hBitmap;
	UINT* bmp;
	int DIBS_WIDTH;
	int DIBS_HEIGHT;

	bool disposeFlg;
	double MOUSE_FIX;

	JOYCAPS joyCaps;	// �Q�[���p�b�h���


	engine3dWin();

	int init(HWND preWnd);
	int update();
	int dispose();

	// openGL
	int GL_CreateContextPls();
	int GL_DeleteContextPls();
	int GL_SwapBuffer();

	int to3d();
	int getOn4loc(short*, double*, UINT*);	//-- on4d�I�u�W�F�N�g�̍��W�擾
	int simulate3(UINT* bmp);
	int createDIBS();
	int disposeDIBS();
	double clcRangeY(double rangeX);

	int SaveDIB32(char *lpFileName,const BYTE *lpPixel,const BITMAPINFO *lpBmpInfo, int loop);
	char* nextInt(char* nowInt, char len, char rank);
	void Write32(HANDLE fh,const BYTE *lpPixel,int w,int h);
};
