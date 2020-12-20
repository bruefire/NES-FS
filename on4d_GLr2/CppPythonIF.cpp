#include "CppPythonIF.h"
#include "functions.h"
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


    if (engine == nullptr)
        return NULL;

    for (int i = 0; i < engine->OBJ_QTY; i++)
    {
        PyObject* objData = PyObject_CallObject(objDataType, nullptr);
        PyObject* setMeth = PyObject_CallMethod(objData, "SetIdx", "i", i);

        PyList_Append(pList, objData);

        Py_XDECREF(objData);
        Py_XDECREF(setMeth);
    }

    //if (!PyArg_ParseTuple(args, ":numargs"))
    //    return NULL;

    return pList;
};


// 相対的な位置設定
PyObject* CppPythonIF::SetLocRelative(PyObject* self, PyObject* args)
{

    if (engine == nullptr)
        return NULL;
    
    PyObject* src;
    PyObject* trg;
    pt3 nLoc;
    double dst;
    if (!PyArg_ParseTuple(args, "OOdddd", &src, &trg, &dst, &nLoc.x, &nLoc.y, &nLoc.z))
        return NULL;

    // 型チェック
    PyObject* objDataType = PyDict_GetItemString(pDict, "ObjData");
    bool typeCheck = 
        PyObject_IsInstance(src, objDataType)
        && PyObject_IsInstance(trg, objDataType);
    if (!typeCheck)
        return NULL;  // エラーにはしない


    PyObject* result = NULL;
    PyObject* selfIdx = PyObject_GetAttrString(src, "idx");
    PyObject* trgIdx  = PyObject_GetAttrString(trg, "idx");

    if (engine->worldGeo == engine3d::WorldGeo::HYPERBOLIC)
    {
        // 位置再設定
        object3d* selfObj = &engine->objs[PyLong_AsLong(selfIdx)];
        object3d* trgObj  = &engine->objs[PyLong_AsLong(trgIdx)];

        if (selfObj->SetLocRelative(trgObj, nLoc, dst))
            result = PyLong_FromLong(0);  // エラーにはしない
    }

    Py_XDECREF(selfIdx);
    Py_XDECREF(trgIdx);
    

    return result;
}


// 相対的な位置設定
PyObject* CppPythonIF::SetRotRelative(PyObject* self, PyObject* args)
{

    if (engine == nullptr)
        return NULL;

    PyObject* src;
    pt3 nLoc;
    if (!PyArg_ParseTuple(args, "Oddd", &src, &nLoc.x, &nLoc.y, &nLoc.z))
        return NULL;

    // 型チェック
    PyObject* objDataType = PyDict_GetItemString(pDict, "ObjData");
    if (!PyObject_IsInstance(src, objDataType))
        return PyLong_FromLong(1);  // エラーにはしない


    PyObject* result = NULL;
    PyObject* selfIdx = PyObject_GetAttrString(src, "idx");

    if (engine->worldGeo == engine3d::WorldGeo::HYPERBOLIC)
    {
        // 位置再設定
        object3d* selfObj = &engine->objs[PyLong_AsLong(selfIdx)];

        if (selfObj->SetRotRelative(nLoc))
            result = PyLong_FromLong(0);  // エラーにはしない
    }

    Py_XDECREF(selfIdx);


    return result;
}


// 相対的な位置設定
PyObject* CppPythonIF::GetPlayerObj(PyObject* self, PyObject* args)
{
    if (engine == nullptr)
        return NULL;

    PyObject* objDataType = PyDict_GetItemString(pDict, "ObjData");
    PyObject* objData = PyObject_CallObject(objDataType, nullptr);
    PyObject* setMeth = PyObject_CallMethod(objData, "SetIdx", "i", engine->PLR_No);

    Py_XDECREF(setMeth);

    return objData;
}


// オブジェクトスケール変更
PyObject* CppPythonIF::SetScale(PyObject* self, PyObject* args)
{
    if (engine == nullptr)
        return NULL;

    PyObject* src;
    double scale;
    if (!PyArg_ParseTuple(args, "Od", &src, &scale))
        return NULL;

    // 型チェック
    PyObject* objDataType = PyDict_GetItemString(pDict, "ObjData");
    if (!PyObject_IsInstance(src, objDataType))
        return NULL;  // エラーにはしない

    PyObject* result = NULL;
    PyObject* selfIdx = PyObject_GetAttrString(src, "idx");
    if (engine->worldGeo == engine3d::WorldGeo::HYPERBOLIC)
    {
        // スケール再設定
        object3d* selfObj = &engine->objs[PyLong_AsLong(selfIdx)];

        if (selfObj->SetScale(scale))
            result = PyLong_FromLong(0);
    }

    Py_XDECREF(selfIdx);
    return result;
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
    {
        "SetLocRelative",
        CppPythonIF::SetLocRelative,
        METH_VARARGS,
        "explanation."
    },
    {
        "SetRotRelative",
        CppPythonIF::SetRotRelative,
        METH_VARARGS,
        "explanation."
    },
    {
        "GetPlayerObj",
        CppPythonIF::GetPlayerObj,
        METH_VARARGS,
        "explanation."
    },
    {
        "SetScale",
        CppPythonIF::SetScale,
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