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
    PythonPlugin(const char *fname);
    ~PythonPlugin();
    std::wstring name();
    Mesh run(const Mesh& mesh);

    PythonPlugin(PythonPlugin const&) = delete;
    void operator=(PythonPlugin const&) = delete;

private:
    PyObject *pluginobject;
};

#endif // PYTHONPLUGIN_H
