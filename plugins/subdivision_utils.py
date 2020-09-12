from api import *

# Calcola la media pesata delle coordinate di una lista di vertici
def vertex_interpolate(vertices, weights=None):
    result = Vertex((0, 0, 0))
    n = len(vertices)

    if n > 0:
        weights = [1 / n] * n if weights is None else weights

        for vertex, weight in zip(vertices, weights):
            result += vertex.scale(weight)

    return result


# Calcola la media pesata degli attributi di una lista di sottovertici
def subvertex_interpolate(vertex_id, subvertices, weights=None):
    result = SubVertex(vertex_id, (0, 0, 0), (0, 0))
    n = len(subvertices)

    if n > 0:
        weights = [1 / n] * n if weights is None else weights

        for subvertex, weight in zip(subvertices, weights):
            result += subvertex.scale(weight)

    return result

# Trova i vertici e le facce adiacenti tra loro
def adjacency(vertices, faces):
    # Mappa che associa ad ogni vertice le facce a cui appartiene
    vertex_faces = {vertex_id : set() for vertex_id in vertices.keys()}

    for face_id, face in faces.items():
       vertex_faces[face.sv1.vertex_id].add(face_id)
       vertex_faces[face.sv2.vertex_id].add(face_id)
       vertex_faces[face.sv3.vertex_id].add(face_id)


    # Crea una mappa che associa ad ogni faccia la lista di facce adiacenti ad ogni suo lato
    adjacent_faces = {face_id:
                      [(vertex_faces[face.sv1.vertex_id] & vertex_faces[face.sv2.vertex_id]),
                       (vertex_faces[face.sv2.vertex_id] & vertex_faces[face.sv3.vertex_id]),
                       (vertex_faces[face.sv3.vertex_id] & vertex_faces[face.sv1.vertex_id])]
                      for face_id, face in faces.items()
                    }

    return vertex_faces, adjacent_faces
