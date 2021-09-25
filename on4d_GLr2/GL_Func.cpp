#include <stdint.h>
#include <iostream>
#include <Complex>
#include <glew.h>
#include <GL/gl.h>

#include "constants.h"
#include "engine3dGL.h"
#include "S3ErrorManager.h"
using namespace std;


int engine3dGL::GL_Init()
{

	// OS�ˑ� �������z�֐�
	try
	{
		GL_CreateContextPls();
	}
	catch(S3ErrorManager ex)
	{
		throw ex;
	}

	// init the scene.
	GL_InitScene();

	return 1;
};


void engine3dGL::GL_InitScene()
{
	glTexLen = meshLen + 3;
	int texs = glTexLen;
	texNames = new uint32_t[texs];	// ���1 + mesh�� + �O��1 + ���j���[1
	buffers = new uint32_t[texs];
	h3trackBuf = new uint32_t[markObjSubLen];

	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glEnable(GL_DEPTH_TEST);
	//glEnable(GL_ALPHA_TEST);
	///---------------- glew ---------------
	GLenum err = glewInit();

	glGenBuffers(texs, buffers);
	glGenBuffers(markObjSubLen, h3trackBuf);
	glBindBuffer(GL_ARRAY_BUFFER, buffers[0]);	//�o�C���h
	//


	int varing;
	glGetIntegerv(GL_MAX_VARYING_VECTORS, &varing);
	cout << "varing :" << varing << endl;

	if (worldGeo == WorldGeo::HYPERBOLIC)
	{
		// H3�V�F�[�_
		qyMode = QY_MODE::LOW;
		shader[5] = LoadShaders("h3vtx.c", "h3pxl.c");
		shader[3] = LoadShaders("h3vtxPt.c", "h3pxlPt.c");
	}
	// S3�V�F�[�_
	else
	{
		if (!GLEW_EXT_geometry_shader4 || varing < 31)
		{
			qyMode = QY_MODE::LOW;
			cout << "the geometry shader will not be used." << endl;

			shader[0] = LoadShaders("vartex_low.c", "pixel_low.c");
			shader[3] = LoadShaders("vtxLowPt.c", "pxlLowPt.c");
		}
		else
		{
			shader[0] = LoadShaders2("vartex.c", "geo.c", "pixel.c", 0);
			shader[3] = LoadShaders2("vartex.c", "lineG.c", "lineF.c", 1);
		}
	}
	shader[1] = LoadShaders("vtx0.c", "pxl.c");
	shader[2] = LoadShaders("vtx3D.c", "pxl3D.c");
	shader[4] = LoadShaders2("vartex.c", "lineG2.c", "lineF2.c", 2);

	///-- �e�N�X�`�� --///
	glEnable(GL_TEXTURE_2D);
	glGenTextures(texs, texNames);
	for (int i = 0; i < texs; i++) {
		glBindTexture(GL_TEXTURE_2D, texNames[i]);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	}
}


void engine3dGL::GL_DisposeScene()
{
	int texs = glTexLen;

	// ���C��VBO���
	glDeleteBuffers(texs, buffers + 0);
	glDeleteBuffers(markObjSubLen, h3trackBuf + 0);
	glDeleteTextures(texs, texNames + 0);

	// �V�F�\�_�[�㏈��
	int shLen = shader.size();
	for (int i = 0; i < shLen; i++)
		glDeleteProgram(shader[i]);

	delete[] texNames;
	delete[] buffers;
	delete[] h3trackBuf;
}



int engine3dGL::GL_End()
{
	// �V�[���㏈��
	GL_DisposeScene();

	// OS�ˑ� �������z�֐�
	GL_DeleteContextPls();


	return 0;
};
