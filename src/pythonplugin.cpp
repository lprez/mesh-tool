#include <Python.h>
#include <QFile>
#include <QTextStream>
#include <QString>

#include "pythonplugin.h"
#include "pythonmesh.h"


PythonPluginContext::PythonPluginContext()
{
    QFile apifile(":/python/src/api.py");

    if (!apifile.open(QFile::ReadOnly)) {
        throw std::runtime_error("Impossibile aprire il file api.py");
    }

    QTextStream stream(&apifile);
    QString src = stream.readAll();

    apifile.close();


    Py_Initialize();
    PyObject *apicode = Py_CompileString(src.toLocal8Bit().constData(), "api.py", Py_file_input);

    if (!PyImport_ExecCodeModule("api", apicode)) {
        PyErr_Print();
        throw std::runtime_error("Impossibile compilare il file api.py");
    }

    PyObject *modules = PyImport_GetModuleDict(),
             *sysmodule = PyMapping_GetItemString(modules, "sys"),
             *syspath = PyObject_GetAttrString(sysmodule, "path"),
             *newpath = PyUnicode_FromString("plugins");

    PyList_Insert(syspath, 1, newpath);

    Py_DECREF(syspath);
    Py_DECREF(newpath);
    Py_DECREF(apicode);
}

PythonPluginContext::~PythonPluginContext()
{
    Py_FinalizeEx();
}

PythonPlugin *PythonPlugin::load(const char *fname)
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
    PyObject *pluginmodule = PyImport_ExecCodeModule(fname, plugincode);
    PyObject *pluginobject;

    if (pluginmodule) {
        pluginobject = PyObject_GetAttrString(pluginmodule, "plugin");

        if (!pluginobject) {
            Py_DECREF(plugincode);
            return nullptr;
        }
    } else {
        PyErr_Print();
        throw std::runtime_error("Impossibile compilare il plugin");
    }

    Py_DECREF(plugincode);

    PyObject *modules = PyImport_GetModuleDict(),
             *apimodule = PyMapping_GetItemString(modules, "api"),
             *transformer_class = PyObject_GetAttrString(apimodule, "TransformerPlugin"),
             *exporter_class = PyObject_GetAttrString(apimodule, "ExporterPlugin"),
             *importer_class = PyObject_GetAttrString(apimodule, "ImporterPlugin");

    if (!transformer_class || !exporter_class || !importer_class) {
        throw std::runtime_error("Modulo API non valido, mancano le classi Plugin");
    }

    PythonPlugin *plugin;

    if (PyObject_IsInstance(pluginobject, transformer_class)) {
        plugin = new PythonTransformer(pluginobject);
    } else if (PyObject_IsInstance(pluginobject, importer_class)) {
        plugin = new PythonImporter(pluginobject);
    } else if (PyObject_IsInstance(pluginobject, exporter_class)) {
        plugin = new PythonExporter(pluginobject);
    } else {
        throw std::runtime_error("plugin non è un'istanza di una classe valida");
    }

    return plugin;
}

PythonPlugin &PythonPlugin::operator=(PythonPlugin &&other)
{
    if (this != &other) {
        cleanup();
        pluginobject = other.pluginobject;
        other.pluginobject = nullptr;
    }

    return *this;
}


PythonPlugin::~PythonPlugin()
{
    cleanup();
}

std::wstring PythonPlugin::name()
{
    return string_property("name");
}

std::wstring PythonPlugin::menu()
{
    return string_property("menu");
}

std::wstring PythonPlugin::entry()
{
    return string_property("entry");
}

std::wstring PythonPlugin::string_property(const char *property)
{
    wchar_t *tmp_property = PyUnicode_AsWideCharString(PyObject_GetAttrString(pluginobject, property), NULL);
    std::wstring prop(tmp_property);
    PyMem_Free(tmp_property);
    return prop;
}

void PythonPlugin::cleanup()
{
    if (pluginobject) {
        Py_DECREF(pluginobject);
    }
}


Mesh PythonTransformer::transform(const Mesh &mesh)
{
    Mesh result;
    PyObject *args = PyTuple_New(1);

    PyTuple_SetItem(args, 0, mesh_to_pyobject(mesh));

    PyObject *pymesh = PyObject_CallObject(pluginobject, args);

    if (!pymesh) {
        PyErr_Print();
        throw std::runtime_error("Si è verificato un errore nell'esecuzione del plugin");
    }

    result = pyobject_to_mesh(pymesh);

    Py_DECREF(args);
    Py_DECREF(pymesh);

    return result;
}

Mesh PythonImporter::import_from(const char *fname)
{
    Mesh result;
    PyObject *args = PyTuple_New(1);

    PyTuple_SetItem(args, 0, PyUnicode_FromString(fname));

    PyObject *pymesh = PyObject_CallObject(pluginobject, args);

    if (!pymesh) {
        PyErr_Print();
        throw std::runtime_error("Si è verificato un errore nell'esecuzione del plugin");
    }


    result = pyobject_to_mesh(pymesh);

    Py_DECREF(args);
    Py_DECREF(pymesh);

    return result;
}


void PythonExporter::export_to(const Mesh &mesh, const char *fname)
{
    Mesh result;
    PyObject *args = PyTuple_New(2);

    PyTuple_SetItem(args, 0, mesh_to_pyobject(mesh));
    PyTuple_SetItem(args, 1, PyUnicode_FromString(fname));

    PyObject *pyresult = PyObject_CallObject(pluginobject, args);

    Py_DECREF(args);
}
