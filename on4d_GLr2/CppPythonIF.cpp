#include "CppPythonIF.h"




PyObject* CppPythonIF::test(PyObject* self, PyObject* args)
{
    if (!PyArg_ParseTuple(args, ":numargs"))
        return NULL;
    return PyLong_FromLong(1);
};

PyMODINIT_FUNC PyInit_CppModule()
{
    PyMethodDef methods[] =
    {
        // 記述例：
        //{
        //    "system",           // Python上での関数名
        //    CppPythonIF::test,  // 登録する関数のポインタ
        //    METH_VARARGS,       
        //    "explanation."      // 関数の説明
        //},       
        {NULL, NULL, 0, NULL}        /* 終端判定用？ */
    };

    PyModuleDef module = {
        PyModuleDef_HEAD_INIT,
        "h3sim",     /* name of module */
        "doc",      /* module documentation, may be NULL */
        -1,         /* size of per-interpreter state of the module,
                     or -1 if the module keeps state in global variables. */
        methods // 登録する関数配列
    };

    return PyModule_Create(&module);
}