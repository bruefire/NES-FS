#pragma once
#include "rcFunc2.h"


// グループ
int unCk_mark_len = 13;
int unCk_mark[13] = {
	UI_WL_NONE, 
	UI_6_POINT, UI_6_LINE, 
	UI_TUDE_POINT, UI_TUDE_LINE, 
	UI_120C_POINT, UI_120C_LINE, UI_120C_SURFACE,
	UI_TORUS_POINT, UI_TORUS_LINE, UI_TORUS_SURFACE,
	UI_MK_EARTH, UI_MK_TC
};

int unCk_thrw_len = 5;
int unCk_thrw[5] = {
	UI_THROW_SPHERE, UI_THROW_CUBE, UI_THROW_HORSE, UI_THROW_ROCK, UI_THROW_REF
};

int unCk_rds_len = 4;
int unCk_rds[4] = {
	UI_RDS_SS, UI_RDS_S, UI_RDS_M, UI_RDS_L
};

int unCk_rng_len = 3;
int unCk_rng[3] = {
	UI_CR_S, UI_CR_M, UI_CR_L
};

int unCk_bg_len = 2;
int unCk_bg[2] = {
	UI_BG_WH, UI_BG_BK
};

int unCk_xyz_len = 2;
int unCk_xyz[2] = {
	UI_LOC_EUC, UI_LOC_TUDE
};

int unCk_dec_len = 4;
int unCk_dec[4] = {
	UI_SD_0, UI_SD_1, UI_SD_2, UI_SD_4
};

int unCk_mv_len = 3;
int unCk_mv[3] = {
	UI_MV_CONST, UI_MV_ATTRACT, UI_MV_REPULSE
};
//__ フレームレート
int unCk_fps_len = 3;
int unCk_fps[3] = {
	UI_FPS_30, UI_FPS_60, UI_FPS_VFR
};
//__ メニューなどの言語
int unCk_lang_len = 2;
int unCk_lang[2] = { 
	UI_LANG_JA, UI_LANG_EN 
};



