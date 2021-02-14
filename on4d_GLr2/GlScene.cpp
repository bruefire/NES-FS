//#include <iostream>
//#include <fstream>
//#include "GlScene.h"
//#include "engine3dGL.h"
//#include <glew.h>
//#include <GL/gl.h>
//using namespace std;
//using WorldGeo = engine3dGL::WorldGeo;
//
//
///// <summary>
///// init the scene.
///// this class assume gl window context already exist.
///// </summary>
//void GlScene::Init(engine3dGL* owner)
//{
//	this->owner = owner;
//	shader.resize(6);
//
//	GLenum err = glewInit();
//	glEnable(GL_CULL_FACE);
//	glCullFace(GL_BACK);
//	glEnable(GL_DEPTH_TEST);
//
//	int varing;
//	glGetIntegerv(GL_MAX_VARYING_VECTORS, &varing);
//	cout << "varing :" << varing << endl;
//
//	InitShader();
//	InitVertex();
//	InitTexture();
//}
//
///// <summary>
///// dispose the scene.
///// </summary>
//void GlScene::Dispose()
//{
//	DisposeShader();
//	DisposeVertex();
//	DisposeTexture();
//}
//
//
//void GlScene::InitShader()
//{
//	int varing;
//	glGetIntegerv(GL_MAX_VARYING_VECTORS, &varing);
//	cout << "varing :" << varing << endl;
//
//	if (owner->worldGeo == WorldGeo::HYPERBOLIC)
//	{
//		// H3シェーダ
//		qyMode = QY_MODE::LOW;
//		shader[5] = LoadShaders("h3vtx.c", "h3pxl.c");
//		shader[3] = LoadShaders("h3vtxPt.c", "h3pxlPt.c");
//	}
//	// S3シェーダ
//	else
//	{
//		if (!GLEW_EXT_geometry_shader4 || varing < 31)
//		{
//			qyMode = QY_MODE::LOW;
//			cout << "the geometry shader will not be used." << endl;
//
//			shader[0] = LoadShaders("vartex_low.c", "pixel_low.c");
//			shader[3] = LoadShaders("vtxLowPt.c", "pxlLowPt.c");
//		}
//		else
//		{
//			shader[0] = LoadShaders2("vartex.c", "geo.c", "pixel.c", 0);
//			shader[3] = LoadShaders2("vartex.c", "lineG.c", "lineF.c", 1);
//		}
//	}
//	shader[1] = LoadShaders("vtx0.c", "pxl.c");
//	shader[2] = LoadShaders("vtx3D.c", "pxl3D.c");
//	shader[4] = LoadShaders2("vartex.c", "lineG2.c", "lineF2.c", 2);
//}
//
//
//void GlScene::InitVertex()
//{
//
//}
//
//
//uint32_t GlScene::LoadShaders(const char* vName, const char* fName)
//{
//	string shDir = "shader\\";
//	string _vPath = shDir + vName;
//	string _fPath = shDir + fName;
//	const char* vPath = _vPath.c_str();
//	const char* fPath = _fPath.c_str();
//
//	// Create the shaders
//	GLuint vtxID = glCreateShader(GL_VERTEX_SHADER);
//	GLuint frgID = glCreateShader(GL_FRAGMENT_SHADER);
//
//	// Read the Vertex Shader code from the file
//	std::string VertexShaderCode;
//	std::ifstream VertexShaderStream(vPath, std::ios::in);
//	if (VertexShaderStream.is_open()) {
//		std::string Line = "";
//		while (getline(VertexShaderStream, Line))
//			VertexShaderCode += "\n" + Line;
//		VertexShaderStream.close();
//	}
//	else {
//		printf("Impossible to open %s. Are you in the right directory ? Don't forget to read the FAQ !\n", vPath);
//		getchar();
//		return 0;
//	}
//
//	// Read the Fragment Shader code from the file
//	std::string FragmentShaderCode;
//	std::ifstream FragmentShaderStream(fPath, std::ios::in);
//	if (FragmentShaderStream.is_open()) {
//		std::string Line = "";
//		while (getline(FragmentShaderStream, Line))
//			FragmentShaderCode += "\n" + Line;
//		FragmentShaderStream.close();
//	}
//
//	GLint Result = GL_FALSE;
//	int InfoLogLength;
//
//
//	// Compile Vertex Shader
//	printf("Compiling shader : %s\n", vPath);
//	char const* VertexSourcePointer = VertexShaderCode.c_str();
//	glShaderSource(vtxID, 1, &VertexSourcePointer, NULL);
//	glCompileShader(vtxID);
//
//	// Check Vertex Shader
//	glGetShaderiv(vtxID, GL_COMPILE_STATUS, &Result);
//	glGetShaderiv(vtxID, GL_INFO_LOG_LENGTH, &InfoLogLength);
//	if (InfoLogLength > 0) {
//		std::vector<char> VertexShaderErrorMessage(InfoLogLength + 1);
//		glGetShaderInfoLog(vtxID, InfoLogLength, NULL, &VertexShaderErrorMessage[0]);
//		printf("%s\n", &VertexShaderErrorMessage[0]);
//	}
//
//
//
//	// Compile Fragment Shader
//	printf("Compiling shader : %s\n", fPath);
//	char const* FragmentSourcePointer = FragmentShaderCode.c_str();
//	glShaderSource(frgID, 1, &FragmentSourcePointer, NULL);
//	glCompileShader(frgID);
//
//	// Check Fragment Shader
//	glGetShaderiv(frgID, GL_COMPILE_STATUS, &Result);
//	glGetShaderiv(frgID, GL_INFO_LOG_LENGTH, &InfoLogLength);
//	if (InfoLogLength > 0) {
//		std::vector<char> FragmentShaderErrorMessage(InfoLogLength + 1);
//		glGetShaderInfoLog(frgID, InfoLogLength, NULL, &FragmentShaderErrorMessage[0]);
//		printf("%s\n", &FragmentShaderErrorMessage[0]);
//	}
//
//
//
//	// Link the program
//	printf("Linking program\n");
//	GLuint ProgramID = glCreateProgram();
//	glAttachShader(ProgramID, vtxID);
//	glAttachShader(ProgramID, frgID);
//	glLinkProgram(ProgramID);
//
//	// Check the program
//	glGetProgramiv(ProgramID, GL_LINK_STATUS, &Result);
//	glGetProgramiv(ProgramID, GL_INFO_LOG_LENGTH, &InfoLogLength);
//	if (InfoLogLength > 0) {
//		std::vector<char> ProgramErrorMessage(InfoLogLength + 1);
//		glGetProgramInfoLog(ProgramID, InfoLogLength, NULL, &ProgramErrorMessage[0]);
//		printf("%s\n", &ProgramErrorMessage[0]);
//	}
//
//
//	glDetachShader(ProgramID, vtxID);
//	glDetachShader(ProgramID, frgID);
//
//	glDeleteShader(vtxID);
//	glDeleteShader(frgID);
//
//	return ProgramID;
//}
//
//uint32_t GlScene::LoadShaders2(const char* vName, const char* gName, const char* fName, char mode)
//{
//	string shDir = "shader\\";
//	string _vPath = shDir + vName;
//	string _gPath = shDir + gName;
//	string _fPath = shDir + fName;
//	const char* vPath = _vPath.c_str();
//	const char* gPath = _gPath.c_str();
//	const char* fPath = _fPath.c_str();
//
//	// シェーダを作る
//	GLuint vtxID = glCreateShader(GL_VERTEX_SHADER);
//	GLuint geoID = glCreateShader(GL_GEOMETRY_SHADER);
//	GLuint frgID = glCreateShader(GL_FRAGMENT_SHADER);///
//
//	// ファイルから頂点シェーダのコードを読む
//	std::string VertexShaderCode;
//	std::ifstream VertexShaderStream(vPath, std::ios::in);
//	if (VertexShaderStream.is_open()) {
//		std::string Line = "";
//		while (getline(VertexShaderStream, Line))
//			VertexShaderCode += "\n" + Line;
//		VertexShaderStream.close();
//	}
//	else {
//		printf("Impossible to open %s. Are you in the right directory ? Don't forget to read the FAQ !\n", vPath);
//		getchar();
//		return 0;
//	}
//
//	// ファイルから断片シェーダのコードを読む
//	std::string FragmentShaderCode;
//	std::ifstream FragmentShaderStream(fPath, std::ios::in);
//	if (FragmentShaderStream.is_open()) {
//		std::string Line = "";
//		while (getline(FragmentShaderStream, Line))
//			FragmentShaderCode += "\n" + Line;
//		FragmentShaderStream.close();
//	}
//	/// ファイルから断片シェーダのコードを読む
//	std::string GeoShaderCode;
//	std::ifstream GeoShaderStream(gPath, std::ios::in);
//	if (GeoShaderStream.is_open()) {
//		std::string Line = "";
//		while (getline(GeoShaderStream, Line))
//			GeoShaderCode += "\n" + Line;
//		GeoShaderStream.close();
//	}
//
//	GLint Result = GL_FALSE;
//	int InfoLogLength;
//
//
//	// 頂点シェーダをコンパイル
//	printf("Compiling shader : %s\n", vPath);
//	char const* VertexSourcePointer = VertexShaderCode.c_str();
//	glShaderSource(vtxID, 1, &VertexSourcePointer, NULL);
//	glCompileShader(vtxID);
//
//	// 頂点シェーダをチェック
//	glGetShaderiv(vtxID, GL_COMPILE_STATUS, &Result);
//	glGetShaderiv(vtxID, GL_INFO_LOG_LENGTH, &InfoLogLength);
//	if (InfoLogLength > 0) {
//		std::vector<char> VertexShaderErrorMessage(InfoLogLength + 1);
//		glGetShaderInfoLog(vtxID, InfoLogLength, NULL, &VertexShaderErrorMessage[0]);
//		printf("%s\n", &VertexShaderErrorMessage[0]);
//	}
//
//
//
//	// 断片シェーダをコンパイル
//	printf("Compiling shader : %s\n", fPath);
//	char const* FragmentSourcePointer = FragmentShaderCode.c_str();
//	glShaderSource(frgID, 1, &FragmentSourcePointer, NULL);
//	glCompileShader(frgID);
//
//	// 断片シェーダをチェック
//	glGetShaderiv(frgID, GL_COMPILE_STATUS, &Result);
//	glGetShaderiv(frgID, GL_INFO_LOG_LENGTH, &InfoLogLength);
//	if (InfoLogLength > 0) {
//		std::vector<char> FragmentShaderErrorMessage(InfoLogLength + 1);
//		glGetShaderInfoLog(frgID, InfoLogLength, NULL, &FragmentShaderErrorMessage[0]);
//		printf("%s\n", &FragmentShaderErrorMessage[0]);
//	}
//
//	/// 幾何シェーダをコンパイル
//	printf("Compiling shader : %s\n", gPath);
//	char const* GeoSourcePointer = GeoShaderCode.c_str();
//	glShaderSource(geoID, 1, &GeoSourcePointer, NULL);
//	glCompileShader(geoID);
//
//	/// 幾何シェーダをチェック
//	glGetShaderiv(geoID, GL_COMPILE_STATUS, &Result);
//	glGetShaderiv(geoID, GL_INFO_LOG_LENGTH, &InfoLogLength);
//	if (InfoLogLength > 0) {
//		std::vector<char> GeoShaderErrorMessage(InfoLogLength + 1);
//		glGetShaderInfoLog(geoID, InfoLogLength, NULL, &GeoShaderErrorMessage[0]);
//		printf("%s\n", &GeoShaderErrorMessage[0]);
//	}
//
//
//
//	// プログラムをリンク
//	printf("Linking program\n");
//	GLuint ProgramID = glCreateProgram();
//	glAttachShader(ProgramID, vtxID);
//	glAttachShader(ProgramID, geoID);///
//	glAttachShader(ProgramID, frgID);
//	///-- 
//	if (mode == 0) {
//		glProgramParameteriEXT(ProgramID, GL_GEOMETRY_INPUT_TYPE_EXT, GL_TRIANGLES);
//		glProgramParameteriEXT(ProgramID, GL_GEOMETRY_OUTPUT_TYPE_EXT, GL_TRIANGLE_STRIP);
//	}
//	else if (mode == 1) {
//		glProgramParameteriEXT(ProgramID, GL_GEOMETRY_INPUT_TYPE_EXT, GL_LINES);
//		glProgramParameteriEXT(ProgramID, GL_GEOMETRY_OUTPUT_TYPE_EXT, GL_POINTS);
//	}
//	else {
//		glProgramParameteriEXT(ProgramID, GL_GEOMETRY_INPUT_TYPE_EXT, GL_LINES);
//		glProgramParameteriEXT(ProgramID, GL_GEOMETRY_OUTPUT_TYPE_EXT, GL_LINE_STRIP);
//	}
//
//	int outMax;
//	glGetIntegerv(GL_MAX_GEOMETRY_OUTPUT_VERTICES, &outMax);
//	glProgramParameteriEXT(ProgramID, GL_GEOMETRY_VERTICES_OUT_EXT, outMax);
//
//	glLinkProgram(ProgramID);
//
//	// プログラムをチェック
//	glGetProgramiv(ProgramID, GL_LINK_STATUS, &Result);
//	glGetProgramiv(ProgramID, GL_INFO_LOG_LENGTH, &InfoLogLength);
//	if (InfoLogLength > 0) {
//		std::vector<char> ProgramErrorMessage(InfoLogLength + 1);
//		glGetProgramInfoLog(ProgramID, InfoLogLength, NULL, &ProgramErrorMessage[0]);
//		printf("%s\n", &ProgramErrorMessage[0]);
//	}
//
//
//	glDetachShader(ProgramID, vtxID);
//	glDetachShader(ProgramID, geoID);///
//	glDetachShader(ProgramID, frgID);
//
//	glDeleteShader(vtxID);
//	glDeleteShader(geoID);///
//	glDeleteShader(frgID);
//
//	return ProgramID;
//}
//
//
