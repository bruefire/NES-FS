#pragma once


class engine3d;

/// <summary>
/// Pythonインタラクション全体管理クラス
/// </summary>
class PyInteract
{
public:
	void Init(engine3d*);
	void Update();
	void Dispose();
private:
	bool initPyFlg;
	std::string updPyStr;
	PyObject* catcher;
};
