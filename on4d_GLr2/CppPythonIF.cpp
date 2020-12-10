#include "CppPythonIF.h"


char CppPythonIF::rawCode[8186] = "";

PyObject* CppPythonIF::test(PyObject* self, PyObject* args)
{

    if (!PyArg_ParseTuple(args, ":numargs"))
        return NULL;
    return PyLong_FromLong(1);
};


//---------���N���X������ƐF�X�ʓ|�Ȃ̂ŃO���[�o��------------
PyMethodDef methods[] =
{
    // �L�q��F
    {
        "testX",           // Python��ł̊֐���
        CppPythonIF::test,  // �o�^����֐��̃|�C���^
        METH_VARARGS,
        "explanation."      // �֐��̐���
    },
    {NULL, NULL, 0, NULL}        /* �I�[����p�H */
};

PyModuleDef module = {
    PyModuleDef_HEAD_INIT,
    "h3sim",     /* name of module */
    "doc",      /* module documentation, may be NULL */
    -1,         /* size of per-interpreter state of the module,
                 or -1 if the module keeps state in global variables. */
    methods // �o�^����֐��z��
};
PyMODINIT_FUNC PyInit_CppModule()
{

    return PyModule_Create(&module);
}