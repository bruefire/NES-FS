#pragma once

#include <stdint.h>
#include <string>
#include <glew.h>
#include <GL/gl.h>
#include <vector>
#include "constants.h"
#include "engine3d.h"


// メニュー関係
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

	// コンストラクタ
	GuiString();
	GuiString(std::string content, GlRect rect);

};
// end メニュー関係 


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
		std::string chrImgNm;	// フォントimgName
		GLuint chrTex;			// フォントテクスチャ
		int guiChrQty;
		uint32_t* chrBuf;
		int charShader;

		GuiFont();
		~GuiFont();
	};


	// フィールド //
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

	// マップ
	mesh3d tMesh;
	object3d tObj;	//4to3 描画用3dオブジェクト
	bool VIEW_ON4;
	uint32_t ptDel4d;

	// メソッド //
	int init();
	int update();
	int dispose();
	// 描画
	int simulateS3GL();
	int drawEachObjs();
	int drawEachObjs_LQY();
	double GetAsp();
	
	void MakeCharVBO();
	void DisposeCharVBO();
	void DrawCoordinate();	// 座標描画
	void DrawDistances();	// 距離描画
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

	// マップ
	void end4D();


	//private:
	uint32_t* buffers;
	uint32_t* texNames;
	int shader[5];
	int outMax;

	// コンストラクタ, デストラクタ
	engine3dGL();
	~engine3dGL();
};

