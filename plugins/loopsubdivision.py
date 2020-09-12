from api import *
from subdivision_utils import *


class LoopSubdivisionPlugin(TransformerPlugin):
    def __call__(self, mesh):
        faces = mesh.faces
        vertices = mesh.vertices

        # Indice da usare per il prossimo vertice da aggiungere
        next_vertex_id = max(vertices.keys()) + 1

        # Crea le mappe di adiacenza tra vertici e facce
        vertex_faces, adjacent_faces = adjacency(vertices, faces)

        # Insieme dei vertici connessi ad ogni vertice originale
        vertex_connected = {vertex_id : set() for vertex_id in mesh.vertices.keys()}
        vertex_boundary_connected = {vertex_id : set() for vertex_id in mesh.vertices.keys()}

        # Nuovi vertici
        edge_vertices = {}
        edge_subvertices = {}

        for face_id, face in faces.items():
            # Itera per ogni lato della faccia, associandovi la lista di facce adiacenti a quel lato
            for endpoints, adj in zip( [[face.sv1, face.sv2],
                                        [face.sv2, face.sv3],
                                        [face.sv3, face.sv1]],
                                       adjacent_faces[face_id] ):
                edge_id = tuple(sorted([endpoints[0].vertex_id, endpoints[1].vertex_id]))

                if edge_id in edge_subvertices:
                    continue

                if len(adj) == 2:
                    opposite_subvertices = []

                    # Trova i punti opposti al lato
                    for adj_id in adj:
                        adjacent_face = faces[adj_id]
                        for subvertex in [adjacent_face.sv1, adjacent_face.sv2, adjacent_face.sv3]:
                            if (subvertex.vertex_id != endpoints[0].vertex_id and
                                subvertex.vertex_id != endpoints[1].vertex_id):
                                opposite_subvertices.append(subvertex)

                    # Calcola il nuovo punto
                    edge_vertices[edge_id] = vertex_interpolate([vertices[sv.vertex_id] for sv in endpoints + opposite_subvertices],
                                                                [3/8, 3/8, 1/8, 1/8])
                    edge_subvertices[edge_id] = subvertex_interpolate(next_vertex_id, endpoints + opposite_subvertices,
                                                                      [3/8, 3/8, 1/8, 1/8])
                else:
                    # Calcola il nuovo punto (al bordo)
                    edge_vertices[edge_id] = vertex_interpolate([vertices[sv.vertex_id] for sv in endpoints])
                    edge_subvertices[edge_id] = subvertex_interpolate(next_vertex_id, endpoints)

                next_vertex_id += 1

                # Associa al vertice i vertici connessi
                vertex_connected[endpoints[0].vertex_id].add(endpoints[1].vertex_id)
                vertex_connected[endpoints[1].vertex_id].add(endpoints[0].vertex_id)

                if len(adj) < 2:
                    vertex_boundary_connected[endpoints[0].vertex_id].add(endpoints[1].vertex_id)
                    vertex_boundary_connected[endpoints[1].vertex_id].add(endpoints[0].vertex_id)


        # Interpola i vertici originali
        moved_vertices = {}

        for vertex_id, original_vertex in vertices.items():
            connected = vertex_connected[vertex_id]
            n = len(connected)

            if n != len(vertex_faces[vertex_id]): # Se il vertice si trova ai bordi
                connected = vertex_boundary_connected[vertex_id]
                n = len(connected)
                weights = [3 / 4] + [1 / (4 * n)] * n if n > 0 else [1]
            else:
                if n == 0:
                    weights = [1]
                elif n == 3:
                    beta = 3 / 16
                    weights = [1 - n * beta] + [beta] * n
                else:
                    beta = 3 / (8 * n)
                    weights = [1 - n * beta] + [beta] * n

            moved_vertices[vertex_id] = vertex_interpolate([original_vertex] + [vertices[v] for v in connected], weights)

        # Crea la mappa con tutti i nuovi vertici
        new_vertices = moved_vertices
        new_vertices.update({edge_subvertices[edge_id].vertex_id : edge_vertices[edge_id] for edge_id in edge_vertices.keys()})


        # Crea le nuove facce
        new_faces = []

        for face_id, face in faces.items():
            new_subvertices = [edge_subvertices[tuple(sorted([begin.vertex_id, end.vertex_id]))]
                               for begin, end in [[face.sv1, face.sv2], [face.sv2, face.sv3], [face.sv3, face.sv1]]]
            original_face = faces[face_id]
            new_faces.append(Face(original_face.sv1, new_subvertices[0], new_subvertices[2]))
            new_faces.append(Face(original_face.sv2, new_subvertices[1], new_subvertices[0]))
            new_faces.append(Face(original_face.sv3, new_subvertices[2], new_subvertices[1]))
            new_faces.append(Face(new_subvertices[0], new_subvertices[1], new_subvertices[2]))

        return Mesh(new_vertices, dict(enumerate(new_faces)), mesh.smooth, True)

plugin = LoopSubdivisionPlugin("Suddivisione di Loop", "Modifica", "Suddivisione di Loop")
