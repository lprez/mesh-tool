#ifndef PYTHONMESH_H
#define PYTHONMESH_H

#include <Python.h>
#include "mesh.h"

PyObject *mesh_to_pyobject(const Mesh &mesh);
Mesh pyobject_to_mesh(PyObject *pyobject);


#endif // PYTHONMESH_H
