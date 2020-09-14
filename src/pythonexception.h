#ifndef PYTHONPLUGINEXCEPTION_H
#define PYTHONPLUGINEXCEPTION_H

#include <exception>
#include <string>
#include <Python.h>

// Rappresenta un'eccezione verificatasi durante l'esecuzione di codice Python
class PythonException : public std::exception
{
public:
    PythonException(const wchar_t *message, PyThreadState *thread_state = nullptr) :
        PythonException(std::wstring(message), thread_state) {}
    PythonException(const std::wstring message, PyThreadState *thread_state);

    PythonException(const PythonException& e) : PythonException(e.wmessage, nullptr) {}
    PythonException& operator=(const PythonException& e) {
        this->message = e.message;
        return *this;
    }
    const char *what() const noexcept override { return message.c_str(); }
    const wchar_t *what_wide() const noexcept { return wmessage.c_str(); }
protected:
    std::string message;
    std::wstring wmessage;
};

#endif // PYTHONPLUGINEXCEPTION_H
