#pragma once


class engine3d;

/// <summary>
/// Python�C���^���N�V�����S�̊Ǘ��N���X
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
