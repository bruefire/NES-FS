#include <Windows.h>
#include <Complex>
#include <algorithm>
#include <mmsystem.h>
#include "constants.h"
#include "global_var2.h"
#include "engine3dWin.h"
#include "functions.h"

#include <glew.h>
#include <GL/gl.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
using namespace std;




int engine3dWin::simulate3(UINT* bmp)
{
	
	int avl_ObjCnt;
	//===== field of view, draw area
	double cRangeX, cRangeY;
	short P_WIDTH, P_HEIGHT, W_REST;
	
	cRangeX = cRangeY = tan((LDBL)10.0 *0.5 *PIE/180);
	//--
	P_WIDTH = P_HEIGHT = (short)(HEIGHT*0.3);
	W_REST = WIDTH-P_WIDTH;
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

	float mapLoc[2] = {0.0, 2.0};
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
		for (UINT h = BWH_QTY; h < objCnt; h++)
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
