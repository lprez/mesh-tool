from api import *

import re

# Ridimensiona una lista di float, riempiendo eventualmente con 0)
def resize_list(list, n):
    if len(list) < n:
        return list + [0.0] * (n - len(list))
    else:
        return list[:n]

class ImportOBJPlugin(ImporterPlugin):
    def __init__(self, name, menu, entry):
        super().__init__(name, menu, entry)

        # L'espressione regolare che rappresenta tutte le righe del file OBJ
        # di interesse (posizione dei vertici, coordinate UV e facce)
        self.command_regex = re.compile(r"(f|vt?|s)\s+(.*)")
        # Parametri del comando f
        self.face_regex = re.compile(r"[1-9][0-9]*(\/[1-9]?[0-9]*)*")

    def __call__(self, fname):
        positions = [(0.0, 0.0, 0.0)]
        uvs = [(0.0, 0.0)]
        faces = []
        smooth = True

        # Mappa da (id posizione, id uv) -> (id vertice)
        # Necessaria perché nel formato OBJ gli indici sono assegnati
        # separatamente a posizione e uv, mentre per OpenGL serve un unico
        # indice per ongni vertice
        mapped_vertices = {}
        next_vertex_id = 0

        # Mappe finali
        vertices = {}
        faces = []

        with open(fname) as file:
            for line in file:
                match = self.command_regex.match(line)
                if match is not None:
                    command = match.group(1)
                    args = match.group(2).split()

                    if command == "f":
                        vertex_ids = []

                        for arg in args:
                            if self.face_regex.match(arg) is None:
                                raise RuntimeError("Formato faccia non valido")

                            indices = [0 if len(index) == 0 else int(index) for index in arg.split("/")]
                            position_index = indices[0]
                            uv_index = 0 if len(indices) < 2 else indices[1]

                            # Bisogna assegnare un indice al vertice specificato nella faccia
                            compound_index = (position_index, uv_index)
                            if compound_index in mapped_vertices:
                                vertex_ids.append(mapped_vertices[compound_index])
                            else:
                                mapped_vertices[compound_index] = next_vertex_id
                                vertex_ids.append(next_vertex_id)
                                vertices[next_vertex_id] = Vertex(positions[position_index], uvs[uv_index])
                                next_vertex_id += 1

                        if len(vertex_ids) == 3:
                            faces.append(Face(*vertex_ids))
                        elif len(vertex_ids) == 4:
                            faces.extend(Face.quad_to_triangles(vertex_ids))
                        else:
                            raise RuntimeError("Sono supportati solo triangoli e quadrilateri")
                    elif command == "s":
                        if args[0] == "off":
                            smooth = False
                    else:
                        coords = [float(coord) for coord in args]
                        if command == "v":
                            positions.append(tuple(resize_list(coords, 3)))
                        else:
                            uvs.append(tuple(resize_list(coords, 2)))

        return Mesh(vertices, dict(enumerate(faces)), smooth)

plugin = ImportOBJPlugin("Importa OBJ", "File", "Importa OBJ")
