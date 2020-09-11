#include "pythonmesh.h"

#include <map>
#include <stdexcept>

template <unsigned int N> Vector<float, N> pytuple_to_vector(PyObject *pytuple)
{
    float values[N];

    for (unsigned int i = 0; i < N; i++) {
        PyObject *pycomponent = PyTuple_GetItem(pytuple, i);

        if (!pycomponent) {
            throw std::runtime_error("Vettore non valido");
        }

        values[i] = PyFloat_AsDouble(pycomponent);
    }

    return Vector<float, N>(values);
}

template <unsigned int N> PyObject *vector_to_pytuple(Vector<float, N> vec)
{
    PyObject *tuple = PyTuple_New((Py_ssize_t) N);

    for (unsigned int i = 0; i < N; i++) {
        // NB: SetItem non aumenta il reference count del valore, quindi
        // non bisogna usare Py_DECREF sul numero Python appena creato.
        PyTuple_SetItem(tuple, i, PyFloat_FromDouble((float) vec[i]));
    }

    return tuple;
}

Mesh pyobject_to_mesh(PyObject *pymesh)
{
    std::map<VertexID, Vertex> vertices;
    std::map<FaceID, Face> faces;

    PyObject *pyvertex_map = PyObject_GetAttrString(pymesh, "vertices"),
             *pyface_map = PyObject_GetAttrString(pymesh, "faces"),
             *pysmooth = PyObject_GetAttrString(pymesh, "faces");

    bool smooth = PyObject_IsTrue(pysmooth) == 1;

    if (!pyvertex_map || !pyface_map || !pysmooth) {
        throw std::runtime_error("Oggetto Mesh malformato (manca vertices o faces)");
    }

    PyObject *pyvertices = PyMapping_Items(pyvertex_map),
             *pyfaces = PyMapping_Items(pyface_map);

    if (!pyvertices || !pyfaces) {
        throw std::runtime_error("Oggetto Mesh malformato (vertices o faces non è un dictionary)");
    }

    for (Py_ssize_t i = 0; i < PySequence_Size(pyvertices); i++) {
        PyObject *tuple = PySequence_GetItem(pyvertices, i);

        if (!tuple) {
            throw std::runtime_error("Oggetto Mesh malformato (vertices non è un dictionary)");
        }

        PyObject *pyvertexid = PyTuple_GetItem(tuple, 0),
                 *pyvertex = PyTuple_GetItem(tuple, 1),
                 *pyposition = PyObject_GetAttrString(pyvertex, "position"),
                 *pyuv = PyObject_GetAttrString(pyvertex, "uv");

        if (!pyposition || !pyuv) {
            throw std::runtime_error("Oggetto Vertex malformato (position o uv mancante)");
        }

        vertices.insert(std::make_pair((VertexID) PyNumber_AsSsize_t(pyvertexid, nullptr),
                                       Vertex(pytuple_to_vector<3>(pyposition),
                                              pytuple_to_vector<2>(pyuv))));

        Py_DECREF(tuple);
        Py_DECREF(pyposition);
        Py_DECREF(pyuv);
    }

    for (Py_ssize_t i = 0; i < PySequence_Size(pyfaces); i++) {
        PyObject *tuple = PySequence_GetItem(pyfaces, i);

        if (!tuple) {
            throw std::runtime_error("Oggetto Mesh malformato (faces non è un dictionary)");
        }

        PyObject *pyfaceid = PyTuple_GetItem(tuple, 0),
                 *pyface = PyTuple_GetItem(tuple, 1),
                 *pyv1 = PyObject_GetAttrString(pyface, "v1"),
                 *pyv2 = PyObject_GetAttrString(pyface, "v2"),
                 *pyv3 = PyObject_GetAttrString(pyface, "v3");

        if (!pyv1 || !pyv2 || !pyv3) {
            throw std::runtime_error("Oggetto Face malformato (v1, v2 o v3 mancante)");
        }

        faces.insert(std::make_pair((FaceID) PyNumber_AsSsize_t(pyfaceid, nullptr),
                                    Face((VertexID) PyNumber_AsSsize_t(pyv1, nullptr),
                                         (VertexID) PyNumber_AsSsize_t(pyv2, nullptr),
                                         (VertexID) PyNumber_AsSsize_t(pyv3, nullptr))));

        Py_DECREF(tuple);
        Py_DECREF(pyv1);
        Py_DECREF(pyv2);
        Py_DECREF(pyv3);
    }

    Py_DECREF(pyvertices);
    Py_DECREF(pyfaces);
    Py_DECREF(pyvertex_map);
    Py_DECREF(pyface_map);
    Py_DECREF(pysmooth);

    return Mesh(vertices, faces, smooth);
}


PyObject *mesh_to_pyobject(const Mesh &mesh)
{
    const std::map<VertexID, Vertex>& vertices = mesh.vertex_map();
    const std::map<FaceID, Face>& faces = mesh.face_map();

    PyObject *modules = PyImport_GetModuleDict(),
             *apimodule = PyMapping_GetItemString(modules, "api"),
             *pymesh = PyObject_CallMethod(apimodule, "Mesh", nullptr);

    if (!pymesh) {
        throw std::runtime_error("Impossibile creare un oggetto Mesh");
    }

    PyObject *pyvertices = PyObject_GetAttrString(pymesh, "vertices"),
             *pyfaces = PyObject_GetAttrString(pymesh, "faces");

    for (auto it = vertices.begin(); it != vertices.end(); it++) {
        PyObject *pyvertex = PyObject_CallMethod(
                    apimodule,
                    "Vertex",
                    "OO",
                    vector_to_pytuple<3>(it->second.position),
                    vector_to_pytuple<2>(it->second.uv)
        );

        PyObject_SetItem(pyvertices, PyLong_FromUnsignedLong(it->first), pyvertex);
        Py_DECREF(pyvertex);
    }

    for (auto it = faces.begin(); it != faces.end(); it++) {
        PyObject *pyface = PyObject_CallMethod(
                    apimodule,
                    "Face",
                    "III",
                    it->second.v1,
                    it->second.v2,
                    it->second.v3
        );


        PyObject_SetItem(pyfaces, PyLong_FromUnsignedLong(it->first), pyface);
        Py_DECREF(pyface);
    }

    PyObject *pysmooth = PyBool_FromLong(mesh.is_smooth() ? 1 : 0);
    PyObject_SetAttrString(pymesh, "smooth", pysmooth);

    Py_DECREF(apimodule);
    Py_DECREF(pyvertices);
    Py_DECREF(pyfaces);

    return pymesh;
}
