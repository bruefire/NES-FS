#pragma once

#include <stdint.h>
#include <string>
#include <glew.h>
#include <GL/gl.h>
#include <vector>
#include "constants.h"
#include "engine3d.h"


// ���j���[�֌W
class GlRect : public Rect
{
public:
	enum STD
	{
		NORMAL,
		HORIZONAL,
		VERTICAL
	};
	STD mode;

	GlRect();
	GlRect(pt2, double, double, STD);
	GlRect(double, double, double, double, STD);
	GlRect(double, STD);
};


class GuiItem
{
public:
	enum GuiType
	{
		BASE,
		CONTAINER,
		STRING
	};
	GuiType type;

	GuiItem* owner;

	GlRect drawArea;
	GlRect padding;
	GlRect margin;

	pt3 bkColor;
	double alpha;

	bool displayed;
	bool checked;

	GuiItem();

};

class GuiContainer : public GuiItem
{
public:
	std::vector<GuiItem*> childs;
	int selectedIdx;

	GuiContainer();
	GuiItem* SelectedChild();
};


class GuiString : public GuiItem
{
public:
	std::string content;
	double fontSz;
	double fontSpan;

	// �R���X�g���N�^
	GuiString();
	GuiString(std::string content, GlRect rect);

};
// end ���j���[�֌W 


class mesh3dGL : public mesh3d
{
public:
	~mesh3dGL();
	int makeDataForGL();
	int makeDataForGL_LQY();
	int transBuffers(uint32_t*, uint32_t*);
};


class engine3dGL : public engine3d
{
public:
	enum QY_MODE
	{
		LOW,
		HIGH
	};

	class GuiFont
	{
	public:
		std::string chrImgNm;	// �t�H���gimgName
		GLuint chrTex;			// �t�H���g�e�N�X�`��
		int guiChrQty;
		uint32_t* chrBuf;
		int charShader;

		GuiFont();
		~GuiFont();
	};


	// �t�B�[���h //
	int lang;
	bool disposeFlg;
	bool first4;
	int WIDTH;
	int HEIGHT;
	int decMode;
	int bgCol;
	double CR_RANGE_X;
	double CR_RANGE_Y;
	bool VIEW_PLR;
	bool VIEW_XYZ;
	bool LOC_MODE;
	bool VIEW_DST;
	bool VIEW_LocRot;
	QY_MODE qyMode;

	GuiFont guiFont;

	// �}�b�v
	mesh3d tMesh;
	object3d tObj;	//4to3 �`��p3d�I�u�W�F�N�g
	bool VIEW_ON4;
	uint32_t ptDel4d;

	// ���\�b�h //
	int init();
	int update();
	int dispose();
	// �`��
	int simulateS3GL();
	int drawEachObjs();
	int drawEachObjs_LQY();
	double GetAsp();
	
	void MakeCharVBO();
	void DisposeCharVBO();
	void DrawCoordinate();	// ���W�`��
	void DrawDistances();	// �����`��
	void DrawChar(uint8_t dChar, double x, double y, double w, double h);
	void DrawChars(GuiString);

	// OpenGL
	int GL_Init();
	int GL_End();
	virtual int GL_CreateContextPls() = 0;
	virtual int GL_DeleteContextPls() = 0;
	virtual int GL_SwapBuffer() = 0;

	uint32_t LoadShaders(const char* vPath, const char* fPath);
	uint32_t LoadShaders2(const char* vPath, const char* gPath, const char* fPath, char mode);

	int allocMesh();

	// �}�b�v
	void end4D();


	//private:
	uint32_t* buffers;
	uint32_t* texNames;
	int shader[5];
	int outMax;

	// �R���X�g���N�^, �f�X�g���N�^
	engine3dGL();
	~engine3dGL();
};

