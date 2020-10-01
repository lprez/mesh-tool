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
        self.command_regex = re.compile(r"(f|v[tn]?|s)\s+(.*)")
        # Parametri del comando f
        self.face_regex = re.compile(r"[1-9][0-9]*(\/[1-9]?[0-9]*)*")

    def __call__(self, fname):
        #positions = [(0.0, 0.0, 0.0)]
        normals = [(0.0, 0.0, 0.0)]
        uvs = [(0.0, 0.0)]
        faces = []
        smooth = True
        recalculate_normals = False

        vertices = []
        faces = []

        with open(fname) as file:
            for line in file:
                match = self.command_regex.match(line)
                if match is not None:
                    command = match.group(1)
                    args = match.group(2).split()

                    if command == "f":
                        subvertices = []

                        for arg in args:
                            if self.face_regex.match(arg) is None:
                                raise RuntimeError("Formato faccia non valido")

                            indices = [0 if len(index) == 0 else int(index) for index in arg.split("/")]
                            position_index = indices[0]
                            uv_index = 0 if len(indices) < 2 else indices[1]
                            normal_index = 0 if len(indices) < 3 else indices[2]
                            vertex_id = position_index - 1

                            if vertex_id >= len(vertices) or uv_index >= len(uvs) or normal_index >= len(normals):
                                raise RuntimeError("Indici non validi")

                            subvertices.append(SubVertex(vertex_id, normals[normal_index], uvs[uv_index]))

                        if len(subvertices) == 3:
                            faces.append(Face(*subvertices))
                        elif len(subvertices) == 4:
                            faces.extend(Face.quad_to_triangles(subvertices))
                        else:
                            raise RuntimeError("Sono supportati solo triangoli e quadrilateri")
                    elif command == "s":
                        if args[0] == "off":
                            smooth = False
                        else:
                            recalculate_normals = True
                    elif command.startswith("v"):
                        coords = [float(coord) for coord in args]
                        if command == "v":
                            vertices.append(Vertex(tuple(resize_list(coords, 3))))
                        elif command == "vt":
                            uvs.append(tuple(resize_list(coords, 2)))
                        else:
                            normals.append(tuple(resize_list(coords, 3)))

        return Mesh(dict(enumerate(vertices)), dict(enumerate(faces)), smooth, recalculate_normals)

plugin = ImportOBJPlugin("Importa OBJ", "File", "Importa OBJ")
