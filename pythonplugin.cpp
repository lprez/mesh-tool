#include <Python.h>
#include <QFile>
#include <QTextStream>
#include <QString>

#include "pythonplugin.h"
#include "pythonmesh.h"


PythonPluginContext::PythonPluginContext()
{
    QFile apifile(":/python/api.py");

    if (!apifile.open(QFile::ReadOnly)) {
        throw std::runtime_error("Impossibile aprire il file api.py");
    }

    QTextStream stream(&apifile);
    QString src = stream.readAll();

    apifile.close();


    Py_Initialize();
    PyObject *apicode = Py_CompileString(src.toLocal8Bit().constData(), "api.py", Py_file_input);

    if (!PyImport_ExecCodeModule("api", apicode)) {
        throw std::runtime_error("Impossibile compilare il file api.py");
        PyErr_Print();
    }

    Py_DECREF(apicode);
}

PythonPluginContext::~PythonPluginContext()
{
    Py_FinalizeEx();
}


PythonPlugin::PythonPlugin(const char *fname)
{
    PythonPluginContext::getContext(); // Inizializza l'interprete Python e il modulo
                                       // API se non è stato già fatto.


    QFile pluginfile(fname);

    if (!pluginfile.open(QFile::ReadOnly)) {
        throw std::runtime_error(std::string("Impossibile aprire il file ").append(std::string(fname)));
    }
    QTextStream stream(&pluginfile);
    QString src = stream.readAll();

    pluginfile.close();


    PyObject *plugincode = Py_CompileString(src.toLocal8Bit().constData(), fname, Py_file_input);
    PyObject *pluginmodule = PyImport_ExecCodeModule("plugin", plugincode);

    if (pluginmodule) {
        pluginobject = PyObject_GetAttrString(pluginmodule, "plugin");

        if (!pluginobject) {
            throw std::runtime_error("Nessun oggetto plugin");
            PyErr_Print();
        }
    } else {
        throw std::runtime_error("Impossibile compilare il plugin");
        PyErr_Print();
    }

    Py_DECREF(plugincode);
}

PythonPlugin::~PythonPlugin()
{
    Py_DECREF(pluginobject);
}

std::wstring PythonPlugin::name()
{
    wchar_t *tmp_name = PyUnicode_AsWideCharString(PyObject_GetAttrString(pluginobject, "name"), NULL);
    std::wstring name(tmp_name);
    PyMem_Free(tmp_name);
    return name;
}

Mesh PythonPlugin::run(const Mesh &mesh)
{
    Mesh result;
    PyObject *args = PyTuple_New(1);

    PyTuple_SetItem(args, 0, mesh_to_pyobject(mesh));

    PyObject *pymesh = PyObject_CallObject(pluginobject, args);

    result = pyobject_to_mesh(pymesh);

    Py_DECREF(args);
    Py_DECREF(pymesh);

    return result;
}
