from api import *
from subdivision_utils import adjacency, vertex_interpolate, subvertex_interpolate

class CatmullClarkPlugin(TransformerPlugin):
    def __call__(self, mesh):
        faces = mesh.faces
        vertices = mesh.vertices

        # Indice da usare per il prossimo vertice da aggiungere
        next_vertex_id = max(vertices.keys()) + 1

        # Crea le mappe di adiacenza tra vertici e facce
        vertex_faces, adjacent_faces = adjacency(vertices, faces)

        # Calcola i punti medi delle facce
        face_points = { face_id : vertex_interpolate([vertices[face.sv1.vertex_id],
                                                      vertices[face.sv2.vertex_id],
                                                      vertices[face.sv3.vertex_id]])
                        for face_id, face in faces.items()
                      }
        # I punti medi faranno parte dei vertici della nuova mesh, dunque bisogna assegnare loro un nuovo ID
        # e un sottovertice con delle UV interpolate
        face_subvertices = {face_id : subvertex_interpolate(next_vertex_id + face_id,
                                                            [face.sv1, face.sv2, face.sv3])
                            for face_id, face in faces.items()}
        new_vertex_id = next_vertex_id + len(faces) + 1

        # Calcola i lati a cui appartiene ogni vertice e i nuovi punti medi dei nuovi lati
        vertex_edges = {vertex_id : set() for vertex_id in mesh.vertices.keys()}
        vertex_hole_edges = {vertex_id : set() for vertex_id in mesh.vertices.keys()}
        edge_points = {}
        edge_subvertices = {}
        
        for face_id, face in faces.items():
            # Itera per ogni lato della faccia, associandovi la lista di facce adiacenti a quel lato
            for endpoints, adj in zip( [[face.sv1, face.sv2],
                                        [face.sv2, face.sv3],
                                        [face.sv3, face.sv1]],
                                       adjacent_faces[face_id] ):
                edge_id = tuple(sorted([endpoints[0].vertex_id, endpoints[1].vertex_id]))

                if edge_id in edge_points:
                    continue

                # Punti medi delle facce adiacenti
                adj_face_points = [] if len(adj) < 2 else [face_points[adj_face_id] for adj_face_id in adj]
                adj_face_subvertices = [] if len(adj) < 2 else [face_subvertices[adj_face_id] for adj_face_id in adj]
                # Calcola il nuovo punto medio del lato
                edge_points[edge_id] = vertex_interpolate([vertices[sv.vertex_id] for sv in endpoints] + adj_face_points)
                edge_subvertices[edge_id] = subvertex_interpolate(new_vertex_id, [sv for sv in endpoints] + adj_face_subvertices)
                new_vertex_id += 1

                # Associa il vecchio lato a entrambi i suoi vertici
                for endpoint in endpoints:
                    if len(adj) < 2:
                        # Se c'è una sola faccia adiacente il lato si trova al bordo di un buco
                        vertex_hole_edges[endpoint.vertex_id].add(edge_id)
                    else:
                        vertex_edges[endpoint.vertex_id].add(edge_id)


        # Calcola i vertici originali spostati verso il baricentro
        moved_vertices = {}

        for vertex_id, original_vertex in vertices.items():
            adj_faces = vertex_faces[vertex_id]
            n = len(adj_faces)

            if len(vertex_hole_edges[vertex_id]) > 0 or n == 0:
                # Se il vertice è ai bordi di un buco si utilizzano i punti medi dei bordi del buco anziché il baricentro
                moved_vertices[vertex_id] = vertex_interpolate([vertices[v] for edge in vertex_hole_edges[vertex_id] for v in edge] + [original_vertex])
            else:
                avg_face_points = vertex_interpolate([face_points[face_id] for face_id in adj_faces])
                avg_edge_midpoints = vertex_interpolate([vertices[v] for edge in vertex_edges[vertex_id] for v in edge])
                # La nuova posizione è il baricentro pesato dei 3 punti
                moved_vertices[vertex_id] = ( original_vertex.scale(n - 3) +
                                              avg_face_points +
                                              avg_edge_midpoints.scale(2)
                                            ).scale(1 / n)

        # Crea la mappa con tutti i nuovi vertici
        new_vertices = moved_vertices
        new_vertices.update({face_subvertices[face_id].vertex_id : face_points[face_id] for face_id in faces.keys()})
        new_vertices.update({edge_subvertices[edge_id].vertex_id : edge_points[edge_id] for edge_id in edge_points.keys()})

        # Crea le nuove facce
        new_faces = []

        for face_id, face in faces.items():
            edge_svs = [edge_subvertices[tuple(sorted([begin.vertex_id, end.vertex_id]))]
                        for begin, end in [[face.sv1, face.sv2], [face.sv2, face.sv3], [face.sv3, face.sv1]]]
            new_faces.extend(Face.quad_to_triangles([face.sv1, edge_svs[0], face_subvertices[face_id], edge_svs[2]]))
            new_faces.extend(Face.quad_to_triangles([face.sv2, edge_svs[1], face_subvertices[face_id], edge_svs[0]]))
            new_faces.extend(Face.quad_to_triangles([face.sv3, edge_svs[2], face_subvertices[face_id], edge_svs[1]]))

        return Mesh(new_vertices, dict(enumerate(new_faces)), mesh.smooth, True)

plugin = CatmullClarkPlugin("Catmull-Clark", "Modifica", "Catmull-Clark")
