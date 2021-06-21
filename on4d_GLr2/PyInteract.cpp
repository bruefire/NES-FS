#define PY_SSIZE_T_CLEAN
#include <Windows.h>
#include <windowsx.h>
#include <Python.h>
#include <iostream>
#include "CppPythonIF.h"
#include "PyInteract.h"
using namespace std;


void PyInteract::Init(engine3d* newEngine)
{
	// Python初期化
	string updPyFile = "pyUpd.py";
	if (PyImport_AppendInittab("h3sim", PyInit_CppModule) == -1)
		initPyFlg = false;
	else
	{
		CppPythonIF::engine = newEngine;
		// python初期化ファイル
		string pyInitFle = "pyInit.py";
		FILE* pyInitFp = fopen(pyInitFle.c_str(), "rb");

		Py_InitializeEx(0);
		CppPythonIF::pModule = PyImport_AddModule("__main__");
		CppPythonIF::pDict = PyModule_GetDict(CppPythonIF::pModule);
		char tmpStr[1024];
		string initPyStr = "";
		while (fgets(tmpStr, 1024, pyInitFp) != NULL)
			initPyStr += tmpStr;

		PyRun_SimpleString(initPyStr.c_str());
		catcher = PyObject_GetAttrString(CppPythonIF::pModule, "catchOutErr");
		initPyFlg = true;

		fclose(pyInitFp);

		// upd file
		FILE* pyUpdFp = fopen(updPyFile.c_str(), "rb");
		while (fgets(tmpStr, 1024, pyUpdFp) != NULL)
			updPyStr += tmpStr;

		fclose(pyUpdFp);
	}
}

void PyInteract::Update()
{
	// Pythonスクリプト処理
	if (initPyFlg)
	{
		// exec constant script
		PyRun_SimpleString(updPyStr.c_str());
	}
	if (initPyFlg && CppPythonIF::rawCode)
	{
		// exec instant script
		PyRun_SimpleString(CppPythonIF::rawCode.get());

		PyObject* output = PyObject_GetAttrString(catcher, "value");
		const char* cnvStr = PyUnicode_AsUTF8AndSize(output, nullptr);
		if (cnvStr != nullptr)
			cout << cnvStr << endl;

		CppPythonIF::rawCode.reset(nullptr);
		Py_XDECREF(output);
		PyRun_SimpleString("sys.stdout.value = ''");
	}
}

void PyInteract::Dispose()
{
	if (initPyFlg) {
		Py_XDECREF(catcher);
		Py_FinalizeEx();
	}
}
