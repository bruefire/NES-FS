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
	// 描画 //
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
			hdc,	// デバイスコンテキストのハンドル
			&bmpInfo,	// ビットマップデータ
			DIB_RGB_COLORS,	// データ種類のインジケータ
			(void**)&bmp,	// ビット値
			NULL,	// ファイルマッピングオブジェクトのハンドル
			0	// ビットマップのビット値へのオフセット
	);
	SelectObject(hMemDC,hBitmap);

	// DIBS作成サイズ
	DIBS_WIDTH = WIDTH;
	DIBS_HEIGHT = HEIGHT;


	return 1;
}


int engine3dWin::disposeDIBS()
{
	//開放
	DeleteDC(hMemDC);
	DeleteObject(hBitmap);  //BMPを削除した時、bmpも自動的に解放される


	return 1;
}


// 初期化
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

	// ウィンドウ初期化
	if(!InitWindow())
		return false;
	
	// 基底クラスinit()
	try
	{
		engine3dGL::init();
		pyInter.Init(this);

		timeBeginPeriod(1);
	}
	catch(S3ErrorManager ex)
	{
		MessageBox(this->preWnd, ex.errMsg.c_str(), "エラー検出", MB_OK);
		dispose();
		return 0;
	}

	return 1;
}

// 開始
int engine3dWin::start()
{
	MSG msg;
	// message loop
	while (wndProcFunc(&msg))
	{
		// 更新
		if(!update())
			return 1;
	}

	return msg.wParam;
}


// 更新
int engine3dWin::update()
{
	bool result;
	pyInter.Update();

	// 基底クラス
	result = engine3dGL::update();

	return result;
}


// 後処理
int engine3dWin::dispose()
{
	if(disposeFlg)
		return 0;

	timeEndPeriod(1);

	// 基底クラス
	engine3dGL::dispose();
	
	//開放
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
    pfd.cAlphaBits = 8;		// アルファチャンネル8bit
    pfd.cDepthBits = 24;	// デプスバッファ24bit
	pfd.cStencilBits = 8;	// ステンシルバッファ8bit
    pfd.iLayerType = PFD_MAIN_PLANE;
    int pixelFormatIndex = ChoosePixelFormat(hdc, &pfd);


    if(!SetPixelFormat(hdc, pixelFormatIndex, &pfd)){ // ピクセルformat設定
        throw S3ErrorManager("ピクセルformat設定エラー");
    }
    hGLRC = wglCreateContext(hdc);
    if(hGLRC == NULL){ // OpenGLレンダリングコンテキストの生成
        throw S3ErrorManager("GLレンダcontext生成エラー");
    }
    if(!wglMakeCurrent(hdc, hGLRC)){ // OpenGLレンダリングコンテキストの設定
        wglDeleteContext(hGLRC);
        throw S3ErrorManager("GLレンダcontext設定エラー");
    }

	return 1;
}


int engine3dWin::GL_DeleteContextPls()
{
    if(!wglDeleteContext(hGLRC)){// OpenGLレンダリングコンテキストの破棄
        MessageBox(preWnd, TEXT("err4"), TEXT("GLレンダcontext破棄エラー"), MB_OK);
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
	//コピー
	BITMAPINFOHEADER bmpInfoH;
	CopyMemory(&bmpInfoH,&lpBmpInfo->bmiHeader,sizeof(BITMAPINFOHEADER));
	
	int bitCount=bmpInfoH.biBitCount;
	bmpInfoH.biBitCount=24;
	int w=bmpInfoH.biWidth , h=bmpInfoH.biHeight;
	DWORD nColorTable=bmpInfoH.biClrUsed;
	
	//長さ
	int len;
	if(w*(bitCount/8)%4) len=w*(bitCount/8)+(4-w*(bitCount/8)%4);
	else len=w*(bitCount/8);

	//bitmapファイル構造体
	BITMAPFILEHEADER bmpfh;
	bmpfh.bfSize=sizeof(BITMAPFILEHEADER)+sizeof(BITMAPINFOHEADER)+len*h;
	bmpfh.bfType=('M'<<8)+'B';
	bmpfh.bfOffBits=sizeof(BITMAPFILEHEADER)+sizeof(BITMAPINFOHEADER);

	//書き込み用名前
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

	//ファイルオープン(作成)
	HANDLE fh=CreateFile(writeName,GENERIC_WRITE,0,NULL,CREATE_NEW,FILE_ATTRIBUTE_NORMAL,NULL);
	if(fh==INVALID_HANDLE_VALUE){
		LPVOID lpMsgBuf;
		FormatMessage( FORMAT_MESSAGE_ALLOCATE_BUFFER |
			FORMAT_MESSAGE_FROM_SYSTEM |
			FORMAT_MESSAGE_IGNORE_INSERTS, 0,
			GetLastError(), MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
			(LPTSTR) &lpMsgBuf, 0, NULL);
		MessageBox(NULL, (LPTSTR) lpMsgBuf, "Error", MB_OK | MB_ICONINFORMATION);
		// バッファを解放する。
		LocalFree(lpMsgBuf);
		PostQuitMessage(0);
		return -2;
	}
	//書き込み
	DWORD dwWriteSize;
	WriteFile(fh,&bmpfh,sizeof(BITMAPFILEHEADER),&dwWriteSize,NULL);
	WriteFile(fh,&bmpInfoH,sizeof(BITMAPINFOHEADER),&dwWriteSize,NULL);

	Write32(fh, lpPixel, w, h);
	//ファイルクローズ
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
    }   //一行分のバイト数を4の倍数に補正

    WriteFile(fh,tmbm,(w*3+extra)*h,&dwWriteSize,NULL);
	HeapFree(GetProcessHeap(), 0, tmbm);
}


