#include <stdint.h>
#include <Complex>
#include <algorithm>
#include <iostream>
#include <String>
#include "constants.h"
#include "engine3dGL.h"
#include "S3ErrorManager.h"
#include "functions.h"
#include "UI_DEF.h"

#include <glew.h>
#include <GL/gl.h>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

using namespace std;




//*** GUIクラス ***//

GlRect::GlRect() {};
GlRect::GlRect(pt2 size, double left, double top, STD mode)
{
	sz = size;
	l = left;
	t = top;

	this->mode = mode;
}
GlRect::GlRect(double left, double top, double right, double bottom, STD mode)
{
	l = left;
	t = top;
	r = right;
	b = bottom;

	this->mode = mode;
}
GlRect::GlRect(double dst, STD mode)
{
	l = dst;
	t = dst;
	r = dst;
	b = dst;

	this->mode = mode;
}

GuiItem::GuiItem()
{
	type = GuiType::BASE;
}

GuiContainer::GuiContainer()
{
	type = GuiType::CONTAINER;
}

GuiString::GuiString()
{
	type = GuiType::STRING;
}
GuiString::GuiString(string content, GlRect drawArea)
	: GuiString()
{
	this->content = content;
	this->drawArea = drawArea;
}


//**** 基本メソッド ****//
//
// init, update, dispose, コンストラクタ, デストラクタ
//

mesh3dGL::~mesh3dGL()
{
	if (texJD)
		stbi_image_free(bmp);
}


int mesh3dGL::makeDataForGL()
{
	if (0 < faceLen)	//-- 主に面
	{
		int fltLen = 8;

		pts2 = new float[faceLen * fltLen * 3];
		ptsGl = new int[faceLen * 3];
		for (int i = 0; i < faceLen; i++) {
			for (char j = 0; j < 3; j++) {
				int idx = i * fltLen * 3 + fltLen * j;
				pts2[idx + 0] = faces[i].pts[j]->x;	//-- 頂点
				pts2[idx + 1] = faces[i].pts[j]->y;
				pts2[idx + 2] = faces[i].pts[j]->z;

				pts2[idx + 3] = faces[i].col.R / 255.0;	//-- 色
				pts2[idx + 4] = faces[i].col.G / 255.0;
				pts2[idx + 5] = faces[i].col.B / 255.0;
				if (texJD) {
					pts2[idx + 6] = faces[i].txs[j]->x;	//-- テクスチャ
					pts2[idx + 7] = faces[i].txs[j]->y;
				}
				else {
					pts2[idx + 6] = 0;	//-- テクスチャ
					pts2[idx + 7] = 0;
				}
			}
		}
		bufLen = faceLen * fltLen * 3;

	}
	else {			//-- 無ければ辺
		pts2 = new float[lLen * 8 * 2];
		ptsGl = new int[1];//未使用
		for (int i = 0; i < lLen; i++) {
			int idx = i * 16;
			pts2[idx + 0] = pts[lines[i].x].x;
			pts2[idx + 1] = pts[lines[i].x].y;
			pts2[idx + 2] = pts[lines[i].x].z;
			pts2[idx + 3] = 0;
			pts2[idx + 4] = 1;
			pts2[idx + 5] = pyth3(pts[lines[i].x]) / 6.3;

			pts2[idx + 8] = pts[lines[i].y].x;
			pts2[idx + 9] = pts[lines[i].y].y;
			pts2[idx + 10] = pts[lines[i].y].z;
			pts2[idx + 11] = 0;
			pts2[idx + 12] = 1;
			pts2[idx + 13] = pyth3(pts[lines[i].y]) / 6.3;

			pts2[idx + 6] = pts2[idx + 7] =
				pts2[idx + 14] = pts2[idx + 15] = 0;

			if(objNameS=="wLines"){
			    if(abs(pts0[ lines[i].x ].x) < pow(0.1,20)) pts2[idx +3] = 0; else pts2[idx +3] = 1;
			    if(abs(pts0[ lines[i].x ].y) < pow(0.1,20)) pts2[idx +4] = 0; else pts2[idx +4] = 1;
			    if(abs(pts0[ lines[i].x ].z) < pow(0.1,20)) pts2[idx +5] = 0; else pts2[idx +5] = 1;
			}
		}
		bufLen = lLen * 16;
	}

	return 1;
}


int mesh3dGL::makeDataForGL_LQY(engine3d* owner)
{
	if (0 < faceLen)	//-- 主に面
	{
		int fltLen = 21;

		pts2 = new float[faceLen * fltLen * 3];
		ptsGl = new int[faceLen * 3];
		for (int i = 0; i < faceLen; i++) {
			for (char j = 0; j < 3; j++) {
				int idx = i * fltLen * 3 + fltLen * j;
				pts2[idx + 0] = faces[i].pts[j]->x;	
				pts2[idx + 1] = faces[i].pts[j]->y;
				pts2[idx + 2] = faces[i].pts[j]->z;

				pts2[idx + 3] = faces[i].col.R / 255.0;	
				pts2[idx + 4] = faces[i].col.G / 255.0;
				pts2[idx + 5] = faces[i].col.B / 255.0;

				// 位置算出用Loc
				pts2[idx + 6] = faces[i].pts[0]->x;	
				pts2[idx + 7] = faces[i].pts[0]->y;
				pts2[idx + 8] = faces[i].pts[0]->z;

				pts2[idx + 9] = faces[i].pts[1]->x;	
				pts2[idx + 10] = faces[i].pts[1]->y;
				pts2[idx + 11] = faces[i].pts[1]->z;

				pts2[idx + 12] = faces[i].pts[2]->x;	
				pts2[idx + 13] = faces[i].pts[2]->y;
				pts2[idx + 14] = faces[i].pts[2]->z;

				// 位置算出用tex
				if (texJD) {
					pts2[idx + 15] = faces[i].txs[0]->x;
					pts2[idx + 16] = faces[i].txs[0]->y;

					pts2[idx + 17] = faces[i].txs[1]->x;
					pts2[idx + 18] = faces[i].txs[1]->y;

					pts2[idx + 19] = faces[i].txs[2]->x;
					pts2[idx + 20] = faces[i].txs[2]->y;
				} else {
					pts2[idx + 15] = 0;
					pts2[idx + 16] = 0;
					pts2[idx + 17] = 0;
					pts2[idx + 18] = 0;
					pts2[idx + 19] = 0;
					pts2[idx + 20] = 0;
				}
			}
		}
		bufLen = faceLen * fltLen * 3;

	}
	else {			//-- 無ければ辺
		int fltLen = 12;
		pts2 = new float[lLen * fltLen * 2];
		ptsGl = new int[1];//未使用

		for (int i = 0; i < lLen; i++) {
			for (char j = 0; j < 2; j++) {
				int idx = i * fltLen * 2 + fltLen * j;
				if (j == 0) {
					pts2[idx + 0] = pts[lines[i].x].x;
					pts2[idx + 1] = pts[lines[i].x].y;
					pts2[idx + 2] = pts[lines[i].x].z;
				}else{
					pts2[idx + 0] = pts[lines[i].y].x;
					pts2[idx + 1] = pts[lines[i].y].y;
					pts2[idx + 2] = pts[lines[i].y].z;
				}

				pts2[idx + 3] = 0;
				pts2[idx + 4] = 1;
				pts2[idx + 5] = 1;

				// 位置算出用Loc
				pts2[idx + 6] = pts[lines[i].x].x;
				pts2[idx + 7] = pts[lines[i].x].y;
				pts2[idx + 8] = pts[lines[i].x].z;

				pts2[idx + 9]  = pts[lines[i].y].x;
				pts2[idx + 10] = pts[lines[i].y].y;
				pts2[idx + 11] = pts[lines[i].y].z;
			}
		}
		bufLen = lLen * fltLen * 2;
	}

	return 1;
}


int mesh3dGL::transBuffers(uint32_t* buffers, uint32_t* texNames)
{

	//-- 頂点転送
	glBindBuffer(GL_ARRAY_BUFFER, buffers[texNo]);
	glBufferData(	//---- pts転送
		GL_ARRAY_BUFFER,
		bufLen * sizeof(float),
		pts2,
		GL_STATIC_DRAW
	);


	// テクスチャ転送
	if (!texJD) return 0;


	glBindTexture(GL_TEXTURE_2D, texNames[texNo]);

	int posA = 0;
	if (!extObjFlg) posA = texName.find_last_of('\\') + 1;
	int posB = texName.find_last_of('\r');
	if (posB == texName.npos)
		posB = texName.find_last_of('\n');
	texName = texName.substr(posA, posB - posA);

	bmp = stbi_load(&texName[0], texWH, texWH + 1, NULL, 4);
	if (bmp == NULL) {
		cout << "テクスチャ読み込み失敗" << endl;
		//PostQuitMessage(0);
	}
	///-- 
	glTexImage2D(		//bmp転送
		GL_TEXTURE_2D, 0, GL_RGBA2, texWH[0], texWH[1],
		0, GL_RGBA, GL_UNSIGNED_BYTE, bmp
	);


	return 1;
}


void engine3dGL::MakeCommonVBO(int i)
{
	if (meshs[i].pts != nullptr)
	{
		switch (qyMode)
		{
		case QY_MODE::HIGH:
			((mesh3dGL*)&meshs[i])->makeDataForGL();
			break;

		case QY_MODE::LOW:
			if (meshs[i].coorType == mesh3d::COOR::Cartesian)
				((mesh3dGL*)&meshs[i])->makeDataForGL();
			else
				((mesh3dGL*)&meshs[i])->makeDataForGL_LQY(this);
			break;
		}

		((mesh3dGL*)&meshs[i])->transBuffers(buffers, texNames);
	}

}


int engine3dGL::init()
{
	try
	{
		// 基底クラスのinit
		engine3d::init();

		GL_Init();
	}
	// 終了に向かう
	catch (S3ErrorManager ex)
	{
		disposeFlg = true;
		throw ex;
	}


	// フォント
	MakeCharVBO();

	// マップ
	tObj.mesh = &tMesh;
	pols = new poly[1]; polCnt = 1;
	pols[0].polyInit(3);
	pol_pLen = pols[0].pLen;


	//-- メッシュ転送
	for (int i = 0; i < meshLen; i++)
	{
		MakeCommonVBO(i);
	}


	return 1;
}

int engine3dGL::update()
{
	// 基底(演算)クラス
	engine3d::update();


	//======== << エンジン >> =======//
	GL_SwapBuffer();
	if (worldGeo == WorldGeo::SPHERICAL)
	{
		glEnable(GL_STENCIL_TEST);
		glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
		simulateS3GL();
		glDisable(GL_STENCIL_TEST);

		// GUI描画
		glDisable(GL_DEPTH_TEST);
		glEnable(GL_BLEND);

		DrawDistances();
		DrawCoordinateS3();

		glDisable(GL_BLEND);
		glEnable(GL_DEPTH_TEST);

		// map描画
		if (VIEW_ON4) DrawMapS3();

	}
	else if (worldGeo == WorldGeo::HYPERBOLIC)
	{
		glEnable(GL_STENCIL_TEST);
		glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
		SimulateH3GL();
		glDisable(GL_STENCIL_TEST);

		// GUI描画
		glDisable(GL_DEPTH_TEST);
		glEnable(GL_BLEND);

		// 座標描画
		DrawCoordinateH3();

		glDisable(GL_BLEND);
		glEnable(GL_DEPTH_TEST);

		// map描画
		if (VIEW_ON4) DrawMapH3();
	}



	return 1;
}


int engine3dGL::dispose()
{
	if (disposeFlg)
		return 0;

	
	end4D();			// マップ後処理
	DisposeCharVBO();	// ASCII VBO 解放
	GL_End();
	delete[] pols;

	

	// 基底クラス
	engine3d::dispose();

	disposeFlg = true;

	return 1;
}

engine3dGL::engine3dGL()
	: WIDTH(1280)
	, HEIGHT(720)
	, decMode(2)
	, bgCol(0)
	, VIEW_ON4(true)
	, CR_RANGE_X(90.0)
	, disposeFlg(false)
	, VIEW_PLR(true)
	, VIEW_XYZ(true)
	, LOC_MODE(true)
	, VIEW_DST(false)
	, VIEW_LocRot(false)
	, ptDel4d(0)
	, qyMode(QY_MODE::HIGH)
{
	CR_RANGE_R = -1.0;
	CR_RANGE_D = -1.0;

	///▼ 3Dスクリーン
	tObj.ctr.asg(0, 0, 0);
	tObj.loc.asg(0, 5, 0);
	tObj.rot.asg(0, 0, 0);
	tObj.lsp.asg(0, 0, 0);
	tObj.rsp.asg(0, 0, 0);
	tObj.polObj = true;
	tObj.alfa = 0.4;

	shader.resize(6);
	for (int i = 0; i < shader.size(); i++)
		shader[i] = -1;


	CR_RANGE_Y = clcRangeY(CR_RANGE_X);	//カメラ設定
}
engine3dGL::~engine3dGL()
{
}

engine3dGL::GuiFont::GuiFont()
	: chrImgNm("Font.png")
	, guiChrQty(120)
{
	chrBuf = new uint32_t[guiChrQty];
}
engine3dGL::GuiFont::~GuiFont()
{
	delete[] chrBuf;
}

//*** GUI, フォント ***
void engine3dGL::MakeCharVBO()
{
	// バッファ生成
	glGenBuffers(guiFont.guiChrQty, guiFont.chrBuf);
	glGenTextures(1, &guiFont.chrTex);

	// まずバインド
	glBindTexture(GL_TEXTURE_2D, guiFont.chrTex);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);


	// テクスチャデータ転送
	int ftxWidth;
	int ftxHeight;

	stbi_uc* ftx = stbi_load(&(guiFont.chrImgNm[0]), &ftxWidth, &ftxHeight, NULL, 4);
	if (ftx == NULL) {
		cout << "テクスチャ読み込み失敗" << endl;
	}

	// テクスチャ転送
	glTexImage2D(
		GL_TEXTURE_2D, 0, GL_RGBA2, ftxWidth, ftxHeight,
		0, GL_BGRA, GL_UNSIGNED_BYTE, ftx
	);



	for (int i = 0; i < guiFont.guiChrQty; i++)
	{
		// !
		pt2 vtx[6];
		pt2 txs[6];


		// ポリゴン頂点設定
		vtx[0].x = 1.0, vtx[0].y = 1.0;
		vtx[1].x = 0, vtx[1].y = 1.0;
		vtx[2].x = 0, vtx[2].y = 0;
		vtx[3] = vtx[0];
		vtx[4] = vtx[2];
		vtx[5].x = 1.0;
		vtx[5].y = 0;


		// テクスチャ頂点設定
		pt2 txOrg(
			(i % 20) * 30,
			(i / 20) * 30
		);

		txs[0].x = (float)(txOrg.x + 30);
		txs[0].y = (float)(txOrg.y);

		txs[1].x = (float)(txOrg.x);
		txs[1].y = (float)(txOrg.y);

		txs[2].x = (float)(txOrg.x);
		txs[2].y = (float)(txOrg.y + 30);

		txs[3] = txs[0];
		txs[4] = txs[2];

		txs[5].x = (float)(txOrg.x + 30);
		txs[5].y = (float)(txOrg.y + 30);


		for (int j = 0; j < 6; j++)
		{
			txs[j].x = txs[j].x / ftxWidth;
			txs[j].y = txs[j].y / ftxHeight;
		}


		// 合成
		float vtxs[24] =
		{
			(float)vtx[0].x, (float)vtx[0].y, (float)txs[0].x, (float)txs[0].y,
			(float)vtx[1].x, (float)vtx[1].y, (float)txs[1].x, (float)txs[1].y,
			(float)vtx[2].x, (float)vtx[2].y, (float)txs[2].x, (float)txs[2].y,
			(float)vtx[3].x, (float)vtx[3].y, (float)txs[3].x, (float)txs[3].y,
			(float)vtx[4].x, (float)vtx[4].y, (float)txs[4].x, (float)txs[4].y,
			(float)vtx[5].x, (float)vtx[5].y, (float)txs[5].x, (float)txs[5].y
		};


		// 頂点転送
		glBindBuffer(GL_ARRAY_BUFFER, guiFont.chrBuf[i]);
		glBufferData(	//---- pts転送
			GL_ARRAY_BUFFER,
			24 * sizeof(float),
			vtxs,
			GL_STATIC_DRAW
		);
	}


	// 解放
	stbi_image_free(ftx);


	//== シェーダ作成
	guiFont.charShader = LoadShaders("charVtx.c", "charPxl.c");
}

void engine3dGL::DisposeCharVBO()
{
	glDeleteTextures(1, &guiFont.chrTex);
	glDeleteBuffers(guiFont.guiChrQty, guiFont.chrBuf);
}

double engine3dGL::GetAsp()
{
	return ((double)WIDTH / HEIGHT);
}

// プレイヤー座標の描画
void engine3dGL::DrawCoordinateS3()
{
	//
	double asp = GetAsp();
	GuiString guiStr;
	guiStr.drawArea.l = 0.01;
	guiStr.drawArea.t = 0.01 * asp;
	guiStr.fontSz = 0.030;
	guiStr.fontSpan = 0.8;
	guiStr.padding.l = 0;
	guiStr.padding.t = 0;
	char kanji[9][2] = { {20, 0}, {21, 0}, {22, 0}, {23, 0}, {24, 0}, {25, 0}, {26, 0}, {27, 0}, {28, 0} };

	if (VIEW_XYZ) 
	{

		string locMode = "", loc_X = "", loc_Y = "", loc_Z = "", loc_W = "";
		if (LOC_MODE) {
			if (lang == UI_LANG_EN)
			{
				locMode = "Polor";
				loc_X = "Longitude: ";
				loc_Y = "Latitude : ";
				loc_Z = "Altitude : ";
			}
			else
			{
				locMode.append(kanji[0]).append(kanji[1]).append(kanji[2]);
				loc_X.append(kanji[6]).append(kanji[8]).append(": ");
				loc_Y.append(kanji[5]).append(kanji[8]).append(": ");
				loc_Z.append(kanji[7]).append(kanji[8]).append(": ");
			}

			loc_X.append( to_string((long double)(-objs[PLR_No].loc.x / PIE * 180)) );
			loc_Y.append( to_string((long double)(-objs[PLR_No].loc.y / PIE * 180 + 90)) );
			loc_Z.append( to_string((long double)(-objs[PLR_No].loc.z / PIE * 180 + 90)) );
		}
		else {
			if (lang == UI_LANG_EN) 
			{
				locMode = "Cartesian";
			}
			else 
			{
				locMode.append(kanji[3]).append(kanji[4]).append(kanji[1]).append(kanji[2]);
			}
			pt4 pEuc = object3d::tudeToEuc(objs[PLR_No].loc);

			loc_X = "Loc X: " + to_string((long double)(pEuc.x));
			loc_Y = "Loc Y: " + to_string((long double)(pEuc.y));
			loc_Z = "Loc Z: " + to_string((long double)(pEuc.z));
			loc_W = "Loc W: " + to_string((long double)(pEuc.w));
		}


		guiStr.content = locMode;
		DrawChars(guiStr);

		guiStr.drawArea.t += guiStr.fontSz * asp;
		guiStr.content = loc_X;
		DrawChars(guiStr);

		guiStr.drawArea.t += guiStr.fontSz * asp;
		guiStr.content = loc_Y;
		DrawChars(guiStr);

		guiStr.drawArea.t += guiStr.fontSz * asp;
		guiStr.content = loc_Z;
		DrawChars(guiStr);

		guiStr.drawArea.t += guiStr.fontSz * asp;
		guiStr.content = loc_W;
		DrawChars(guiStr);
	}

	if (VIEW_LocRot) 
	{
		string LR = "", UD = "", FB = "", rot1 = "", rot2 = "", rot3 = "";
		LR.append("L-R: ").append(cmData.loc.x < 0 ? "-" : "+").append( to_string(abs(cmData.loc.x)) );
		UD.append("U-D: ").append(cmData.loc.y < 0 ? "-" : "+").append( to_string(abs(cmData.loc.y)) );
		FB.append("F-B: ").append(cmData.loc.z < 0 ? "-" : "+").append( to_string(abs(cmData.loc.z)) );
		rot1.append("ROT1: ").append(cmData.rot.x < 0 ? "-" : "+").append( to_string(abs(cmData.rot.x)) );
		rot2.append("ROT2: ").append(cmData.rot.y < 0 ? "-" : "+").append( to_string(abs(cmData.rot.y)) );
		rot3.append("ROT3: ").append(cmData.rot.z < 0 ? "-" : "+").append( to_string(abs(cmData.rot.z)) );

		// 移動
		guiStr.drawArea.t += guiStr.fontSz * asp;
		guiStr.content = LR;
		DrawChars(guiStr);

		guiStr.drawArea.t += guiStr.fontSz * asp;
		guiStr.content = UD;
		DrawChars(guiStr);

		guiStr.drawArea.t += guiStr.fontSz * asp;
		guiStr.content = FB;
		DrawChars(guiStr);

		// 回転
		guiStr.drawArea.t += guiStr.fontSz * asp;
		guiStr.content = rot1;
		DrawChars(guiStr);

		guiStr.drawArea.t += guiStr.fontSz * asp;
		guiStr.content = rot2;
		DrawChars(guiStr);

		guiStr.drawArea.t += guiStr.fontSz * asp;
		guiStr.content = rot3;
		DrawChars(guiStr);
	}
}

void engine3dGL::DrawCoordinateH3()
{
	if (!objs[0].used)
		return;

	//
	double asp = GetAsp();
	GuiString guiStr;
	guiStr.drawArea.l = 0.01;
	guiStr.drawArea.t = 0.01 * asp;
	guiStr.fontSz = 0.030;
	guiStr.fontSpan = 0.8;
	guiStr.padding.l = 0;
	guiStr.padding.t = 0;

	if (VIEW_XYZ)
	{
		// 内容設定
		string locMode = "", loc_X = "", loc_Y = "", loc_Z = "", loc_W = "";
		locMode = "Coordinate";
		loc_X = "Radius   : ";
		loc_Y = "Longitude: ";
		loc_Z = "Latitude : ";
		loc_X.append(to_string((long double)(cmCo.x)));
		loc_Y.append(to_string((long double)(cmCo.y / PIE * 180)));
		loc_Z.append(to_string((long double)(-cmCo.z / PIE * 180 + 90)));

		// 描画処理
		guiStr.content = locMode;
		DrawChars(guiStr);

		guiStr.drawArea.t += guiStr.fontSz * asp;
		guiStr.content = loc_X;
		DrawChars(guiStr);

		guiStr.drawArea.t += guiStr.fontSz * asp;
		guiStr.content = loc_Y;
		DrawChars(guiStr);

		guiStr.drawArea.t += guiStr.fontSz * asp;
		guiStr.content = loc_Z;
		DrawChars(guiStr);
	}
}

// プレイヤーと他オブジェクト間の距離を描画
void engine3dGL::DrawDistances()
{
	if (VIEW_DST)
	{
		double asp = GetAsp();
		GuiString guiStr;
		guiStr.fontSz = 0.025;
		guiStr.fontSpan = 0.8;
		guiStr.padding.l = 0;
		guiStr.padding.t = 0;

		//=====カメラの範囲
		double cRangeX = tan((LDBL)CR_RANGE_X * 0.5 * PIE / 180);
		double cRangeY = tan((LDBL)CR_RANGE_Y * 0.5 * PIE / 180);
		object3d * pObj = objs + PLR_No;

		for (int h = 0; h < objCnt; h++) 	//-- 距離の表示
		{
			if (h == PLR_No) continue;
			if (!(objs + h)->used) continue;
			if (!VIEW_PLR && BWH_QTY <= h && h < BWH_QTY + PLR_QTY) continue;
			object3d * eObj = objs + h;

			double len = tan(eObj->locr.y);
			double dx = len * sin(eObj->locr.x);
			double dy = len * cos(eObj->locr.x);

			double sPtX = (dx / cRangeX + 1.0) * 0.5;
			double sPtY = (dy / cRangeY + 1.0) * -0.5 + 1.0;

			//-- 画面内なら表示
			if ((0 < sPtX && sPtX < 1) && (0 < sPtY && sPtY < 1)) 
			{
				//-- 物体間の距離
				pt4 pLoc = pObj->tudeToEuc(pObj->loc);
				pt4 eLoc = pObj->tudeToEuc(eObj->loc);
				double val = asin((pyth4(pLoc.mns(eLoc)) * 0.5)) * 2 * radius;
				string dstR =
					to_string((long double)(val));
				int dstLen = (val < 100.0) ? 4 : 3;		// 有効桁数

				// GL描画
				guiStr.content = dstR.substr(0, dstLen);
				guiStr.drawArea.l = sPtX * 2;
				guiStr.drawArea.t = sPtY * 2;
				DrawChars(guiStr);
			}
		}
	}
}

// 文字列描画
void engine3dGL::DrawChars(GuiString gs) 
{
	double asp = GetAsp();
	double fSz = gs.fontSz;
	double fSp = gs.fontSpan;
	double fl = gs.padding.l;
	double ft = gs.padding.t;
	double l = gs.drawArea.l;
	double t = gs.drawArea.t;

	for (int i = 0; i < gs.content.length(); i++)
	{
		DrawChar
		(
			gs.content[i],
			(-1) + fl + l + fSz * i * fSp,
			1 + (-fSz) * asp + (-ft) * asp + (-t),
			fSz,
			fSz * asp
		);
	}

}

// 1字描画
void engine3dGL::DrawChar(uint8_t dChar, double x, double y, double w, double h)
{
	int charShader = guiFont.charShader;

	glUseProgram(charShader);
	glBindBuffer(GL_ARRAY_BUFFER, guiFont.chrBuf[dChar - 20]);
	glBindTexture(GL_TEXTURE_2D, guiFont.chrTex);

	// 表示位置, 表示幅
	GLint xID = glGetUniformLocation(charShader, "vMod");
	glUniform4f(xID, (float)x, (float)y, (float)w, (float)h);

	GLuint vPosition = glGetAttribLocation(charShader, "vPos");
	GLuint tPosition = glGetAttribLocation(charShader, "tPos");

	// 有効化
	glEnableVertexAttribArray(vPosition);
	glEnableVertexAttribArray(tPosition);

	glVertexAttribPointer(
		vPosition, 2, GL_FLOAT, GL_FALSE,
		4 * sizeof(GLfloat), 0
	);
	glVertexAttribPointer(
		tPosition, 2, GL_FLOAT, GL_FALSE,
		4 * sizeof(GLfloat), (void*)(2 * sizeof(GLfloat))
	);

	glDrawArrays(GL_TRIANGLES, 0, 3 * 2);

	// 無効化
	glDisableVertexAttribArray(vPosition);
	glDisableVertexAttribArray(tPosition);

}

//**** その他 ****//

int engine3dGL::allocMesh()
{
	meshs = new mesh3dGL[meshLen];

	return 1;
}

void engine3dGL::end4D() 
{
	for (uint32_t h = 0; h < ptDel4d; h++) delete[] tMesh.faces[h].pts;
	delete[] tMesh.faces;///4D 後始末
	delete[] tMesh.pts;	///4D 後始末

	tMesh.setNull();
}

// クリック処理
void engine3dGL::ClickProc()
{
	if (ope.clickState == Operation::ClickState::None)
		return;

	if (0 <= ope.clickCoord.x && ope.clickCoord.x < WIDTH
		&& 0 <= ope.clickCoord.y && ope.clickCoord.y < HEIGHT)
	{
		unsigned char tmpIdx;
		int chkIdx = -1;
		glReadPixels(ope.clickCoord.x, ope.clickCoord.y, 1, 1, GL_STENCIL_INDEX, GL_UNSIGNED_BYTE, &tmpIdx);
		chkIdx += tmpIdx;
		glClear(GL_STENCIL_BUFFER_BIT);
		glClear(GL_DEPTH_BUFFER_BIT);
		glClearStencil(0);

		// 1ピクセル再描画
		glScissor(ope.clickCoord.x, ope.clickCoord.y, 1, 1);
		if(worldGeo == WorldGeo::HYPERBOLIC)
			DrawEachObjsH3(1);
		else if(qyMode == QY_MODE::HIGH)
			DrawEachObjsS3(1);
		else
			DrawEachObjsS3_LQY(1);

		glScissor(0, 0, WIDTH, HEIGHT);

		glReadPixels(ope.clickCoord.x, ope.clickCoord.y, 1, 1, GL_STENCIL_INDEX, GL_UNSIGNED_BYTE, &tmpIdx);
		chkIdx += tmpIdx * 256;

		// クリックしたオブジェクトの特定
		if (ope.clickState == Operation::ClickState::Left)
			viewTrackIdx = chkIdx;
		else if (ope.clickState == Operation::ClickState::Right)
			selectedIdx = chkIdx;

		//// 選択オブジェクト情報をコンソール表示
		//if (selectedIdx != -1)
		//	cout << "selected object index : " << selectedIdx << endl;
	}

	ope.clickState = Operation::ClickState::None;
}

double engine3dGL::clcRangeY(double rangeX)
{
	return atan(tan((LDBL)(rangeX) / 2 * PIE / 180) * HEIGHT / WIDTH) * 2 / PIE * 180;	//カメラ設定
}


