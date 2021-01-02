#include "CppPythonIF.h"
#include "functions.h"
using namespace std;

engine3d* CppPythonIF::engine = nullptr;
char CppPythonIF::rawCode[8186] = "";
PyObject* CppPythonIF::pModule;
PyObject* CppPythonIF::pDict;



// �I�u�W�F�N�g�f�[�^�擾
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


// ���ΓI�Ȉʒu�ݒ�
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

    // �^�`�F�b�N
    PyObject* objDataType = PyDict_GetItemString(pDict, "ObjData");
    bool typeCheck = 
        PyObject_IsInstance(src, objDataType)
        && PyObject_IsInstance(trg, objDataType);
    if (!typeCheck)
        return NULL;  // �G���[�ɂ͂��Ȃ�


    PyObject* result = NULL;
    PyObject* selfIdx = PyObject_GetAttrString(src, "idx");
    PyObject* trgIdx  = PyObject_GetAttrString(trg, "idx");

    // �ʒu�Đݒ�
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


// ���ΓI�Ȉʒu�ݒ�
PyObject* CppPythonIF::SetRotRelative(PyObject* self, PyObject* args)
{

    if (engine == nullptr)
        return NULL;

    PyObject* src;
    pt3 nLoc;
    if (!PyArg_ParseTuple(args, "Oddd", &src, &nLoc.x, &nLoc.y, &nLoc.z))
        return NULL;

    // �^�`�F�b�N
    PyObject* objDataType = PyDict_GetItemString(pDict, "ObjData");
    if (!PyObject_IsInstance(src, objDataType))
        return PyLong_FromLong(1);  


    PyObject* result = NULL;
    PyObject* selfIdx = PyObject_GetAttrString(src, "idx");

    // �ʒu�Đݒ�
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


// ���ΓI�Ȉʒu�ݒ�
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


// set one double parameter
PyObject* CppPythonIF::Func_NoParam(PyObject* self, PyObject* args)
{
    if (engine == nullptr)
        return NULL;

    PyObject* src;
    int funcID;
    if (!PyArg_ParseTuple(args, "Oi", &src, &funcID))
        return NULL;

    // �^�`�F�b�N
    PyObject* objDataType = PyDict_GetItemString(pDict, "ObjData");
    if (!PyObject_IsInstance(src, objDataType))
        return NULL;

    PyObject* result = NULL;
    PyObject* selfIdx = PyObject_GetAttrString(src, "idx");
    object3d* selfObj = &engine->objs[PyLong_AsLong(selfIdx)];

    switch ((FuncObject)funcID)
    {
    case FuncObject::GetScale:
        // �X�P�[���Đݒ�
        result = PyFloat_FromDouble(selfObj->GetScale());
        break;
    case FuncObject::GetVelocity:
        // �X�P�[���Đݒ�
        result = PyFloat_FromDouble(selfObj->GetVelocity());
        break;
    }


    Py_XDECREF(selfIdx);
    return result;
}


// set one double parameter
PyObject* CppPythonIF::Func_Pram1d(PyObject* self, PyObject* args)
{
    if (engine == nullptr)
        return NULL;

    PyObject* src;
    int funcID;
    double value;
    if (!PyArg_ParseTuple(args, "Oid", &src, &funcID, &value))
        return NULL;

    // �^�`�F�b�N
    PyObject* objDataType = PyDict_GetItemString(pDict, "ObjData");
    if (!PyObject_IsInstance(src, objDataType))
        return NULL;  

    PyObject* result = NULL;
    PyObject* selfIdx = PyObject_GetAttrString(src, "idx");
    object3d* selfObj = &engine->objs[PyLong_AsLong(selfIdx)];

    switch ((FuncObject)funcID)
    {
    case FuncObject::SetScale:
        // �X�P�[���Đݒ�
        if (selfObj->SetScale(value))
            result = PyLong_FromLong(0);
        break;

    case FuncObject::SetVelocity:
        // ���x�Đݒ�
        if (selfObj->SetVelocity(value))
            result = PyLong_FromLong(0);
        break;
    }


    Py_XDECREF(selfIdx);
    return result;
}



//---------���N���X������ƐF�X�ʓ|�Ȃ̂ŃO���[�o��------------
PyMethodDef methods[] =
{
    // �L�q��F
    //{
    //    "test",           // Python��ł̊֐���
    //    CppPythonIF::test,  // �o�^����֐��̃|�C���^
    //    METH_VARARGS,
    //    "explanation."      // �֐��̐���
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
        "Func_NoParam",
        CppPythonIF::Func_NoParam,
        METH_VARARGS,
        "explanation."
    },
    {
        "Func_Pram1d",
        CppPythonIF::Func_Pram1d,
        METH_VARARGS,
        "explanation."
    },
    {NULL, NULL, 0, NULL}   /* �I�[����p�H */
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