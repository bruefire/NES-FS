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
#include "VR_Manager.h"
#include "engine3dWin.h"
#include "engine3dWinOVR.h"
#include "functions.h"
#include "UI_DEF.h"
#include "rcFunc.h"
#include "modlgRc.h"
#include "randomUI.h"
#include "editor.h"
#include "objSetting.h"
using namespace std;

// procedures..
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK DialogProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK RandomRelocProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK howToDlgProc(HWND hDlg, UINT msg, WPARAM wp, LPARAM lp);
INT_PTR CALLBACK moveObjProc(HWND hDlg, UINT msg, WPARAM wp, LPARAM lp);
INT_PTR CALLBACK EditorProc(HWND hDlg, UINT msg, WPARAM wp, LPARAM lp);
INT_PTR CALLBACK ModObjProc(HWND hDlg, UINT msg, WPARAM wp, LPARAM lp);

//
ATOM InitApp(HINSTANCE);
HWND InitInstance(HINSTANCE, int);
HWND InitStdWndFunc(HINSTANCE hCurInst, int nCmdShow);
bool StdWndMsgLoop(MSG*);


TCHAR szClassName[] = TEXT("3d_engine");	//ウィンドウクラス
engine3dWin* newEngine = nullptr;
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
engine3d::RandMode rdmRandMode;



int WINAPI WinMain(HINSTANCE hCurInst, HINSTANCE hPrevInst, LPSTR lpsCmdLine, int nCmdShow)
{
	//InitCommonControls();
	curInst = hCurInst;
	GetCurrentDirectory(MAX_PATH, curDir);

	//-- create console
    if(!AllocConsole()) return 0;
    freopen("CONOUT$", "w", stdout);
    freopen("CONIN$", "r", stdin);

	//---- handle command strings;
	string awakeCmd = GetCommandLine();
	cout << awakeCmd << endl;
	int awIdx = awakeCmd.find_last_of(" ") +1;
	 
	if(awIdx <= awakeCmd.length()){
		if (awakeCmd.substr(awIdx) == "sim:S3;lang:EN;view:VR")
		{
			newEngine = new engine3dWinOVR();
			newEngine->lang = UI_LANG_JA;
			newEngine->worldGeo = engine3d::WorldGeo::SPHERICAL;
		}
		else if (awakeCmd.substr(awIdx) == "sim:H3;lang:EN;view:VR")
		{
			newEngine = new engine3dWinOVR();
			newEngine->lang = UI_LANG_JA;
			newEngine->worldGeo = engine3d::WorldGeo::HYPERBOLIC;
		}
		else if (awakeCmd.substr(awIdx) == "sim:H3;lang:JA")
		{
			menuName = "KITTY_H3";
			titleName = "NES-FS -双曲空間-";
			newEngine = new engine3dWin();
			newEngine->lang = UI_LANG_JA;
			newEngine->worldGeo = engine3d::WorldGeo::HYPERBOLIC;
		}
		else if (awakeCmd.substr(awIdx) == "sim:H3;lang:EN")
		{
			menuName = "KITTY_H3_EN";
			titleName = "NES-FS -Hyperbolic Space-";
			newEngine = new engine3dWin();
			newEngine->lang = UI_LANG_EN;
			newEngine->worldGeo = engine3d::WorldGeo::HYPERBOLIC;
		}
		else if(awakeCmd.substr(awIdx) == "lang:EN")
		{
			menuName = "KITTY_EN";
			titleName = "NES-FS -Spherical Space-";
			newEngine = new engine3dWin();
			newEngine->lang = UI_LANG_EN;
			newEngine->worldGeo = engine3d::WorldGeo::SPHERICAL;
		}
		else
		{
			menuName = "KITTY";
			titleName = "NES-FS -球面空間-";
			newEngine = new engine3dWin();
			newEngine->lang = UI_LANG_JA;
			newEngine->worldGeo = engine3d::WorldGeo::SPHERICAL;
		}
		cout << awakeCmd.substr(awIdx) << endl;
	}


	// initialize the simulator
	void* iparam[2] = {&hCurInst, &nCmdShow};
	if (!newEngine->init(iparam, InitStdWndFunc, StdWndMsgLoop))
		PostQuitMessage(0);

	///-- init gamepad
	JoyInfoEx.dwSize = sizeof(JOYINFOEX);
	JoyInfoEx.dwFlags = JOY_RETURNALL;
	if (JOYERR_NOERROR == joySetCapture(preWnd, JOYSTICKID1, 1, FALSE))
	{
		joyGetDevCaps(JOYSTICKID1, &newEngine->joyCaps, sizeof(JOYCAPS));
		newEngine->useJoyPad = true;
		cout << "joypad1 is avaliable." << endl;
	}


	//**** main processing ****//
	int result = newEngine->start();


	///-- dispose resources
	newEngine->dispose();
	delete newEngine;
	FreeConsole();
	

	return result;
}


// 標準ウィンドウ初期化処理
HWND InitStdWndFunc(HINSTANCE hCurInst, int nCmdShow)
{
	hpMenu = LoadMenu(hCurInst, "POPUP");
	subMenu = GetSubMenu(hpMenu, 0);
	//------------

	if (!InitApp(hCurInst)) 
		return nullptr;

	HWND hWnd = InitInstance(hCurInst, nCmdShow);
	if (!hWnd) 
		return nullptr;

	MENUITEMINFO menuItemInfo = { sizeof(MENUITEMINFO), MIIM_STATE };
	menuCheckDef(hMenu, &menuItemInfo);

	return hWnd;
}

// 標準ウィンドウ用メッセージ処理
bool StdWndMsgLoop(MSG* msg)
{
	while (true)
	{
		if (!(PeekMessage(msg, NULL, 0, 0, PM_REMOVE)) != 0)
			return true;

		if (msg->message == WM_QUIT)
			return false;

		// まず子ダイアログにメッセージ処理を試す
		if (editDlg && IsDialogMessage(editDlg, msg))
			continue;
		else if (modObjDlg && IsDialogMessage(modObjDlg, msg))
			continue;

		TranslateMessage(msg);	//メッセージを変換
		DispatchMessage(msg);	//メッセージを送出
	}

	return false;
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
HWND InitInstance(HINSTANCE hInst, int nCmdShow)
{
	HWND hWnd = CreateWindow(szClassName,	//クラス名
				titleName,	//ウィンドウ名
				WS_OVERLAPPEDWINDOW,	//ウィンドウスタイル
				CW_USEDEFAULT,	//x位置
				CW_USEDEFAULT,	//y位置
				newEngine->WIDTH + GetSystemMetrics(SM_CXFRAME)*2,	//xウィンドウ幅
				newEngine->HEIGHT + GetSystemMetrics(SM_CYFRAME)*2
					+ GetSystemMetrics(SM_CYCAPTION)
					+ GetSystemMetrics(SM_CYMENU),	//ウィンドウ高さ
				NULL,	//親ウィンドウのハンドル、親を作るときはNULL
				hMenu,	//メニューハンドル、クラスメニューを使うときはNULL
				hInst,	//インスタンスハンドル
				NULL	//ウィンドウ作成データ
			);

	if(!hWnd) return nullptr; 
	else preWnd = hWnd;
	hMenu = GetMenu( hWnd );

	ShowWindow(hWnd, nCmdShow);	//ウィンドウの表示状態を設定
	UpdateWindow(hWnd);	//ウィンドウを更新
	
	return hWnd;
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
				newEngine->WIDTH = lp & 0xFFFF;
				newEngine->HEIGHT = (lp>>16) & 0xFFFF;
				newEngine->CR_RANGE_Y = newEngine->clcRangeY(newEngine->CR_RANGE_X);	//カメラ設定
				glViewport(0, 0, newEngine->WIDTH, newEngine->HEIGHT);	//-- GLwndサイズ更新
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
					newEngine->fps = newEngine->STD_PMSEC;
					newEngine->adjSpd = 1.0;
					menuCheck(hMenu, &menuItemInfo, UI_FPS_30, unCk_fps, unCk_fps_len);
					break;
				case UI_FPS_60:
					newEngine->fps = newEngine->MAX_PMSEC;
					newEngine->adjSpd = 0.5;
					menuCheck(hMenu, &menuItemInfo, UI_FPS_60, unCk_fps, unCk_fps_len);
					break;
				case UI_FPS_VFR:
					newEngine->fps = newEngine->MAX_PMSEC;
					newEngine->adjSpd = 1.0;
					menuCheck(hMenu, &menuItemInfo, UI_FPS_VFR, unCk_fps, unCk_fps_len);
					break;
				//-- 言語
				case UI_LANG_JA:
					newEngine->CreateNewEngine(" lang:JA");
					PostQuitMessage(0);
					break;
				case UI_LANG_EN:
					newEngine->CreateNewEngine(" lang:EN");
					PostQuitMessage(0);
					break;

				case UI_H3_JA:
					newEngine->CreateNewEngine(" sim:H3;lang:JA");
					PostQuitMessage(0);
					break;

				case UI_H3_EN:
					newEngine->CreateNewEngine(" sim:H3;lang:EN");
					PostQuitMessage(0);
					break;

				case UI_S3_VR:
					newEngine->CreateNewEngine(" sim:S3;lang:EN;view:VR");
					PostQuitMessage(0);
					break;

				case UI_H3_VR:
					newEngine->CreateNewEngine(" sim:H3;lang:EN;view:VR");
					PostQuitMessage(0);
					break;

				case UI_THROW_CLEAR:
					newEngine->ClearFloatObjs();
					break;

				case UI_THROW_RANDOM:
					newEngine->player.ep = 0;
					rdmRandMode = engine3d::RandMode::Cluster;
					DialogBox(curInst, "RDM_OBJ_DLG", preWnd, RandomRelocProc); 
					break;

				case UI_THROW_RANDOM2:
					newEngine->player.ep = 0;
					rdmRandMode = engine3d::RandMode::Uniform;
					DialogBox(curInst, "RDM_OBJ_DLG", preWnd, RandomRelocProc);
					break;

				case UI_CR_S:
					newEngine->CR_RANGE_X = 70;
					newEngine->CR_RANGE_Y = newEngine->clcRangeY( newEngine->CR_RANGE_X );	//カメラ設定
					menuCheck(hMenu, &menuItemInfo, UI_CR_S, unCk_rng, unCk_rng_len);
					break;
				case UI_CR_M:
					newEngine->CR_RANGE_X = 90;
					newEngine->CR_RANGE_Y = newEngine->clcRangeY( newEngine->CR_RANGE_X );	//カメラ設定
					menuCheck(hMenu, &menuItemInfo, UI_CR_M, unCk_rng, unCk_rng_len);
					break;
				case UI_CR_L:
					newEngine->CR_RANGE_X = 110;
					newEngine->CR_RANGE_Y = newEngine->clcRangeY( newEngine->CR_RANGE_X );	//カメラ設定
					menuCheck(hMenu, &menuItemInfo, UI_CR_L, unCk_rng, unCk_rng_len);
					break;

				case UI_SD_0:
					newEngine->decMode = 0;
					menuCheck(hMenu, &menuItemInfo, UI_SD_0, unCk_dec, unCk_dec_len);
					break;
				case UI_SD_1:
					newEngine->decMode= 1;
					menuCheck(hMenu, &menuItemInfo, UI_SD_1, unCk_dec, unCk_dec_len);
					break;
				case UI_SD_2:
					newEngine->decMode = 2;
					menuCheck(hMenu, &menuItemInfo, UI_SD_2, unCk_dec, unCk_dec_len);
					break;
				case UI_SD_4:
					newEngine->decMode = 4;
					menuCheck(hMenu, &menuItemInfo, UI_SD_4, unCk_dec, unCk_dec_len);
					break;

				case UI_BG_WH:
					newEngine->bgCol = 1;
					menuCheck(hMenu, &menuItemInfo, UI_BG_WH, unCk_bg, unCk_bg_len);
					break;
				case UI_BG_BK:
					newEngine->bgCol = 0; 
					menuCheck(hMenu, &menuItemInfo, UI_BG_BK, unCk_bg, unCk_bg_len);
					break;

				case UI_RDS_SS:
					newEngine->radius = 20;
					menuCheck(hMenu, &menuItemInfo, UI_RDS_SS, unCk_rds, unCk_rds_len);
					break;
				case UI_RDS_S:
					newEngine->radius = 30;
					menuCheck(hMenu, &menuItemInfo, UI_RDS_S, unCk_rds, unCk_rds_len);
					break;
				case UI_RDS_M:
					newEngine->radius = 60;
					menuCheck(hMenu, &menuItemInfo, UI_RDS_M, unCk_rds, unCk_rds_len);
					break;
				case UI_RDS_L:
					newEngine->radius = 100;
					menuCheck(hMenu, &menuItemInfo, UI_RDS_L, unCk_rds, unCk_rds_len);
					break;

				case UI_MAP:
					newEngine->VIEW_ON4 = !newEngine->VIEW_ON4;
					menuCheck2(hMenu, &menuItemInfo, UI_MAP);
					break;
				case UI_XYZ:
					newEngine->VIEW_XYZ = !newEngine->VIEW_XYZ;
					menuCheck2(hMenu, &menuItemInfo, UI_XYZ);
					break;
				case UI_LR_RESULT:
					newEngine->VIEW_LocRot = !newEngine->VIEW_LocRot; 
					menuCheck2(hMenu, &menuItemInfo, UI_LR_RESULT);
					break;
				case UI_PLR:
					newEngine->VIEW_PLR = !newEngine->VIEW_PLR;
					menuCheck2(hMenu, &menuItemInfo, UI_PLR);
					break;
				case UI_DST:
					newEngine->VIEW_DST = !newEngine->VIEW_DST;
					menuCheck2(hMenu, &menuItemInfo, UI_DST);
					break;
				case UI_PAST:
					newEngine->markObj.used = !newEngine->markObj.used;
					menuCheck2(hMenu, &menuItemInfo, UI_PAST);
					break;

				case UI_WL_NONE:
					newEngine->ChangeBasicObject(-1, 0);
					menuCheck(hMenu, &menuItemInfo, UI_WL_NONE, unCk_mark, unCk_mark_len);
					break;
				case UI_6_POINT:
					newEngine->ChangeBasicObject(0, 0);
					menuCheck(hMenu, &menuItemInfo, UI_6_POINT, unCk_mark, unCk_mark_len);
					break;
				case UI_6_LINE:
					newEngine->ChangeBasicObject(0, 1);
					menuCheck(hMenu, &menuItemInfo, UI_6_LINE, unCk_mark, unCk_mark_len);
					break;
				case UI_TUDE_POINT:
					newEngine->ChangeBasicObject(3, 0);
					menuCheck(hMenu, &menuItemInfo, UI_TUDE_POINT, unCk_mark, unCk_mark_len);
					break;
				case UI_TUDE_LINE:
					newEngine->ChangeBasicObject(3, 1);
					menuCheck(hMenu, &menuItemInfo, UI_TUDE_LINE, unCk_mark, unCk_mark_len);
					break;
				case UI_120C_POINT:
					newEngine->ChangeBasicObject(10, 0);
					menuCheck(hMenu, &menuItemInfo, UI_120C_POINT, unCk_mark, unCk_mark_len);
					break;
				case UI_120C_LINE:
					newEngine->ChangeBasicObject(10, 1);
					menuCheck(hMenu, &menuItemInfo, UI_120C_LINE, unCk_mark, unCk_mark_len);
					break;
				case UI_Dodeca_POINT:
					newEngine->ChangeBasicObject(24, 0);
					menuCheck(hMenu, &menuItemInfo, UI_Dodeca_POINT, unCk_mark, unCk_mark_len);
					break;
				case UI_Dodeca_LINE:
					newEngine->ChangeBasicObject(24, 1);
					menuCheck(hMenu, &menuItemInfo, UI_Dodeca_LINE, unCk_mark, unCk_mark_len);
					break;
				case UI_Dodeca_SURFACE:
					newEngine->ChangeBasicObject(25, 2);
					menuCheck(hMenu, &menuItemInfo, UI_Dodeca_SURFACE, unCk_mark, unCk_mark_len);
					break;
				case UI_120C_SURFACE:
					newEngine->ChangeBasicObject(11, 2);
					menuCheck(hMenu, &menuItemInfo, UI_120C_SURFACE, unCk_mark, unCk_mark_len);
					break;
				case UI_TORUS_POINT:
					newEngine->ChangeBasicObject(12, 0);
					menuCheck(hMenu, &menuItemInfo, UI_TORUS_POINT, unCk_mark, unCk_mark_len);
					break;
				case UI_TORUS_LINE:
					newEngine->ChangeBasicObject(12, 1);
					menuCheck(hMenu, &menuItemInfo, UI_TORUS_LINE, unCk_mark, unCk_mark_len);
					break;
				case UI_TORUS_SURFACE:
					newEngine->ChangeBasicObject(13, 2);
					menuCheck(hMenu, &menuItemInfo, UI_TORUS_SURFACE, unCk_mark, unCk_mark_len);
					break;
				case UI_PLANE_SURFACE:
					newEngine->ChangeBasicObject(21, 2);
					menuCheck(hMenu, &menuItemInfo, UI_PLANE_SURFACE, unCk_mark, unCk_mark_len);
					break;
				case UI_MK_EARTH:
					newEngine->ChangeBasicObject(2, 2);
					menuCheck(hMenu, &menuItemInfo, UI_MK_EARTH, unCk_mark, unCk_mark_len);
					break;
				case UI_MK_TC:
					newEngine->ChangeBasicObject(7, 2, true);
					menuCheck(hMenu, &menuItemInfo, UI_MK_TC, unCk_mark, unCk_mark_len);
					break;

				case UI_THROW_SPHERE:
					newEngine->ChangeThrowObject(8);
					menuCheck(hMenu, &menuItemInfo, UI_THROW_SPHERE, unCk_thrw, unCk_thrw_len);
					break;
				case UI_THROW_CUBE:
					newEngine->ChangeThrowObject(4);
					menuCheck(hMenu, &menuItemInfo, UI_THROW_CUBE, unCk_thrw, unCk_thrw_len);
					break;
				case UI_THROW_HORSE:
					newEngine->ChangeThrowObject(5);
					menuCheck(hMenu, &menuItemInfo, UI_THROW_HORSE, unCk_thrw, unCk_thrw_len); 
					break;
				case UI_THROW_ROCK:
					newEngine->ChangeThrowObject(14);
					menuCheck(hMenu, &menuItemInfo, UI_THROW_ROCK, unCk_thrw, unCk_thrw_len); 
					break;
				case UI_THROW_REF:
					if( GetOpenFileName( &ofName ) ){	//-- ファイル参照
						SetCurrentDirectory(curDir);
						newEngine->meshs[9].~mesh3d();
						newEngine->meshs[9].Init();
						refName[ strlen(refName)-strlen(".obj") ] = 0x00;
						if(!newEngine->meshs[9].meshInit( refName, 9+1, 1))
						{
							newEngine->meshs[9].meshInit( newEngine->meshNames[9], 9+1, 0);
						}
						if (newEngine->meshs[9].faces != nullptr)
							newEngine->MakeCommonVBO((mesh3dGL*)(newEngine->meshs + 9));

						for(int i=newEngine->BWH_QTY+newEngine->PLR_QTY; i<newEngine->OBJ_QTY; i++)
							newEngine->objs[i].mesh = newEngine->meshs+9;
						menuCheck(hMenu, &menuItemInfo, UI_THROW_REF, unCk_thrw, unCk_thrw_len); 
					}
					break;

				case UI_MV_CONST:
					newEngine->GRAVITY = 0;
					for (int h = 0; h < newEngine->OBJ_QTY; h++) {
						newEngine->objs[h].fc.asg(0, 0, 0, 0);
						newEngine->objs[h].mkLspX_S3( newEngine->objs[h].lspX );
					}
					menuCheck(hMenu, &menuItemInfo, UI_MV_CONST, unCk_mv, unCk_mv_len);
					break;
				case UI_MV_ATTRACT:
					if(!newEngine->GRAVITY) newEngine->all_cnvForce();
					newEngine->GRAVITY = 1;
					menuCheck(hMenu, &menuItemInfo, UI_MV_ATTRACT, unCk_mv, unCk_mv_len);
					break;
				case UI_MV_REPULSE:
					if(!newEngine->GRAVITY) newEngine->all_cnvForce();
					newEngine->GRAVITY = 2;
					menuCheck(hMenu, &menuItemInfo, UI_MV_REPULSE, unCk_mv, unCk_mv_len);
					break;

				case UI_LOC_EUC:
					newEngine->LOC_MODE = 0; 
					menuCheck(hMenu, &menuItemInfo, UI_LOC_EUC, unCk_xyz, unCk_xyz_len); 
					break;
				case UI_LOC_TUDE:
					newEngine->LOC_MODE = 1; 
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
					newEngine->h3objLoop = !newEngine->h3objLoop;
					break;

				case SUBUI_CLEAR_OBJ:
					if (newEngine->CheckSelectedEnable())
						newEngine->objs[newEngine->selectedIdx].used = false;
					break;
				case SUBUI_SETTING_OBJ:
					if (modObjDlg != nullptr)
						DestroyWindow(modObjDlg);
					
					modObjDlg = CreateDialog(curInst, "MODOBJ_DLG", preWnd, ModObjProc);
					ShowWindow(modObjDlg, SW_NORMAL);
					
					break;
				}
			}
			break;

		case WM_LBUTTONDOWN:
		{
			POINTS tmp = MAKEPOINTS(lp);
			newEngine->ope.clickState = Operation::ClickState::Left;
			newEngine->ope.clickCoord = pt2i(tmp.x, newEngine->HEIGHT - (tmp.y + 1));
		}
			break;
			
		case WM_MBUTTONDOWN:
			if(!(wp&0x0004)) cmJD = 1; else cmJD = 2;
			cm_rot[0] = cm_rot[1] = MAKEPOINTS(lp);
			break;
			
		case WM_MOUSEMOVE:
			if(cmJD){	// 左クリック中
				cm_rot[1] = MAKEPOINTS(lp);
				newEngine->inPutMouseMv
				(
					(cm_rot[1].x - cm_rot[0].x) / newEngine->MOUSE_FIX,
					(cm_rot[1].y - cm_rot[0].y) / newEngine->MOUSE_FIX,
					cmJD
				);
				cm_rot[0] = cm_rot[1];
			}
			break;

		case WM_MBUTTONUP:
			if(cmJD==1){
				newEngine->ope.cmRot.x = fmod(newEngine->ope.cmRot.x, PI*2);
				newEngine->ope.cmRot.y = fmod(newEngine->ope.cmRot.y, PI*2);
			}
			cmJD = 0;
			break;

		case WM_MOUSEWHEEL:
			if(!cmJD){
				double tmp = ((int)wp >> 16) * 0.002;

				newEngine->inPutWheel
				(
					tmp,
					shiftK
				);
			}
			break;

		case WM_RBUTTONDOWN:
			POINTS tmp = MAKEPOINTS(lp);
			newEngine->ope.clickState = Operation::ClickState::Right;
			newEngine->ope.clickCoord = pt2i(tmp.x, newEngine->HEIGHT - (tmp.y + 1));
			break;
		case WM_RBUTTONUP:
		{
			//if (!newEngine->CheckSelectedEnable())
			//	break;
			POINT po;
			po.x = LOWORD(lp);
			po.y = HIWORD(lp);
			ClientToScreen(hWnd, &po);

			MENUITEMINFO miInfo = menuItemInfo;
			miInfo.cbSize = sizeof(MENUITEMINFO);
			miInfo.fMask = MIIM_TYPE;
			miInfo.fType = MFT_STRING;

			string itemStr = (newEngine->selectedIdx != -1)
				? ("object." + to_string(newEngine->selectedIdx))
				: string("オブジェクト全体");
			itemStr += "の設定";
			
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
				newEngine->inPutKey(newEngine->IK::No_1, NULL); break;
			case 0x32: 
				newEngine->inPutKey(newEngine->IK::No_2, NULL); break;
			case 0x33:
				newEngine->inPutKey(newEngine->IK::No_3, NULL); break;
			case 0x34:
				newEngine->inPutKey(newEngine->IK::No_4, NULL); break;
			case 0x35:
				newEngine->inPutKey(newEngine->IK::No_5, NULL); break;
			case VK_SPACE:
				newEngine->inPutKey(newEngine->IK::SPACE, NULL); break;
			case VK_ESCAPE:
				newEngine->inPutKey(newEngine->IK::ESCAPE, NULL); break;


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
				if (newEngine->ope.inputByKey)
					break;
				double xMaxHf = newEngine->joyCaps.wXmax * 0.5;
				double yMaxHf = newEngine->joyCaps.wYmax * 0.5;
				double zMaxHf = newEngine->joyCaps.wZmax * 0.5;
				double uMaxHf = newEngine->joyCaps.wUmax * 0.5;
				double rMaxHf = newEngine->joyCaps.wRmax * 0.5;

				pt3 gLoc = pt3(	-(LOWORD(JoyInfoEx.dwZpos)-zMaxHf) / zMaxHf,	// 前後
								 (LOWORD(JoyInfoEx.dwXpos)-xMaxHf) / xMaxHf,	// 左右
								-(LOWORD(JoyInfoEx.dwYpos)-yMaxHf) / yMaxHf)	// 上下
							.mtp(0.32767 * newEngine->adjSpd * powi(3.0, newEngine->ope.speed));	// 係数
				
				pt2 gRot = pt2(	-(LOWORD(JoyInfoEx.dwUpos)-uMaxHf) / uMaxHf,	//左右
								-(LOWORD(JoyInfoEx.dwRpos)-rMaxHf) / rMaxHf)	//上下
							.mtp(0.32767 * 0.3);

				// 位置
				double lLim = 0.05 * newEngine->adjSpd * powi(3.0, newEngine->ope.speed);
				newEngine->ope.cmLoc.x = (lLim < abs(gLoc.x)) ? gLoc.x : 0.0;
				newEngine->ope.cmLoc.y = (lLim < abs(gLoc.y)) ? gLoc.y : 0.0;
				newEngine->ope.cmLoc.z = (lLim < abs(gLoc.z)) ? gLoc.z : 0.0;
				
				// 回転
				newEngine->ope.cmRot.x = (0.02 < abs(gRot.x)) ? gRot.x * newEngine->adjSpd : 0.0;
				newEngine->ope.cmRot.y = (0.02 < abs(gRot.y)) ? gRot.y * newEngine->adjSpd : 0.0;
				if (JoyInfoEx.dwButtons == 16)
					newEngine->ope.cmRot.z = newEngine->adjSpd * -0.1;
				else if (JoyInfoEx.dwButtons == 32)
					newEngine->ope.cmRot.z = newEngine->adjSpd * 0.1;
				else
					newEngine->ope.cmRot.z = 0.0;

				// マップ
				newEngine->ope.chgMapState = JoyInfoEx.dwPOV;
				if (newEngine->ope.chgMapState != newEngine->ope.chgMapStateOld)
				{
					if (newEngine->ope.chgMapState == JOY_POVFORWARD)
					{
						newEngine->inPutKey(newEngine->IK::No_4, NULL); break;
					}
					else if (newEngine->ope.chgMapState == JOY_POVLEFT)
					{
						newEngine->inPutKey(newEngine->IK::No_5, NULL); break;
					}
				}

				// 後ろを見る
				newEngine->ope.cmBack = (JoyInfoEx.dwButtons & JOY_BUTTON10) ? true : false;
			}
			break;

		case MM_JOY1BUTTONDOWN:	//-- ボタン操作
			if(JOYERR_NOERROR == joyGetPosEx(0, &JoyInfoEx))
			{
				//cout << ((JoyInfoEx.dwButtons))  << endl;
				if(JoyInfoEx.dwButtons==4) newEngine->ope.speed = ++newEngine->ope.speed%3;
				else if(JoyInfoEx.dwButtons==1) newEngine->shoot();
				else if(JoyInfoEx.dwButtons==2) newEngine->obMove = !newEngine->obMove;
				else if(JoyInfoEx.dwButtons==8) 
					newEngine->PLR_No = newEngine->BWH_QTY 
						+ ((newEngine->PLR_No-newEngine->BWH_QTY+1) % newEngine->PLR_QTY);
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

// infoプロシージャ
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
// random relocation procedure
INT_PTR CALLBACK RandomRelocProc(HWND hDlg, UINT msg, WPARAM wp, LPARAM lp) 
{
	// スライダー変数
	LONG_PTR sVal;
	static int sRst = 100;
	LONG_PTR sMin = 1;
	LONG_PTR sMax = 500;
	const double qtyMax = sMax;

	switch (msg) {
	case WM_INITDIALOG:
		SendDlgItemMessage(hDlg, MODLG_RDM_SLIDER, TBM_SETRANGE, true, MAKELPARAM(sMin, sMax));
		SendDlgItemMessage(
			hDlg,
			MODLG_RDM_SLIDER,
			TBM_SETPOS,
			true,
			sRst
		);
		SetDlgItemText(hDlg, MODLG_RDM_TXT, to_string(sRst).c_str());
		break;

	case WM_COMMAND:
		switch (LOWORD(wp)) {
		case IDOK:
			newEngine->RandLoc(rdmRandMode, sRst);

			EndDialog(hDlg, IDOK);
			return true;

		case IDCANCEL:
			EndDialog(hDlg, IDOK);
			return true;
		}
		return true;
	case WM_DESTROY:
		EndDialog(hDlg, IDOK);
		return true;

	case WM_HSCROLL:

		if (lp != 0)
		{
			sVal = SendMessage((HWND)lp, (UINT)TBM_GETPOS, NULL, NULL);
			// スライダー特定
			int sliderID = GetDlgCtrlID((HWND)lp);

			// スライダーに応じた処理
			switch (sliderID)
			{
			case MODLG_RDM_SLIDER:
				sRst = sVal;
				SetDlgItemText(hDlg, MODLG_RDM_TXT, to_string(sRst).c_str());
				break;
			}
		}
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
INT_PTR CALLBACK EditorProc(HWND hDlg, UINT msg, WPARAM wp, LPARAM lp) 
{
	char oldPath[256];
	GetCurrentDirectory(256, oldPath);

	char myPath[256];
	static char refName[1024];
	GetModuleFileName(NULL, myPath, 256);
	string sampleDir = myPath;
	int mpIdx = sampleDir.find_last_of("\\");
	sampleDir = sampleDir.substr(0, mpIdx) + "\\sample_script\\sample";
	strcpy(refName, sampleDir.c_str());

	static OPENFILENAME ofName = {
		sizeof(OPENFILENAME),
		hDlg,
		0,
		"Python .pyファイル {*.py}\0*.py\0"
		"All files {*.*}\0*.*\0\0",
		0,
		0,
		0,
		refName,
		1024,
		0,
		0,
		0,
		"スクリプト参照",
		OFN_FILEMUSTEXIST

	};
	
	switch (msg) {
	case WM_INITDIALOG:
		SendMessage(GetDlgItem(hDlg, EDITDLG_CODE_TXT), EM_SETLIMITTEXT, INT_MAX, 0);
		break;

	case WM_COMMAND:
		switch (LOWORD(wp)) {
		case IDOK:
		{
			LRESULT len = SendMessage(GetDlgItem(hDlg, EDITDLG_CODE_TXT), WM_GETTEXTLENGTH, 0, 0) + 1;
			CppPythonIF::rawCode.reset(new char[len]);
			GetDlgItemText(editDlg, EDITDLG_CODE_TXT, CppPythonIF::rawCode.get(), len);
			return true;
		}
		case IDCANCEL:
			DestroyWindow(editDlg);
			return true;
		case EDITDLG_REF_SCRIPT:
		{
			if (GetOpenFileName(&ofName))
			{
				char _refName[256];
				strcpy(_refName, refName);
				SetDlgItemText(editDlg, EDITDLG_PATH_TXT, refName);

				// upd file
				char tmpStr[1024];
				string scriptStr = "";

				FILE* fp = fopen(_refName, "rb");
				while (fgets(tmpStr, 1024, fp) != NULL)
					scriptStr += tmpStr;
				fclose(fp);

				SetDlgItemText(editDlg, EDITDLG_CODE_TXT, scriptStr.c_str());
			}
			SetCurrentDirectory(oldPath);
			return true;
		}
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
		trgObjIdx = newEngine->selectedIdx;
		SendDlgItemMessage(hDlg, MODOBJ_SCALE_SLIDER, TBM_SETRANGE, true, MAKELPARAM(sMin, sMax));
		SendDlgItemMessage(hDlg, MODOBJ_VELOC_SLIDER, TBM_SETRANGE, true, MAKELPARAM(sMin, sMax));
		SetDlgItemText(hDlg, MODOBJ_OBJNO_TXT, to_string(trgObjIdx).c_str());

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
		if (trgObjIdx == -1)
		{
			// scale
			sRst = 1;
			SetSliderVal(sRst, MODOBJ_SCALE_SLIDER, scaleMax);
			SetDlgItemText(hDlg, MODOBJ_SCALE_TXT, to_string((long double)sRst).c_str());
			// velocity
			sRst = newEngine->SPEED_MAX;
			SetSliderVal(sRst, MODOBJ_VELOC_SLIDER, velocMax);
			SetDlgItemText(hDlg, MODOBJ_VELOC_TXT, to_string((long double)sRst).c_str());
			// rot x velocity
			sRst = 0;
			SetSliderVal(sRst, MODOBJ_ROTX_SLIDER, rotvMax);
			SetDlgItemText(hDlg, MODOBJ_ROTX_TXT, to_string((long double)sRst).c_str());
		}
		else 
		{
			// scale
			sRst = newEngine->objs[newEngine->selectedIdx].scale;
			SetSliderVal(sRst, MODOBJ_SCALE_SLIDER, scaleMax);
			SetDlgItemText(hDlg, MODOBJ_SCALE_TXT, to_string((long double)sRst).c_str());
			// velocity
			sRst = newEngine->objs[newEngine->selectedIdx].lspX.w;
			SetSliderVal(sRst, MODOBJ_VELOC_SLIDER, velocMax);
			SetDlgItemText(hDlg, MODOBJ_VELOC_TXT, to_string((long double)sRst).c_str());
			// rot x velocity
			sRst = newEngine->objs[newEngine->selectedIdx].rsp.x / PI * 180;
			SetSliderVal(sRst, MODOBJ_ROTX_SLIDER, rotvMax);
			SetDlgItemText(hDlg, MODOBJ_ROTX_TXT, to_string((long double)sRst).c_str());
		}

		// add comboBox strings
		HWND cmb = GetDlgItem(hDlg, MODOBJ_MESH_CBOX);
		for (int i=0; i<newEngine->meshLen; i++)
		{
			mesh3d* curMesh = &newEngine->meshs[i];
			if (curMesh->coorType != mesh3d::COOR::POLAR || curMesh->faceLen == 0)
				continue;
			ComboBox_AddString(cmb, newEngine->meshs[i].objNameS.c_str());
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

		case MODOBJ_MESH_CBOX:
			if (HIWORD(wp) == CBN_SELCHANGE)
			{
				// get selected string
				char tmpc[256];
				HWND cmb = GetDlgItem(hDlg, MODOBJ_MESH_CBOX);
				LRESULT idx = SendMessage(cmb, CB_GETCURSEL, 0, 0);
				SendMessage(cmb, CB_GETLBTEXT, idx, (LPARAM)tmpc);

				if (trgObjIdx == -1)
				{
					for (int h = newEngine->BWH_QTY + newEngine->PLR_QTY; h < newEngine->OBJ_QTY; h++)
					{
						for (int i = 0; i < newEngine->meshLen; i++)
						{
							if (newEngine->meshs[i].objNameS == tmpc)
							{
								newEngine->objs[h].SetMesh(i);
								break;
							}
						}
					}
				}
				else
				{
					for (int i = 0; i < newEngine->meshLen; i++)
					{
						if (newEngine->meshs[i].objNameS == tmpc)
						{
							newEngine->objs[trgObjIdx].SetMesh(i);
							break;
						}
					}
				}
			}
			return true;
		}
		break;

	case WM_HSCROLL:
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
				if(trgObjIdx == -1)
					for (int h = newEngine->BWH_QTY + newEngine->PLR_QTY; h < newEngine->OBJ_QTY; h++)
						newEngine->objs[h].scale = sRst;
				else
					newEngine->objs[trgObjIdx].scale = sRst;
				SetDlgItemText(hDlg, MODOBJ_SCALE_TXT, to_string((long double)sRst).c_str());
				break;

			case MODOBJ_VELOC_SLIDER:
				sRst = (double)sVal / sMax * velocMax;
				if (trgObjIdx == -1)
					for (int h = newEngine->BWH_QTY + newEngine->PLR_QTY; h < newEngine->OBJ_QTY; h++)
						newEngine->objs[h].lspX.w = sRst;
				else
					newEngine->objs[trgObjIdx].lspX.w = sRst;
				SetDlgItemText(hDlg, MODOBJ_VELOC_TXT, to_string((long double)sRst).c_str());
				break;

			case MODOBJ_ROTX_SLIDER:
				sRst = (double)sVal / sMax * rotvMax;
				if (trgObjIdx == -1)
					for (int h = newEngine->BWH_QTY + newEngine->PLR_QTY; h < newEngine->OBJ_QTY; h++)
						newEngine->objs[h].rsp.x = sRst / 180 * PI;
				else
					newEngine->objs[trgObjIdx].rsp.x = sRst / 180 * PI;
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
			sVal = (int)(((sRst = newEngine->objs[newEngine->PLR_No].loc.x)/PI+1)*0.5 *(sMax-sMin)+sMin);
			if(sVal<sMin) sVal = sMin; else if(sMax<sVal) sVal = sMax;
			SendDlgItemMessage(
				hDlg,        // トラックバーのハンドル
				MODLG_LON_SLIDER,
				TBM_SETPOS,
				true,
				sVal
			);
			sRst = -1*(sRst/PI*180);
			SetDlgItemText(hDlg, MODLG_LON_TXT, to_string((long double)sRst).c_str());

			sVal = (int)(((sRst = newEngine->objs[newEngine->PLR_No].loc.y)/PI) *(sMax-sMin)+sMin);
			if(sVal<sMin) sVal = sMin; else if(sMax<sVal) sVal = sMax;
			SendDlgItemMessage(
				hDlg,        // トラックバーのハンドル
				MODLG_LAT_SLIDER,
				TBM_SETPOS,
				true,
				sVal
			);
			sRst = -1*(sRst/PI*180-90);
			SetDlgItemText(hDlg, MODLG_LAT_TXT, to_string((long double)sRst).c_str());

			sVal = (int)(((sRst = newEngine->objs[newEngine->PLR_No].loc.z)/PI) *(sMax-sMin)+sMin);
			if(sVal<sMin) sVal = sMin; else if(sMax<sVal) sVal = sMax;
			SendDlgItemMessage(
				hDlg,        // トラックバーのハンドル
				MODLG_ALT_SLIDER,
				TBM_SETPOS,
				true,
				sVal
			);
			sRst = -1*(sRst/PI*180-90);
			SetDlgItemText(hDlg, MODLG_ALT_TXT, to_string((long double)sRst).c_str());

			newEngine->mvObjParam.loc.asg2(newEngine->objs[newEngine->PLR_No].loc);
			
			//
			sVal = (int)(((sRst = newEngine->objs[newEngine->PLR_No].std[0].x)/PI+1)*0.5 *(sMax-sMin)+sMin);
			if(sVal<sMin) sVal = sMin; else if(sMax<sVal) sVal = sMax;
			SendDlgItemMessage(
				hDlg,        // トラックバーのハンドル
				MODLG_LON_SLIDER2,
				TBM_SETPOS,
				true,
				sVal
			);
			sRst = -1*(sRst/PI*180);
			SetDlgItemText(hDlg, MODLG_LON_TXT2, to_string((long double)sRst).c_str());

			sVal = (int)(((sRst = newEngine->objs[newEngine->PLR_No].std[0].y)/PI) *(sMax-sMin)+sMin);
			if(sVal<sMin) sVal = sMin; else if(sMax<sVal) sVal = sMax;
			SendDlgItemMessage(
				hDlg,        // トラックバーのハンドル
				MODLG_LAT_SLIDER2,
				TBM_SETPOS,
				true,
				sVal
			);
			sRst = -1*(sRst/PI*180-90);
			SetDlgItemText(hDlg, MODLG_LAT_TXT2, to_string((long double)sRst).c_str());

			sVal = (int)(((sRst = newEngine->objs[newEngine->PLR_No].std[0].z)/PI) *(sMax-sMin)+sMin);
			if(sVal<sMin) sVal = sMin; else if(sMax<sVal) sVal = sMax;
			SendDlgItemMessage(
				hDlg,        // トラックバーのハンドル
				MODLG_ALT_SLIDER2,
				TBM_SETPOS,
				true,
				sVal
			);
			sRst = -1*(sRst/PI*180-90);
			SetDlgItemText(hDlg, MODLG_ALT_TXT2, to_string((long double)sRst).c_str());

			newEngine->mvObjParam.rot.asg2(newEngine->objs[newEngine->PLR_No].std[0]);

			newEngine->mvObjFlg = true;

			return true;

		case WM_COMMAND:
			switch(LOWORD(wp)){
			case IDOK:
				EndDialog(hDlg, IDOK);
				return true;

			case IDCANCEL:
				newEngine->mvObjFlg = false;
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
						newEngine->mvObjParam.loc.x = sRst = (((double)sVal-sMin)/((double)sMax-sMin)*2-1)*PI;
						sRst = -1*(sRst/PI*180);
						SetDlgItemText(hDlg, MODLG_LON_TXT, to_string((long double)sRst).c_str());
						break;

					case MODLG_LAT_SLIDER:
						newEngine->mvObjParam.loc.y = sRst = (((double)sVal-sMin)/((double)sMax-sMin))*PI;
						sRst = -1*(sRst/PI*180-90);
						SetDlgItemText(hDlg, MODLG_LAT_TXT, to_string((long double)sRst).c_str());
						break;

					case MODLG_ALT_SLIDER:
						newEngine->mvObjParam.loc.z = sRst = (((double)sVal-sMin)/((double)sMax-sMin))*PI;
						sRst = -1*(sRst/PI*180-90);
						SetDlgItemText(hDlg, MODLG_ALT_TXT, to_string((long double)sRst).c_str());
						break;

					// 方向
					case MODLG_LON_SLIDER2:
						newEngine->mvObjParam.rot.x = sRst = (((double)sVal-sMin)/((double)sMax-sMin)*2-1)*PI;
						sRst = -1*(sRst/PI*180);
						SetDlgItemText(hDlg, MODLG_LON_TXT2, to_string((long double)sRst).c_str());
						break;

					case MODLG_LAT_SLIDER2:
						newEngine->mvObjParam.rot.y = sRst = (((double)sVal-sMin)/((double)sMax-sMin))*PI;
						sRst = -1*(sRst/PI*180-90);
						SetDlgItemText(hDlg, MODLG_LAT_TXT2, to_string((long double)sRst).c_str());
						break;

					case MODLG_ALT_SLIDER2:
						newEngine->mvObjParam.rot.z = sRst = (((double)sVal-sMin)/((double)sMax-sMin))*PI;
						sRst = -1*(sRst/PI*180-90);
						SetDlgItemText(hDlg, MODLG_ALT_TXT2, to_string((long double)sRst).c_str());
						break;
				}
				newEngine->mvObjFlg = true;

			} 
			return true;
		
	}
	return false;
}
