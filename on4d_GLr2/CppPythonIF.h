#pragma once
#define PY_SSIZE_T_CLEAN
#include <Python.h>
#include <string>
#include "object.h"
#include "engine3d.h"


class CppPythonIF
{
public:
    enum class FuncObject
    {
        SetScale = 0,
        GetScale,
        SetVelocity,
        GetVelocity
    };

    static engine3d* engine;
    static char rawCode[8186];
    static PyObject* GetObjData(PyObject* self, PyObject* args);
    static PyObject* SetLocRelative(PyObject* self, PyObject* args);
    static PyObject* SetRotRelative(PyObject* self, PyObject* args);
    static PyObject* GetPlayerObj(PyObject* self, PyObject* args);
    static PyObject* Func_Pram1d(PyObject* self, PyObject* args);
    static PyObject* Func_NoParam(PyObject* self, PyObject* args);

    // Pythonオブジェクト
    static PyObject* pModule;
    static PyObject* pDict;

};

PyMODINIT_FUNC PyInit_CppModule();

