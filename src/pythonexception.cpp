#include "pythonexception.h"
#include <Python.h>

PythonException::PythonException(const std::wstring message, PyThreadState *thread_state)
{
    this->wmessage = message;

    if (thread_state != nullptr) {
        PyEval_RestoreThread(thread_state);

        PyObject *pytype, *pyvalue, *pytraceback;
        PyErr_Fetch(&pytype, &pyvalue, &pytraceback);

        if (pytype != nullptr) {
            PyErr_NormalizeException(&pytype, &pyvalue, &pytraceback);

            if (pyvalue != nullptr) {
                this->wmessage.append(L"\n");

                wchar_t *errstr = PyUnicode_AsWideCharString(PyObject_Repr(pyvalue), NULL);
                this->wmessage.append(std::wstring(errstr));
                PyMem_Free(errstr);

                Py_DECREF(pyvalue);
            }

            if (pytraceback != nullptr) Py_DECREF(pytraceback);

            Py_DECREF(pytype);
        }
    }

    this->message = std::string(wmessage.begin(), wmessage.end());
}
