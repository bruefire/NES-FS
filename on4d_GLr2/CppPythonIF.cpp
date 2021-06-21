#include "CppPythonIF.h"
#include "functions.h"
using namespace std;

engine3d* CppPythonIF::engine = nullptr;
unique_ptr<char> CppPythonIF::rawCode(nullptr);
PyObject* CppPythonIF::pModule;
PyObject* CppPythonIF::pDict;
CppPythonIF::FuncObject CppPythonIF::funcID;



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
PyObject* CppPythonIF::SetLoc(PyObject* self, PyObject* args)
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
        return NULL;  // エラーにはしない


    PyObject* result = NULL;
    PyObject* selfIdx = PyObject_GetAttrString(src, "idx");

    // 位置再設定
    object3d* selfObj = &engine->objs[PyLong_AsLong(selfIdx)];

    if (engine->worldGeo == engine3d::WorldGeo::HYPERBOLIC)
    {
        if (selfObj->SetLocH3(nLoc))
            result = PyLong_FromLong(0);
    }
    else if (engine->worldGeo == engine3d::WorldGeo::SPHERICAL)
    {
        if (selfObj->SetLocS3(nLoc))
            result = PyLong_FromLong(0);
    }

    Py_XDECREF(selfIdx);


    return result;
}


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

    // 位置再設定
    object3d* selfObj = &engine->objs[PyLong_AsLong(selfIdx)];
    object3d* trgObj = &engine->objs[PyLong_AsLong(trgIdx)];

    if (engine->worldGeo == engine3d::WorldGeo::HYPERBOLIC)
    {
        if (selfObj->SetLocRelativeH3(trgObj, nLoc, dst))
            result = PyLong_FromLong(0);
    }
    else if(engine->worldGeo == engine3d::WorldGeo::SPHERICAL)
    {
        if (selfObj->SetLocRelativeS3(trgObj, nLoc, dst))
            result = PyLong_FromLong(0);
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
        return PyLong_FromLong(1);  


    PyObject* result = NULL;
    PyObject* selfIdx = PyObject_GetAttrString(src, "idx");

    // 位置再設定
    object3d* selfObj = &engine->objs[PyLong_AsLong(selfIdx)];

    if (engine->worldGeo == engine3d::WorldGeo::HYPERBOLIC)
    {
        if (selfObj->SetRotRelative(nLoc))
            result = PyLong_FromLong(0);  
    }
    else if (engine->worldGeo == engine3d::WorldGeo::SPHERICAL)
    {
        if (selfObj->SetRotRelativeS3(nLoc))
            result = PyLong_FromLong(0);  
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


// set common func name
PyObject* CppPythonIF::SetCommonFunc(PyObject* self, PyObject* args)
{
    if (engine == nullptr)
        return NULL;

    int funcID;
    if (!PyArg_ParseTuple(args, "i", &funcID))
        return NULL;

    CppPythonIF::funcID = (CppPythonIF::FuncObject)funcID;

    return PyLong_FromLong(1);
}


// exec common process called from python
PyObject* CppPythonIF::ExecCommonFunc(PyObject* self, PyObject* args)
{
    if (engine == nullptr)
        return NULL;


    // get/set objobj
    auto GetObjObj = [](PyObject** src, PyObject** selfIdx)
    {
        PyObject* objDataType = PyDict_GetItemString(pDict, "ObjData");
        if (!PyObject_IsInstance(*src, objDataType))
            return false;
        *selfIdx = PyObject_GetAttrString(*src, "idx");
        return true;
    };
    auto ReleaseObjObj = [](PyObject* selfIdx)
    {
        Py_XDECREF(selfIdx);
    };


    PyObject* result = NULL;

    // exec process called from python
    switch (CppPythonIF::funcID)
    {
    case FuncObject::GetObjScale:
    {
        PyObject* src,* selfIdx;
        if (!PyArg_ParseTuple(args, "O", &src) || !GetObjObj(&src, &selfIdx))
            return NULL;

        object3d* selfObj = &engine->objs[PyLong_AsLong(selfIdx)];
        result = PyFloat_FromDouble(selfObj->GetScale());

        ReleaseObjObj(selfIdx);
        break;
    }
    case FuncObject::GetObjVelocity:
    {
        PyObject* src, * selfIdx;
        if (!PyArg_ParseTuple(args, "O", &src) || !GetObjObj(&src, &selfIdx))
            return NULL;

        object3d* selfObj = &engine->objs[PyLong_AsLong(selfIdx)];
        result = PyFloat_FromDouble(selfObj->GetVelocity());

        ReleaseObjObj(selfIdx);
        break;
    }
    case FuncObject::SetObjScale:
    {
        PyObject* src, * selfIdx;
        double value;
        if (!PyArg_ParseTuple(args, "Od", &src, &value) || !GetObjObj(&src, &selfIdx))
            return NULL;

        object3d* selfObj = &engine->objs[PyLong_AsLong(selfIdx)];
        if (selfObj->SetScale(value))
            result = PyLong_FromLong(1);
        else
            result = PyLong_FromLong(0);

        ReleaseObjObj(selfIdx);
        break;
    }
    case FuncObject::SetObjVelocity:
    {
        PyObject* src, * selfIdx;
        double value;
        if (!PyArg_ParseTuple(args, "Od", &src, &value) || !GetObjObj(&src, &selfIdx))
            return NULL;

        object3d* selfObj = &engine->objs[PyLong_AsLong(selfIdx)];
        if (selfObj->SetVelocity(value))
            result = PyLong_FromLong(1);
        else
            result = PyLong_FromLong(0);

        ReleaseObjObj(selfIdx);
        break;
    }
    case FuncObject::GetObjRotVelocity:
    {
        PyObject* src, * selfIdx;
        if (!PyArg_ParseTuple(args, "O", &src) || !GetObjObj(&src, &selfIdx))
            return NULL;

        object3d* selfObj = &engine->objs[PyLong_AsLong(selfIdx)];
        PyObject* Dvec3 = PyDict_GetItemString(pDict, "Dvec3");
        PyObject* dVec3 = PyObject_CallObject(Dvec3, nullptr);

        pt3 pts = selfObj->GetRsp();
        
        PyObject* px, *py, *pz, *pt;
        px = PyObject_CallMethod(dVec3, "SetX", "d", pts.x);
        py = PyObject_CallMethod(dVec3, "SetY", "d", pts.y);
        pz = PyObject_CallMethod(dVec3, "SetZ", "d", pts.z);
        PyObject_SetAttrString(dVec3, "owIdx", selfIdx);
        PyObject_SetAttrString(dVec3, "type", pt = PyLong_FromLong((int)FuncObject::GetObjRotVelocity));
        Py_XDECREF(px);
        Py_XDECREF(py);
        Py_XDECREF(pz);
        Py_XDECREF(pt);
        ReleaseObjObj(selfIdx);

        result = dVec3;
        break;
    }
    case FuncObject::SetObjRotVelocity:
    {
        PyObject* src, * selfIdx, * value;
        if (!PyArg_ParseTuple(args, "OO", &src, &value) || !GetObjObj(&src, &selfIdx))
            return NULL;

        object3d* selfObj = &engine->objs[PyLong_AsLong(selfIdx)];
        PyObject* px = PyObject_GetAttrString(value, "x");
        PyObject* py = PyObject_GetAttrString(value, "y");
        PyObject* pz = PyObject_GetAttrString(value, "z");
        pt3 pts = pt3(
            PyFloat_AsDouble(px),
            PyFloat_AsDouble(py),
            PyFloat_AsDouble(pz));

        if (selfObj->SetRsp(pts))
            result = PyLong_FromLong(1);
        else
            result = PyLong_FromLong(0);

        Py_XDECREF(px);
        Py_XDECREF(py);
        Py_XDECREF(pz);
        ReleaseObjObj(selfIdx);
        break;
    }
    case FuncObject::SetPt3x:
    case FuncObject::SetPt3y:
    case FuncObject::SetPt3z:
    {
        int srcIdx, type;
        double value;
        if (!PyArg_ParseTuple(args, "iid", &srcIdx, &type, &value))
            return NULL;

        object3d* selfObj = &engine->objs[srcIdx];
        switch ((FuncObject)type)
        {
        case FuncObject::GetObjRotVelocity:
            if (CppPythonIF::funcID == FuncObject::SetPt3x)
                selfObj->rsp.x = value;
            else if (CppPythonIF::funcID == FuncObject::SetPt3y)
                selfObj->rsp.y = value;
            else if (CppPythonIF::funcID == FuncObject::SetPt3z)
                selfObj->rsp.z = value;
            break;
        }
        result = PyLong_FromLong(1);

        break;
    }
    case FuncObject::SetWorldScale:
    {
        PyObject* src, *selfIdx;
        double value;
        if (!PyArg_ParseTuple(args, "Od", &src, &value))
            return NULL;

        engine->SetRadius(value);
        result = PyLong_FromLong(1);
        break;
    }
    case FuncObject::GetWorldScale:
    {
        PyObject* src;
        if (!PyArg_ParseTuple(args, "O", &src))
            return NULL;

        result = PyFloat_FromDouble(engine->GetRadius());
        break;
    }
    case FuncObject::TrackObjDirection:
    {
        PyObject* src, *trg, *selfIdx, *trgIdx;
        if (!PyArg_ParseTuple(args, "OO", &src, &trg) || !GetObjObj(&src, &selfIdx) || !GetObjObj(&trg, &trgIdx))
            return NULL;

        object3d* selfObj = &engine->objs[PyLong_AsLong(selfIdx)];
        object3d* trgObj = &engine->objs[PyLong_AsLong(trgIdx)];

        selfObj->TrackObjDirection(trgObj);
        result = PyLong_FromLong(1);

        ReleaseObjObj(selfIdx);
        ReleaseObjObj(trgIdx);
        break;
    }
    case FuncObject::TrackObjDirectionHead:
    {
        PyObject* src, * trg, * selfIdx, * trgIdx;
        if (!PyArg_ParseTuple(args, "OO", &src, &trg) || !GetObjObj(&src, &selfIdx) || !GetObjObj(&trg, &trgIdx))
            return NULL;

        object3d* selfObj = &engine->objs[PyLong_AsLong(selfIdx)];
        object3d* trgObj = &engine->objs[PyLong_AsLong(trgIdx)];

        selfObj->TrackObjDirectionHead(trgObj);
        result = PyLong_FromLong(1);

        ReleaseObjObj(selfIdx);
        ReleaseObjObj(trgIdx);
        break;
    }
    case FuncObject::GetDistance:
    {
        PyObject* src, * trg, * selfIdx, * trgIdx;
        if (!PyArg_ParseTuple(args, "OO", &src, &trg) || !GetObjObj(&src, &selfIdx) || !GetObjObj(&trg, &trgIdx))
            return NULL;

        object3d* selfObj = &engine->objs[PyLong_AsLong(selfIdx)];
        object3d* trgObj = &engine->objs[PyLong_AsLong(trgIdx)];

        result = PyFloat_FromDouble(selfObj->GetDistance(trgObj));

        ReleaseObjObj(selfIdx);
        ReleaseObjObj(trgIdx);

        break;
    }
    }
    CppPythonIF::funcID = FuncObject::None;

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
        "SetLoc",
        CppPythonIF::SetLoc,
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
        "SetCommonFunc",
        CppPythonIF::SetCommonFunc,
        METH_VARARGS,
        "explanation."
    },
    {
        "ExecCommonFunc",
        CppPythonIF::ExecCommonFunc,
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