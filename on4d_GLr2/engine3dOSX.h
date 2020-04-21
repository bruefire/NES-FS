#pragma once
#include <GLFW/glfw3.h>
#include <vector>
#include "engine3dGL.h"


class GlRect : public Rect
{
public:
	enum STD
	{
		NORMAL,
		HORIZONAL,
		VERTICAL
	};
	STD moode;
	
	GlRect();
	GlRect(pt2, double, double, STD);
	GlRect(double, double, double, double, STD);
	GlRect(double, STD);
};


class GuiItem
{
public:
	GuiItem* owner;
	GlRect drawArea;
	GlRect padding;
	pt3 bkColor;
	double alpha;

	bool displayed;
	bool checked;

};

class GuiContainer : public GuiItem
{
public:
	vector<GuiParts*> childs;
	int selectedIdx;
}
	

class GuiString : public GuiItem
{
public:
	std::string content;
	double fontSz;
	
	// コンストラクタ
	GuiString();
	GuiString(std::string content, Rect rect);
	
}


class engine3dOSX : public engine3dGL
{
public:
    bool disposeFlg;
    GLFWwindow* window;
	
	//== input 操作 (後々別クラスに移動？)
	pt2 cm_rot[2];
	bool shiftKeyFlg;
	bool ctrlKeyFlg;
	// マウス
	bool msMidBtnFlg;
	double MOUSE_FIX;
    
    // コンストラクタ
    engine3dOSX();
    
    // 基本3method
    int init();
    int update();
    int dispose();
    
    // openGL
    int GL_CreateContextPls();
    int GL_DeleteContextPls();
    int GL_SwapBuffer();
	
	void displayGui();
	
	//== cntrol with GLFW
	// mouse
	void glfwMouseBtnMethod(GLFWwindow*, int, int, int);
	void glfwCursorPosMethod(GLFWwindow*, double, double);
	void glfwScrollMethod(GLFWwindow*, double, double);
	// key
	void glfwKeyMethod(GLFWwindow*, int, int, int, int);

    
    double clcRangeY(double rangeX);
    

	// 文字列用vbo, tex
	int guiChrQty;
	uint32_t* chrBuf;
	uint32_t chrTex;
	int makeCharVBO();
	GuiContainer menu;
};
