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
	int texs = meshLen+2;

	texNames = new uint32_t[ texs ];	// ���1 + mesh�� + �O��1
	buffers = new uint32_t[ texs ];


	// OS�ˑ� �������z�֐�
	try
	{
		GL_CreateContextPls();
	}
	catch(S3ErrorManager ex)
	{
		throw ex;
	}


	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glEnable(GL_DEPTH_TEST);
	//glEnable(GL_ALPHA_TEST);
	///---------------- glew ---------------
	GLenum err = glewInit();

	glGenBuffers(texs, buffers);	//�B���_�o�b�t�@���쐬����
	glBindBuffer(GL_ARRAY_BUFFER, buffers[0]);	//�o�C���h
	//
	

	int varing;
	glGetIntegerv(GL_MAX_VARYING_VECTORS, &varing);
	cout << "varing :" << varing << endl;

	// S3�V�F�[�_
	if (!GLEW_EXT_geometry_shader4 || varing < 31)
	{
		qyMode = QY_MODE::LOW;
		cout << "the geometry shader will not be used." << endl;

		shader[0] = LoadShaders("vartex_low.c", "pixel_low.c");
	}
	else
	{
		shader[0] = LoadShaders2("vartex.c", "geo.c", "pixel.c", 0);
	}
	shader[1] = LoadShaders( "vtx0.c", "pxl.c" );
	shader[2] = LoadShaders( "vtx3D.c", "pxl3D.c" );
	shader[3] = LoadShaders2( "vartex.c", "lineG.c", "lineF.c" , 1);
	shader[4] = LoadShaders2( "vartex.c", "lineG2.c", "lineF2.c" , 2);
	// H3�V�F�[�_
	shader[5] = LoadShaders("h3vtx.c", "h3pxl.c");

	///-- �e�N�X�`�� --///
	glEnable(GL_TEXTURE_2D);
	glGenTextures( texs, texNames );
	for(int i=0;i<texs;i++){
		glBindTexture(GL_TEXTURE_2D, texNames[i]);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	}


	return 1;
};



int engine3dGL::GL_End()
{
	int texs = meshLen+2;
	
	// ���C��VBO���
	glDeleteBuffers(texs, buffers+0);
	glDeleteTextures(texs, texNames+0);

	// OS�ˑ� �������z�֐�
	GL_DeleteContextPls();

	
	delete[] texNames;
	delete[] buffers;

	return 0;
};
