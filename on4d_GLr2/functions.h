#pragma once


//===▼ピタゴラスの定理
double pyth2(double x, double y);
double pyth3(double x, double y, double z);
double pyth4(double w, double x, double y, double z);
double pyth3(pt3 pts);
double pyth4(pt4 pts);
double pyth4Sq(pt4 pts);
double pyth3OS(pt3 pts, double hLen = 1);	// 対辺(斜辺でない方)の長さ
double powi(double, int);
//=====▼値の調整
void degAdj(pt4* pt2, pt4* pt1, char mode);
void degAdj(pt4* pt2, char mode);

//====4dから3dへ===
void pConv4to3(pt4* pts4, pt3* pts3, int len);
void fConv4to3(face4*, face3*, pt3*);

//====
void cubePts(uint32_t p1, uint32_t p2, uint32_t p3, uint32_t p4, uint32_t* idxs, int len);	//立方体
void tetraPts(uint32_t p1, uint32_t p2, uint32_t p3, uint32_t p4, uint32_t* idxs);			//四面体
void piraPts(uint32_t p1, uint32_t p2, uint32_t p3, uint32_t p4, uint32_t p5, uint32_t* idxs);	//四角錐
void prismPts(uint32_t p1, uint32_t p2, uint32_t p3, uint32_t* idxs, int len);			//四角柱
///---
void tudeRst(double* vec1, double* vec2, double locT, bool mode);
void all_tudeRst(pt4* vec, pt3 locT, bool mode);
