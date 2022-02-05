#pragma once

#include <stdint.h>
#include <string>
#include <vector>
#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include "constants.h"
#include "guiItem.h"
#include "engine3d.h"
#include "MenuLgc.h"


// todo: フィールドを追加する場合は"meshs + n"or"meshs[n]"etc部分を仮想関数に変更
// さもないと恐らくアクセスエラー
class mesh3dGL : public mesh3d
{
public:
	~mesh3dGL();
	int makeDataForGL();
	int makeDataForGL_LQY(engine3d*);
	int transBuffers(uint32_t*, uint32_t*);
};


class engine3dGL : public engine3d
{
public:
	enum QY_MODE
	{
		LOW,
		HIGH
	} qyMode;

	class GuiFont
	{
	public:
		std::string chrImgNm;	// フォントimgName
		uint32_t chrTex;			// フォントテクスチャ
		int guiChrQty;
		uint32_t* chrBuf;
		int charShader;

		GuiFont();
		~GuiFont();
	};


	// フィールド //
	int lang;
	bool disposeFlg;
	int WIDTH;
	int HEIGHT;
	int decMode;
	int bgCol;
	double CR_RANGE_X;
	double CR_RANGE_Y;
	double CR_RANGE_R;
	double CR_RANGE_D;
	bool VIEW_PLR;
	bool VIEW_XYZ;
	bool LOC_MODE;
	bool VIEW_DST;
	bool VIEW_LocRot;
	int glTexLen;

	GuiFont guiFont;

	// マップ
	mesh3d tMesh;
	object3d tObj;	//4to3 描画用3dオブジェクト
	bool VIEW_ON4;
	uint32_t ptDel4d;

	// メニュー
	int guiShader;
	uint32_t ctnrBuf;
	void makeGuiPlateVBO();
	void drawGui();
	void drawGuiRecur(GuiContainer* ct);
	void drawGuiPlate(double x, double y, double w, double h, pt3 col);
	void InitGUI();
	void DisposeGUI();


	// メソッド //
	int init();
	int update();
	int dispose();
	void MakeCommonVBO(mesh3dGL*);
	// 描画
	void simulateS3GL();
	void SimulateH3GL();
	int DrawEachObjsS3(int);
	int DrawEachObjsS3_LQY(int);
	int DrawEachObjsH3(int);
	int DrawObjectH3(object3d*, int, int, uint32_t* = nullptr);
	double GetAsp();
	double clcRangeY(double rangeX);
	void ClcCoordinate();
	
	void MakeCharVBO();
	void DisposeCharVBO();
	void DrawCoordinateS3();	// 座標描画
	void DrawCoordinateH3();	// 座標描画 h3
	void DrawDistancesS3();	// 距離描画
	void DrawDistancesH3();	// 距離描画
	void DrawChar(uint8_t dChar, double x, double y, double w, double h);
	void DrawChars(GuiString);
	int DrawMapS3();
	int DrawMapH3();
	void ClickProc();
	glm::mat4 GetPerspective(double, double);
	glm::mat4 GetPerspective(double, double, double, double);

	// OpenGL
	int GL_Init();
	int GL_End();
	virtual int GL_CreateContextPls() = 0;
	virtual int GL_DeleteContextPls() = 0;
	virtual int GL_SwapBuffer() = 0;
	void GL_InitScene();
	void GL_DisposeScene();
	uint32_t menuFBO;
	uint32_t menuTex;
	void CreateBuffersForVRMenu();
	void DisposeBuffersForVRMenu();
	void DrawGUIForVR();
	virtual void MakeTracingLinesS3();
	virtual void MakeTracingLinesH3();
	virtual bool LoadLazyObject(int idx);

	uint32_t LoadShaders(const char* vPath, const char* fPath);
	uint32_t LoadShaders2(const char* vPath, const char* gPath, const char* fPath, char mode);

	virtual mesh3d* GetMesh(int idx);
	virtual int allocMesh();

	// マップ
	void end4D();


	//private:
	uint32_t* buffers;
	uint32_t* texNames;
	std::vector<uint32_t> shader;
	int outMax;
	//uint32_t* h3trackBuf;

	// コンストラクタ, デストラクタ
	engine3dGL();
	~engine3dGL();
};

