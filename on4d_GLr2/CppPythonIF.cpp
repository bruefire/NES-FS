#include "CppPythonIF.h"
#include <vector>
using namespace std;

engine3d* CppPythonIF::engine = nullptr;
char CppPythonIF::rawCode[8186] = "";
PyObject* CppPythonIF::pModule;
PyObject* CppPythonIF::pDict;



// オブジェクトデータ取得
PyObject* CppPythonIF::GetObjData(PyObject* self, PyObject* args)
{
    PyObject* objDataType = PyDict_GetItemString(pDict, "ObjData");
    PyObject* pList = PyList_New(0);


    if (engine != nullptr)
    {
        for (int i = 0; i < engine->OBJ_QTY; i++)
        {
            PyObject* objData = PyObject_CallObject(objDataType, nullptr);
            PyObject* setMeth = PyObject_CallMethod(objData, "SetIdx", "i", i);
            
            PyList_Append(pList, objData);

            Py_XDECREF(objData);
            Py_XDECREF(setMeth);
        }
    }

    if (!PyArg_ParseTuple(args, ":numargs"))
        return NULL;

    return pList;
};


// 相対的な位置設定
PyObject* CppPythonIF::SetLocRelative(PyObject* self, PyObject* args)
{

    return PyLong_FromLong(1);
}



//---------▼クラス化すると色々面倒なのでグローバル------------
PyMethodDef methods[] =
{
    // 記述例：
    //{
    //    "test",           // Python上での関数名
    //    CppPythonIF::test,  // 登録する関数のポインタ
    //    METH_VARARGS,
    //    "explanation."      // 関数の説明
    //},
    {
        "GetObjData",               
        CppPythonIF::GetObjData,    
        METH_VARARGS,
        "explanation."            
    },
    {NULL, NULL, 0, NULL}   /* 終端判定用？ */
};

PyModuleDef module = {
    PyModuleDef_HEAD_INIT,
    "h3sim",     /* name of module */
    "doc",      /* module documentation, may be NULL */
    -1,         /* size of per-interpreter state of the module,
                 or -1 if the module keeps state in global variables. */
    methods // 登録する関数配列
};
PyMODINIT_FUNC PyInit_CppModule()
{

    return PyModule_Create(&module);
}