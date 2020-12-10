#pragma once
#define PY_SSIZE_T_CLEAN
#include <Python.h>
#include <string>


class CppPythonIF
{
public:
    static char rawCode[8186];
    static PyObject* test(PyObject* self, PyObject* args);
};

PyMODINIT_FUNC PyInit_CppModule();

