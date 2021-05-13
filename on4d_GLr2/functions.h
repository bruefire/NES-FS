#pragma once


//===���s�^�S���X�̒藝
double pyth2(double x, double y);
double pyth3(double x, double y, double z);
double pyth4(double w, double x, double y, double z);
double pyth3(pt3 pts);
double pyth4(pt4 pts);
double pyth4Sq(pt4 pts);
double pyth3OS(pt3 pts, double hLen = 1);	// �Ε�(�ΕӂłȂ���)�̒���
double powi(double, int);
//=====���l�̒���
void degAdj(pt4* pt2, pt4* pt1, char mode);
void degAdj(pt4* pt2, char mode);

//====4d����3d��===
void pConv4to3(pt4* pts4, pt3* pts3, int len);
void fConv4to3(face4*, face3*, pt3*);

//====
void cubePts(uint32_t p1, uint32_t p2, uint32_t p3, uint32_t p4, uint32_t* idxs, int len);	//������
void tetraPts(uint32_t p1, uint32_t p2, uint32_t p3, uint32_t p4, uint32_t* idxs);			//�l�ʑ�
void piraPts(uint32_t p1, uint32_t p2, uint32_t p3, uint32_t p4, uint32_t p5, uint32_t* idxs);	//�l�p��
void prismPts(uint32_t p1, uint32_t p2, uint32_t p3, uint32_t* idxs, int len);			//�l�p��
///---
void tudeRst(double* vec1, double* vec2, double locT, bool mode);
void all_tudeRst(pt4* vec, pt3 locT, bool mode);
