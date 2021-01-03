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
        None = -1,
        SetObjScale = 0,
        GetObjScale,
        SetObjVelocity,
        GetObjVelocity,
        SetObjRotVelocity,
        GetObjRotVelocity,
        SetPt3x,
        SetPt3y,
        SetPt3z,
        SetWorldScale,
        GetWorldScale
    };
    static FuncObject funcID;


    static engine3d* engine;
    static char rawCode[8186];
    static PyObject* GetObjData(PyObject* self, PyObject* args);
    static PyObject* SetLocRelative(PyObject* self, PyObject* args);
    static PyObject* SetRotRelative(PyObject* self, PyObject* args);
    static PyObject* GetPlayerObj(PyObject* self, PyObject* args);

    static PyObject* SetCommonFunc(PyObject* self, PyObject* args);
    static PyObject* ExecCommonFunc(PyObject* self, PyObject* args);

    // Pythonオブジェクト
    static PyObject* pModule;
    static PyObject* pDict;

};

PyMODINIT_FUNC PyInit_CppModule();

