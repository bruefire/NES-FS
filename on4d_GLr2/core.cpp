#define PY_SSIZE_T_CLEAN
#include <Python.h>
#include "CppPythonIF.h"

#include <Windows.h>
#include <windowsx.h>
#include <Complex>
#include <iostream>
#include <String>
#include <commctrl.h>
#include "constants.h"
#include "global_var.h"
#include "engine3dWin.h"
#include "functions.h"
#include "UI_DEF.h"
#include "rcFunc.h"
#include "modlgRc.h"
#include "editor.h"
#include "objSetting.h"
#include <glew.h>
#include <GL/gl.h>
using namespace std;

// プロシージャ
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK DialogProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK howToDlgProc(HWND hDlg, UINT msg, WPARAM wp, LPARAM lp);
INT_PTR CALLBACK moveObjProc(HWND hDlg, UINT msg, WPARAM wp, LPARAM lp);
INT_PTR CALLBACK EditorProc(HWND hDlg, UINT msg, WPARAM wp, LPARAM lp);
INT_PTR CALLBACK ModObjProc(HWND hDlg, UINT msg, WPARAM wp, LPARAM lp);

//
ATOM InitApp(HINSTANCE);
BOOL InitInstance(HINSTANCE, int);


TCHAR szClassName[] = TEXT("3d_engine");	//ウィンドウクラス
engine3dWin newEngine;
JOYINFOEX JoyInfoEx;	//-- ゲームパッド
HMENU hMenu;
HINSTANCE curInst;
char curDir[MAX_PATH];
char* menuName = "KITTY";	// メニュー名
char* titleName = "超球面遊泳シミュレータ";	// ウィンドウタイトル

HWND preWnd;
HWND editDlg = nullptr;
HWND modObjDlg = nullptr;
char cmJD = 0;
POINTS cm_rot[2] = {{}, {}};
HMENU hpMenu;
HMENU subMenu;



//windowsプログラムのエントリーポイント
int WINAPI WinMain(HINSTANCE hCurInst, HINSTANCE hPrevInst, LPSTR lpsCmdLine, int nCmdShow)
{
	// コントロール
	//InitCommonControls();
	//
	curInst = hCurInst;
	MSG msg;
	GetCurrentDirectory(MAX_PATH, curDir);

	//-- コンソール作成 --//
    if(!AllocConsole()) return 0;
    freopen("CONOUT$", "w", stdout);
    freopen("CONIN$", "r", stdin);

	//---- コマンド取得 英語か日本語
	string awakeCmd = GetCommandLine();
	cout << awakeCmd << endl;
	int awIdx = awakeCmd.find_last_of(" ") +1;
	
	if(awIdx <= awakeCmd.length()){
		if (awakeCmd.substr(awIdx) == "sim:H3;lang:JA")
		{
			menuName = "KITTY_H3";
			titleName = "双曲面遊泳シミュレータ";
			newEngine.lang = UI_LANG_JA;
			newEngine.worldGeo = engine3d::WorldGeo::HYPERBOLIC;
		}
		else if(awakeCmd.substr(awIdx) == "lang:EN")
		{
			menuName = "KITTY_EN";
			titleName = "S3 wondering simlator";
			newEngine.lang = UI_LANG_EN;
			newEngine.worldGeo = engine3d::WorldGeo::SPHERICAL;
		}
		else
		{
			menuName = "KITTY";
			titleName = "超球面遊泳シミュレータ";
			newEngine.lang = UI_LANG_JA;
			newEngine.worldGeo = engine3d::WorldGeo::SPHERICAL;
		}
		cout << awakeCmd.substr(awIdx) << endl;
	}

	// ポップアップメニュー初期化
	hpMenu = LoadMenu(hCurInst, "POPUP");
	subMenu = GetSubMenu(hpMenu, 0);
	//------------

	if(!InitApp(hCurInst)) return FALSE;
	if(!InitInstance(hCurInst, nCmdShow)) return FALSE;

	MENUITEMINFO menuItemInfo = { sizeof(MENUITEMINFO), MIIM_STATE };
	menuCheckDef(hMenu, &menuItemInfo);
	//

	// Python初期化
	bool initPyFlg;
	PyObject* catcher;
	if (PyImport_AppendInittab("h3sim", PyInit_CppModule) == -1)
		initPyFlg = false;
	else
	{
		// python初期化ファイル
		string pyInitFle = "pyInit.py";
		FILE* pyInitFp = fopen(pyInitFle.c_str(), "rb");

		Py_InitializeEx(0);	// todo★ python実行環境ごとこっちで用意する
		CppPythonIF::pModule = PyImport_AddModule("__main__");
		CppPythonIF::pDict = PyModule_GetDict(CppPythonIF::pModule);
		PyRun_SimpleFile(pyInitFp, pyInitFle.c_str());
		catcher = PyObject_GetAttrString(CppPythonIF::pModule, "catchOutErr");
		CppPythonIF::engine = &newEngine;
		initPyFlg = true;

		fclose(pyInitFp);
	}

	// S3シミュレータクラス 初期化
	if(!newEngine.init(preWnd))
		PostQuitMessage(0);
	

	///-- ゲームパッド
	JoyInfoEx.dwSize = sizeof(JOYINFOEX);
	JoyInfoEx.dwFlags = JOY_RETURNALL;
	if (JOYERR_NOERROR == joySetCapture(preWnd, JOYSTICKID1, 1, FALSE))
	{
		joyGetDevCaps(JOYSTICKID1, &newEngine.joyCaps, sizeof(JOYCAPS));
		newEngine.useJoyPad = true;
		cout << "joypad1 is avaliable." << endl;
	}

	///--------------メッセージを取得--------------///
	while(true){
		if((PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) != 0){
			if(msg.message == WM_QUIT) break;
			else{
				// まず子ダイアログにメッセージ処理を試す
				if (editDlg && IsDialogMessage(editDlg, &msg))
					continue;
				else if (modObjDlg && IsDialogMessage(modObjDlg, &msg))
					continue;

				TranslateMessage(&msg);	//メッセージを変換
				DispatchMessage(&msg);	//メッセージを送出
			}
		}else{
			// Pythonスクリプト処理
			if (initPyFlg && CppPythonIF::rawCode[0] != 0)
			{
				PyRun_SimpleString(CppPythonIF::rawCode);
				
				PyObject* output = PyObject_GetAttrString(catcher, "value");
				const char* cnvStr = PyUnicode_AsUTF8AndSize(output, nullptr);
				if(cnvStr != nullptr)
					cout << cnvStr << endl;

				ZeroMemory(CppPythonIF::rawCode, 8186);
				Py_XDECREF(output);
				PyRun_SimpleString("sys.stdout.value = ''");
			}
			// S3更新
			newEngine.update();
		}
	}

	///-- 後処理 --//
	newEngine.dispose();

	if (initPyFlg) {
		Py_XDECREF(catcher);
		Py_FinalizeEx();
	}
	FreeConsole();
	///--
	

	return (int)msg.wParam;
}

//ウィンドウクラスの登録
ATOM InitApp(HINSTANCE hInst)
{
	WNDCLASSEX wc;
	wc.cbSize = sizeof(WNDCLASSEX);	//構造体のサイズ
	wc.style = CS_OWNDC | CS_HREDRAW | CS_VREDRAW;	//クラスのスタイル
	wc.lpfnWndProc = WndProc;	//プロシージャ名
	wc.cbClsExtra = 0;	//補助メモリ
	wc.cbWndExtra = 0;	//補助メモリ
	wc.hInstance = hInst;	//インスタンス

	wc.hIcon = (HICON)LoadImage(NULL, "s3sim.ico",
				IMAGE_ICON, 0, 0, LR_DEFAULTSIZE | LR_SHARED | LR_LOADFROMFILE);
	wc.hIconSm = (HICON)LoadImage(NULL, "s3sim_sm.ico",
				IMAGE_ICON, 0, 0, LR_DEFAULTSIZE | LR_SHARED | LR_LOADFROMFILE);
	wc.hCursor = (HCURSOR)LoadImage(NULL, MAKEINTRESOURCE(IDC_ARROW),
				IMAGE_CURSOR, 0, 0, LR_DEFAULTSIZE | LR_SHARED);
	wc.hbrBackground = (HBRUSH)GetStockObject(DKGRAY_BRUSH);
	wc.lpszMenuName = menuName;	//メニュー名
	wc.lpszClassName = szClassName;	//クラス名

	return (RegisterClassEx(&wc));
}

//=========メインウィンドウの生成
BOOL InitInstance(HINSTANCE hInst, int nCmdShow)
{
	HWND hWnd;
	hWnd = CreateWindow(szClassName,	//クラス名
				titleName,	//ウィンドウ名
				WS_OVERLAPPEDWINDOW,	//ウィンドウスタイル
				CW_USEDEFAULT,	//x位置
				CW_USEDEFAULT,	//y位置
				newEngine.WIDTH + GetSystemMetrics(SM_CXFRAME)*2,	//xウィンドウ幅
				newEngine.HEIGHT + GetSystemMetrics(SM_CYFRAME)*2
					+ GetSystemMetrics(SM_CYCAPTION)
					+ GetSystemMetrics(SM_CYMENU),	//ウィンドウ高さ
				NULL,	//親ウィンドウのハンドル、親を作るときはNULL
				hMenu,	//メニューハンドル、クラスメニューを使うときはNULL
				hInst,	//インスタンスハンドル
				NULL	//ウィンドウ作成データ
			);

	if(!hWnd) return FALSE; else preWnd = hWnd;
	hMenu = GetMenu( hWnd );

	ShowWindow(hWnd, nCmdShow);	//ウィンドウの表示状態を設定
	UpdateWindow(hWnd);	//ウィンドウを更新

	newEngine.CR_RANGE_Y = newEngine.clcRangeY(newEngine.CR_RANGE_X);	//カメラ設定
	
	return TRUE;
}



//ウィンドウプロシージャ(コールバック関数)
LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp)
{
 if(hWnd==preWnd){///===プライマリ
	static bool shiftK = false;
	static bool ctrlK = false;
	static MENUITEMINFO menuItemInfo = {
					sizeof(MENUITEMINFO), MIIM_STATE
	};
	static char refName[1024];
	static OPENFILENAME ofName = {
		sizeof(OPENFILENAME),
		hWnd,
		0,
		"Wavefront .objファイル {*.obj}\0*.obj\0"
		"All files {*.*}\0*.*\0\0",
		0,
		0,
		0,
		refName,
		1024,
		0,
		0,
		0,
		"オブジェクトの参照",
		OFN_FILEMUSTEXIST

	};
	//-----
	char myPath[256];
	int myPathLen = 256;
	int mpIdx;
	string fName;
	PROCESS_INFORMATION pi = { 0 }; 
	STARTUPINFO si = { sizeof(STARTUPINFO) };


	switch (msg){
		case WM_CREATE:
			break;

		case WM_DESTROY:
			PostQuitMessage(0);
			break;

		case WM_SIZE:
			if(!wp||wp==2){
				newEngine.WIDTH = lp & 0xFFFF;
				newEngine.HEIGHT = (lp>>16) & 0xFFFF;
				newEngine.CR_RANGE_Y = newEngine.clcRangeY(newEngine.CR_RANGE_X);	//カメラ設定
				glViewport(0, 0, newEngine.WIDTH, newEngine.HEIGHT);	//-- GLwndサイズ更新
			}
			break;

		case WM_COMMAND:
			if(lp==0){	//メニュー
				int wVal = LOWORD(wp);
				switch (wVal) {

				//-- プログラム終了
				case UI_END:
					PostQuitMessage(0);
					break;
				//-- フレームレート
				case UI_FPS_30:
					newEngine.fps = newEngine.STD_PMSEC;
					newEngine.adjSpd = 1.0;
					menuCheck(hMenu, &menuItemInfo, UI_FPS_30, unCk_fps, unCk_fps_len);
					break;
				case UI_FPS_60:
					newEngine.fps = newEngine.MAX_PMSEC;
					newEngine.adjSpd = 0.5;
					menuCheck(hMenu, &menuItemInfo, UI_FPS_60, unCk_fps, unCk_fps_len);
					break;
				case UI_FPS_VFR:
					newEngine.fps = newEngine.MAX_PMSEC;
					newEngine.adjSpd = 1.0;
					menuCheck(hMenu, &menuItemInfo, UI_FPS_VFR, unCk_fps, unCk_fps_len);
					break;
				//-- 言語
				case UI_LANG_JA:
					//changeLang(hMenu, &menuItemInfo, UI_LANG_JA);
					//menuCheck(hMenu, &menuItemInfo, UI_LANG_JA, unCk_lang, unCk_lang_len);
					GetModuleFileName(NULL, myPath, myPathLen);
					fName = myPath;
					mpIdx = fName.find_last_of("\\") +1;
					fName = fName.substr(mpIdx) + " lang:JA";

					CreateProcess(NULL, &fName[0], NULL, NULL, FALSE, NORMAL_PRIORITY_CLASS, NULL, NULL, &si, &pi);
					PostQuitMessage(0);
					break;
				case UI_LANG_EN:
					//changeLang(hMenu, &menuItemInfo, UI_LANG_EN);
					//menuCheck(hMenu, &menuItemInfo, UI_LANG_EN, unCk_lang, unCk_lang_len);
					GetModuleFileName(NULL, myPath, myPathLen);
					fName = myPath;
					mpIdx = fName.find_last_of("\\") +1;
					fName = fName.substr(mpIdx) + " lang:EN";

					CreateProcess(NULL, &fName[0], NULL, NULL, FALSE, NORMAL_PRIORITY_CLASS, NULL, NULL, &si, &pi);
					PostQuitMessage(0);
					break;

				case UI_H3_JA:
					GetModuleFileName(NULL, myPath, myPathLen);
					fName = myPath;
					mpIdx = fName.find_last_of("\\") + 1;
					fName = fName.substr(mpIdx) + " sim:H3;lang:JA";

					CreateProcess(NULL, &fName[0], NULL, NULL, FALSE, NORMAL_PRIORITY_CLASS, NULL, NULL, &si, &pi);
					PostQuitMessage(0);
					break;

				case UI_THROW_CLEAR:
					newEngine.DisableShootObjs();
					break;
				case UI_THROW_RANDOM:
					newEngine.player.ep = 0;
					newEngine.RandLocS3(engine3d::RandMode::Cluster);
					break;
				case UI_THROW_RANDOM2:
					newEngine.player.ep = 0;
					if (newEngine.worldGeo == engine3d::WorldGeo::HYPERBOLIC)
					{
						object3d* plrObj = &newEngine.objs[newEngine.PLR_No];
						for (int h = newEngine.BWH_QTY + newEngine.PLR_QTY; h < newEngine.OBJ_QTY; h++)
						{
							newEngine.objs[h].loc = plrObj->loc;
							newEngine.objs[h].std[0] = plrObj->std[0];
							newEngine.objs[h].std[1] = plrObj->std[1];
							newEngine.objs[h].lspX = plrObj->lspX;
							newEngine.objs[h].used = true;
						}
						newEngine.RandLocH3(engine3d::RandMode::Uniform, engine3d::ObjType::Energy);
					}
					else 
						newEngine.RandLocS3(engine3d::RandMode::Uniform);
					
					break;

				case UI_CR_S:
					newEngine.CR_RANGE_X = 70;
					newEngine.CR_RANGE_Y = newEngine.clcRangeY( newEngine.CR_RANGE_X );	//カメラ設定
					menuCheck(hMenu, &menuItemInfo, UI_CR_S, unCk_rng, unCk_rng_len);
					break;
				case UI_CR_M:
					newEngine.CR_RANGE_X = 90;
					newEngine.CR_RANGE_Y = newEngine.clcRangeY( newEngine.CR_RANGE_X );	//カメラ設定
					menuCheck(hMenu, &menuItemInfo, UI_CR_M, unCk_rng, unCk_rng_len);
					break;
				case UI_CR_L:
					newEngine.CR_RANGE_X = 110;
					newEngine.CR_RANGE_Y = newEngine.clcRangeY( newEngine.CR_RANGE_X );	//カメラ設定
					menuCheck(hMenu, &menuItemInfo, UI_CR_L, unCk_rng, unCk_rng_len);
					break;

				case UI_SD_0:
					newEngine.decMode = 0;
					menuCheck(hMenu, &menuItemInfo, UI_SD_0, unCk_dec, unCk_dec_len);
					break;
				case UI_SD_1:
					newEngine.decMode= 1;
					menuCheck(hMenu, &menuItemInfo, UI_SD_1, unCk_dec, unCk_dec_len);
					break;
				case UI_SD_2:
					newEngine.decMode = 2;
					menuCheck(hMenu, &menuItemInfo, UI_SD_2, unCk_dec, unCk_dec_len);
					break;
				case UI_SD_4:
					newEngine.decMode = 4;
					menuCheck(hMenu, &menuItemInfo, UI_SD_4, unCk_dec, unCk_dec_len);
					break;

				case UI_BG_WH:
					newEngine.bgCol = 1;
					menuCheck(hMenu, &menuItemInfo, UI_BG_WH, unCk_bg, unCk_bg_len);
					break;
				case UI_BG_BK:
					newEngine.bgCol = 0; 
					menuCheck(hMenu, &menuItemInfo, UI_BG_BK, unCk_bg, unCk_bg_len);
					break;

				case UI_RDS_SS:
					newEngine.radius = 20;
					menuCheck(hMenu, &menuItemInfo, UI_RDS_SS, unCk_rds, unCk_rds_len);
					break;
				case UI_RDS_S:
					newEngine.radius = 30;
					menuCheck(hMenu, &menuItemInfo, UI_RDS_S, unCk_rds, unCk_rds_len);
					break;
				case UI_RDS_M:
					newEngine.radius = 60;
					menuCheck(hMenu, &menuItemInfo, UI_RDS_M, unCk_rds, unCk_rds_len);
					break;
				case UI_RDS_L:
					newEngine.radius = 100;
					menuCheck(hMenu, &menuItemInfo, UI_RDS_L, unCk_rds, unCk_rds_len);
					break;

				case UI_MAP:
					newEngine.VIEW_ON4 = !newEngine.VIEW_ON4;
					menuCheck2(hMenu, &menuItemInfo, UI_MAP);
					break;
				case UI_XYZ:
					newEngine.VIEW_XYZ = !newEngine.VIEW_XYZ;
					menuCheck2(hMenu, &menuItemInfo, UI_XYZ);
					break;
				case UI_LR_RESULT:
					newEngine.VIEW_LocRot = !newEngine.VIEW_LocRot; 
					menuCheck2(hMenu, &menuItemInfo, UI_LR_RESULT);
					break;
				case UI_PLR:
					newEngine.VIEW_PLR = !newEngine.VIEW_PLR;
					menuCheck2(hMenu, &menuItemInfo, UI_PLR);
					break;
				case UI_DST:
					newEngine.VIEW_DST = !newEngine.VIEW_DST;
					menuCheck2(hMenu, &menuItemInfo, UI_DST);
					break;
				case UI_PAST:
					newEngine.markObj.used = !newEngine.markObj.used;
					menuCheck2(hMenu, &menuItemInfo, UI_PAST);
					break;

				case UI_WL_NONE:
					newEngine.objs[0].used = false;
					newEngine.sun.used = false;
					menuCheck(hMenu, &menuItemInfo, UI_WL_NONE, unCk_mark, unCk_mark_len);
					break;
				case UI_6_POINT:
					newEngine.objs[0].mesh = newEngine.meshs+0; 
					newEngine.objs[0].draw = 0; 
					newEngine.objs[0].used = true;
					newEngine.sun.used = false;
					menuCheck(hMenu, &menuItemInfo, UI_6_POINT, unCk_mark, unCk_mark_len);
					break;
				case UI_6_LINE:
					newEngine.objs[0].mesh = newEngine.meshs+0; 
					newEngine.objs[0].draw = 1;
					newEngine.objs[0].used = true;
					newEngine.sun.used = false;
					menuCheck(hMenu, &menuItemInfo, UI_6_LINE, unCk_mark, unCk_mark_len);
					break;
				case UI_TUDE_POINT:
					newEngine.objs[0].mesh = newEngine.meshs+3; 
					newEngine.objs[0].draw = 0; 
					newEngine.objs[0].used = true;
					newEngine.sun.used = false;
					menuCheck(hMenu, &menuItemInfo, UI_TUDE_POINT, unCk_mark, unCk_mark_len);
					break;
				case UI_TUDE_LINE:
					newEngine.objs[0].mesh = newEngine.meshs+3; 
					newEngine.objs[0].draw = 1;
					newEngine.objs[0].used = true;
					newEngine.sun.used = false;
					menuCheck(hMenu, &menuItemInfo, UI_TUDE_LINE, unCk_mark, unCk_mark_len);
					break;
				case UI_120C_POINT:
					newEngine.objs[0].mesh = newEngine.meshs+10; 
					newEngine.objs[0].draw = 0; 
					newEngine.objs[0].used = true;
					newEngine.sun.used = false;
					menuCheck(hMenu, &menuItemInfo, UI_120C_POINT, unCk_mark, unCk_mark_len);
					break;
				case UI_120C_LINE:
					newEngine.objs[0].mesh = newEngine.meshs+10; 
					newEngine.objs[0].draw = 1; 
					newEngine.objs[0].used = true;
					newEngine.sun.used = false;
					menuCheck(hMenu, &menuItemInfo, UI_120C_LINE, unCk_mark, unCk_mark_len);
					break;
				case UI_120C_SURFACE:
					newEngine.objs[0].mesh = newEngine.meshs+11; 
					newEngine.objs[0].draw = 2; 
					newEngine.objs[0].used = true;
					newEngine.sun.used = false;
					menuCheck(hMenu, &menuItemInfo, UI_120C_SURFACE, unCk_mark, unCk_mark_len);
					break;
				case UI_TORUS_POINT:
					newEngine.objs[0].mesh = newEngine.meshs+12; 
					newEngine.objs[0].draw = 0; 
					newEngine.objs[0].used = true;
					newEngine.sun.used = false;
					menuCheck(hMenu, &menuItemInfo, UI_TORUS_POINT, unCk_mark, unCk_mark_len);
					break;
				case UI_TORUS_LINE:
					newEngine.objs[0].mesh = newEngine.meshs+12; 
					newEngine.objs[0].draw = 1; 
					newEngine.objs[0].used = true;
					newEngine.sun.used = false;
					menuCheck(hMenu, &menuItemInfo, UI_TORUS_LINE, unCk_mark, unCk_mark_len);
					break;
				case UI_TORUS_SURFACE:
					newEngine.objs[0].mesh = newEngine.meshs+13; 
					newEngine.objs[0].draw = 2; 
					newEngine.objs[0].used = true;
					newEngine.sun.used = false;
					menuCheck(hMenu, &menuItemInfo, UI_TORUS_SURFACE, unCk_mark, unCk_mark_len);
					break;
				case UI_MK_EARTH:
					newEngine.objs[0].mesh = newEngine.meshs+2; 
					newEngine.objs[0].draw = 2;
					newEngine.objs[0].used = true;
					newEngine.sun.used = false;
					menuCheck(hMenu, &menuItemInfo, UI_MK_EARTH, unCk_mark, unCk_mark_len);
					break;
				case UI_MK_TC:
					newEngine.objs[0].mesh = newEngine.meshs+7; 
					newEngine.objs[0].draw = 2;
					newEngine.objs[0].used = true;
					newEngine.sun.used = true;
					menuCheck(hMenu, &menuItemInfo, UI_MK_TC, unCk_mark, unCk_mark_len);
					break;

				case UI_THROW_SPHERE:
					for(int i=newEngine.BWH_QTY+newEngine.PLR_QTY; i<newEngine.OBJ_QTY; i++)
						newEngine.objs[i].mesh = newEngine.meshs+8; 
					menuCheck(hMenu, &menuItemInfo, UI_THROW_SPHERE, unCk_thrw, unCk_thrw_len);
					break;
				case UI_THROW_CUBE:
					for(int i=newEngine.BWH_QTY+newEngine.PLR_QTY; i<newEngine.OBJ_QTY; i++)
						newEngine.objs[i].mesh = newEngine.meshs+4; 
					menuCheck(hMenu, &menuItemInfo, UI_THROW_CUBE, unCk_thrw, unCk_thrw_len);
					break;
				case UI_THROW_HORSE:
					for(int i=newEngine.BWH_QTY+newEngine.PLR_QTY; i<newEngine.OBJ_QTY; i++)
						newEngine.objs[i].mesh = newEngine.meshs+5;
					menuCheck(hMenu, &menuItemInfo, UI_THROW_HORSE, unCk_thrw, unCk_thrw_len); 
					break;
				case UI_THROW_ROCK:
					for(int i=newEngine.BWH_QTY+newEngine.PLR_QTY; i<newEngine.OBJ_QTY; i++)
						newEngine.objs[i].mesh = newEngine.meshs+14;
					menuCheck(hMenu, &menuItemInfo, UI_THROW_ROCK, unCk_thrw, unCk_thrw_len); 
					break;
				case UI_THROW_REF:
					if( GetOpenFileName( &ofName ) ){	//-- ファイル参照
						SetCurrentDirectory(curDir);
						newEngine.meshs[9].~mesh3d();
						refName[ strlen(refName)-strlen(".obj") ] = 0x00;
						if(!newEngine.meshs[9].meshInit( refName, 9+1, 1))
						{
							newEngine.meshs[9].meshInit( newEngine.meshNames[9], 9+1, 0);
						}
						if(newEngine.meshs[9].faces != nullptr)
							((mesh3dGL*)&newEngine.meshs[9])->transBuffers(newEngine.buffers, newEngine.texNames);

						for(int i=newEngine.BWH_QTY+newEngine.PLR_QTY; i<newEngine.OBJ_QTY; i++)
							newEngine.objs[i].mesh = newEngine.meshs+9;
						menuCheck(hMenu, &menuItemInfo, UI_THROW_REF, unCk_thrw, unCk_thrw_len); 
					}
					break;

				case UI_MV_CONST:
					newEngine.GRAVITY = 0;
					for (int h = 0; h < newEngine.OBJ_QTY; h++) {
						newEngine.objs[h].fc.asg(0, 0, 0, 0);
						newEngine.objs[h].mkLspX_S3( newEngine.objs[h].lspX );
					}
					menuCheck(hMenu, &menuItemInfo, UI_MV_CONST, unCk_mv, unCk_mv_len);
					break;
				case UI_MV_ATTRACT:
					if(!newEngine.GRAVITY) newEngine.all_cnvForce();
					newEngine.GRAVITY = 1;
					menuCheck(hMenu, &menuItemInfo, UI_MV_ATTRACT, unCk_mv, unCk_mv_len);
					break;
				case UI_MV_REPULSE:
					if(!newEngine.GRAVITY) newEngine.all_cnvForce();
					newEngine.GRAVITY = 2;
					menuCheck(hMenu, &menuItemInfo, UI_MV_REPULSE, unCk_mv, unCk_mv_len);
					break;

				case UI_LOC_EUC:
					newEngine.LOC_MODE = 0; 
					menuCheck(hMenu, &menuItemInfo, UI_LOC_EUC, unCk_xyz, unCk_xyz_len); 
					break;
				case UI_LOC_TUDE:
					newEngine.LOC_MODE = 1; 
					menuCheck(hMenu, &menuItemInfo, UI_LOC_TUDE, unCk_xyz, unCk_xyz_len); 
					break;
					
				case UI_HOWTO_DLG:
					DialogBox(curInst, "HOWTO_DLG", preWnd, howToDlgProc); break;
				case UI_INFO_DLG:
					DialogBox(curInst, "INFO_DLG", preWnd, DialogProc); break;
				case UI_CALL_MODLG:
					DialogBox(curInst, "MOVE_OBJ_DLG", preWnd, moveObjProc); break;
				case UI_CALL_SCRIPT:
					if (editDlg == nullptr)
					{
						editDlg = CreateDialog(curInst, "EDITOR_DLG", preWnd, EditorProc);
						ShowWindow(editDlg, SW_NORMAL);
					}
					break;
				case UI_OBJ_LOOP:
					menuCheck2(hMenu, &menuItemInfo, UI_OBJ_LOOP);
					newEngine.h3objLoop = !newEngine.h3objLoop;
					break;

				case SUBUI_CLEAR_OBJ:
					if (newEngine.CheckSelectedEnable())
						newEngine.objs[newEngine.selectedIdx].used = false;
					break;
				case SUBUI_SETTING_OBJ:
					if (modObjDlg == nullptr)
					{
						modObjDlg = CreateDialog(curInst, "MODOBJ_DLG", preWnd, ModObjProc);
						ShowWindow(modObjDlg, SW_NORMAL);
					}
					break;
				}
			}
			break;

		case WM_LBUTTONDOWN:
		{
			POINTS tmp = MAKEPOINTS(lp);
			newEngine.ope.clickState = Operation::ClickState::Left;
			newEngine.ope.clickCoord = pt2i(tmp.x, newEngine.HEIGHT - (tmp.y + 1));
		}
			break;
			
		case WM_MBUTTONDOWN:
			if((wp&0x0004) != 0x0004) cmJD = 1; else cmJD = 2;
			cm_rot[0] = cm_rot[1] = MAKEPOINTS(lp);
			break;
			
		case WM_MOUSEMOVE:
			if(cmJD){	// 左クリック中
				cm_rot[1] = MAKEPOINTS(lp);
				newEngine.inPutMouseMv
				(
					(cm_rot[1].x - cm_rot[0].x) / newEngine.MOUSE_FIX,
					(cm_rot[1].y - cm_rot[0].y) / newEngine.MOUSE_FIX,
					cmJD
				);
				cm_rot[0] = cm_rot[1];
			}
			break;

		case WM_MBUTTONUP:
			if(cmJD==1){
				newEngine.ope.cmRot.x = fmod(newEngine.ope.cmRot.x, PIE*2);
				newEngine.ope.cmRot.y = fmod(newEngine.ope.cmRot.y, PIE*2);
			}
			cmJD = 0;
			break;

		case WM_MOUSEWHEEL:
			if(!cmJD){
				double tmp = ((int)wp >> 16) * 0.002;

				newEngine.inPutWheel
				(
					tmp,
					shiftK
				);
			}
			break;

		case WM_RBUTTONDOWN:
			POINTS tmp = MAKEPOINTS(lp);
			newEngine.ope.clickState = Operation::ClickState::Right;
			newEngine.ope.clickCoord = pt2i(tmp.x, newEngine.HEIGHT - (tmp.y + 1));
			break;
		case WM_RBUTTONUP:
		{
			if (!newEngine.CheckSelectedEnable())
				break;
			POINT po;
			po.x = LOWORD(lp);
			po.y = HIWORD(lp);
			ClientToScreen(hWnd, &po);

			MENUITEMINFO miInfo = menuItemInfo;
			miInfo.cbSize = sizeof(MENUITEMINFO);
			miInfo.fMask = MIIM_TYPE;
			miInfo.fType = MFT_STRING;

			string itemStr = "object." + to_string(newEngine.selectedIdx) + "の設定";
			char* cstr = new char[itemStr.size() + 1];
			strcpy(cstr, itemStr.c_str());
			miInfo.dwTypeData = cstr;
			miInfo.cch = itemStr.size() + 1;

			SetMenuItemInfo(hpMenu, SUBUI_SETTING_OBJ, 0, &miInfo);
			TrackPopupMenu(subMenu, TPM_LEFTALIGN | TPM_BOTTOMALIGN,
				po.x, po.y, 0, hWnd, NULL
			);
			delete[] cstr;
		}
			break;
			
		case WM_KEYDOWN:///------------ キーメッセージ (down) -----------
			switch(wp)
			{
				///----- 補助キー ▼
			case VK_SHIFT: shiftK = true; break;
			case VK_CONTROL: ctrlK = true; break;
			case 0x31: 
				newEngine.inPutKey(newEngine.IK::No_1, NULL); break;
			case 0x32: 
				newEngine.inPutKey(newEngine.IK::No_2, NULL); break;
			case 0x33:
				newEngine.inPutKey(newEngine.IK::No_3, NULL); break;
			case 0x34:
				newEngine.inPutKey(newEngine.IK::No_4, NULL); break;
			case 0x35:
				newEngine.inPutKey(newEngine.IK::No_5, NULL); break;
			case VK_SPACE:
				newEngine.inPutKey(newEngine.IK::SPACE, NULL); break;


			}
			break;
			
		case WM_KEYUP:///------------ キーメッセージ (up) -----------
			switch(wp)
			{
					///----- 補助キー ▼
				case VK_SHIFT: shiftK = false; break;
				case VK_CONTROL: ctrlK = false; break;
			}
			break;

		///------------ ゲームパッドメッセージ ------------
		case MM_JOY1MOVE:
			if(JOYERR_NOERROR == joyGetPosEx(0, &JoyInfoEx))
			{
				// 既にキー操作済なら退出
				if (newEngine.ope.inputByKey)
					break;
				double xMaxHf = newEngine.joyCaps.wXmax * 0.5;
				double yMaxHf = newEngine.joyCaps.wYmax * 0.5;
				double zMaxHf = newEngine.joyCaps.wZmax * 0.5;
				double uMaxHf = newEngine.joyCaps.wUmax * 0.5;
				double rMaxHf = newEngine.joyCaps.wRmax * 0.5;

				pt3 gLoc = pt3(	-(LOWORD(JoyInfoEx.dwZpos)-zMaxHf) / zMaxHf,	// 前後
								 (LOWORD(JoyInfoEx.dwXpos)-xMaxHf) / xMaxHf,	// 左右
								-(LOWORD(JoyInfoEx.dwYpos)-yMaxHf) / yMaxHf)	// 上下
							.mtp(0.32767 * newEngine.adjSpd * powi(3.0, newEngine.ope.speed));	// 係数
				
				pt2 gRot = pt2(	-(LOWORD(JoyInfoEx.dwUpos)-uMaxHf) / uMaxHf,	//左右
								-(LOWORD(JoyInfoEx.dwRpos)-rMaxHf) / rMaxHf)	//上下
							.mtp(0.32767 * 0.3);

				// 位置
				double lLim = 0.05 * newEngine.adjSpd * powi(3.0, newEngine.ope.speed);
				newEngine.ope.cmLoc.x = (lLim < abs(gLoc.x)) ? gLoc.x : 0.0;
				newEngine.ope.cmLoc.y = (lLim < abs(gLoc.y)) ? gLoc.y : 0.0;
				newEngine.ope.cmLoc.z = (lLim < abs(gLoc.z)) ? gLoc.z : 0.0;
				
				// 回転
				newEngine.ope.cmRot.x = (0.02 < abs(gRot.x)) ? gRot.x * newEngine.adjSpd : 0.0;
				newEngine.ope.cmRot.y = (0.02 < abs(gRot.y)) ? gRot.y * newEngine.adjSpd : 0.0;
				if (JoyInfoEx.dwButtons == 16)
					newEngine.ope.cmRot.z = newEngine.adjSpd * -0.1;
				else if (JoyInfoEx.dwButtons == 32)
					newEngine.ope.cmRot.z = newEngine.adjSpd * 0.1;
				else
					newEngine.ope.cmRot.z = 0.0;

				// マップ
				newEngine.ope.chgMapState = JoyInfoEx.dwPOV;
				if (newEngine.ope.chgMapState != newEngine.ope.chgMapStateOld)
				{
					if (newEngine.ope.chgMapState == JOY_POVFORWARD)
					{
						newEngine.inPutKey(newEngine.IK::No_4, NULL); break;
					}
					else if (newEngine.ope.chgMapState == JOY_POVLEFT)
					{
						newEngine.inPutKey(newEngine.IK::No_5, NULL); break;
					}
				}

				// 後ろを見る
				newEngine.ope.cmBack = (JoyInfoEx.dwButtons & JOY_BUTTON10) ? true : false;
			}
			break;

		case MM_JOY1BUTTONDOWN:	//-- ボタン操作
			if(JOYERR_NOERROR == joyGetPosEx(0, &JoyInfoEx))
			{
				//cout << ((JoyInfoEx.dwButtons))  << endl;
				if(JoyInfoEx.dwButtons==4) newEngine.ope.speed = ++newEngine.ope.speed%3;
				else if(JoyInfoEx.dwButtons==1) newEngine.shoot();
				else if(JoyInfoEx.dwButtons==2) newEngine.obMove = !newEngine.obMove;
				else if(JoyInfoEx.dwButtons==8) 
					newEngine.PLR_No = newEngine.BWH_QTY 
						+ ((newEngine.PLR_No-newEngine.BWH_QTY+1) % newEngine.PLR_QTY);
			}
			break;

		default:
			return (DefWindowProc(hWnd, msg, wp, lp));
	}
	return 0;

 }else{///=======サブ
	
			return (DefWindowProc(hWnd, msg, wp, lp));
 }
}

// プロシージャ
INT_PTR CALLBACK DialogProc(HWND hDlg, UINT msg, WPARAM wp, LPARAM lp){

	switch(msg){
		case WM_COMMAND:
			switch(LOWORD(wp)){
			case IDOK:
				EndDialog(hDlg, IDOK);
				return true;
			}
		case WM_DESTROY:
			EndDialog(hDlg, IDOK);
			return true;
		
	}
	return false;

};
INT_PTR CALLBACK howToDlgProc(HWND hDlg, UINT msg, WPARAM wp, LPARAM lp){

	switch(msg){
		case WM_COMMAND:
			switch(LOWORD(wp)){
			case IDOK:
				EndDialog(hDlg, IDOK);
				return true;
			}
			break;
		case WM_DESTROY:
			EndDialog(hDlg, IDOK);
			return true;
		
	}
	return false;

};
// Pythonエディタプロシージャ
INT_PTR CALLBACK EditorProc(HWND hDlg, UINT msg, WPARAM wp, LPARAM lp) {
	
	switch (msg) {
	case WM_COMMAND:
		switch (LOWORD(wp)) {
		case IDOK:
			GetDlgItemText(editDlg, EDITDLG_CODE_TXT, CppPythonIF::rawCode, 8186);
			return true;
		case IDCANCEL:
			DestroyWindow(editDlg);
			return true;
		}
		break;
	case WM_DESTROY:
		editDlg = nullptr;
		return true;

	}
	return false;

};

// オブジェクト設定ダイアログ
INT_PTR CALLBACK ModObjProc(HWND hDlg, UINT msg, WPARAM wp, LPARAM lp) 
{
	// スライダー変数
	LONG_PTR sVal;
	LONG_PTR sMin = 0;
	LONG_PTR sMax = 100;
	const double velocMax = 10;
	const double rotvMax = 15;
	const double scaleMax = 30;
	double sRst;
	static int trgObjIdx;


	switch (msg) {
	case WM_INITDIALOG:
	{
		trgObjIdx = newEngine.selectedIdx;
		SendDlgItemMessage(hDlg, MODOBJ_SCALE_SLIDER, TBM_SETRANGE, true, MAKELPARAM(sMin, sMax));
		SendDlgItemMessage(hDlg, MODOBJ_VELOC_SLIDER, TBM_SETRANGE, true, MAKELPARAM(sMin, sMax));
		SetDlgItemText(hDlg, MODOBJ_OBJNO_TXT, to_string(trgObjIdx).c_str());

		if (!newEngine.CheckSelectedEnable())
		{
			DestroyWindow(modObjDlg);
			return true;
		}

		// 現在の速度, スケールを取得してダイアログに反映
		auto SetSliderVal = [sMin, sMax, hDlg](double inVal, int sliderID, double maxVal)
		{
			LONG_PTR sVal = (int)((inVal / maxVal) * sMax);
			if (sVal < sMin) sVal = sMin; else if (sMax < sVal) sVal = sMax;	// adjust
			SendDlgItemMessage(
				hDlg,
				sliderID,
				TBM_SETPOS,
				true,
				sVal
				);
		};
		// scale
		sRst = newEngine.objs[newEngine.selectedIdx].scale;
		SetSliderVal(sRst, MODOBJ_SCALE_SLIDER, scaleMax);
		SetDlgItemText(hDlg, MODOBJ_SCALE_TXT, to_string((long double)sRst).c_str());
		// velocity
		sRst = newEngine.objs[newEngine.selectedIdx].lspX.w;
		SetSliderVal(sRst, MODOBJ_VELOC_SLIDER, velocMax);
		SetDlgItemText(hDlg, MODOBJ_VELOC_TXT, to_string((long double)sRst).c_str());
		// rot x velocity
		sRst = newEngine.objs[newEngine.selectedIdx].rsp.x / PIE * 180;
		SetSliderVal(sRst, MODOBJ_ROTX_SLIDER, rotvMax);
		SetDlgItemText(hDlg, MODOBJ_ROTX_TXT, to_string((long double)sRst).c_str());

		// add comboBox strings
		HWND cmb = GetDlgItem(hDlg, MODOBJ_MESH_CBOX);
		for (int i=0; i<newEngine.meshLen; i++)
		{
			ComboBox_AddString(cmb, newEngine.meshNames[i]);
		}
		return true;
	}

	case WM_DESTROY:
		modObjDlg = nullptr;
		return true;

	case WM_COMMAND:
		switch (LOWORD(wp)) {
		case IDOK:
			//GetDlgItemText(modObjDlg, EDITDLG_CODE_TXT, CppPythonIF::rawCode, 8186);
			return true;
		case IDCANCEL:
			DestroyWindow(modObjDlg);
			return true;
		}
		break;

	case WM_HSCROLL:
		if (!newEngine.CheckSelectedEnable(trgObjIdx))
			break;

		if (lp != 0)
		{
			sVal = SendMessage((HWND)lp, (UINT)TBM_GETPOS, NULL, NULL);
			// スライダー特定
			int sliderID = GetDlgCtrlID((HWND)lp);

			// スライダーに応じた処理
			switch (sliderID)
			{
			case MODOBJ_SCALE_SLIDER:
				sRst = (double)sVal / sMax * scaleMax;
				newEngine.objs[trgObjIdx].scale = sRst;
				SetDlgItemText(hDlg, MODOBJ_SCALE_TXT, to_string((long double)sRst).c_str());
				break;

			case MODOBJ_VELOC_SLIDER:
				sRst = (double)sVal / sMax * velocMax;
				newEngine.objs[trgObjIdx].lspX.w = sRst;
				SetDlgItemText(hDlg, MODOBJ_VELOC_TXT, to_string((long double)sRst).c_str());
				break;

			case MODOBJ_ROTX_SLIDER:
				sRst = (double)sVal / sMax * rotvMax;
				newEngine.objs[trgObjIdx].rsp.x = sRst / 180 * PIE;
				SetDlgItemText(hDlg, MODOBJ_ROTX_TXT, to_string((long double)sRst).c_str());
				break;
			}
		}
		return true;

	}
	return false;

};

// オブジェクト操作プロシージャ
INT_PTR CALLBACK moveObjProc(HWND hDlg, UINT msg, WPARAM wp, LPARAM lp)
{
	// スライダー変数
	LONG_PTR sVal;
	LONG_PTR sMin;
	LONG_PTR sMax;
	double sRst;

	sMin = SendDlgItemMessage
	(
		hDlg,    
		MODLG_LON_SLIDER,
		TBM_GETRANGEMIN,
		NULL,
		NULL
	);
	sMax = SendDlgItemMessage
	(
		hDlg,      
		MODLG_LON_SLIDER,
		TBM_GETRANGEMAX,
		NULL,
		NULL
	);

	switch(msg){
		case WM_INITDIALOG:
			// 現在のloc・stdを取得してダイアログに反映
			sVal = (int)(((sRst = newEngine.objs[newEngine.PLR_No].loc.x)/PIE+1)*0.5 *(sMax-sMin)+sMin);
			if(sVal<sMin) sVal = sMin; else if(sMax<sVal) sVal = sMax;
			SendDlgItemMessage(
				hDlg,        // トラックバーのハンドル
				MODLG_LON_SLIDER,
				TBM_SETPOS,
				true,
				sVal
			);
			sRst = -1*(sRst/PIE*180);
			SetDlgItemText(hDlg, MODLG_LON_TXT, to_string((long double)sRst).c_str());

			sVal = (int)(((sRst = newEngine.objs[newEngine.PLR_No].loc.y)/PIE) *(sMax-sMin)+sMin);
			if(sVal<sMin) sVal = sMin; else if(sMax<sVal) sVal = sMax;
			SendDlgItemMessage(
				hDlg,        // トラックバーのハンドル
				MODLG_LAT_SLIDER,
				TBM_SETPOS,
				true,
				sVal
			);
			sRst = -1*(sRst/PIE*180-90);
			SetDlgItemText(hDlg, MODLG_LAT_TXT, to_string((long double)sRst).c_str());

			sVal = (int)(((sRst = newEngine.objs[newEngine.PLR_No].loc.z)/PIE) *(sMax-sMin)+sMin);
			if(sVal<sMin) sVal = sMin; else if(sMax<sVal) sVal = sMax;
			SendDlgItemMessage(
				hDlg,        // トラックバーのハンドル
				MODLG_ALT_SLIDER,
				TBM_SETPOS,
				true,
				sVal
			);
			sRst = -1*(sRst/PIE*180-90);
			SetDlgItemText(hDlg, MODLG_ALT_TXT, to_string((long double)sRst).c_str());

			newEngine.mvObjParam.loc.asg2(newEngine.objs[newEngine.PLR_No].loc);
			
			//
			sVal = (int)(((sRst = newEngine.objs[newEngine.PLR_No].std[0].x)/PIE+1)*0.5 *(sMax-sMin)+sMin);
			if(sVal<sMin) sVal = sMin; else if(sMax<sVal) sVal = sMax;
			SendDlgItemMessage(
				hDlg,        // トラックバーのハンドル
				MODLG_LON_SLIDER2,
				TBM_SETPOS,
				true,
				sVal
			);
			sRst = -1*(sRst/PIE*180);
			SetDlgItemText(hDlg, MODLG_LON_TXT2, to_string((long double)sRst).c_str());

			sVal = (int)(((sRst = newEngine.objs[newEngine.PLR_No].std[0].y)/PIE) *(sMax-sMin)+sMin);
			if(sVal<sMin) sVal = sMin; else if(sMax<sVal) sVal = sMax;
			SendDlgItemMessage(
				hDlg,        // トラックバーのハンドル
				MODLG_LAT_SLIDER2,
				TBM_SETPOS,
				true,
				sVal
			);
			sRst = -1*(sRst/PIE*180-90);
			SetDlgItemText(hDlg, MODLG_LAT_TXT2, to_string((long double)sRst).c_str());

			sVal = (int)(((sRst = newEngine.objs[newEngine.PLR_No].std[0].z)/PIE) *(sMax-sMin)+sMin);
			if(sVal<sMin) sVal = sMin; else if(sMax<sVal) sVal = sMax;
			SendDlgItemMessage(
				hDlg,        // トラックバーのハンドル
				MODLG_ALT_SLIDER2,
				TBM_SETPOS,
				true,
				sVal
			);
			sRst = -1*(sRst/PIE*180-90);
			SetDlgItemText(hDlg, MODLG_ALT_TXT2, to_string((long double)sRst).c_str());

			newEngine.mvObjParam.rot.asg2(newEngine.objs[newEngine.PLR_No].std[0]);

			newEngine.mvObjFlg = true;

			return true;

		case WM_COMMAND:
			switch(LOWORD(wp)){
			case IDOK:
				EndDialog(hDlg, IDOK);
				return true;

			case IDCANCEL:
				newEngine.mvObjFlg = false;
				EndDialog(hDlg, IDOK);
				return true;


			default:
				return true;

			}
		case WM_DESTROY:
			EndDialog(hDlg, IDOK);
			return true;

		case WM_HSCROLL:
			if (lp != 0) 
			{
				sVal = SendMessage(
					(HWND) lp,        // トラックバーのハンドル
					(UINT) TBM_GETPOS,
					NULL,
					NULL
				);
				// スライダー特定
				int sliderID = GetDlgCtrlID((HWND)lp);

				// スライダーに応じた処理
				switch(sliderID)
				{
					// 位置
					case MODLG_LON_SLIDER:
						newEngine.mvObjParam.loc.x = sRst = (((double)sVal-sMin)/((double)sMax-sMin)*2-1)*PIE;
						sRst = -1*(sRst/PIE*180);
						SetDlgItemText(hDlg, MODLG_LON_TXT, to_string((long double)sRst).c_str());
						break;

					case MODLG_LAT_SLIDER:
						newEngine.mvObjParam.loc.y = sRst = (((double)sVal-sMin)/((double)sMax-sMin))*PIE;
						sRst = -1*(sRst/PIE*180-90);
						SetDlgItemText(hDlg, MODLG_LAT_TXT, to_string((long double)sRst).c_str());
						break;

					case MODLG_ALT_SLIDER:
						newEngine.mvObjParam.loc.z = sRst = (((double)sVal-sMin)/((double)sMax-sMin))*PIE;
						sRst = -1*(sRst/PIE*180-90);
						SetDlgItemText(hDlg, MODLG_ALT_TXT, to_string((long double)sRst).c_str());
						break;

					// 方向
					case MODLG_LON_SLIDER2:
						newEngine.mvObjParam.rot.x = sRst = (((double)sVal-sMin)/((double)sMax-sMin)*2-1)*PIE;
						sRst = -1*(sRst/PIE*180);
						SetDlgItemText(hDlg, MODLG_LON_TXT2, to_string((long double)sRst).c_str());
						break;

					case MODLG_LAT_SLIDER2:
						newEngine.mvObjParam.rot.y = sRst = (((double)sVal-sMin)/((double)sMax-sMin))*PIE;
						sRst = -1*(sRst/PIE*180-90);
						SetDlgItemText(hDlg, MODLG_LAT_TXT2, to_string((long double)sRst).c_str());
						break;

					case MODLG_ALT_SLIDER2:
						newEngine.mvObjParam.rot.z = sRst = (((double)sVal-sMin)/((double)sMax-sMin))*PIE;
						sRst = -1*(sRst/PIE*180-90);
						SetDlgItemText(hDlg, MODLG_ALT_TXT2, to_string((long double)sRst).c_str());
						break;
				}
				newEngine.mvObjFlg = true;

			} 
			return true;
		
	}
	return false;
}


