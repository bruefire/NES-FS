#include <Windows.h>
#include <Complex>
#include <iostream>
#include <time.h>
#include "constants.h"
#include "global_var2.h"
#include "engine3dWin.h"
#include "functions.h"
#include "UI_DEF.h"
#include "rcFunc2.h"


void menuCheckDef(HMENU hMenu, MENUITEMINFO* miInfo){	//--デフォルトのチェック項目
	
	miInfo->fState = MFS_CHECKED;
	SetMenuItemInfo(hMenu, UI_BG_BK, 0, miInfo);	//background
	SetMenuItemInfo(hMenu, UI_LOC_TUDE, 0, miInfo);	//表示座標
	SetMenuItemInfo(hMenu, UI_RDS_S, 0, miInfo);
	SetMenuItemInfo(hMenu, UI_CR_M, 0, miInfo);
	SetMenuItemInfo(hMenu, UI_6_POINT, 0, miInfo);
	SetMenuItemInfo(hMenu, UI_THROW_CUBE, 0, miInfo);
	SetMenuItemInfo(hMenu, UI_SD_2, 0, miInfo);		//fog effect
	SetMenuItemInfo(hMenu, UI_MV_CONST, 0, miInfo);

	SetMenuItemInfo(hMenu, UI_XYZ, 0, miInfo);
	SetMenuItemInfo(hMenu, UI_PLR, 0, miInfo);
	SetMenuItemInfo(hMenu, UI_MAP, 0, miInfo);
	//SetMenuItemInfo(hMenu, UI_LR_RESULT, 0, miInfo);

	SetMenuItemInfo(hMenu, UI_FPS_30, 0, miInfo);	// fps 30
	//SetMenuItemInfo(hMenu, UI_LANG_JA, 0, miInfo);	// language > japanese

}

// チェック(グループで一つのみ)
void menuCheck(HMENU hMenu, MENUITEMINFO* miInfo, int ckId, int* unCks, int len){

	miInfo->fState = MFS_UNCHECKED;
	for(int i=0;i<len;i++){
		SetMenuItemInfo(hMenu, unCks[i], 0, miInfo);
	}

	miInfo->fState = MFS_CHECKED;
	SetMenuItemInfo(hMenu, ckId, 0, miInfo);

}

// チェック(独立)
void menuCheck2(HMENU hMenu, MENUITEMINFO* miInfo, int ckId){

	GetMenuItemInfo(hMenu, ckId, 0, miInfo);
	if(miInfo->fState == MFS_UNCHECKED)
		miInfo->fState = MFS_CHECKED;
	else
		miInfo->fState = MFS_UNCHECKED;
	
	SetMenuItemInfo(hMenu, ckId, 0, miInfo);
};

void changeLang(HMENU hMenu, MENUITEMINFO* miInfo, int lang)
{
	char* itemName;
	MENUINFO mInfo = {};
	mInfo.fMask = MIM_MENUDATA;

	if(lang == UI_LANG_JA)	// japanese
	{
	}
	else if(lang == UI_LANG_EN)	// english
	{
		
		//HMENU prog = GetSubMenu(hMenu, 0);
		//SetMenuInfo(prog, &mInfo, "program", UI_FPS);
		changeLangItem(hMenu, miInfo, "frame rate", UI_FPS);
		changeLangItem(hMenu, miInfo, "30 (CFR)", UI_FPS_30);
		changeLangItem(hMenu, miInfo, "60 (CFR)", UI_FPS_60);
		changeLangItem(hMenu, miInfo, "25-60 (VFR)", UI_FPS_VFR);
	}
}

void changeLangItem(HMENU hMenu, MENUITEMINFO* miInfo, char* itemName, int menuID)
{
	miInfo->fMask = MIIM_STRING;
	miInfo->dwTypeData = itemName;
	SetMenuItemInfo(hMenu, menuID, 0, miInfo);
}

