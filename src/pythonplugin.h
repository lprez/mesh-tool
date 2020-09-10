#ifndef PYTHONPLUGIN_H
#define PYTHONPLUGIN_H

#include <Python.h>
#include <string>

#include "mesh.h"

// Rappresenta il contesto di esecuzione dei plugin Python. Può esservi una sola istanza.
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

class PythonTransformer : public PythonPlugin
{
    friend class PythonPlugin;
public:
    Mesh transform(const Mesh& mesh);
private:
    PythonTransformer(PyObject *plugin) : PythonPlugin(plugin) {}
};


class PythonImporter : public PythonPlugin
{
    friend class PythonPlugin;
public:
    Mesh import_from(const char *fname);
private:
    PythonImporter(PyObject *plugin) : PythonPlugin(plugin) {}
};


class PythonExporter : public PythonPlugin
{
    friend class PythonPlugin;
public:
    void export_to(const Mesh& mesh, const char *fname);
private:
    PythonExporter(PyObject *plugin) : PythonPlugin(plugin) {}
};

#endif // PYTHONPLUGIN_H
