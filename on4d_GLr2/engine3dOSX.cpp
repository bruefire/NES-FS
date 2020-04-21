
#include <glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <math.h>
#include <vector>
#include "engine3dOSX.h"
#include "S3ErrorManager.h"
using namespace std;


GlRect::GlRect(){};
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

GuiString::GuiString(string content, Rect drawArea)
{
	this->content = content;
	this->drawArea = drawArea;
}


// 基本 //
engine3dOSX::engine3dOSX()
    : disposeFlg(false)
	, shiftKeyFlg(false)
	, ctrlKeyFlg(false)
	, msMidBtnFlg(false)
	, MOUSE_FIX(180.0)
	, guiChrQty(92)
{
    clsType = CLS_TYPE::GRAPHIC;
    WIDTH = 640;
    HEIGHT = 480;
	cm_rot[0] = cm_rot[1] = pt2(0,0);
    
    CR_RANGE_Y = clcRangeY(CR_RANGE_X);
}

int engine3dOSX::init()
{
    try
    {
        // 基底init
        engine3dGL::init();

		// ASCII VBO
		glGenBuffers(guiChrQty, chrBuf);
		glGenTextures(1, &chrTex);
		makeCharVBO();
		
		// Gui Container作成
		menu.drawArea = GlRect(pt2(0.2, 0.9), 0.01, 0.01, GlRect::STD::HORIZONAL);
		menu.padding = GlRect(0.01, GlRect::STD::HORIZONAL);
		menu.bkColor = pt3(0.5, 0.5, 1.0);
		menu.alpha = 0.6;
		displayed = false;
		
		
		
		this->menu.push_back(menu);
		
    }
    catch(S3ErrorManager ex)
    {
        //MessageBox(this->preWnd, ex.errMsg.c_str(), "ÉGÉâÅ[åüèo", MB_OK);
        cout << ex.errMsg << endl;
        dispose();
        return 0;
    }
    
    return 1;
}

int engine3dOSX::update()
{
    // ウィンドウが閉じるまでループ
    while (!glfwWindowShouldClose(window))
    {
		// カーソル位置取得
		glfwGetCursorPos(window, &(cm_rot[0].x), &(cm_rot+[0].y));
        
        // 基底update
        engine3dGL::update();
        
        // マウスやキーボードなどのイベント取得
        glfwPollEvents();
        
    }
    
    return 1;
}

int engine3dOSX::dispose()
{
    if(disposeFlg)
        return 0;

	// ASCII VBO 解放
	glDeleteBuffers(guiChrQty, chrBuf);
	glDeleteTextures(1, &chrTex);
    
    // 基底dispose
    engine3dGL::dispose();
    
    disposeFlg = true;
    
    return 1;
}


// OpenGL //

int engine3dOSX::GL_CreateContextPls()
{
    
    // GLFWの初期化
    if (!glfwInit())
    {
        throw S3ErrorManager("GLFW初期化の失敗");
    }
    
    // 内部プロファイルとやらを変更
    glfwWindowHint(GLFW_RED_BITS, 8);
    glfwWindowHint(GLFW_GREEN_BITS, 8);
    glfwWindowHint(GLFW_BLUE_BITS, 8);
    glfwWindowHint(GLFW_ALPHA_BITS, 8);
    glfwWindowHint(GLFW_DEPTH_BITS, 24);
    glfwWindowHint(GLFW_STENCIL_BITS, 8);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
    //glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
    
    // GLFWのウィンドウ生成
    window = glfwCreateWindow(WIDTH, HEIGHT, "Hello GLFW!!", nullptr, nullptr);
    if (!window)
    {
        glfwTerminate();
        throw S3ErrorManager("GLFWウィンドウ作成失敗");
    }
    glfwMakeContextCurrent(window);
	
	//== コールバック関数設定
	// マウス
	glfwSetMouseButtonCallback(window, glfwMouseBtnMethod);
	glfwSetCursorPosCallback(window, glfwCursorPosMethod);
	glfwSetScrollCallback(window, glfwScrollMethod);
	
	// キー入力
	glfwSetKeyCallback(window, glfwKeyMethod);
	
    
    return 1;
};


int engine3dOSX::GL_DeleteContextPls()
{
    // glfw後処理
    glfwTerminate();
    
    return 1;
}


int engine3dOSX::GL_SwapBuffer()
{
    // バッファの入れ替え
    glfwSwapBuffers(window);
    
    
    return 1;
}

int engine3dGL::makeCharVBO()
{
	// まずバインド
	glBindTexture(GL_TEXTURE_2D, chrTex);


	// テクスチャデータ転送
	string texName = "Font.png";
	int ftxWidth;
	int ftxHeight;

	stbi_uc* ftx = stbi_load(&texName[0], &ftxWidth, &ftxHeight, NULL, 4);
	if (ftx == NULL) {
		cout << "テクスチャ読み込み失敗" << endl;
	}

	// テクスチャ転送
	glTexImage2D(
		GL_TEXTURE_2D, 0, GL_RGBA2, ftxWidth, ftxHeight,
		0, GL_BGRA, GL_UNSIGNED_BYTE, ftx
	);



	for (int i=0; i<guiChrQty; i++)
	{
		// !
		pt2 vtx[6];
		pt2 txs[6];


		// ポリゴン頂点設定
		vtx[0].x = -1.0;
		vtx[0].y = -1.0;
		vtx[1].x = -1.0;
		vtx[1].y = 1.0;
		vtx[2].x = 1.0;
		vtx[2].y = 1.0;
		vtx[3] = vtx[2];
		vtx[4] = vtx[1];
		vtx[5].x = 1.0;
		vtx[5].y = -1.0;


		// テクスチャ頂点設定
		txs[0].x = (float)(((i - 13) % 20) * 30);
		txs[0].y = (float)(((i - 13) / 20) * 30);

		txs[1].x = txs[0].x;
		txs[1].y = txs[0].y + 30;

		txs[2].x = txs[0].x + 30;
		txs[2].y = txs[0].y + 30;

		txs[3] = txs[2];
		txs[4] = txs[1];

		txs[5].x = txs[0].x + 30;
		txs[5].y = txs[0].y;


		for (int j = 0; j < 6; j++)
		{
			txs[j].x = txs[j].x / ftxWidth;
			txs[j].y = txs[j].y / ftxHeight;
		}


		// 合成
		float vtxs[24] =
		{
			vtx[0].x, vtx[0].y, txs[0].x, txs[0].y,
			vtx[1].x, vtx[1].y, txs[1].x, txs[1].y,
			vtx[2].x, vtx[2].y, txs[2].x, txs[2].y,
			vtx[3].x, vtx[3].y, txs[3].x, txs[3].y,
			vtx[4].x, vtx[4].y, txs[4].x, txs[4].y,
			vtx[5].x, vtx[5].y, txs[5].x, txs[5].y
		};


		// 頂点転送
		glBindBuffer(GL_ARRAY_BUFFER, chrBuf[i]);
		glBufferData(	//---- pts転送
			GL_ARRAY_BUFFER,
			20 * sizeof(float),
			vtxs,
			GL_STATIC_DRAW
		);
	}


	// 解放
	stbi_image_free(ftx);


	return 1;
}

//== cntrol with GLFW
// マウス入力
void engine3dOSX::glfwMouseBtnMethod(GLFWwindow* wnd, int button, int action, int mods)
{
	// key down
	if(action==GLFW_PRESS)
	{
		switch(button)
		{
		case GLFW_MOUSE_BUTTON_MIDDLE:
			midBtnFlg = true;
			break;
		
		case GLFW_MOUSE_BUTTON_RIGHT:
			break;
		}
	}
	// key up
	else if(action==GLFW_RELEASE)
	{
		switch(button)
		{
		case GLFW_MOUSE_BUTTON_MIDDLE:
			midBtnFlg = false;
			break;
		
		case GLFW_MOUSE_BUTTON_RIGHT:
			break;
		}
	}
}
// カーソル入力
void engine3dOSX::glfwCursorPosMethod(GLFWwindow* wnd, double xPos, double yPos)
{
	if(midBtnFlg)
	{
		inPutMouseMv
		(
			(cm_rot[1].x - cm_rot[0].x) / MOUSE_FIX,
			(cm_rot[1].y - cm_rot[0].y) / MOUSE_FIX,
			shiftKeyFlg
		);
	}
}

void engine3dOSX::glfwScrollMethod(GLFWwindow* wnd, double x, double y)
{
	inPutWheel(y, shiftKeyFlg);
}
void engine3dOSX::glfwKeyMethod(GLFWwindow* window, int key, int scanMode, int action, int mods)
{
	// key down
	if(action==GLFW_PRESS)
	{
		switch(key)
		{
		case GLFW_KEY_LEFT_SHIFT:
		case GLFW_KEY_RIGHT_SHIFT:
			shiftKeyFlg = true;
			break;
			
		case GLFW_KEY_LEFT_CONTROL:
		case GLFW_KEY_RIGHT_CONTROL:
			ctrlKeyFlg = true;
			break;
			
		case GLFW_KEY_1: 
			newEngine.inPutKey(newEngine.IK::No_1, NULL); 
			break;
		case GLFW_KEY_2: 
			newEngine.inPutKey(newEngine.IK::No_2, NULL); 
			break;
		case GLFW_KEY_3:
			newEngine.inPutKey(newEngine.IK::No_3, NULL); 
			break;
		case GLFW_KEY_SPACE:
			newEngine.inPutKey(newEngine.IK::SPACE, NULL); 
			break;
			
		}
	}
	// key up
	else if(action==GLFW_RELEASE)
	{
		case GLFW_KEY_LEFT_SHIFT:
		case GLFW_KEY_RIGHT_SHIFT:
			shiftKeyFlg = false;
			break;
			
		case GLFW_KEY_LEFT_CONTROL:
		case GLFW_KEY_RIGHT_CONTROL:
			ctrlKeyFlg = false;
			break;
	}
}


// その他

void engine3dOSX::displayGui()
{
}

double engine3dOSX::clcRangeY(double rangeX)
{
	return atan( tan((LDBL)( rangeX )/2*PIE/180)*HEIGHT/WIDTH ) *2/PIE*180;	//カメラ設定
}



