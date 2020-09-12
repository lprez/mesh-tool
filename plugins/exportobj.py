from api import *

class ExportOBJPlugin(ExporterPlugin):
    def __call__(self, mesh, fname):
        with open(fname, "w") as file:
            next_vertex_idx = 1
            vertex_idx = {}

            for vertex_id, vertex in mesh.vertices.items():
                vertex_idx[vertex_id] = next_vertex_idx
                next_vertex_idx += 1

                (x, y, z) = vertex.position
                print("v", x, y, z, file=file)

            for face in mesh.faces.values():
                for sv in [face.sv1, face.sv2, face.sv3]:
                    (u, v) = sv.uv
                    print("vt", u, v, file=file)


            for face in mesh.faces.values():
                for sv in [face.sv1, face.sv2, face.sv3]:
                    (nx, ny, nz) = sv.normal
                    print("vn", nx, ny, nz, file=file)

            subvertex_id = 1
            for face in mesh.faces.values():
                print("f {}/{}/{} {}/{}/{} {}/{}/{}".format(
                                                vertex_idx[face.sv1.vertex_id], subvertex_id, subvertex_id,
                                                vertex_idx[face.sv2.vertex_id], subvertex_id + 1, subvertex_id + 1,
                                                vertex_idx[face.sv3.vertex_id], subvertex_id + 2, subvertex_id + 2
                      ), file = file)
                subvertex_id += 3

            if not mesh.smooth:
                print("s off", file=file)

plugin = ExportOBJPlugin("Esporta OBJ", "File", "Esporta OBJ")
