from api import *

# Calcola la media delle coordinate di una lista di vertici
def vertex_average(vertices):
    result = Vertex((0, 0, 0), (0, 0))

    for vertex in vertices:
        result += vertex

    count = len(vertices)
    if count > 0:
        result = result.scale(1 / count)

    return result


class CatmullClarkPlugin(TransformerPlugin):
    def __call__(self, mesh):
        faces = mesh.faces
        vertices = mesh.vertices

        # Indice da usare per il prossimo vertice da aggiungere
        next_vertex_id = max(vertices.keys()) + 1

        # Crea una mappa che associa ad ogni vertice l'insieme delle facce di cui fa parte
        vertex_faces = {vertex_id : set() for vertex_id in vertices.keys()}

        for face_id, face in faces.items():
            vertex_faces[face.v1].add(face_id)
            vertex_faces[face.v2].add(face_id)
            vertex_faces[face.v3].add(face_id)


        # Crea una mappa che associa ad ogni faccia la lista di facce adiacenti ad ogni suo lato
        adjacent_faces = { face_id:
                           [ (vertex_faces[face.v1] & vertex_faces[face.v2]),
                             (vertex_faces[face.v2] & vertex_faces[face.v3]),
                             (vertex_faces[face.v3] & vertex_faces[face.v1])
                           ]
                           for face_id, face in faces.items()
                         }

        # Calcola i punti medi delle facce
        face_points = { face_id : vertex_average([vertices[face.v1], vertices[face.v2], vertices[face.v3]])
                        for face_id, face in faces.items()
                      }
        # I punti medi faranno parte dei vertici della nuova mesh, dunque bisogna assegnare loro un nuovo ID
        face_point_ids = {face_id : next_vertex_id + face_id for face_id, face in faces.items()}
        new_vertex_id = max(face_point_ids.values()) + 1

        # Calcola i lati a cui appartiene ogni vertice e i nuovi punti medi dei nuovi lati
        vertex_edges = {vertex_id : set() for vertex_id in mesh.vertices.keys()}
        vertex_hole_edges = {vertex_id : set() for vertex_id in mesh.vertices.keys()}
        edge_points = {}
        edge_vertices = {}
        edge_point_ids = {}
        
        for face_id, face in faces.items():
            # Itera per ogni lato della faccia, associandovi la lista di facce adiacenti a quel lato
            for endpoints, adj in zip( [[face.v1, face.v2], [face.v2, face.v3], [face.v3, face.v1]],
                                       adjacent_faces[face_id] ):
                edge_id = tuple(sorted(endpoints))

                if edge_id in edge_points:
                    continue

                # Punti medi delle facce adiacenti
                adj_face_points = [] if len(adj) < 2 else [face_points[adj_face_id] for adj_face_id in adj]
                # Calcola il nuovo punto medio del lato
                edge_points[edge_id] = vertex_average([vertices[v] for v in endpoints] + adj_face_points)
                # Assegna un nuovo vertex ID al punto
                edge_point_ids[edge_id] = new_vertex_id
                new_vertex_id += 1

                # Associa il vecchio lato a entrambi i suoi vertici
                for endpoint in endpoints:
                    if len(adj) == 1:
                        # Se c'è una sola faccia adiacente il lato si trova al bordo di un buco
                        vertex_hole_edges[endpoint].add(edge_id)
                    else:
                        vertex_edges[endpoint].add(edge_id)


        # Calcola i vertici originali spostati verso il baricentro
        moved_vertices = {}

        for vertex_id, original_vertex in vertices.items():
            if len(vertex_hole_edges[vertex_id]) > 0:
                # Se il vertice è ai bordi di un buco si utilizzano i punti medi dei bordi del buco anziché il baricentro
                moved_vertices[vertex_id] = vertex_average([vertices[v] for edge in vertex_hole_edges[vertex_id] for v in edge] + [original_vertex])
            else:
                adj_faces = vertex_faces[vertex_id]
                n = len(adj_faces)
                avg_face_points = vertex_average([face_points[face_id] for face_id in adj_faces])
                avg_edge_midpoints = vertex_average([vertices[v] for edge in vertex_edges[vertex_id] for v in edge])
                # La nuova posizione è il baricentro pesato dei 3 punti
                moved_vertices[vertex_id] = ( original_vertex.scale(n - 3) +
                                              avg_face_points +
                                              avg_edge_midpoints.scale(2)
                                            ).scale(1 / n)

        # Crea la mappa con tutti i nuovi vertici
        new_vertices = moved_vertices
        new_vertices.update({face_point_ids[face_id] : face_points[face_id] for face_id in faces.keys()})
        new_vertices.update({edge_point_ids[edge_id] : edge_points[edge_id] for edge_id in edge_points.keys()})

        # Crea le nuove facce
        new_faces = []

        for face_id, face in faces.items():
            edge_ids = [edge_point_ids[tuple(sorted(endpoints))] for endpoints in [[face.v1, face.v2], [face.v2, face.v3], [face.v3, face.v1]]]
            new_faces.extend(Face.quad_to_triangles([face.v1, edge_ids[0], face_point_ids[face_id], edge_ids[2]]))
            new_faces.extend(Face.quad_to_triangles([face.v2, edge_ids[1], face_point_ids[face_id], edge_ids[0]]))
            new_faces.extend(Face.quad_to_triangles([face.v3, edge_ids[2], face_point_ids[face_id], edge_ids[1]]))

        return Mesh(new_vertices, dict(enumerate(new_faces)))

plugin = CatmullClarkPlugin("Catmull-Clark", "Modifica", "Catmull-Clark")
