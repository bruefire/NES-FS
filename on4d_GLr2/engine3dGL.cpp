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




//*** GUI�N���X ***//

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

GuiItem* GuiContainer::SelectedChild()
{
	return childs[selectedIdx];
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


//**** ��{���\�b�h ****//
//
// init, update, dispose, �R���X�g���N�^, �f�X�g���N�^
//

mesh3dGL::~mesh3dGL()
{
	if (texJD)
		stbi_image_free(bmp);
}


int mesh3dGL::makeDataForGL()
{
	if (0 < faceLen)	//-- ��ɖ�
	{
		int fltLen = 8;

		pts2 = new float[faceLen * fltLen * 3];
		ptsGl = new int[faceLen * 3];
		for (int i = 0; i < faceLen; i++) {
			for (char j = 0; j < 3; j++) {
				int idx = i * fltLen * 3 + fltLen * j;
				pts2[idx + 0] = faces[i].pts[j]->x;	//-- ���_
				pts2[idx + 1] = faces[i].pts[j]->y;
				pts2[idx + 2] = faces[i].pts[j]->z;

				pts2[idx + 3] = faces[i].col.R / 255.0;	//-- �F
				pts2[idx + 4] = faces[i].col.G / 255.0;
				pts2[idx + 5] = faces[i].col.B / 255.0;
				if (texJD) {
					pts2[idx + 6] = faces[i].txs[j]->x;	//-- �e�N�X�`��
					pts2[idx + 7] = faces[i].txs[j]->y;
				}
				else {
					pts2[idx + 6] = 0;	//-- �e�N�X�`��
					pts2[idx + 7] = 0;
				}
			}
		}
		bufLen = faceLen * fltLen * 3;

	}
	else {			//-- ������Ε�
		pts2 = new float[lLen * 8 * 2];
		ptsGl = new int[1];//���g�p
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
	if (0 < faceLen)	//-- ��ɖ�
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

				// �ʒu�Z�o�pLoc
				pts2[idx + 6] = faces[i].pts[0]->x;	
				pts2[idx + 7] = faces[i].pts[0]->y;
				pts2[idx + 8] = faces[i].pts[0]->z;

				pts2[idx + 9] = faces[i].pts[1]->x;	
				pts2[idx + 10] = faces[i].pts[1]->y;
				pts2[idx + 11] = faces[i].pts[1]->z;

				pts2[idx + 12] = faces[i].pts[2]->x;	
				pts2[idx + 13] = faces[i].pts[2]->y;
				pts2[idx + 14] = faces[i].pts[2]->z;

				// �ʒu�Z�o�ptex
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
	else {			//-- ������Ε�
		int fltLen = 12;
		pts2 = new float[lLen * fltLen * 2];
		ptsGl = new int[1];//���g�p

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

				// �ʒu�Z�o�pLoc
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

	//-- ���_�]��
	glBindBuffer(GL_ARRAY_BUFFER, buffers[texNo]);
	glBufferData(	//---- pts�]��
		GL_ARRAY_BUFFER,
		bufLen * sizeof(float),
		pts2,
		GL_STATIC_DRAW
	);


	// �e�N�X�`���]��
	if (!texJD) return 0;
	if (texName.empty()) return 0;


	glBindTexture(GL_TEXTURE_2D, texNames[texNo]);

	int posA = 0;
	if (!extObjFlg) posA = texName.find_last_of('\\') + 1;
	int posB = texName.find_last_of('\r');
	if (posB == texName.npos)
		posB = texName.find_last_of('\n');
	texName = texName.substr(posA, posB - posA);

	bmp = stbi_load(&texName[0], texWH, texWH + 1, NULL, 4);
	if (bmp == NULL) {
		cout << "�e�N�X�`���ǂݍ��ݎ��s" << endl;
		//PostQuitMessage(0);
	}
	///-- 
	glTexImage2D(		//bmp�]��
		GL_TEXTURE_2D, 0, GL_RGBA2, texWH[0], texWH[1],
		0, GL_RGBA, GL_UNSIGNED_BYTE, bmp
	);


	return 1;
}


void engine3dGL::MakeCommonVBO(mesh3dGL* mesh)
{
	if (mesh->pts != nullptr)
	{
		switch (qyMode)
		{
		case QY_MODE::HIGH:
			(mesh)->makeDataForGL();
			break;

		case QY_MODE::LOW:
			if (mesh->coorType == mesh3d::COOR::Cartesian)
				(mesh)->makeDataForGL();
			else
				(mesh)->makeDataForGL_LQY(this);
			break;
		}

		(mesh)->transBuffers(buffers, texNames);
	}

}


int engine3dGL::init()
{
	try
	{
		// ���N���X��init
		engine3d::init();

		GL_Init();
	}
	// �I���Ɍ�����
	catch (S3ErrorManager ex)
	{
		disposeFlg = true;
		throw ex;
	}


	// �t�H���g GUI VBO
	InitGUI();

	// �}�b�v
	tObj.mesh = &tMesh;
	pols = new poly[1]; polCnt = 1;
	pols[0].polyInit(3);
	pol_pLen = pols[0].pLen;

	//-- ���b�V���]��
	for (int i = 0; i < meshLen; i++)
	{
		if(!meshs[i].isLazyLoaded)
			MakeCommonVBO((mesh3dGL*)(meshs + i));
	}


	return 1;
}

int engine3dGL::update()
{
	bool result;

	// ���(���Z)�N���X
	result = engine3d::update();


	//======== << �G���W�� >> =======//
	GL_SwapBuffer();
	if (worldGeo == WorldGeo::SPHERICAL)
	{
		glEnable(GL_STENCIL_TEST);
		glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
		MakeTracingLinesS3();
		simulateS3GL();
		glDisable(GL_STENCIL_TEST);

		// GUI�`��
		glDisable(GL_DEPTH_TEST);
		glEnable(GL_BLEND);

		DrawDistancesS3();
		DrawCoordinateS3();

		glDisable(GL_BLEND);
		glEnable(GL_DEPTH_TEST);

		// map�`��
		if (VIEW_ON4) DrawMapS3();

	}
	else if (worldGeo == WorldGeo::HYPERBOLIC)
	{
		glEnable(GL_STENCIL_TEST);
		glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
		MakeTracingLinesH3();
		SimulateH3GL();
		glDisable(GL_STENCIL_TEST);

		// GUI�`��
		glDisable(GL_DEPTH_TEST);
		glEnable(GL_BLEND);

		// draw info
		DrawDistancesH3();
		DrawCoordinateH3();

		glDisable(GL_BLEND);
		glEnable(GL_DEPTH_TEST);

		// map�`��
		if (VIEW_ON4) DrawMapH3();
	}

	drawGui();


	return result;
}


int engine3dGL::dispose()
{
	if (disposeFlg)
		return 0;

	
	end4D();			// �}�b�v�㏈��
	DisposeGUI();		// ASCII GUI VBO ���
	GL_End();
	delete[] pols;


	// ���N���X
	engine3d::dispose();

	disposeFlg = true;

	return 1;
}

void engine3dGL::InitGUI()
{
	MakeCharVBO();
	glGenBuffers(1, &ctnrBuf);
	makeGuiPlateVBO();
}

void engine3dGL::DisposeGUI()
{
	DisposeCharVBO();	// ASCII VBO ���
	glDeleteBuffers(1, &ctnrBuf);
	glDeleteProgram(guiShader);
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

	///�� 3D�X�N���[��
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


	CR_RANGE_Y = clcRangeY(CR_RANGE_X);	//�J�����ݒ�
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

//*** GUI, �t�H���g ***
void engine3dGL::MakeCharVBO()
{
	// �o�b�t�@����
	glGenBuffers(guiFont.guiChrQty, guiFont.chrBuf);
	glGenTextures(1, &guiFont.chrTex);

	// �܂��o�C���h
	glBindTexture(GL_TEXTURE_2D, guiFont.chrTex);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);


	// �e�N�X�`���f�[�^�]��
	int ftxWidth;
	int ftxHeight;

	stbi_uc* ftx = stbi_load(&(guiFont.chrImgNm[0]), &ftxWidth, &ftxHeight, NULL, 4);
	if (ftx == NULL) {
		cout << "�e�N�X�`���ǂݍ��ݎ��s" << endl;
	}

	// �e�N�X�`���]��
	glTexImage2D(
		GL_TEXTURE_2D, 0, GL_RGBA2, ftxWidth, ftxHeight,
		0, GL_BGRA, GL_UNSIGNED_BYTE, ftx
	);



	for (int i = 0; i < guiFont.guiChrQty; i++)
	{
		// !
		pt2 vtx[6];
		pt2 txs[6];


		// �|���S�����_�ݒ�
		vtx[0].x = 1.0, vtx[0].y = 1.0;
		vtx[1].x = 0, vtx[1].y = 1.0;
		vtx[2].x = 0, vtx[2].y = 0;
		vtx[3] = vtx[0];
		vtx[4] = vtx[2];
		vtx[5].x = 1.0;
		vtx[5].y = 0;


		// �e�N�X�`�����_�ݒ�
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


		// ����
		float vtxs[24] =
		{
			(float)vtx[0].x, (float)vtx[0].y, (float)txs[0].x, (float)txs[0].y,
			(float)vtx[1].x, (float)vtx[1].y, (float)txs[1].x, (float)txs[1].y,
			(float)vtx[2].x, (float)vtx[2].y, (float)txs[2].x, (float)txs[2].y,
			(float)vtx[3].x, (float)vtx[3].y, (float)txs[3].x, (float)txs[3].y,
			(float)vtx[4].x, (float)vtx[4].y, (float)txs[4].x, (float)txs[4].y,
			(float)vtx[5].x, (float)vtx[5].y, (float)txs[5].x, (float)txs[5].y
		};


		// ���_�]��
		glBindBuffer(GL_ARRAY_BUFFER, guiFont.chrBuf[i]);
		glBufferData(	//---- pts�]��
			GL_ARRAY_BUFFER,
			24 * sizeof(float),
			vtxs,
			GL_STATIC_DRAW
		);
	}


	// ���
	stbi_image_free(ftx);


	//== �V�F�[�_�쐬
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

// �v���C���[���W�̕`��
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

			loc_X.append( to_string((long double)(-objs[PLR_No].loc.x / PI * 180)) );
			loc_Y.append( to_string((long double)(-objs[PLR_No].loc.y / PI * 180 + 90)) );
			loc_Z.append( to_string((long double)(-objs[PLR_No].loc.z / PI * 180 + 90)) );
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

		// �ړ�
		guiStr.drawArea.t += guiStr.fontSz * asp;
		guiStr.content = LR;
		DrawChars(guiStr);

		guiStr.drawArea.t += guiStr.fontSz * asp;
		guiStr.content = UD;
		DrawChars(guiStr);

		guiStr.drawArea.t += guiStr.fontSz * asp;
		guiStr.content = FB;
		DrawChars(guiStr);

		// ��]
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
		// ���e�ݒ�
		string locMode = "", loc_X = "", loc_Y = "", loc_Z = "", loc_W = "";

		if (viewTrackIdx >= 0 && objs[viewTrackIdx].used)
		{
			locMode = "Relative Coords";
			loc_X = "Radius   : ";
			loc_Y = "Longitude: ";
			loc_Z = "Latitude : ";
			loc_X.append(to_string((long double)(cmCo.x)));
			loc_Y.append(to_string((long double)(cmCo.y / PI * 180)));
			loc_Z.append(to_string((long double)(-cmCo.z / PI * 180 + 90)));
		}
		else
		{
			locMode = "Half Space Coords";
			loc_X = "X : ";
			loc_Y = "Y : ";
			loc_Z = "Z : ";
			loc_W = "AREA :("; 
			loc_X.append(to_string((long double)(cmCo.x)));
			loc_Y.append(to_string((long double)(cmCo.y / H3_HALF_SPACE_AREA_IM_RATE)));
			loc_Z.append(to_string((long double)(cmCo.z / H3_HALF_SPACE_AREA_IM_RATE)));
			loc_W.append(to_string((int)objs[PLR_No].area.x) + ", ");
			loc_W.append(to_string((int)objs[PLR_No].area.y) + ", ");
			loc_W.append(to_string((int)objs[PLR_No].area.z) + ")");
		}

		// �`�揈��
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
}

// �v���C���[�Ƒ��I�u�W�F�N�g�Ԃ̋�����`��
void engine3dGL::DrawDistancesS3()
{
	if (VIEW_DST)
	{
		double asp = GetAsp();
		GuiString guiStr;
		guiStr.fontSz = 0.025;
		guiStr.fontSpan = 0.8;
		guiStr.padding.l = 0;
		guiStr.padding.t = 0;

		//=====�J�����͈̔�
		double cRangeX = tan((LDBL)CR_RANGE_X * 0.5 * PI / 180);
		double cRangeY = tan((LDBL)CR_RANGE_Y * 0.5 * PI / 180);
		object3d * pObj = objs + PLR_No;

		for (int h = 0; h < objCnt; h++) 	//-- �����̕\��
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

			//-- ��ʓ��Ȃ�\��
			if ((0 < sPtX && sPtX < 1) && (0 < sPtY && sPtY < 1)) 
			{
				//-- ���̊Ԃ̋���
				pt4 pLoc = pObj->tudeToEuc(pObj->loc);
				pt4 eLoc = pObj->tudeToEuc(eObj->loc);
				double val = asin((pyth4(pLoc.mns(eLoc)) * 0.5)) * 2 * radius;
				string dstR =
					to_string((long double)(val));
				int dstLen = (val < 100.0) ? 4 : 3;		// �L������

				// GL�`��
				guiStr.content = dstR.substr(0, dstLen);
				guiStr.drawArea.l = sPtX * 2;
				guiStr.drawArea.t = sPtY * 2;
				DrawChars(guiStr);
			}
		}
	}
}

// �v���C���[�Ƒ��I�u�W�F�N�g�Ԃ̋�����`��
void engine3dGL::DrawDistancesH3()
{
	if (VIEW_DST)
	{
		double asp = GetAsp();
		GuiString guiStr;
		guiStr.fontSz = 0.025;
		guiStr.fontSpan = 0.8;
		guiStr.padding.l = 0;
		guiStr.padding.t = 0;

		//=====�J�����͈̔�
		double cRangeX = tan((LDBL)CR_RANGE_X * 0.5 * PI / 180);
		double cRangeY = tan((LDBL)CR_RANGE_Y * 0.5 * PI / 180);
		object3d* pObj = objs + PLR_No;

		for (int h = 0; h < objCnt; h++) 	//-- �����̕\��
		{
			if (h == PLR_No) continue;
			if (!(objs + h)->used) continue;
			if (!VIEW_PLR && BWH_QTY <= h && h < BWH_QTY + PLR_QTY) continue;
			object3d* eObj = objs + h;

			double dx = eObj->locr.x / eObj->locr.z;
			double dy = eObj->locr.y / eObj->locr.z;
			if (eObj->locr.z < 0 || isinf(dx) || isinf(dy))
				continue;

			double sPtX = (dx / cRangeX + 1.0) * 0.5;
			double sPtY = (dy / cRangeY + 1.0) * -0.5 + 1.0;

			//-- ��ʓ��Ȃ�\��
			if ((0 < sPtX && sPtX < 1) && (0 < sPtY && sPtY < 1))
			{
				//-- get distance between player and obj.
				double val = object3d::ClcHypbFromEuc(pyth3(eObj->locr)) * radius;
				string dstR =
					to_string((long double)(val));
				int dstLen = (val < 100.0) ? 4 : 3;		// �L������

				// GL�`��
				guiStr.content = dstR.substr(0, dstLen);
				guiStr.drawArea.l = sPtX * 2;
				guiStr.drawArea.t = sPtY * 2;
				DrawChars(guiStr);
			}
		}
	}
}

// ������`��
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

// 1���`��
void engine3dGL::DrawChar(uint8_t dChar, double x, double y, double w, double h)
{
	int charShader = guiFont.charShader;

	glUseProgram(charShader);
	glBindBuffer(GL_ARRAY_BUFFER, guiFont.chrBuf[dChar - 20]);
	glBindTexture(GL_TEXTURE_2D, guiFont.chrTex);

	// �\���ʒu, �\����
	GLint xID = glGetUniformLocation(charShader, "vMod");
	glUniform4f(xID, (float)x, (float)y, (float)w, (float)h);

	GLuint vPosition = glGetAttribLocation(charShader, "vPos");
	GLuint tPosition = glGetAttribLocation(charShader, "tPos");

	// �L����
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

	// ������
	glDisableVertexAttribArray(vPosition);
	glDisableVertexAttribArray(tPosition);

}

//**** ���̑� ****//

mesh3d* engine3dGL::GetMesh(int idx)
{
	return (mesh3dGL*)meshs + idx;
}

int engine3dGL::allocMesh()
{
	meshs = new mesh3dGL[meshLen];

	return 1;
}

void engine3dGL::end4D() 
{
	for (uint32_t h = 0; h < ptDel4d; h++) delete[] tMesh.faces[h].pts;
	delete[] tMesh.faces;///4D ��n��
	delete[] tMesh.pts;	///4D ��n��

	tMesh.setNull();
}

// �N���b�N����
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

		// 1�s�N�Z���ĕ`��
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

		// �N���b�N�����I�u�W�F�N�g�̓���
		if (ope.clickState == Operation::ClickState::Left)
			viewTrackIdx = chkIdx;
		else if (ope.clickState == Operation::ClickState::Right)
			selectedIdx = chkIdx;

		//// �I���I�u�W�F�N�g�����R���\�[���\��
		//if (selectedIdx != -1)
		//	cout << "selected object index : " << selectedIdx << endl;
	}

	ope.clickState = Operation::ClickState::None;
}

double engine3dGL::clcRangeY(double rangeX)
{
	return atan(tan((LDBL)(rangeX) / 2 * PI / 180) * HEIGHT / WIDTH) * 2 / PI * 180;	//�J�����ݒ�
}

void engine3dGL::makeGuiPlateVBO()
{

	// �|���S�����_�ݒ� �P�Ȃ�l�p�`
	pt2 vtx[6];

	vtx[0].x = 1.0, vtx[0].y = 1.0;
	vtx[1].x = 0, vtx[1].y = 1.0;
	vtx[2].x = 0, vtx[2].y = 0;
	vtx[3] = vtx[0];
	vtx[4] = vtx[2];
	vtx[5].x = 1.0;
	vtx[5].y = 0;

	// �]���p�ɍč\��
	float vtxs[12] =
	{
		(float)vtx[0].x, (float)vtx[0].y,
		(float)vtx[1].x, (float)vtx[1].y,
		(float)vtx[2].x, (float)vtx[2].y,
		(float)vtx[3].x, (float)vtx[3].y,
		(float)vtx[4].x, (float)vtx[4].y,
		(float)vtx[5].x, (float)vtx[5].y
	};


	// ���_�]��
	glBindBuffer(GL_ARRAY_BUFFER, ctnrBuf);
	glBufferData(	//---- pts�]��
		GL_ARRAY_BUFFER,
		24 * sizeof(float),
		vtxs,
		GL_STATIC_DRAW
		);

	// �V�F�[�_�쐬
	guiShader = LoadShaders("guiVtx.c", "guiPxl.c");
}


// GUI �`��
void engine3dGL::drawGui()
{
	GuiContainer& menu = menuLgc->menu;

	if (!menu.displayed)
		return;

	glEnable(GL_BLEND);
	glDisable(GL_DEPTH_TEST);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	double asp = ((double)WIDTH / HEIGHT);

	double x = menu.drawArea.sz.x;
	double y = menu.drawArea.sz.y * asp;
	double l = menu.drawArea.l;
	double t = menu.drawArea.t * asp;
	drawGuiPlate
	(
	(-1) + l,
		1 + (-y) + (-t),
		x,
		y,
		menu.bkColor
		);
	// �ċA
	GuiContainer* active = (GuiContainer*)menu.childs[menu.selectedIdx];
	drawGuiRecur(active);

	glEnable(GL_DEPTH_TEST);
	glDisable(GL_BLEND);

}



void engine3dGL::drawGuiRecur(GuiContainer* ct)
{
	int len = ct->childs.size();
	double asp = ((double)WIDTH / HEIGHT);

	for (int i = 0; i < len; i++)
	{
		double x = ct->childs[i]->drawArea.sz.x;
		double y = ct->childs[i]->drawArea.sz.y * asp;
		double l = ct->childs[i]->drawArea.l;
		double t = ct->childs[i]->drawArea.t * asp;
		pt3 col = (ct->selectedIdx == i) ? pt3(0.8, 0.8, 1.0) : ct->childs[i]->bkColor;
		drawGuiPlate
		(
		(-1) + l,
			1 + (-y) + (-t),
			x,
			y,
			col
			);

		string tStr;
		double fSz;
		double fSp;
		double fl;
		double ft;

		switch (ct->childs[i]->type)
		{
			// ������`��
		case GuiItem::GuiType::STRING:
			tStr = ((GuiString*)ct->childs[i])->content;
			fSz = ((GuiString*)ct->childs[i])->fontSz;
			fSp = ((GuiString*)ct->childs[i])->fontSpan;
			fl = ct->childs[i]->padding.l;
			ft = ct->childs[i]->padding.t;

			for (int i = 0; i < tStr.length(); i++)
			{
				DrawChar
				(
					tStr[i],
					(-1) + fl + l + fSz * i * fSp,
					1 + (-fSz) * asp + (-ft) * asp + (-t),
					fSz,
					fSz * asp
					);
			}

			break;

			// �R���e�i�`��
		case GuiItem::GuiType::CONTAINER:
			drawGuiRecur((GuiContainer*)ct->childs[i]);
			break;
		}
	}
}


// GUI�R���e�i�`��
void engine3dGL::drawGuiPlate(double x, double y, double w, double h, pt3 col)
{
	int charShader = guiFont.charShader;

	glUseProgram(guiShader);
	glBindBuffer(GL_ARRAY_BUFFER, ctnrBuf);

	// �\���ʒu, �\����
	GLint xID = glGetUniformLocation(guiShader, "vMod");
	glUniform4f(xID, (float)x, (float)y, (float)w, (float)h);
	// �F
	xID = glGetUniformLocation(guiShader, "bkColor");
	glUniform4f(xID, col.x, col.y, col.z, (float)0.5);


	GLuint vPosition = glGetAttribLocation(charShader, "vPos");

	glEnableVertexAttribArray(vPosition);
	glVertexAttribPointer(
		vPosition, 2, GL_FLOAT, GL_FALSE,
		2 * sizeof(GLfloat), 0
		);

	glDrawArrays(GL_TRIANGLES, 0, 3 * 2);

	glDisableVertexAttribArray(vPosition);

}

void engine3dGL::CreateBuffersForVRMenu()
{
	MakeCommonVBO((mesh3dGL*)&vrMenuMesh);
	menuTex = texNames[vrMenuMesh.texNo];

	glGenFramebuffers(1, &menuFBO);
	glBindFramebuffer(GL_FRAMEBUFFER, menuFBO);
	glBindTexture(GL_TEXTURE_2D, menuTex);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, WIDTH, HEIGHT, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, menuTex, 0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void engine3dGL::DisposeBuffersForVRMenu()
{
	glDeleteFramebuffers(1, &menuFBO);
}

void engine3dGL::DrawGUIForVR()
{
	int oldFbo;
	glGetIntegerv(GL_FRAMEBUFFER_BINDING, &oldFbo);

	glBindFramebuffer(GL_FRAMEBUFFER, menuFBO);
	glBindTexture(GL_TEXTURE_2D, menuTex);

	glClearColor(0.2, 0.2, 0.2, 1.0);
	glClear(GL_COLOR_BUFFER_BIT);
	glClear(GL_DEPTH_BUFFER_BIT);

	drawGui();

	glBindFramebuffer(GL_FRAMEBUFFER, oldFbo);

}

void engine3dGL::MakeTracingLinesS3()
{
	if (qyMode == engine3dGL::QY_MODE::HIGH)
	{
		//-- �O�Ռ`�� --//
		for (int h = 0; h < OBJ_QTY; h++) {
			object3d* curObj = objs + h;

			for (int i = 0; i < object3d::PAST_QTY - 1; i++) {
				int idx = h * (object3d::PAST_QTY - 1) * 16 + i * 16;

				markMesh.pts2[idx + 0] = curObj->past[i].x;
				markMesh.pts2[idx + 1] = curObj->past[i].y;
				markMesh.pts2[idx + 2] = curObj->past[i].z;
				markMesh.pts2[idx + 3] = 1.0;
				markMesh.pts2[idx + 4] = 0;
				markMesh.pts2[idx + 5] = (double)i / object3d::PAST_QTY;

				markMesh.pts2[idx + 8] = curObj->past[i + 1].x;
				markMesh.pts2[idx + 9] = curObj->past[i + 1].y;
				markMesh.pts2[idx + 10] = curObj->past[i + 1].z;
				markMesh.pts2[idx + 11] = 1.0;
				markMesh.pts2[idx + 12] = 0;
				markMesh.pts2[idx + 13] = (double)i / object3d::PAST_QTY;

				markMesh.pts2[idx + 6] = markMesh.pts2[idx + 7] =
					markMesh.pts2[idx + 14] = markMesh.pts2[idx + 15] = 0;

			}
		}
	}
	else
	{
		//-- �O�Ռ`�� --//
		for (int h = 0; h < OBJ_QTY; h++) {
			object3d* curObj = objs + h;

			for (int i = 0; i < object3d::PAST_QTY - 1; i++) {
				for (int j = 0; j < 2; j++)
				{
					int idx = (h * (object3d::PAST_QTY - 1) * 2 + i * 2 + j) * 12;
					if (!j)
					{
						markMesh.pts2[idx + 0] = curObj->past[i].x;
						markMesh.pts2[idx + 1] = curObj->past[i].y;
						markMesh.pts2[idx + 2] = curObj->past[i].z;
					} else {
						markMesh.pts2[idx + 0] = curObj->past[i + 1].x;
						markMesh.pts2[idx + 1] = curObj->past[i + 1].y;
						markMesh.pts2[idx + 2] = curObj->past[i + 1].z;
					}
					markMesh.pts2[idx + 3] = 1.0;
					markMesh.pts2[idx + 4] = 0;
					markMesh.pts2[idx + 5] = (double)i / object3d::PAST_QTY;

					markMesh.pts2[idx + 6] = curObj->past[i].x;
					markMesh.pts2[idx + 7] = curObj->past[i].y;
					markMesh.pts2[idx + 8] = curObj->past[i].z;
					markMesh.pts2[idx + 9] =  curObj->past[i + 1].x;
					markMesh.pts2[idx + 10] = curObj->past[i + 1].y;
					markMesh.pts2[idx + 11] = curObj->past[i + 1].z;
				}
			}
		}
	}
}

void engine3dGL::MakeTracingLinesH3()
{
	if (!markObj.used)
		return;

	object3d* plrObj = &objs[PLR_No];
	object3d kleinPlr = plrObj->HalfSpace2Klein();
	pt3 std1N = kleinPlr.std[0].mtp(1 / H3_STD_LEN);
	pt3 std2N = kleinPlr.std[1].mtp(1 / H3_STD_LEN);
	pt3 sideN = pt3::cross(std2N, std1N);

	double radius = this->radius;
	auto test = [radius, plrObj, std1N, std2N, sideN](int i, object3d* curObj)
	{
		// HACK
		object3d curCpy = *curObj;
		curCpy.loc = curObj->past[i];
		curCpy.std[0] = curObj->past[i];
		curCpy.std[1] = curObj->past[i];
		curCpy.area = curObj->pastArea[i];
		object3d kleinObj = curCpy.HalfSpace2Klein(plrObj);

		pt3 loc = pt3(
			pt3::dot(sideN, kleinObj.loc),
			pt3::dot(std2N, kleinObj.loc),
			pt3::dot(std1N, kleinObj.loc));

		pt3 tmpt;
		tmpt.x = atan2(loc.x, loc.z);		//--����1
		tmpt.y = atan2(pyth2(loc.x, loc.z), loc.y);	//--����2
		tmpt.z = object3d::ClcHypbFromEuc(pyth3(loc)) * radius;	//--����(����)

		return tmpt;
	};

	//-- �O�Ռ`�� --//
	for (int h = 0; h < OBJ_QTY; h++) 
	{
		object3d* curObj = &objs[h];

		pt3 tmpt[2];
		tmpt[1] = test(0, curObj);

		for (int i = 0; i < object3d::PAST_QTY - 1; i++) 
		{
			tmpt[0] = tmpt[1];
			tmpt[1] = test(i + 1, curObj);

			for (int j = 0; j < 2; j++)
			{

				int idx = (h * (object3d::PAST_QTY - 1) * 2 + i * 2 + j) * 12;

				markMesh.pts2[idx + 0] = tmpt[j].x;
				markMesh.pts2[idx + 1] = tmpt[j].y;
				markMesh.pts2[idx + 2] = tmpt[j].z;

				markMesh.pts2[idx + 3] = 1.0;
				markMesh.pts2[idx + 4] = 0;
				markMesh.pts2[idx + 5] = (double)i / object3d::PAST_QTY;

				markMesh.pts2[idx + 6] = tmpt[0].x;
				markMesh.pts2[idx + 7] = tmpt[0].y;
				markMesh.pts2[idx + 8] = tmpt[0].z;
				markMesh.pts2[idx + 9] =  tmpt[1].x;
				markMesh.pts2[idx + 10] = tmpt[1].y;
				markMesh.pts2[idx + 11] = tmpt[1].z;
			}
		}
	}
}

bool engine3dGL::LoadLazyObject(int idx)
{
	if (!engine3d::LoadLazyObject(idx))
		return false;

	MakeCommonVBO((mesh3dGL*)(meshs + idx));
	return true;
}



