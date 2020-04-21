#include <stdint.h>
#include <Complex>
#include <algorithm>
#include "constants.h"
#include "engine3dGL.h"
#include "functions.h"

#include <glew.h>
#include <GL/gl.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
using namespace std;





int engine3dGL::simulateS3GL()
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
		drawEachObjs();
		break;

	case QY_MODE::LOW:
		drawEachObjs_LQY();
		break;
	}


	return 1;
}


int engine3dGL::drawEachObjs()
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

int engine3dGL::drawEachObjs_LQY()
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
		xID = glGetUniformLocation(shader[SDR], "revMd");
		glUniform1f(xID, 1);


		glBindBuffer(GL_ARRAY_BUFFER, buffers[curObj->mesh->texNo]);
		glBindTexture(GL_TEXTURE_2D, texNames[curObj->mesh->texNo]);



		glEnable(GL_TEXTURE_2D);


		int fltLen = 21;

		// (low環境用)
		if (SDR != 0) continue;
		GLuint vPosition = glGetAttribLocation(shader[SDR], "vPosition");
		GLuint vColor = glGetAttribLocation(shader[SDR], "vColor");
		GLuint vPos1 = glGetAttribLocation(shader[SDR], "vPos1");
		GLuint vPos2 = glGetAttribLocation(shader[SDR], "vPos2");
		GLuint vPos3 = glGetAttribLocation(shader[SDR], "vPos3");
		GLuint tPos1 = glGetAttribLocation(shader[SDR], "tPos1");
		GLuint tPos2 = glGetAttribLocation(shader[SDR], "tPos2");
		GLuint tPos3 = glGetAttribLocation(shader[SDR], "tPos3");


		// 最初の属性バッファ : 頂点
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

	return 1;
}
