#include <stdint.h>
#include <stdio.h>
#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <algorithm>
using namespace std;

#include <stdlib.h>

#include <glew.h>
#include <GL/gl.h>

#include "engine3dGL.h"

uint32_t engine3dGL::LoadShaders(const char * vPath,const char * fPath){

	// Create the shaders
	GLuint vtxID = glCreateShader(GL_VERTEX_SHADER);
	GLuint frgID = glCreateShader(GL_FRAGMENT_SHADER);

	// Read the Vertex Shader code from the file
	std::string VertexShaderCode;
	std::ifstream VertexShaderStream(vPath, std::ios::in);
	if(VertexShaderStream.is_open()){
		std::string Line = "";
		while(getline(VertexShaderStream, Line))
			VertexShaderCode += "\n" + Line;
		VertexShaderStream.close();
	}else{
		printf("Impossible to open %s. Are you in the right directory ? Don't forget to read the FAQ !\n", vPath);
		getchar();
		return 0;
	}

	// Read the Fragment Shader code from the file
	std::string FragmentShaderCode;
	std::ifstream FragmentShaderStream(fPath, std::ios::in);
	if(FragmentShaderStream.is_open()){
		std::string Line = "";
		while(getline(FragmentShaderStream, Line))
			FragmentShaderCode += "\n" + Line;
		FragmentShaderStream.close();
	}

	GLint Result = GL_FALSE;
	int InfoLogLength;


	// Compile Vertex Shader
	printf("Compiling shader : %s\n", vPath);
	char const * VertexSourcePointer = VertexShaderCode.c_str();
	glShaderSource(vtxID, 1, &VertexSourcePointer , NULL);
	glCompileShader(vtxID);

	// Check Vertex Shader
	glGetShaderiv(vtxID, GL_COMPILE_STATUS, &Result);
	glGetShaderiv(vtxID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	if ( InfoLogLength > 0 ){
		std::vector<char> VertexShaderErrorMessage(InfoLogLength+1);
		glGetShaderInfoLog(vtxID, InfoLogLength, NULL, &VertexShaderErrorMessage[0]);
		printf("%s\n", &VertexShaderErrorMessage[0]);
	}



	// Compile Fragment Shader
	printf("Compiling shader : %s\n", fPath);
	char const * FragmentSourcePointer = FragmentShaderCode.c_str();
	glShaderSource(frgID, 1, &FragmentSourcePointer , NULL);
	glCompileShader(frgID);

	// Check Fragment Shader
	glGetShaderiv(frgID, GL_COMPILE_STATUS, &Result);
	glGetShaderiv(frgID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	if ( InfoLogLength > 0 ){
		std::vector<char> FragmentShaderErrorMessage(InfoLogLength+1);
		glGetShaderInfoLog(frgID, InfoLogLength, NULL, &FragmentShaderErrorMessage[0]);
		printf("%s\n", &FragmentShaderErrorMessage[0]);
	}



	// Link the program
	printf("Linking program\n");
	GLuint ProgramID = glCreateProgram();
	glAttachShader(ProgramID, vtxID);
	glAttachShader(ProgramID, frgID);
	glLinkProgram(ProgramID);

	// Check the program
	glGetProgramiv(ProgramID, GL_LINK_STATUS, &Result);
	glGetProgramiv(ProgramID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	if ( InfoLogLength > 0 ){
		std::vector<char> ProgramErrorMessage(InfoLogLength+1);
		glGetProgramInfoLog(ProgramID, InfoLogLength, NULL, &ProgramErrorMessage[0]);
		printf("%s\n", &ProgramErrorMessage[0]);
	}

	
	glDetachShader(ProgramID, vtxID);
	glDetachShader(ProgramID, frgID);
	
	glDeleteShader(vtxID);
	glDeleteShader(frgID);

	return ProgramID;
}

uint32_t engine3dGL::LoadShaders2(const char * vPath, const char * gPath, const char * fPath, char mode){

	// シェーダを作る
	GLuint vtxID = glCreateShader(GL_VERTEX_SHADER);
	GLuint geoID = glCreateShader(GL_GEOMETRY_SHADER);
	GLuint frgID = glCreateShader(GL_FRAGMENT_SHADER);///

	// ファイルから頂点シェーダのコードを読む
	std::string VertexShaderCode;
	std::ifstream VertexShaderStream(vPath, std::ios::in);
	if(VertexShaderStream.is_open()){
		std::string Line = "";
		while(getline(VertexShaderStream, Line))
			VertexShaderCode += "\n" + Line;
		VertexShaderStream.close();
	}else{
		printf("Impossible to open %s. Are you in the right directory ? Don't forget to read the FAQ !\n", vPath);
		getchar();
		return 0;
	}

	// ファイルから断片シェーダのコードを読む
	std::string FragmentShaderCode;
	std::ifstream FragmentShaderStream(fPath, std::ios::in);
	if(FragmentShaderStream.is_open()){
		std::string Line = "";
		while(getline(FragmentShaderStream, Line))
			FragmentShaderCode += "\n" + Line;
		FragmentShaderStream.close();
	}
	/// ファイルから断片シェーダのコードを読む
	std::string GeoShaderCode;
	std::ifstream GeoShaderStream(gPath, std::ios::in);
	if(GeoShaderStream.is_open()){
		std::string Line = "";
		while(getline(GeoShaderStream, Line))
			GeoShaderCode += "\n" + Line;
		GeoShaderStream.close();
	}

	GLint Result = GL_FALSE;
	int InfoLogLength;


	// 頂点シェーダをコンパイル
	printf("Compiling shader : %s\n", vPath);
	char const * VertexSourcePointer = VertexShaderCode.c_str();
	glShaderSource(vtxID, 1, &VertexSourcePointer , NULL);
	glCompileShader(vtxID);

	// 頂点シェーダをチェック
	glGetShaderiv(vtxID, GL_COMPILE_STATUS, &Result);
	glGetShaderiv(vtxID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	if ( InfoLogLength > 0 ){
		std::vector<char> VertexShaderErrorMessage(InfoLogLength+1);
		glGetShaderInfoLog(vtxID, InfoLogLength, NULL, &VertexShaderErrorMessage[0]);
		printf("%s\n", &VertexShaderErrorMessage[0]);
	}



	// 断片シェーダをコンパイル
	printf("Compiling shader : %s\n", fPath);
	char const * FragmentSourcePointer = FragmentShaderCode.c_str();
	glShaderSource(frgID, 1, &FragmentSourcePointer , NULL);
	glCompileShader(frgID);

	// 断片シェーダをチェック
	glGetShaderiv(frgID, GL_COMPILE_STATUS, &Result);
	glGetShaderiv(frgID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	if ( InfoLogLength > 0 ){
		std::vector<char> FragmentShaderErrorMessage(InfoLogLength+1);
		glGetShaderInfoLog(frgID, InfoLogLength, NULL, &FragmentShaderErrorMessage[0]);
		printf("%s\n", &FragmentShaderErrorMessage[0]);
	}

	/// 幾何シェーダをコンパイル
	printf("Compiling shader : %s\n", gPath);
	char const * GeoSourcePointer = GeoShaderCode.c_str();
	glShaderSource(geoID, 1, &GeoSourcePointer , NULL);
	glCompileShader(geoID);

	/// 幾何シェーダをチェック
	glGetShaderiv(geoID, GL_COMPILE_STATUS, &Result);
	glGetShaderiv(geoID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	if ( InfoLogLength > 0 ){
		std::vector<char> GeoShaderErrorMessage(InfoLogLength+1);
		glGetShaderInfoLog(geoID, InfoLogLength, NULL, &GeoShaderErrorMessage[0]);
		printf("%s\n", &GeoShaderErrorMessage[0]);
	}



	// プログラムをリンク
	printf("Linking program\n");
	GLuint ProgramID = glCreateProgram();
	glAttachShader(ProgramID, vtxID);
	glAttachShader(ProgramID, geoID);///
	glAttachShader(ProgramID, frgID);
	///-- 
	if(mode==0){
		glProgramParameteriEXT(ProgramID, GL_GEOMETRY_INPUT_TYPE_EXT, GL_TRIANGLES);
		glProgramParameteriEXT(ProgramID, GL_GEOMETRY_OUTPUT_TYPE_EXT, GL_TRIANGLE_STRIP);
	}else if(mode==1){
		glProgramParameteriEXT(ProgramID, GL_GEOMETRY_INPUT_TYPE_EXT, GL_LINES);
		glProgramParameteriEXT(ProgramID, GL_GEOMETRY_OUTPUT_TYPE_EXT, GL_POINTS);
	}else{
		glProgramParameteriEXT(ProgramID, GL_GEOMETRY_INPUT_TYPE_EXT, GL_LINES);
		glProgramParameteriEXT(ProgramID, GL_GEOMETRY_OUTPUT_TYPE_EXT, GL_LINE_STRIP);
	}

	glGetIntegerv(GL_MAX_GEOMETRY_OUTPUT_VERTICES, &outMax);
	glProgramParameteriEXT(ProgramID, GL_GEOMETRY_VERTICES_OUT_EXT, outMax);

	glLinkProgram(ProgramID);

	// プログラムをチェック
	glGetProgramiv(ProgramID, GL_LINK_STATUS, &Result);
	glGetProgramiv(ProgramID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	if ( InfoLogLength > 0 ){
		std::vector<char> ProgramErrorMessage(InfoLogLength+1);
		glGetProgramInfoLog(ProgramID, InfoLogLength, NULL, &ProgramErrorMessage[0]);
		printf("%s\n", &ProgramErrorMessage[0]);
	}

	
	glDetachShader(ProgramID, vtxID);
	glDetachShader(ProgramID, geoID);///
	glDetachShader(ProgramID, frgID);
	
	glDeleteShader(vtxID);
	glDeleteShader(geoID);///
	glDeleteShader(frgID);

	return ProgramID;
}

