#include <Windows.h>
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
#include <glew.h>
#include <GL/gl.h>
using namespace std;

// �v���V�[�W��
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK DialogProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK howToDlgProc(HWND hDlg, UINT msg, WPARAM wp, LPARAM lp);
INT_PTR CALLBACK moveObjProc(HWND hDlg, UINT msg, WPARAM wp, LPARAM lp);

//
ATOM InitApp(HINSTANCE);
BOOL InitInstance(HINSTANCE, int);


TCHAR szClassName[] = TEXT("3d_engine");	//�E�B���h�E�N���X
engine3dWin newEngine;
JOYINFOEX JoyInfoEx;	//-- �Q�[���p�b�h
HMENU hMenu;
HINSTANCE curInst;
char curDir[MAX_PATH];
char* menuName = "KITTY";	// ���j���[��
char* titleName = "�����ʗV�j�V�~�����[�^";	// �E�B���h�E�^�C�g��

HWND preWnd;
char cmJD = 0;
POINTS cm_rot[2] = {{}, {}};



//windows�v���O�����̃G���g���[�|�C���g
int WINAPI WinMain(HINSTANCE hCurInst, HINSTANCE hPrevInst, LPSTR lpsCmdLine, int nCmdShow)
{
	// �R���g���[��
	//InitCommonControls();
	//
	curInst = hCurInst;
	MSG msg;
	GetCurrentDirectory(MAX_PATH, curDir);

	//-- �R���\�[���쐬 --//
    if(!AllocConsole()) return 0;
    freopen("CONOUT$", "w", stdout);
    freopen("CONIN$", "r", stdin);

	//---- �R�}���h�擾 �p�ꂩ���{��
	string awakeCmd = GetCommandLine();
	cout << awakeCmd << endl;
	int awIdx = awakeCmd.find_last_of(" ") +1;
	
	if(awIdx <= awakeCmd.length()){
		if(awakeCmd.substr(awIdx) == "lang:EN")
		{
			menuName = "KITTY_EN";
			titleName = "S3 wondering simlator";
			newEngine.lang = UI_LANG_EN;
		}else
		{
			menuName = "KITTY";
			titleName = "�����ʗV�j�V�~�����[�^";
			newEngine.lang = UI_LANG_JA;
		}
		cout << awakeCmd.substr(awIdx) << endl;
	}
	//------------

	if(!InitApp(hCurInst)) return FALSE;
	if(!InitInstance(hCurInst, nCmdShow)) return FALSE;

	MENUITEMINFO menuItemInfo = { sizeof(MENUITEMINFO), MIIM_STATE };
	menuCheckDef(hMenu, &menuItemInfo);
	//


	// S3�V�~�����[�^�N���X ������
	if(!newEngine.init(preWnd))
		PostQuitMessage(0);
	

	///-- �Q�[���p�b�h
	JoyInfoEx.dwSize = sizeof(JOYINFOEX);
	JoyInfoEx.dwFlags = JOY_RETURNALL;
	if (JOYERR_NOERROR == joySetCapture(preWnd, JOYSTICKID1, 1, FALSE))
	{
		joyGetDevCaps(JOYSTICKID1, &newEngine.joyCaps, sizeof(JOYCAPS));
		newEngine.useJoyPad = true;
		cout << "joypad1 is avaliable." << endl;
	}

	///--------------���b�Z�[�W���擾--------------///
	while(true){
		if((PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) != 0){
			if(msg.message == WM_QUIT) break;
			else{
				TranslateMessage(&msg);	//���b�Z�[�W��ϊ�
				DispatchMessage(&msg);	//���b�Z�[�W�𑗏o
			}
		}else{
			// S3�X�V
			newEngine.update();
		}
	}

	///-- �㏈�� --//
	newEngine.dispose();

	FreeConsole();
	///--
	

	return (int)msg.wParam;
}

//�E�B���h�E�N���X�̓o�^
ATOM InitApp(HINSTANCE hInst)
{
	WNDCLASSEX wc;
	wc.cbSize = sizeof(WNDCLASSEX);	//�\���̂̃T�C�Y
	wc.style = CS_OWNDC | CS_HREDRAW | CS_VREDRAW;	//�N���X�̃X�^�C��
	wc.lpfnWndProc = WndProc;	//�v���V�[�W����
	wc.cbClsExtra = 0;	//�⏕������
	wc.cbWndExtra = 0;	//�⏕������
	wc.hInstance = hInst;	//�C���X�^���X

	wc.hIcon = (HICON)LoadImage(NULL, "s3sim.ico",
				IMAGE_ICON, 0, 0, LR_DEFAULTSIZE | LR_SHARED | LR_LOADFROMFILE);
	wc.hIconSm = (HICON)LoadImage(NULL, "s3sim_sm.ico",
				IMAGE_ICON, 0, 0, LR_DEFAULTSIZE | LR_SHARED | LR_LOADFROMFILE);
	wc.hCursor = (HCURSOR)LoadImage(NULL, MAKEINTRESOURCE(IDC_ARROW),
				IMAGE_CURSOR, 0, 0, LR_DEFAULTSIZE | LR_SHARED);
	wc.hbrBackground = (HBRUSH)GetStockObject(DKGRAY_BRUSH);
	wc.lpszMenuName = menuName;	//���j���[��
	wc.lpszClassName = szClassName;	//�N���X��

	return (RegisterClassEx(&wc));
}

//=========���C���E�B���h�E�̐���
BOOL InitInstance(HINSTANCE hInst, int nCmdShow)
{
	HWND hWnd;
	hWnd = CreateWindow(szClassName,	//�N���X��
				titleName,	//�E�B���h�E��
				WS_OVERLAPPEDWINDOW,	//�E�B���h�E�X�^�C��
				CW_USEDEFAULT,	//x�ʒu
				CW_USEDEFAULT,	//y�ʒu
				newEngine.WIDTH + GetSystemMetrics(SM_CXFRAME)*2,	//x�E�B���h�E��
				newEngine.HEIGHT + GetSystemMetrics(SM_CYFRAME)*2
					+ GetSystemMetrics(SM_CYCAPTION)
					+ GetSystemMetrics(SM_CYMENU),	//�E�B���h�E����
				NULL,	//�e�E�B���h�E�̃n���h���A�e�����Ƃ���NULL
				hMenu,	//���j���[�n���h���A�N���X���j���[���g���Ƃ���NULL
				hInst,	//�C���X�^���X�n���h��
				NULL	//�E�B���h�E�쐬�f�[�^
			);

	if(!hWnd) return FALSE; else preWnd = hWnd;
	hMenu = GetMenu( hWnd );

	ShowWindow(hWnd, nCmdShow);	//�E�B���h�E�̕\����Ԃ�ݒ�
	UpdateWindow(hWnd);	//�E�B���h�E���X�V

	newEngine.CR_RANGE_Y = newEngine.clcRangeY(newEngine.CR_RANGE_X);	//�J�����ݒ�
	
	return TRUE;
}



//�E�B���h�E�v���V�[�W��(�R�[���o�b�N�֐�)
LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp)
{
 if(hWnd==preWnd){///===�v���C�}��
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
		"Wavefront .obj�t�@�C�� {*.obj}\0*.obj\0"
		"All files {*.*}\0*.*\0\0",
		0,
		0,
		0,
		refName,
		1024,
		0,
		0,
		0,
		"�I�u�W�F�N�g�̎Q��",
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
				newEngine.CR_RANGE_Y = newEngine.clcRangeY(newEngine.CR_RANGE_X);	//�J�����ݒ�
				glViewport(0, 0, newEngine.WIDTH, newEngine.HEIGHT);	//-- GLwnd�T�C�Y�X�V
			}
			break;

		case WM_COMMAND:
			if(lp==0){	//���j���[
				int wVal = LOWORD(wp);
				switch (wVal) {

				//-- �v���O�����I��
				case UI_END:
					PostQuitMessage(0);
					break;
				//-- �t���[�����[�g
				case UI_FPS_30:
					newEngine.fps = newEngine.STD_PMSEC;
					newEngine.adjSpd = 1.0;
					newEngine.vfrFlg = false;
					menuCheck(hMenu, &menuItemInfo, UI_FPS_30, unCk_fps, unCk_fps_len);
					break;
				case UI_FPS_60:
					newEngine.fps = newEngine.MAX_PMSEC;
					newEngine.adjSpd = 0.5;
					newEngine.vfrFlg = false;
					menuCheck(hMenu, &menuItemInfo, UI_FPS_60, unCk_fps, unCk_fps_len);
					break;
				case UI_FPS_VFR:
					newEngine.fps = newEngine.MAX_PMSEC;
					newEngine.adjSpd = 1.0;
					newEngine.vfrFlg = true;
					menuCheck(hMenu, &menuItemInfo, UI_FPS_VFR, unCk_fps, unCk_fps_len);
					break;
				//-- ����
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

				case UI_THROW_CLEAR:
					newEngine.player.ep = newEngine.ENR_QTY;
					for(int i=newEngine.BWH_QTY+newEngine.PLR_QTY;i<newEngine.OBJ_QTY;i++){
						newEngine.objs[i].used = false;
						newEngine.objs[i].markInit(newEngine.radius);
					}
					break;
				case UI_THROW_RANDOM:
					newEngine.player.ep = 0;
					newEngine.randLoc(1);
					break;
				case UI_THROW_RANDOM2:
					newEngine.player.ep = 0;
					newEngine.randLoc(0);
					break;

				case UI_CR_S:
					newEngine.CR_RANGE_X = 70;
					newEngine.CR_RANGE_Y = newEngine.clcRangeY( newEngine.CR_RANGE_X );	//�J�����ݒ�
					menuCheck(hMenu, &menuItemInfo, UI_CR_S, unCk_rng, unCk_rng_len);
					break;
				case UI_CR_M:
					newEngine.CR_RANGE_X = 90;
					newEngine.CR_RANGE_Y = newEngine.clcRangeY( newEngine.CR_RANGE_X );	//�J�����ݒ�
					menuCheck(hMenu, &menuItemInfo, UI_CR_M, unCk_rng, unCk_rng_len);
					break;
				case UI_CR_L:
					newEngine.CR_RANGE_X = 110;
					newEngine.CR_RANGE_Y = newEngine.clcRangeY( newEngine.CR_RANGE_X );	//�J�����ݒ�
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
					if( GetOpenFileName( &ofName ) ){	//-- �t�@�C���Q��
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
						newEngine.objs[h].mkLspX( newEngine.objs[h].lspX );
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
				}
			}
			break;
			
		case WM_MBUTTONDOWN:
			if((wp&0x0004) != 0x0004) cmJD = 1; else cmJD = 2;
			cm_rot[0] = cm_rot[1] = MAKEPOINTS(lp);
			break;
			
		case WM_MOUSEMOVE:
			if(cmJD){	// ���N���b�N��
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
				newEngine.preCm_rotX = fmod(newEngine.preCm_rotX, PIE*2);
				newEngine.preCm_rotY = fmod(newEngine.preCm_rotY, PIE*2);
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
			//DRAW_MODE = !DRAW_MODE;
			//MIST = !MIST;
			break;
			
		case WM_KEYDOWN:///------------ �L�[���b�Z�[�W (down) -----------
			switch(wp)
			{
			case VK_F1:
				SC_BGN = !SC_BGN;	//-- �^��
				///----- �⏕�L�[ ��
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
			
		case WM_KEYUP:///------------ �L�[���b�Z�[�W (up) -----------
			switch(wp)
			{
					///----- �⏕�L�[ ��
				case VK_SHIFT: shiftK = false; break;
				case VK_CONTROL: ctrlK = false; break;
			}
			break;

		///------------ �Q�[���p�b�h���b�Z�[�W ------------
		case MM_JOY1MOVE:
			if(JOYERR_NOERROR == joyGetPosEx(0, &JoyInfoEx))
			{
				// ���ɃL�[����ςȂ�ޏo
				if (newEngine.inputByKey)
					break;
				double xMaxHf = newEngine.joyCaps.wXmax * 0.5;
				double yMaxHf = newEngine.joyCaps.wYmax * 0.5;
				double zMaxHf = newEngine.joyCaps.wZmax * 0.5;
				double uMaxHf = newEngine.joyCaps.wUmax * 0.5;
				double rMaxHf = newEngine.joyCaps.wRmax * 0.5;

				pt3 gLoc = pt3(	-(LOWORD(JoyInfoEx.dwZpos)-zMaxHf) / zMaxHf,	// �O��
								 (LOWORD(JoyInfoEx.dwXpos)-xMaxHf) / xMaxHf,	// ���E
								-(LOWORD(JoyInfoEx.dwYpos)-yMaxHf) / yMaxHf)	// �㉺
							.mtp(0.32767 * newEngine.adjSpd * powi(3.0, newEngine.speed1));	// �W��
				
				pt2 gRot = pt2(	-(LOWORD(JoyInfoEx.dwUpos)-uMaxHf) / uMaxHf,	//���E
								-(LOWORD(JoyInfoEx.dwRpos)-rMaxHf) / rMaxHf)	//�㉺
							.mtp(0.32767 * 0.3);

				// �ʒu
				double lLim = 0.05 * newEngine.adjSpd * powi(3.0, newEngine.speed1);
				newEngine.cm_loc[0] = (lLim < abs(gLoc.x)) ? gLoc.x : 0.0;
				newEngine.cm_loc[1] = (lLim < abs(gLoc.y)) ? gLoc.y : 0.0;
				newEngine.cm_loc[2] = (lLim < abs(gLoc.z)) ? gLoc.z : 0.0;
				
				// ��]
				newEngine.preCm_rotX = (0.02 < abs(gRot.x)) ? gRot.x * newEngine.adjSpd : 0.0;
				newEngine.preCm_rotY = (0.02 < abs(gRot.y)) ? gRot.y * newEngine.adjSpd : 0.0;
				if (JoyInfoEx.dwButtons == 16)
					newEngine.preCm_rotZ = -0.1;
				else if (JoyInfoEx.dwButtons == 32)
					newEngine.preCm_rotZ = 0.1;
				else
					newEngine.preCm_rotZ = 0.0;

				// �}�b�v
				newEngine.chgMapState = JoyInfoEx.dwPOV;
				if (newEngine.chgMapState != newEngine.chgMapStateOld)
				{
					if (newEngine.chgMapState == JOY_POVFORWARD)
					{
						newEngine.inPutKey(newEngine.IK::No_4, NULL); break;
					}
					else if (newEngine.chgMapState == JOY_POVLEFT)
					{
						newEngine.inPutKey(newEngine.IK::No_5, NULL); break;
					}
				}

				// ��������
				newEngine.cmBack = (JoyInfoEx.dwButtons & JOY_BUTTON10) ? true : false;
			}
			break;

		case MM_JOY1BUTTONDOWN:	//-- �{�^������
			if(JOYERR_NOERROR == joyGetPosEx(0, &JoyInfoEx))
			{
				//cout << ((JoyInfoEx.dwButtons))  << endl;
				if(JoyInfoEx.dwButtons==4) newEngine.speed1 = ++newEngine.speed1%3;
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

 }else{///=======�T�u
	
			return (DefWindowProc(hWnd, msg, wp, lp));
 }
}

// �v���V�[�W��
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
		case WM_DESTROY:
			EndDialog(hDlg, IDOK);
			return true;
		
	}
	return false;

};

// �I�u�W�F�N�g����v���V�[�W��
INT_PTR CALLBACK moveObjProc(HWND hDlg, UINT msg, WPARAM wp, LPARAM lp)
{
	// �X���C�_�[�ϐ�
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
			// ���݂�loc�Estd���擾���ă_�C�A���O�ɔ��f
			sVal = (int)(((sRst = newEngine.objs[newEngine.PLR_No].loc.x)/PIE+1)*0.5 *(sMax-sMin)+sMin);
			if(sVal<sMin) sVal = sMin; else if(sMax<sVal) sVal = sMax;
			SendDlgItemMessage(
				hDlg,        // �g���b�N�o�[�̃n���h��
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
				hDlg,        // �g���b�N�o�[�̃n���h��
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
				hDlg,        // �g���b�N�o�[�̃n���h��
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
				hDlg,        // �g���b�N�o�[�̃n���h��
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
				hDlg,        // �g���b�N�o�[�̃n���h��
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
				hDlg,        // �g���b�N�o�[�̃n���h��
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
					(HWND) lp,        // �g���b�N�o�[�̃n���h��
					(UINT) TBM_GETPOS,
					NULL,
					NULL
				);
				// �X���C�_�[����
				int sliderID = GetDlgCtrlID((HWND)lp);

				// �X���C�_�[�ɉ���������
				switch(sliderID)
				{
					// �ʒu
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

					// ����
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


