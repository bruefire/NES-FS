#pragma once

#include <stdint.h>
#include <string>
#include <glew.h>
#include <GL/gl.h>
#include <vector>
#include "constants.h"
#include "guiItem.h"
#include "engine3d.h"



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
	void simulateS3GL();
	void SimulateH3GL();
	int DrawEachObjsS3();
	int DrawEachObjsS3_LQY();
	int DrawEachObjsH3(int);
	double GetAsp();
	
	void MakeCharVBO();
	void DisposeCharVBO();
	void DrawCoordinateS3();	// ���W�`��
	void DrawCoordinateH3();	// ���W�`�� h3
	void DrawDistances();	// �����`��
	void DrawChar(uint8_t dChar, double x, double y, double w, double h);
	void DrawChars(GuiString);
	int DrawMapS3();
	int DrawMapH3();
	void ClickProc();

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
	int shader[6];
	int outMax;

	// �R���X�g���N�^, �f�X�g���N�^
	engine3dGL();
	~engine3dGL();
};

