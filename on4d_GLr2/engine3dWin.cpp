#include <Windows.h>
#include <glew.h>
#include <GL/gl.h>
#include <iostream>
#include "constants.h"
#include "global_var2.h"
#include "engine3dWin.h"
#include "S3ErrorManager.h"
#include "UI_DEF.h"
#include "functions.h"
using namespace std;



engine3dWin::engine3dWin()
	: disposeFlg(false)
	, MOUSE_FIX(180.0)
	, DIBS_WIDTH(-1)
	, DIBS_HEIGHT(-1)
{
	WIDTH = 1280;
	HEIGHT = 720;
	clsType = CLS_TYPE::GRAPHIC;
}


int engine3dWin::createDIBS()
{
	// �`�� //
	hMemDC = CreateCompatibleDC(hdc);
	hBitmap;

	BITMAPINFO bmpInfo;
	SecureZeroMemory(&bmpInfo, sizeof(bmpInfo));

	bmpInfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	bmpInfo.bmiHeader.biWidth = WIDTH;
	bmpInfo.bmiHeader.biHeight = HEIGHT;
	bmpInfo.bmiHeader.biPlanes = 1;
	bmpInfo.bmiHeader.biBitCount = 32;
	bmpInfo.bmiHeader.biCompression = BI_RGB;

	//--
	hBitmap = CreateDIBSection(
			hdc,	// �f�o�C�X�R���e�L�X�g�̃n���h��
			&bmpInfo,	// �r�b�g�}�b�v�f�[�^
			DIB_RGB_COLORS,	// �f�[�^��ނ̃C���W�P�[�^
			(void**)&bmp,	// �r�b�g�l
			NULL,	// �t�@�C���}�b�s���O�I�u�W�F�N�g�̃n���h��
			0	// �r�b�g�}�b�v�̃r�b�g�l�ւ̃I�t�Z�b�g
	);
	SelectObject(hMemDC,hBitmap);

	// DIBS�쐬�T�C�Y
	DIBS_WIDTH = WIDTH;
	DIBS_HEIGHT = HEIGHT;


	return 1;
}


int engine3dWin::disposeDIBS()
{
	//�J��
	DeleteDC(hMemDC);
	DeleteObject(hBitmap);  //BMP���폜�������Abmp�������I�ɉ�������


	return 1;
}


// ������
int engine3dWin::init(
			void** params, 
			HWND (*initStdWndFunc)(HINSTANCE hCurInst, int nCmdShow), 
			bool (*loopFunc)(MSG*))
{

	HINSTANCE hInst = *((HINSTANCE*)params[0]);
	int nCmdShow = *((int*)params[1]);
	this->hInst = hInst;
	this->nCmdShow = nCmdShow;
	this->initWndFunc = initStdWndFunc;
	this->wndProcFunc = loopFunc;

	// �E�B���h�E������
	if(!InitWindow())
		return false;
	
	// ���N���Xinit()
	try
	{
		engine3dGL::init();
		pyInter.Init(this);

		timeBeginPeriod(1);
	}
	catch(S3ErrorManager ex)
	{
		MessageBox(this->preWnd, ex.errMsg.c_str(), "�G���[���o", MB_OK);
		dispose();
		return 0;
	}

	return 1;
}

// �J�n
int engine3dWin::start()
{
	MSG msg;
	// message loop
	while (wndProcFunc(&msg))
	{
		// �X�V
		if(!update())
			return 1;
	}

	return msg.wParam;
}


// �X�V
int engine3dWin::update()
{
	bool result;
	pyInter.Update();

	// ���N���X
	result = engine3dGL::update();

	return result;
}


// �㏈��
int engine3dWin::dispose()
{
	if(disposeFlg)
		return 0;

	timeEndPeriod(1);

	// ���N���X
	engine3dGL::dispose();
	
	//�J��
	ReleaseDC(preWnd, hdc);

	// python
	pyInter.Dispose();


	disposeFlg = true;

	return 1;
}

bool engine3dWin::InitWindow()
{
	HWND hWnd = initWndFunc(hInst, nCmdShow);

	if(!hWnd)
		return false;

	this->preWnd = hWnd;
	hdc = GetDC(preWnd);

	return true;
}


//**** OpenGL ****//

int engine3dWin::GL_CreateContextPls()
{
	PIXELFORMATDESCRIPTOR pfd = { 0 };
    pfd.nSize = sizeof pfd;
    pfd.nVersion = 1;
    pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
    pfd.iPixelType = PFD_TYPE_RGBA;
    pfd.cColorBits = 32;
    pfd.cAlphaBits = 8;		// �A���t�@�`�����l��8bit
    pfd.cDepthBits = 24;	// �f�v�X�o�b�t�@24bit
	pfd.cStencilBits = 8;	// �X�e���V���o�b�t�@8bit
    pfd.iLayerType = PFD_MAIN_PLANE;
    int pixelFormatIndex = ChoosePixelFormat(hdc, &pfd);


    if(!SetPixelFormat(hdc, pixelFormatIndex, &pfd)){ // �s�N�Z��format�ݒ�
        throw S3ErrorManager("�s�N�Z��format�ݒ�G���[");
    }
    hGLRC = wglCreateContext(hdc);
    if(hGLRC == NULL){ // OpenGL�����_�����O�R���e�L�X�g�̐���
        throw S3ErrorManager("GL�����_context�����G���[");
    }
    if(!wglMakeCurrent(hdc, hGLRC)){ // OpenGL�����_�����O�R���e�L�X�g�̐ݒ�
        wglDeleteContext(hGLRC);
        throw S3ErrorManager("GL�����_context�ݒ�G���[");
    }

	return 1;
}


int engine3dWin::GL_DeleteContextPls()
{
    if(!wglDeleteContext(hGLRC)){// OpenGL�����_�����O�R���e�L�X�g�̔j��
        MessageBox(preWnd, TEXT("err4"), TEXT("GL�����_context�j���G���["), MB_OK);
        return false;
    }

	return 1;
}

int engine3dWin::GL_SwapBuffer()
{
	SwapBuffers(hdc);

	return 1;
}


////////////////


int engine3dWin::SaveDIB32(char *lpFileName,const BYTE *lpPixel,const BITMAPINFO *lpBmpInfo, int loop)
{
	//�R�s�[
	BITMAPINFOHEADER bmpInfoH;
	CopyMemory(&bmpInfoH,&lpBmpInfo->bmiHeader,sizeof(BITMAPINFOHEADER));
	
	int bitCount=bmpInfoH.biBitCount;
	bmpInfoH.biBitCount=24;
	int w=bmpInfoH.biWidth , h=bmpInfoH.biHeight;
	DWORD nColorTable=bmpInfoH.biClrUsed;
	
	//����
	int len;
	if(w*(bitCount/8)%4) len=w*(bitCount/8)+(4-w*(bitCount/8)%4);
	else len=w*(bitCount/8);

	//bitmap�t�@�C���\����
	BITMAPFILEHEADER bmpfh;
	bmpfh.bfSize=sizeof(BITMAPFILEHEADER)+sizeof(BITMAPINFOHEADER)+len*h;
	bmpfh.bfType=('M'<<8)+'B';
	bmpfh.bfOffBits=sizeof(BITMAPFILEHEADER)+sizeof(BITMAPINFOHEADER);

	//�������ݗp���O
	char idt[] = ".bmp";
	char nameSz;
	for(char i=0;i<255;i++) if(lpFileName[i]==0){ nameSz = i; break; }
	char* writeName = new char[nameSz+6+5];
	for(char i=0;i<nameSz+6+4+1;i++){
		if(i<nameSz) writeName[i] = lpFileName[i];
		else if(i<nameSz+6){
			int per = 1;
			for(char j=i;j<nameSz+6-1;j++) per *= 10;
			writeName[i] = (loop/per)%10+48;
			
		}else writeName[i] = idt[i-(nameSz+6)];
	}

	//�t�@�C���I�[�v��(�쐬)
	HANDLE fh=CreateFile(writeName,GENERIC_WRITE,0,NULL,CREATE_NEW,FILE_ATTRIBUTE_NORMAL,NULL);
	if(fh==INVALID_HANDLE_VALUE){
		LPVOID lpMsgBuf;
		FormatMessage( FORMAT_MESSAGE_ALLOCATE_BUFFER |
			FORMAT_MESSAGE_FROM_SYSTEM |
			FORMAT_MESSAGE_IGNORE_INSERTS, 0,
			GetLastError(), MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
			(LPTSTR) &lpMsgBuf, 0, NULL);
		MessageBox(NULL, (LPTSTR) lpMsgBuf, "Error", MB_OK | MB_ICONINFORMATION);
		// �o�b�t�@���������B
		LocalFree(lpMsgBuf);
		PostQuitMessage(0);
		return -2;
	}
	//��������
	DWORD dwWriteSize;
	WriteFile(fh,&bmpfh,sizeof(BITMAPFILEHEADER),&dwWriteSize,NULL);
	WriteFile(fh,&bmpInfoH,sizeof(BITMAPINFOHEADER),&dwWriteSize,NULL);

	Write32(fh, lpPixel, w, h);
	//�t�@�C���N���[�Y
	CloseHandle(fh);
	delete[] writeName;
	return 0;
}

char* engine3dWin::nextInt(char* nowInt, char len, char rank)
{
	if(rank<=len){
		++nowInt[sizeof(nowInt)-rank];

		if(58<=nowInt[sizeof(nowInt)-rank]){
			nowInt[sizeof(nowInt)-rank] = 48;
			return nextInt(nowInt, len, rank+1);
		}
		else return nowInt;
	}
	return nowInt;
}

void engine3dWin::Write32(HANDLE fh,const BYTE *lpPixel,int w,int h)
{
    int extra;
    if(w*3%4) extra=4-w*3%4;
    else extra=0;

	UCHAR* tmbm = (UCHAR*)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, (w*3+extra)*h);

    DWORD dwWriteSize;
    int zero=0;
    for(int y=0;y<h;y++){
        for(int x=0;x<w+extra;x++)
			for(int b=0;b<3;b++) tmbm[(x*3+b)+(w*3+extra)*y] = lpPixel[(x*4+b)+(w*4)*y];
    }   //��s���̃o�C�g����4�̔{���ɕ␳

    WriteFile(fh,tmbm,(w*3+extra)*h,&dwWriteSize,NULL);
	HeapFree(GetProcessHeap(), 0, tmbm);
}


