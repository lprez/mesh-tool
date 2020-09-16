
#include <Python.h>
#include <QFile>
#include <QTextStream>
#include <QString>

#include "pythonplugin.h"
#include "pythonexception.h"
#include "pythonmesh.h"

// Conversione da stringa Python a wstring STL
std::wstring pyunicode_to_wstring(PyObject *pystr)
{
    wchar_t *tmp_str = PyUnicode_AsWideCharString(pystr, NULL);
    std::wstring prop(tmp_str);
    PyMem_Free(tmp_str);
    return prop;
}

PythonPluginContext::PythonPluginContext()
{
    // Carica il sorgente del modulo API
    QFile apifile(":/python/src/api.py");

    if (!apifile.open(QFile::ReadOnly)) {
        throw PythonPluginException(L"(API)", L"Impossibile aprire il file api.py");
    }

    QTextStream stream(&apifile);
    QString src = stream.readAll();

    apifile.close();

    // Inizializza l'interprete Python
    Py_Initialize();

    // Compila il modulo API
    PyObject *apicode = Py_CompileString(src.toLocal8Bit().constData(), "api.py", Py_file_input);

    if (!PyImport_ExecCodeModule("api", apicode)) {
        throw PythonPluginException(L"(API)", L"Impossibile compilare il file api.py", PyEval_SaveThread());
    }

    // Aggiunge la directory plugins al sys.path
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

    // Carica il sorgente del plugin
    std::wstring wfname(fname, fname + strlen(fname));
    QFile pluginfile(fname);

    if (!pluginfile.open(QFile::ReadOnly)) {
        throw PythonPluginException(wfname, std::wstring(L"Impossibile aprire il file ").append(wfname));
    }
    QTextStream stream(&pluginfile);
    QString src = stream.readAll();

    pluginfile.close();

    // Compila il plugin
    PyObject *plugincode = Py_CompileString(src.toLocal8Bit().constData(), fname, Py_file_input);
    PyObject *pluginmodule = PyImport_ExecCodeModule(fname, plugincode);
    PyObject *pluginobject;

    if (pluginmodule) {
        pluginobject = PyObject_GetAttrString(pluginmodule, "plugin");

        if (!pluginobject) {
            PyErr_Clear();
            Py_DECREF(plugincode);
            return nullptr;
        }
    } else {
        throw PythonPluginException(wfname, L"Impossibile compilare il plugin", PyEval_SaveThread());
    }

    Py_DECREF(plugincode);

    // Rileva la sottoclasse del plugin

    PyObject *modules = PyImport_GetModuleDict(),
             *apimodule = PyMapping_GetItemString(modules, "api"),
             *transformer_class = PyObject_GetAttrString(apimodule, "TransformerPlugin"),
             *exporter_class = PyObject_GetAttrString(apimodule, "ExporterPlugin"),
             *importer_class = PyObject_GetAttrString(apimodule, "ImporterPlugin");

    if (!transformer_class || !exporter_class || !importer_class) {
        throw PythonPluginException(wfname, L"Modulo API non valido, mancano le classi Plugin", PyEval_SaveThread());
    }

    PythonPlugin *plugin;

    if (PyObject_IsInstance(pluginobject, transformer_class)) {
        plugin = new PythonTransformer(pluginobject);
    } else if (PyObject_IsInstance(pluginobject, importer_class)) {
        plugin = new PythonImporter(pluginobject);
    } else if (PyObject_IsInstance(pluginobject, exporter_class)) {
        plugin = new PythonExporter(pluginobject);
    } else {
        throw PythonPluginException(wfname, L"plugin non è un'istanza di una classe valida", PyEval_SaveThread());
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

// Converte un attributo del plugin di tipo stringa a una wstring
std::wstring PythonPlugin::string_property(const char *property)
{
    return pyunicode_to_wstring(PyObject_GetAttrString(pluginobject, property));
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

    // Argomenti per la chiamata al plugin (mesh convertita)
    try {
        PyTuple_SetItem(args, 0, mesh_to_pyobject(mesh));
    } catch (PythonException& e) {
        throw PythonPluginException(e, name());
    }

    // Chiamata al plugin
    PyObject *pymesh = PyObject_CallObject(pluginobject, args);
    Py_DECREF(args);

    if (!pymesh) {
        throw PythonPluginException(name(), L"Si è verificato un errore nell'esecuzione del plugin", PyEval_SaveThread());
    }

    // Riconversione della mesh
    try {
        result = pyobject_to_mesh(pymesh);
    } catch (PythonException& e) {
        Py_DECREF(pymesh);
        throw PythonPluginException(e, name());
    }

    Py_DECREF(pymesh);

    return result;
}

Mesh PythonImporter::import_from(const char *fname)
{
    Mesh result;
    PyObject *args = PyTuple_New(1);

    // Argomenti (filename da cui caricare)
    PyTuple_SetItem(args, 0, PyUnicode_FromString(fname));

    PyObject *pymesh = PyObject_CallObject(pluginobject, args);

    Py_DECREF(args);

    if (!pymesh) {
        throw PythonPluginException(name(), L"Si è verificato un errore nell'esecuzione del plugin", PyEval_SaveThread());
    }

    try {
        result = pyobject_to_mesh(pymesh);
    } catch (PythonException& e) {
        Py_DECREF(pymesh);
        throw PythonPluginException(e, name());
    }

    Py_DECREF(pymesh);

    return result;
}


void PythonExporter::export_to(const Mesh &mesh, const char *fname)
{
    Mesh result;
    PyObject *args = PyTuple_New(2);

    PyErr_Clear();

    // Argomenti (mesh convertita e filename)

    try {
        PyTuple_SetItem(args, 0, mesh_to_pyobject(mesh));
    } catch (PythonException& e) {
        Py_DECREF(args);
        throw PythonPluginException(e, name());
    }

    PyTuple_SetItem(args, 1, PyUnicode_FromString(fname));

    PyObject_CallObject(pluginobject, args);

    Py_DECREF(args);

    if (PyErr_Occurred() != nullptr) {
        throw PythonPluginException(name(), L"Si è verificato un errore nell'esecuzione del plugin", PyEval_SaveThread());
    }
}
