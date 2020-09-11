from api import *

class ExportOBJPlugin(ExporterPlugin):
    def __call__(self, mesh, fname):
        with open(fname, "w") as file:
            for vertex in mesh.vertices.values():
                (x, y, z) = vertex.position
                print("v", x, y, z, file=file)

            for vertex in mesh.vertices.values():
                (u, v) = vertex.uv
                print("vt", u, v, file=file)

            for face in mesh.faces.values():
                print("f {}/{} {}/{} {}/{}".format(
                                                face.v1 + 1, face.v1 + 1,
                                                face.v2 + 1, face.v2 + 1,
                                                face.v3 + 1, face.v3 + 1
                      ), file = file)

            if not mesh.smooth:
                print("s off", file=file)

plugin = ExportOBJPlugin("Esporta OBJ", "File", "Esporta OBJ")
