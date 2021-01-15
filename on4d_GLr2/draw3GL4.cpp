#include <stdint.h>
#include <Complex>
#include <algorithm>
#include <iostream>
#include "constants.h"
#include "engine3dGL.h"
#include "functions.h"

#include <glew.h>
#include <GL/gl.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
using namespace std;




/// <summary>
/// S3世界描画
/// </summary>
void engine3dGL::simulateS3GL()
{
	//=====カメラの範囲
	double cRangeX = tan((LDBL)CR_RANGE_X/2 *PIE/180);	
	double cRangeY = tan((LDBL)CR_RANGE_Y/2 *PIE/180);

	
	//-- 軌跡データ転送
	glBindBuffer(GL_ARRAY_BUFFER, buffers[ markMesh.texNo ]);
	glBufferData(	//---- pts転送
		GL_ARRAY_BUFFER,
		markMesh.lLen*16 * sizeof(float),
		markMesh.pts2,
		GL_STATIC_DRAW
	);

	objs[0].scale = 0.5*radius;	//-- 調整
	if(sun.used) glClearColor(0.2, 0.8, 1, 0.0);
	else if(bgCol) glClearColor(1, 1, 1, 0.0);
	else glClearColor(0, 0, 0, 0.0);
	glClear(GL_COLOR_BUFFER_BIT); 
	glClear(GL_DEPTH_BUFFER_BIT); 
	//glFlush(); 
	//------------
	glUseProgram(shader[3]);
	GLint xID = glGetUniformLocation(shader[3], "WH_CR");
	glUniform4f(xID, (float)WIDTH, (float)HEIGHT, cRangeX, cRangeY);

	//--
	glUseProgram(shader[0]);
	xID = glGetUniformLocation(shader[0], "WH_CR");
	glUniform4f(xID, (float)WIDTH, (float)HEIGHT, cRangeX, cRangeY);
	glUseProgram(shader[4]);
	xID = glGetUniformLocation(shader[0], "WH_CR");
	glUniform4f(xID, (float)WIDTH, (float)HEIGHT, cRangeX, cRangeY);

	//------------	

	/// Projection matrix : 45ｰ Field of View, 4:3 ratio, display range : 0.1 unit <-> 100 units
	glm::mat4 Projection = glm::perspective((float)CR_RANGE_Y, (float)(cRangeX / cRangeY), 0.00001f, 2.0f);
	glm::mat4 View       = glm::lookAt(
								glm::vec3(0,0,0), // Camera location in World Space
								glm::vec3(0,1,0), // direction of view
								glm::vec3(0,0,1)  // Head of POV
						   );
	glm::mat4 Model      = glm::mat4(1.0f);
	glm::mat4 MVP        = Projection * View * Model;
	/// Get a handle for our "MVP" uniform
	glUseProgram(shader[0]);
	GLuint MatrixID = glGetUniformLocation(shader[0], "MVP");
	glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP[0][0]);
	glUseProgram(shader[4]);
	 MatrixID = glGetUniformLocation(shader[0], "MVP");
	glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP[0][0]);
	glUseProgram(shader[3]);
	MatrixID = glGetUniformLocation(shader[3], "MVP");
	glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP[0][0]);

	//===========================



	pt4 sunE = object3d::tudeToEuc(sun.locr);
	glUseProgram(shader[0]);
	xID = glGetUniformLocation(shader[0], "sLoc");
	glUniform4f(xID, (float)sunE.x, sunE.y, sunE.z, sunE.w);
	xID = glGetUniformLocation( shader[0], "sunJD" );
	glUniform1i(xID, sun.used);
	

	switch (qyMode)
	{
	case QY_MODE::HIGH:
		DrawEachObjsS3(0);
		break;

	case QY_MODE::LOW:
		DrawEachObjsS3_LQY(0);
		break;
	}

	// クリック処理
	ClickProc();


}

/// <summary>
/// H3世界描画
/// </summary>
void engine3dGL::SimulateH3GL()
{
	//=====カメラの範囲
	double cRangeX = tan((LDBL)CR_RANGE_X / 2 * PIE / 180);
	double cRangeY = tan((LDBL)CR_RANGE_Y / 2 * PIE / 180);

	// todo★//-- 軌跡データ転送
	//glBindBuffer(GL_ARRAY_BUFFER, buffers[markMesh.texNo]);
	//glBufferData(	//---- pts転送
	//	GL_ARRAY_BUFFER,
	//	markMesh.lLen * 16 * sizeof(float),
	//	markMesh.pts2,
	//	GL_STATIC_DRAW
	//	);

	// 描画内容の初期化
	objs[0].scale = 0.5 * radius;	//-- 調整
	/*if (sun.used) glClearColor(0.2, 0.8, 1, 0.0);
	else if (bgCol) glClearColor(1, 1, 1, 0.0);*/
	if (bgCol) 
		glClearColor(1, 1, 1, 0.0);
	else 
		glClearColor(0, 0, 0, 0.0);
	glClear(GL_COLOR_BUFFER_BIT);
	glClear(GL_DEPTH_BUFFER_BIT);
	glClear(GL_STENCIL_BUFFER_BIT);
	glClearStencil(0);

	// ユニフォーム変数設定
	int sdrs[2] = {3, 5};
	for (int i = 0; i < 2; i++)
	{
		glUseProgram(shader[sdrs[i]]);
		GLint xID = glGetUniformLocation(shader[sdrs[i]], "WH_CR");
		glUniform4f(xID, (float)WIDTH, (float)HEIGHT, cRangeX, cRangeY);

		// MVPマトリックス設定
		glm::mat4 Projection = glm::perspective((float)CR_RANGE_Y, (float)(cRangeX / cRangeY), 0.00001f, 2.0f);
		glm::mat4 View = glm::lookAt(
			glm::vec3(0, 0, 0), // Camera location in World Space
			glm::vec3(0, 1, 0), // direction of view
			glm::vec3(0, 0, 1)  // Head of POV
			);
		glm::mat4 Model = glm::mat4(1.0f);
		glm::mat4 MVP = Projection * View * Model;

		GLuint MatrixID = glGetUniformLocation(shader[sdrs[i]], "MVP");
		glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP[0][0]);
	}

	// 各オブジェクトの描画
	DrawEachObjsH3(0);

	// クリック処理
	ClickProc();

}


int engine3dGL::DrawEachObjsS3(int loop)
{
	//==============オブジェクトごとのGL描画==============//
	for (int h = -2; h < objCnt; h++)
	{

		object3d* curObj;
		if (h == -2) curObj = &markObj; else if (h == -1) curObj = &sun; else curObj = objs + h;

		if (!curObj->used) continue;
		if (!VIEW_PLR && BWH_QTY <= h && h < BWH_QTY + PLR_QTY) continue;
		if (h == PLR_No) continue;
		///...++++++++++++ 一括GL描画 +++++++++++++...///
		char SDR = (0 < curObj->mesh->faceLen) ? 0 : (curObj->draw == 0) ? 3 : 4;


		///===================///
		glUseProgram(shader[SDR]);

		GLint xID = glGetUniformLocation(shader[SDR], "scl_rad");
		glUniform2f(xID, (float)curObj->scale, (float)radius);
		xID = glGetUniformLocation(shader[SDR], "objRot");
		glUniform3f(xID, curObj->rot.x, curObj->rot.y, curObj->rot.z);
		xID = glGetUniformLocation(shader[SDR], "objStd");
		glUniform3f(xID, curObj->objStd.x, curObj->objStd.y, curObj->objStd.z);
		xID = glGetUniformLocation(shader[SDR], "locR");
		glUniform3f(xID, curObj->locr.x, curObj->locr.y, curObj->locr.z);
		xID = glGetUniformLocation(shader[SDR], "decMode");
		if (h != -1)	glUniform1i(xID, decMode);
		else		glUniform1i(xID, 3);
		xID = glGetUniformLocation(shader[SDR], "bgMode");
		glUniform1i(xID, bgCol);
		xID = glGetUniformLocation(shader[SDR], "texJD");
		glUniform1i(xID, curObj->mesh->texJD);


		glBindBuffer(GL_ARRAY_BUFFER, buffers[curObj->mesh->texNo]);
		glBindTexture(GL_TEXTURE_2D, texNames[curObj->mesh->texNo]);



		glEnable(GL_TEXTURE_2D);

		if (loop == 0)
			glStencilFunc(GL_ALWAYS, (h + 1) % 256, -1);
		else
			glStencilFunc(GL_ALWAYS, (h + 1) % (256 * 256) / 256, -1);

		// 最初の属性バッファ : 頂点
		glEnableVertexAttribArray(0);
		glEnableVertexAttribArray(1);
		glEnableVertexAttribArray(2);

		glVertexAttribPointer(  //---- 属性1    
			0, 3, GL_FLOAT, GL_FALSE,
			8 * sizeof(GLfloat), (void*)0
		);
		glVertexAttribPointer(    //---- 属性2
			1, 3, GL_FLOAT, GL_FALSE,
			8 * sizeof(GLfloat), (void*)(3 * sizeof(GLfloat))
		);
		glVertexAttribPointer(    //---- 属性3 テクスチャ
			2, 2, GL_FLOAT, GL_FALSE,
			8 * sizeof(GLfloat), (void*)(6 * sizeof(GLfloat))
		);


		if (curObj->draw == 2)
			glDrawArrays(GL_TRIANGLES, 0, curObj->mesh->faceLen * 3);	// 三角形を描く
		else if (curObj->draw == 1 || curObj->draw == 0) {
			glPointSize(2.0);
			glDrawArrays(GL_LINES, 0, curObj->mesh->lLen * 2);	// 線を描く
			glPointSize(1.0);
		}
		else {
			glPointSize(2.0);
			glDrawArrays(GL_LINE_LOOP, 0, 24);	// 点を描く
			glPointSize(1.0);
		}

		glDisableVertexAttribArray(0);
		glDisableVertexAttribArray(1);
		glDisableVertexAttribArray(2);

	}

	return 1;
}

int engine3dGL::DrawEachObjsS3_LQY(int loop)
{
	//==============オブジェクトごとのGL描画==============//
	for (int h = -2; h < objCnt; h++)
	{

		object3d* curObj;
		if (h == -2) curObj = &markObj; else if (h == -1) curObj = &sun; else curObj = objs + h;

		if (!curObj->used) continue;
		if (!VIEW_PLR && BWH_QTY <= h && h < BWH_QTY + PLR_QTY) continue;
		if (h == PLR_No) continue;
		///...++++++++++++ 一括GL描画 +++++++++++++...///
		char SDR = (0 < curObj->mesh->faceLen) ? 0 : 3;


		///===================///
		glUseProgram(shader[SDR]);

		GLint xID = glGetUniformLocation(shader[SDR], "scl_rad");
		glUniform2f(xID, (float)curObj->scale, (float)radius);
		xID = glGetUniformLocation(shader[SDR], "objRot");
		glUniform3f(xID, curObj->rot.x, curObj->rot.y, curObj->rot.z);
		xID = glGetUniformLocation(shader[SDR], "objStd");
		glUniform3f(xID, curObj->objStd.x, curObj->objStd.y, curObj->objStd.z);
		xID = glGetUniformLocation(shader[SDR], "locR");
		glUniform3f(xID, curObj->locr.x, curObj->locr.y, curObj->locr.z);
		xID = glGetUniformLocation(shader[SDR], "decMode");
		if (h != -1)	glUniform1i(xID, decMode);
		else		glUniform1i(xID, 3);
		xID = glGetUniformLocation(shader[SDR], "bgMode");
		glUniform1i(xID, bgCol);
		xID = glGetUniformLocation(shader[SDR], "texJD");
		glUniform1i(xID, curObj->mesh->texJD);
		xID = glGetUniformLocation(shader[SDR], "revMd");
		glUniform1f(xID, 1);


		glBindBuffer(GL_ARRAY_BUFFER, buffers[curObj->mesh->texNo]);
		glBindTexture(GL_TEXTURE_2D, texNames[curObj->mesh->texNo]);


		if (loop == 0)
			glStencilFunc(GL_ALWAYS, (h + 1) % 256, -1);
		else
			glStencilFunc(GL_ALWAYS, (h + 1) % (256 * 256) / 256, -1);

		glEnable(GL_TEXTURE_2D);



		GLuint vPosition = glGetAttribLocation(shader[SDR], "vPosition");
		GLuint vColor = glGetAttribLocation(shader[SDR], "vColor");
		GLuint vPos1 = glGetAttribLocation(shader[SDR], "vPos1");
		GLuint vPos2 = glGetAttribLocation(shader[SDR], "vPos2");
		GLuint vPos3 = glGetAttribLocation(shader[SDR], "vPos3");
		GLuint tPos1 = glGetAttribLocation(shader[SDR], "tPos1");
		GLuint tPos2 = glGetAttribLocation(shader[SDR], "tPos2");
		GLuint tPos3 = glGetAttribLocation(shader[SDR], "tPos3");


		// 最初の属性バッファ : 頂点
		if (curObj->mesh->faceLen)
		{
			int fltLen = 21;
			glEnableVertexAttribArray(vPosition);
			glEnableVertexAttribArray(vColor);
			glEnableVertexAttribArray(vPos1);
			glEnableVertexAttribArray(vPos2);
			glEnableVertexAttribArray(vPos3);
			glEnableVertexAttribArray(tPos1);
			glEnableVertexAttribArray(tPos2);
			glEnableVertexAttribArray(tPos3);

			glVertexAttribPointer(  //---- Loc
				vPosition, 3, GL_FLOAT, GL_FALSE,
				fltLen * sizeof(GLfloat), (void*)0
				);
			glVertexAttribPointer(    //---- 色
				vColor, 3, GL_FLOAT, GL_FALSE,
				fltLen * sizeof(GLfloat), (void*)(3 * sizeof(GLfloat))
				);

			//---- 位置算出用Loc1, 2, 3
			glVertexAttribPointer(
				vPos1, 3, GL_FLOAT, GL_FALSE,
				fltLen * sizeof(GLfloat), (void*)(6 * sizeof(GLfloat))
				);
			glVertexAttribPointer(
				vPos2, 3, GL_FLOAT, GL_FALSE,
				fltLen * sizeof(GLfloat), (void*)(9 * sizeof(GLfloat))
				);
			glVertexAttribPointer(
				vPos3, 3, GL_FLOAT, GL_FALSE,
				fltLen * sizeof(GLfloat), (void*)(12 * sizeof(GLfloat))
				);

			//---- 位置算出用tex1, 2, 3
			glVertexAttribPointer(
				tPos1, 2, GL_FLOAT, GL_FALSE,
				fltLen * sizeof(GLfloat), (void*)(15 * sizeof(GLfloat))
				);
			glVertexAttribPointer(
				tPos2, 2, GL_FLOAT, GL_FALSE,
				fltLen * sizeof(GLfloat), (void*)(17 * sizeof(GLfloat))
				);
			glVertexAttribPointer(
				tPos3, 2, GL_FLOAT, GL_FALSE,
				fltLen * sizeof(GLfloat), (void*)(19 * sizeof(GLfloat))
				);


			if (curObj->draw == 2) {
				glDrawArrays(GL_TRIANGLES, 0, curObj->mesh->faceLen * 3);	// 三角形を描く

				xID = glGetUniformLocation(shader[SDR], "revMd");
				glUniform1f(xID, -1);

				glDrawArrays(GL_TRIANGLES, 0, curObj->mesh->faceLen * 3);	// 三角形 半周以上
			}
			else if (curObj->draw == 1 || curObj->draw == 0) {
				glPointSize(2.0);
				glDrawArrays(GL_LINES, 0, curObj->mesh->lLen * 2);	// 線を描く
				glPointSize(1.0);
			}
			else {
				glPointSize(2.0);
				glDrawArrays(GL_LINE_LOOP, 0, 24);	// 点を描く
				glPointSize(1.0);
			}

			glDisableVertexAttribArray(vPosition);
			glDisableVertexAttribArray(vColor);
			glDisableVertexAttribArray(vPos1);
			glDisableVertexAttribArray(vPos2);
			glDisableVertexAttribArray(vPos3);
		}
		else
		{
			int fltLen = 12;
			glEnableVertexAttribArray(vPosition);
			glEnableVertexAttribArray(vColor);
			glEnableVertexAttribArray(vPos1);
			glEnableVertexAttribArray(vPos2);

			glVertexAttribPointer(  //---- Loc
				vPosition, 3, GL_FLOAT, GL_FALSE,
				fltLen * sizeof(GLfloat), (void*)0
				);
			glVertexAttribPointer(    //---- 色
				vColor, 3, GL_FLOAT, GL_FALSE,
				fltLen * sizeof(GLfloat), (void*)(3 * sizeof(GLfloat))
				);

			//---- 位置算出用Loc1, 2
			glVertexAttribPointer(
				vPos1, 3, GL_FLOAT, GL_FALSE,
				fltLen * sizeof(GLfloat), (void*)(6 * sizeof(GLfloat))
				);
			glVertexAttribPointer(
				vPos2, 3, GL_FLOAT, GL_FALSE,
				fltLen * sizeof(GLfloat), (void*)(9 * sizeof(GLfloat))
				);


			if (curObj->draw == 0) {
				glPointSize(2.0);
				glDrawArrays(GL_POINTS, 0, curObj->mesh->lLen * 2);	// 点を描く
				glPointSize(1.0);
			}
			else if(curObj->draw == 1){
				glLineWidth(2.0);
				glDrawArrays(GL_LINES, 0, curObj->mesh->lLen * 2);	// 線を描く
				glLineWidth(1.0);
			}

			glDisableVertexAttribArray(vPosition);
			glDisableVertexAttribArray(vColor);
			glDisableVertexAttribArray(vPos1);
			glDisableVertexAttribArray(vPos2);
		}

	}

	return 1;
}

int engine3dGL::DrawEachObjsH3(int loop)
{
	//==============オブジェクトごとのGL描画==============//
	for (int h = 0; h < objCnt; h++)
	{
		object3d* curObj = objs + h;

		if (!curObj->used) continue;
		if (!VIEW_PLR && BWH_QTY <= h && h < BWH_QTY + PLR_QTY) continue;
		if (h == PLR_No) continue;
		///...++++++++++++ 一括GL描画 +++++++++++++...///
		char SDR = (curObj->mesh->faceLen) ? 5 : 3;


		/////===================///
		glUseProgram(shader[SDR]);

		// ユニフォーム変数設定
		GLint xID = glGetUniformLocation(shader[SDR], "scl_rad");
		glUniform2f(xID, (float)curObj->scale, (float)radius);
		xID = glGetUniformLocation(shader[SDR], "objRot");
		glUniform3f(xID, curObj->rot.x, curObj->rot.y, curObj->rot.z);
		xID = glGetUniformLocation(shader[SDR], "objStd");
		glUniform3f(xID, curObj->objStd.x, curObj->objStd.y, curObj->objStd.z);
		xID = glGetUniformLocation(shader[SDR], "locR");
		glUniform3f(xID, curObj->locr.x, curObj->locr.y, curObj->locr.z);
		xID = glGetUniformLocation(shader[SDR], "H3_MAX_RADIUS");
		glUniform1f(xID, H3_MAX_RADIUS);
		//else		glUniform1i(xID, 3);
		xID = glGetUniformLocation(shader[SDR], "decMode");
		glUniform1i(xID, decMode);
		xID = glGetUniformLocation(shader[SDR], "texJD");
		glUniform1i(xID, curObj->mesh->texJD);


		glBindBuffer(GL_ARRAY_BUFFER, buffers[curObj->mesh->texNo]);
		glBindTexture(GL_TEXTURE_2D, texNames[curObj->mesh->texNo]);


		if(loop == 0)
			glStencilFunc(GL_ALWAYS, (h + 1) % 256, -1);
		else
			glStencilFunc(GL_ALWAYS, (h + 1) % (256*256) / 256, -1);

		glEnable(GL_TEXTURE_2D);


		// 最初の属性バッファ : 頂点
		if (curObj->mesh->faceLen)
		{
			glEnableVertexAttribArray(0);
			glEnableVertexAttribArray(1);
			glEnableVertexAttribArray(2);
			glEnableVertexAttribArray(3);
			glEnableVertexAttribArray(4);
			glEnableVertexAttribArray(5);
			glEnableVertexAttribArray(6);
			glEnableVertexAttribArray(7);

			int fltLen = 21;
			glVertexAttribPointer(  //---- Loc
				0, 3, GL_FLOAT, GL_FALSE,
				fltLen * sizeof(GLfloat), (void*)0
				);
			glVertexAttribPointer(    //---- 色
				1, 3, GL_FLOAT, GL_FALSE,
				fltLen * sizeof(GLfloat), (void*)(3 * sizeof(GLfloat))
				);

			//---- 位置算出用Loc1, 2, 3
			glVertexAttribPointer(
				2, 3, GL_FLOAT, GL_FALSE,
				fltLen * sizeof(GLfloat), (void*)(6 * sizeof(GLfloat))
				);
			glVertexAttribPointer(
				3, 3, GL_FLOAT, GL_FALSE,
				fltLen * sizeof(GLfloat), (void*)(9 * sizeof(GLfloat))
				);
			glVertexAttribPointer(
				4, 3, GL_FLOAT, GL_FALSE,
				fltLen * sizeof(GLfloat), (void*)(12 * sizeof(GLfloat))
				);

			//---- 位置算出用tex1, 2, 3
			glVertexAttribPointer(
				5, 2, GL_FLOAT, GL_FALSE,
				fltLen * sizeof(GLfloat), (void*)(15 * sizeof(GLfloat))
				);
			glVertexAttribPointer(
				6, 2, GL_FLOAT, GL_FALSE,
				fltLen * sizeof(GLfloat), (void*)(17 * sizeof(GLfloat))
				);
			glVertexAttribPointer(
				7, 2, GL_FLOAT, GL_FALSE,
				fltLen * sizeof(GLfloat), (void*)(19 * sizeof(GLfloat))
				);


			if (curObj->draw == 2)
				glDrawArrays(GL_TRIANGLES, 0, curObj->mesh->faceLen * 3);	// 三角形を描く
			else if (curObj->draw == 1 || curObj->draw == 0) {
				glLineWidth(2.0);
				glDrawArrays(GL_LINES, 0, curObj->mesh->lLen * 2);	// 線を描く
				glLineWidth(1.0);
			}
			else {
				glPointSize(2.0);
				glDrawArrays(GL_LINE_LOOP, 0, 24);	// 点を描く
				glPointSize(1.0);
			}

			glDisableVertexAttribArray(0);
			glDisableVertexAttribArray(1);
			glDisableVertexAttribArray(2);
			glDisableVertexAttribArray(3);
			glDisableVertexAttribArray(4);
			glDisableVertexAttribArray(5);
			glDisableVertexAttribArray(6);
			glDisableVertexAttribArray(7);
		}
		// draw lines
		else
		{
			glEnableVertexAttribArray(0);
			glEnableVertexAttribArray(1);
			glEnableVertexAttribArray(2);
			glEnableVertexAttribArray(3);

			int fltLen = 12;
			glVertexAttribPointer(  //---- Loc
				0, 3, GL_FLOAT, GL_FALSE,
				fltLen * sizeof(GLfloat), (void*)0
				);
			glVertexAttribPointer(    //---- 色
				1, 3, GL_FLOAT, GL_FALSE,
				fltLen * sizeof(GLfloat), (void*)(3 * sizeof(GLfloat))
				);

			//---- 位置算出用Loc1, 2, 3
			glVertexAttribPointer(
				2, 3, GL_FLOAT, GL_FALSE,
				fltLen * sizeof(GLfloat), (void*)(6 * sizeof(GLfloat))
				);
			glVertexAttribPointer(
				3, 3, GL_FLOAT, GL_FALSE,
				fltLen * sizeof(GLfloat), (void*)(9 * sizeof(GLfloat))
				);

			if (curObj->draw == 1) {
				glLineWidth(2.0);
				glDrawArrays(GL_LINES, 0, curObj->mesh->lLen * 2);	// 線を描く
				glLineWidth(1.0);
			}
			else {
				glPointSize(2.0);
				glDrawArrays(GL_POINTS, 0, curObj->mesh->lLen * 2);	// 点を描く
				glPointSize(1.0);
			}

			glDisableVertexAttribArray(0);
			glDisableVertexAttribArray(1);
			glDisableVertexAttribArray(2);
			glDisableVertexAttribArray(3);
		}

	}

	return 1;
}


int engine3dGL::DrawMapS3()
{

	int avl_ObjCnt;
	//===== field of view, draw area
	double cRangeX, cRangeY;
	short P_WIDTH, P_HEIGHT, W_REST;

	cRangeX = cRangeY = tan((LDBL)10.0 * 0.5 * PIE / 180);
	//--
	P_WIDTH = P_HEIGHT = (short)(HEIGHT * 0.3);
	W_REST = WIDTH - P_WIDTH;
	avl_ObjCnt = 0;


	//--- openGL -----
	glUseProgram(shader[2]);
	glClear(GL_DEPTH_BUFFER_BIT);

	// Projection matrix : left, right, bottom, top
	float aspf = GetAsp();
	glm::mat4 Projection = glm::ortho(-aspf, aspf, -1.0f, 1.0f, -2.0f, 2.0f);
	glm::mat4 View = glm::lookAt(
		glm::vec3(0, 0, 0), // Camera in World Space
		glm::vec3(0, 1, 0), // direction of view
		glm::vec3(0, 0, 1)  // head of POV
		);
	glm::mat4 Model = glm::mat4(1.0f);
	glm::mat4 MVP = Projection * View * Model;
	// Get a handle for our "MVP" uniform
	GLuint MatrixID = glGetUniformLocation(shader[2], "MVP");
	glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP[0][0]);
	GLuint aspID = glGetUniformLocation(shader[2], "asp");
	glUniform1f(aspID, aspf);

	//===========================

	float mapLoc[2] = { 0.0, 2.0 };
	int mapLen = 1;
	if (mapMode == MapMode::SINGLE)
		mapLen = 1;
	else if (mapMode == MapMode::DUAL)
		mapLen = 2;

	for (int g = 0; g < mapLen; g++)
	{
		glBindBuffer(GL_ARRAY_BUFFER, buffers[mapMesh[0]->texNo]);

		// 最初の属性バッファ : 頂点
		glEnableVertexAttribArray(0);
		glEnableVertexAttribArray(1);

		glVertexAttribPointer(  //---- 属性1    
			0, 3, GL_FLOAT, GL_FALSE,
			8 * sizeof(GLfloat), (void*)0
			);
		glVertexAttribPointer(    //---- 属性2
			1, 3, GL_FLOAT, GL_FALSE,
			8 * sizeof(GLfloat), (void*)(3 * sizeof(GLfloat))
			);

		GLuint scaleID = glGetUniformLocation(shader[2], "scale");
		glUniform1f(scaleID, 1.0);
		GLuint dirID = glGetUniformLocation(shader[2], "direct");
		if (mapDir == MapDirection::FRONT)
			glUniform1f(dirID, 1.0);
		else
			glUniform1f(dirID, 2.0);
		GLuint alphaID = glGetUniformLocation(shader[2], "alpha");
		glUniform1f(alphaID, 1.0);
		GLuint locID = glGetUniformLocation(shader[2], "loc");
		glUniform3f(locID, mapLoc[g], 0.0, 0.0);

		glDrawArrays(GL_TRIANGLES, 0, mapMesh[0]->faceLen * 3);

		glDisableVertexAttribArray(0);
		glDisableVertexAttribArray(1);


		// draw objects on map
		for (uint32_t h = BWH_QTY; h < objCnt; h++)
		{
			object3d* curObj = objs + h;

			if (!curObj->used) continue;
			else if (h == PLR_No) continue;
			else if (h < BWH_QTY + PLR_QTY && !VIEW_PLR) continue;
			else if (mapMode == MapMode::DUAL)
			{
				if (g == 0 && curObj->locr.z > 0.5 * PIE)
					continue;
				else if (g == 1 && curObj->locr.z <= 0.5 * PIE)
					continue;
			}


			pt4 tmpPt = object3d::tudeToEuc(curObj->locr);
			// 軸の一致
			double tmpZ = tmpPt.z;
			tmpPt.z = tmpPt.y;
			tmpPt.y = tmpZ;

			// マップの方向
			if (mapDir == MapDirection::FRONT)
			{
			}
			else if (mapDir == MapDirection::TOP)
			{
				double tmpY = tmpPt.y;
				tmpPt.y = -tmpPt.z;
				tmpPt.z = tmpY;
			}

			// farther than 2/π or not 
			if (curObj->locr.z < 0.5 * PIE)
			{
				// front or back
				if (curObj->locr.y > 0.5 * PIE)
					glBindBuffer(GL_ARRAY_BUFFER, buffers[meshs[17].texNo]);
				else
					glBindBuffer(GL_ARRAY_BUFFER, buffers[meshs[19].texNo]);
			}
			else
			{
				// front or back
				if (curObj->locr.y > 0.5 * PIE)
					glBindBuffer(GL_ARRAY_BUFFER, buffers[meshs[18].texNo]);
				else
					glBindBuffer(GL_ARRAY_BUFFER, buffers[meshs[20].texNo]);
			}

			// 最初の属性バッファ : 頂点
			glEnableVertexAttribArray(0);
			glEnableVertexAttribArray(1);

			glVertexAttribPointer(  //---- 属性1    
				0, 3, GL_FLOAT, GL_FALSE,
				8 * sizeof(GLfloat), (void*)0
				);
			glVertexAttribPointer(    //---- 属性2
				1, 3, GL_FLOAT, GL_FALSE,
				8 * sizeof(GLfloat), (void*)(3 * sizeof(GLfloat))
				);

			GLuint scaleID = glGetUniformLocation(shader[2], "scale");
			glUniform1f(scaleID, 1.0);
			GLuint dirID = glGetUniformLocation(shader[2], "direct");
			glUniform1f(dirID, 1.0);
			GLuint alphaID = glGetUniformLocation(shader[2], "alpha");
			glUniform1f(alphaID, 1);
			GLuint locID = glGetUniformLocation(shader[2], "loc");
			glUniform3f(locID, tmpPt.x + mapLoc[g], tmpPt.y, tmpPt.z);

			glDrawArrays(GL_TRIANGLES, 0, meshs[17].faceLen * 3);

			glDisableVertexAttribArray(0);
			glDisableVertexAttribArray(1);
		}


		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glDisable(GL_CULL_FACE);
		float c45d = 0.70710678118;
		float direct[5] = { 1.0, 1.0, 1.0, -1.0, -1.0 };
		float scale[5] = { 1.05, 1.0, c45d, c45d, 1.0 };
		float alpha[5] = { -1, 0.14 , 0.14 , 0.14 , 0.14 };

		for (int i = 0; i < 5; i++)
		{
			glBindBuffer(GL_ARRAY_BUFFER, buffers[mapMesh[1]->texNo]);

			// 最初の属性バッファ : 頂点
			glEnableVertexAttribArray(0);
			glEnableVertexAttribArray(1);

			glVertexAttribPointer(  //---- 属性1    
				0, 3, GL_FLOAT, GL_FALSE,
				8 * sizeof(GLfloat), (void*)0
				);
			glVertexAttribPointer(    //---- 属性2
				1, 3, GL_FLOAT, GL_FALSE,
				8 * sizeof(GLfloat), (void*)(3 * sizeof(GLfloat))
				);

			GLuint scaleID = glGetUniformLocation(shader[2], "scale");
			glUniform1f(scaleID, scale[i]);
			GLuint dirID = glGetUniformLocation(shader[2], "direct");
			glUniform1f(dirID, direct[i]);
			GLuint alphaID = glGetUniformLocation(shader[2], "alpha");
			glUniform1f(alphaID, alpha[i]);
			GLuint locID = glGetUniformLocation(shader[2], "loc");
			glUniform3f(locID, mapLoc[g], 0.0, 0.0);

			glDrawArrays(GL_TRIANGLES, 0, mapMesh[1]->faceLen * 3);

			glDisableVertexAttribArray(0);
			glDisableVertexAttribArray(1);
		}
		glDisable(GL_BLEND);
		glEnable(GL_CULL_FACE);
		glCullFace(GL_BACK);
	}


	return 0;
}

/// <summary>
/// H3用マップ描画
/// </summary>
int engine3dGL::DrawMapH3()
{

	//===== field of view, draw area


	//--- openGL -----
	glUseProgram(shader[2]);
	glClear(GL_DEPTH_BUFFER_BIT);

	// Projection matrix : left, right, bottom, top
	float aspf = GetAsp();
	glm::mat4 Projection = glm::ortho(-aspf, aspf, -1.0f, 1.0f, -2.0f, 2.0f);
	glm::mat4 View = glm::lookAt(
		glm::vec3(0, 0, 0), // Camera in World Space
		glm::vec3(0, 1, 0), // direction of view
		glm::vec3(0, 0, 1)  // head of POV
		);
	glm::mat4 Model = glm::mat4(1.0f);
	glm::mat4 MVP = Projection * View * Model;
	// Get a handle for our "MVP" uniform
	GLuint MatrixID = glGetUniformLocation(shader[2], "MVP");
	glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP[0][0]);
	GLuint aspID = glGetUniformLocation(shader[2], "asp");
	glUniform1f(aspID, aspf);

	//===========================

	float mapLoc = 0.0;
	int mapLen = 1;

	if (true)
	{
		glBindBuffer(GL_ARRAY_BUFFER, buffers[mapMesh[0]->texNo]);

		// 最初の属性バッファ : 頂点
		glEnableVertexAttribArray(0);
		glEnableVertexAttribArray(1);

		glVertexAttribPointer(  //---- 属性1    
			0, 3, GL_FLOAT, GL_FALSE,
			8 * sizeof(GLfloat), (void*)0
			);
		glVertexAttribPointer(    //---- 属性2
			1, 3, GL_FLOAT, GL_FALSE,
			8 * sizeof(GLfloat), (void*)(3 * sizeof(GLfloat))
			);

		GLuint scaleID = glGetUniformLocation(shader[2], "scale");
		glUniform1f(scaleID, 1.0);
		GLuint dirID = glGetUniformLocation(shader[2], "direct");
		if (mapDir == MapDirection::FRONT)
			glUniform1f(dirID, 1.0);
		else
			glUniform1f(dirID, 2.0);
		GLuint alphaID = glGetUniformLocation(shader[2], "alpha");
		glUniform1f(alphaID, 1.0);
		GLuint locID = glGetUniformLocation(shader[2], "loc");
		glUniform3f(locID, mapLoc, 0.0, 0.0);

		glDrawArrays(GL_TRIANGLES, 0, mapMesh[0]->faceLen * 3);

		glDisableVertexAttribArray(0);
		glDisableVertexAttribArray(1);


		// draw objects on map
		for (uint32_t h = BWH_QTY; h < objCnt; h++)
		{
			object3d* curObj = objs + h;

			if (!curObj->used) continue;
			else if (h == PLR_No) continue;
			else if (h < BWH_QTY + PLR_QTY && !VIEW_PLR) continue;


			pt3 tmpPt = curObj->locr;
			if (mapMode == MapMode::DUAL)
			{
				double tmpPtW = pyth3OS(tmpPt);
				pt4 tmpP4 = pt4(1 + tmpPtW, tmpPt.x, tmpPt.y, tmpPt.z);
				tmpPt = tmpP4.mtp(1 / (1 + tmpPtW)).xyz();
			}

			// 軸の一致
			double tmpZ = tmpPt.z;
			tmpPt.z = tmpPt.y;
			tmpPt.y = tmpZ;

			// マップの方向
			if (mapDir == MapDirection::FRONT)
			{
			}
			else if (mapDir == MapDirection::TOP)
			{
				double tmpY = tmpPt.y;
				tmpPt.y = -tmpPt.z;
				tmpPt.z = tmpY;
			}

			// front or back
			if (curObj->locr.z < 0)
				glBindBuffer(GL_ARRAY_BUFFER, buffers[meshs[17].texNo]);
			else
				glBindBuffer(GL_ARRAY_BUFFER, buffers[meshs[19].texNo]);


			// 最初の属性バッファ : 頂点
			glEnableVertexAttribArray(0);
			glEnableVertexAttribArray(1);

			glVertexAttribPointer(  //---- 属性1    
				0, 3, GL_FLOAT, GL_FALSE,
				8 * sizeof(GLfloat), (void*)0
				);
			glVertexAttribPointer(    //---- 属性2
				1, 3, GL_FLOAT, GL_FALSE,
				8 * sizeof(GLfloat), (void*)(3 * sizeof(GLfloat))
				);

			GLuint scaleID = glGetUniformLocation(shader[2], "scale");
			glUniform1f(scaleID, 1.0);
			GLuint dirID = glGetUniformLocation(shader[2], "direct");
			glUniform1f(dirID, 1.0);
			GLuint alphaID = glGetUniformLocation(shader[2], "alpha");
			glUniform1f(alphaID, 1);
			GLuint locID = glGetUniformLocation(shader[2], "loc");
			glUniform3f(locID, tmpPt.x + mapLoc, tmpPt.y, tmpPt.z);

			glDrawArrays(GL_TRIANGLES, 0, meshs[17].faceLen * 3);

			glDisableVertexAttribArray(0);
			glDisableVertexAttribArray(1);
		}


		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glDisable(GL_CULL_FACE);
		float c45d = 0.70710678118;
		float direct[5] = { 1.0, 1.0, 1.0, -1.0, -1.0 };
		float scale[5] = { 1.05, 1.0, c45d, c45d, 1.0 };
		float alpha[5] = { -1, 0.14 , 0.14 , 0.14 , 0.14 };

		for (int i = 0; i < 5; i++)
		{
			glBindBuffer(GL_ARRAY_BUFFER, buffers[mapMesh[1]->texNo]);

			// 最初の属性バッファ : 頂点
			glEnableVertexAttribArray(0);
			glEnableVertexAttribArray(1);

			glVertexAttribPointer(  //---- 属性1    
				0, 3, GL_FLOAT, GL_FALSE,
				8 * sizeof(GLfloat), (void*)0
				);
			glVertexAttribPointer(    //---- 属性2
				1, 3, GL_FLOAT, GL_FALSE,
				8 * sizeof(GLfloat), (void*)(3 * sizeof(GLfloat))
				);

			GLuint scaleID = glGetUniformLocation(shader[2], "scale");
			glUniform1f(scaleID, scale[i]);
			GLuint dirID = glGetUniformLocation(shader[2], "direct");
			glUniform1f(dirID, direct[i]);
			GLuint alphaID = glGetUniformLocation(shader[2], "alpha");
			glUniform1f(alphaID, alpha[i]);
			GLuint locID = glGetUniformLocation(shader[2], "loc");
			glUniform3f(locID, mapLoc, 0.0, 0.0);

			glDrawArrays(GL_TRIANGLES, 0, mapMesh[1]->faceLen * 3);

			glDisableVertexAttribArray(0);
			glDisableVertexAttribArray(1);
		}
		glDisable(GL_BLEND);
		glEnable(GL_CULL_FACE);
		glCullFace(GL_BACK);


		// 表示マップ種別描画
		// todo★ 文字とマップのサイズ調整処理を統一する
		glEnable(GL_BLEND);

		double asp = GetAsp();
		GuiString guiStr;
		guiStr.drawArea.l = 0.01;
		guiStr.fontSz = 0.030;
		guiStr.drawArea.t = (2 - (0.01 + guiStr.fontSz) * asp);
		guiStr.fontSpan = 0.8;
		guiStr.padding.l = 0;
		guiStr.padding.t = 0;

		guiStr.content = (mapMode == MapMode::DUAL) ? "P" : "K";
		DrawChars(guiStr);

		glDisable(GL_BLEND);
	}


	return 0;
}
