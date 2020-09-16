#ifndef PYTHONPLUGIN_H
#define PYTHONPLUGIN_H

#include <Python.h>
#include <string>

#include "pythonexception.h"
#include "mesh.h"

// Rappresenta il contesto di esecuzione dei plugin Python. Può esservi una sola istanza
class PythonPluginContext
{
public:
    static PythonPluginContext& getContext() {
        static PythonPluginContext instance;
        return instance;
    }

    PythonPluginContext(PythonPluginContext const&) = delete;
    void operator=(PythonPluginContext const&) = delete;

private:
    PythonPluginContext();
    ~PythonPluginContext();
};

class PythonPlugin
{
public:
    // Carica un file .py. Se ritorna null, vuol dire che il codice non contiene un plugin
    // (es. modulo di utilità usato da altri plugin)
    static PythonPlugin *load(const char *fname);
    virtual ~PythonPlugin();
    std::wstring name();
    std::wstring menu();
    std::wstring entry();

    PythonPlugin(PythonPlugin&& other) : pluginobject(other.pluginobject) {
        other.pluginobject = nullptr;
    }
    PythonPlugin &operator=(PythonPlugin &&other);

protected:
    PythonPlugin(PyObject *pluginobject) : pluginobject(pluginobject) {}
    std::wstring string_property(const char *property);
    void cleanup();
    PyObject *pluginobject;
};

// Plugin di trasformazione (riceve la mesh attuale e ne ritorna una nuova)
class PythonTransformer : public PythonPlugin
{
    friend class PythonPlugin;
public:
    Mesh transform(const Mesh& mesh);
private:
    PythonTransformer(PyObject *plugin) : PythonPlugin(plugin) {}
};


// Plugin per il caricamento di mesh da un file
class PythonImporter : public PythonPlugin
{
    friend class PythonPlugin;
public:
    Mesh import_from(const char *fname);
private:
    PythonImporter(PyObject *plugin) : PythonPlugin(plugin) {}
};

// Plugin per il salvataggio di mesh su file
class PythonExporter : public PythonPlugin
{
    friend class PythonPlugin;
public:
    void export_to(const Mesh& mesh, const char *fname);
private:
    PythonExporter(PyObject *plugin) : PythonPlugin(plugin) {}
};

// Estende PythonException aggiungendo il nome del plugin (o il nome del file quando
// non è possibile ottenere il nome)
class PythonPluginException : public PythonException
{
public:
    PythonPluginException(const std::wstring name_or_fname, const std::wstring message, PyThreadState *thread_state = nullptr) :
        PythonException(message, thread_state), plugin_name(name_or_fname) {}
    PythonPluginException(const std::wstring name_or_fname, const wchar_t *message, PyThreadState *thread_state = nullptr) :
        PythonException(message, thread_state), plugin_name(name_or_fname) {}
    PythonPluginException(const wchar_t *name_or_fname, const wchar_t *message, PyThreadState *thread_state = nullptr) :
        PythonException(message, thread_state), plugin_name(name_or_fname) {}
    PythonPluginException(const PythonException &e, std::wstring name_or_fname) :
        PythonException(e), plugin_name(name_or_fname) {}
    PythonPluginException(const PythonException &e, const wchar_t *name_or_fname) :
        PythonException(e), plugin_name(name_or_fname) {}

    const std::wstring plugin_name;
};

#endif // PYTHONPLUGIN_H
