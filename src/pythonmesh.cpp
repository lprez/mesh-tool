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

Vertex pyobject_to_vertex(PyObject *pyvertex)
{
    PyObject *pyposition = PyObject_GetAttrString(pyvertex, "position");

    if (!pyposition) {
        throw std::runtime_error("Oggetto Vertex malformato (position mancante)");
    }

    Vertex vertex(pytuple_to_vector<3>(pyposition));

    Py_DECREF(pyposition);

    return vertex;
}

PyObject *vertex_to_pyobject(PyObject *apimodule, const Vertex& vertex)
{
    return PyObject_CallMethod(
                        apimodule,
                        "Vertex",
                        "(O)",
                        vector_to_pytuple<3>(vertex.position)
    );
}

SubVertex pyobject_to_subvertex(PyObject *pysubvertex)
{
    PyObject *pyvertexid = PyObject_GetAttrString(pysubvertex, "vertex_id"),
             *pynormal = PyObject_GetAttrString(pysubvertex, "normal"),
             *pyuv = PyObject_GetAttrString(pysubvertex, "uv");

    if (!pyvertexid || !pynormal || !pyuv) {
        throw std::runtime_error("Oggetto SubVertex malformato");
    }

    SubVertex subvertex(
                (VertexID) PyNumber_AsSsize_t(pyvertexid, nullptr),
                pytuple_to_vector<3>(pynormal),
                pytuple_to_vector<2>(pyuv));

    Py_DECREF(pyvertexid);
    Py_DECREF(pynormal);
    Py_DECREF(pyuv);

    return subvertex;
}

PyObject *subvertex_to_pyobject(PyObject *apimodule, const SubVertex& subvertex)
{
    return PyObject_CallMethod(
                apimodule,
                "SubVertex",
                "IOO",
                subvertex.vertex_id,
                vector_to_pytuple<3>(subvertex.normal),
                vector_to_pytuple<2>(subvertex.uv)
    );
}

Mesh pyobject_to_mesh(PyObject *pymesh)
{
    std::map<VertexID, Vertex> vertices;
    std::map<FaceID, Face> faces;

    PyObject *pyvertex_map = PyObject_GetAttrString(pymesh, "vertices"),
             *pyface_map = PyObject_GetAttrString(pymesh, "faces"),
             *pysmooth = PyObject_GetAttrString(pymesh, "smooth"),
             *pydirtynormals = PyObject_GetAttrString(pymesh, "dirty_normals");

    bool smooth = PyObject_IsTrue(pysmooth) == 1,
         recalculate_normals = PyObject_IsTrue(pydirtynormals) == 1;

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
                 *pyvertex = PyTuple_GetItem(tuple, 1);

        vertices.insert(std::make_pair((VertexID) PyNumber_AsSsize_t(pyvertexid, nullptr),
                                       pyobject_to_vertex(pyvertex)));

        Py_DECREF(tuple);
    }

    for (Py_ssize_t i = 0; i < PySequence_Size(pyfaces); i++) {
        PyObject *tuple = PySequence_GetItem(pyfaces, i);

        if (!tuple) {
            throw std::runtime_error("Oggetto Mesh malformato (faces non è un dictionary)");
        }

        PyObject *pyfaceid = PyTuple_GetItem(tuple, 0),
                 *pyface = PyTuple_GetItem(tuple, 1),
                 *pysv1 = PyObject_GetAttrString(pyface, "sv1"),
                 *pysv2 = PyObject_GetAttrString(pyface, "sv2"),
                 *pysv3 = PyObject_GetAttrString(pyface, "sv3");

        if (!pysv1 || !pysv2 || !pysv3) {
            throw std::runtime_error("Oggetto Face malformato (sv1, sv2 o sv3 mancante)");
        }

        faces.insert(std::make_pair((FaceID) PyNumber_AsSsize_t(pyfaceid, nullptr),
                                    Face(pyobject_to_subvertex(pysv1),
                                         pyobject_to_subvertex(pysv2),
                                         pyobject_to_subvertex(pysv3))));

        Py_DECREF(tuple);
    }

    Py_DECREF(pyvertices);
    Py_DECREF(pyfaces);
    Py_DECREF(pyvertex_map);
    Py_DECREF(pyface_map);
    Py_DECREF(pysmooth);
    Py_DECREF(pydirtynormals);

    return Mesh(vertices, faces, smooth, recalculate_normals);
}


PyObject *mesh_to_pyobject(const Mesh &mesh)
{
    const std::map<VertexID, Vertex>& vertices = mesh.vertex_map();
    const std::map<FaceID, Face>& faces = mesh.face_map();

    PyObject *modules = PyImport_GetModuleDict(),
             *apimodule = PyMapping_GetItemString(modules, "api"),
             *pysmooth = PyBool_FromLong(mesh.is_smooth() ? 1 : 0),
             *pydirtynormals = PyBool_FromLong(0),
             *pymesh = PyObject_CallMethod(apimodule, "Mesh", "{}{}OO", pysmooth, pydirtynormals);

    if (!pymesh) {
        throw std::runtime_error("Impossibile creare un oggetto Mesh");
    }


    PyObject *pyvertices = PyObject_GetAttrString(pymesh, "vertices"),
             *pyfaces = PyObject_GetAttrString(pymesh, "faces");

    for (auto it = vertices.begin(); it != vertices.end(); it++) {
        PyObject *pyvertex = vertex_to_pyobject(apimodule, it->second);

        PyObject_SetItem(pyvertices, PyLong_FromUnsignedLong(it->first), pyvertex);
        Py_DECREF(pyvertex);
    }

    for (auto it = faces.begin(); it != faces.end(); it++) {
        PyObject *pysubvertices[3] = {
            subvertex_to_pyobject(apimodule, it->second.sv1),
            subvertex_to_pyobject(apimodule, it->second.sv2),
            subvertex_to_pyobject(apimodule, it->second.sv3)
        };

        PyObject *pyface = PyObject_CallMethod(
                    apimodule,
                    "Face",
                    "OOO",
                    pysubvertices[0],
                    pysubvertices[1],
                    pysubvertices[2]
        );

        PyObject_SetItem(pyfaces, PyLong_FromUnsignedLong(it->first), pyface);
        Py_DECREF(pysubvertices[0]);
        Py_DECREF(pysubvertices[1]);
        Py_DECREF(pysubvertices[2]);
        Py_DECREF(pyface);
    }

    Py_DECREF(apimodule);
    Py_DECREF(pyvertices);
    Py_DECREF(pyfaces);
    Py_DECREF(pysmooth);
    Py_DECREF(pydirtynormals);

    return pymesh;
}
