#pragma once
#define PY_SSIZE_T_CLEAN
#include <Python.h>


class CppPythonIF
{
public:
    static PyObject* test(PyObject* self, PyObject* args);
};

PyMODINIT_FUNC PyInit_CppModule();

