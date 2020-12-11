#pragma once
#define PY_SSIZE_T_CLEAN
#include <Python.h>
#include <string>
#include "object.h"
#include "engine3d.h"


class CppPythonIF
{
public:
    static engine3d* engine;
    static char rawCode[8186];
    static PyObject* GetObjData(PyObject* self, PyObject* args);
    static PyObject* SetLocRelative(PyObject* self, PyObject* args);
    static PyObject* GetPlayerObj(PyObject* self, PyObject* args);

    // Pythonオブジェクト
    static PyObject* pModule;
    static PyObject* pDict;

};

PyMODINIT_FUNC PyInit_CppModule();

