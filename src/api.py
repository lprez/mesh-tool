class Vertex:
    def __init__(self, position):
        self.position = position

    def __repr__(self):
        return "Vertex(({}, {}, {}))".format(*self.position)

    # Somma delle posizioni
    def __add__(self, other):
        position = (self.position[0] + other.position[0],
                    self.position[1] + other.position[1],
                    self.position[2] + other.position[2])
        return Vertex(position)

    # Moltiplicazione delle posizioni per un fattore
    def scale(self, factor):
        position = (self.position[0] * factor,
                    self.position[1] * factor,
                    self.position[2] * factor)
        return Vertex(position)

class SubVertex:
    def __init__(self, vertex_id, normal, uv):
        self.vertex_id = vertex_id
        self.normal = normal
        self.uv = uv

    def __repr__(self):
        return "SubVertex({}, ({}, {}, {}), ({}, {}))".format(self.vertex_id, *self.normal, *self.uv)

    # Somma delle normali e delle coordinate UV
    def __add__(self, other):
        normal = (self.normal[0] + other.normal[0],
                  self.normal[1] + other.normal[1],
                  self.normal[2] + other.normal[2])
        uv = (self.uv[0] + other.uv[0],
              self.uv[1] + other.uv[1])
        return SubVertex(self.vertex_id, normal, uv)

    # Moltiplicazione delle coordinate per un fattore
    def scale(self, factor):
        normal = (self.normal[0] * factor,
                  self.normal[1] * factor,
                  self.normal[2] * factor)
        uv = (self.uv[0] * factor,
              self.uv[1] * factor)
        return SubVertex(self.vertex_id, normal, uv)

class Face:
    def __init__(self, sv1, sv2, sv3):
        self.sv1 = sv1
        self.sv2 = sv2
        self.sv3 = sv3

    def __repr__(self):
        return "Face({}, {}, {})".format(self.sv1, self.sv2, self.sv3)

    # Converte una faccia con quattro lati in due facce con tre lati
    def quad_to_triangles(vertices):
        return [Face(vertices[0], vertices[1], vertices[2]),
                Face(vertices[0], vertices[2], vertices[3])]


class Mesh:
    def __init__(self, vertices={}, faces={}, smooth=False, dirty_normals=False):
        # Se dirty_normals=True, le normali verranno ricalcolate quando la Mesh sarà
        # trasferita al programma principale
        self.vertices = vertices
        self.faces = faces
        self.smooth = smooth
        self.dirty_normals = dirty_normals

    def __repr__(self):
        return "Mesh({}, {})".format(self.vertices, self.faces)


class Plugin:
    def __init__(self, name, menu, entry):
        self.name = name
        self.menu = menu
        self.entry = entry


class TransformerPlugin(Plugin):
    def __call__(self, mesh):
        raise NotImplementedError



class ImporterPlugin(Plugin):
    def __call__(self, mesh):
        raise NotImplementedError


class ExporterPlugin(Plugin):
    def __call__(self, mesh, fname):
        raise NotImplementedError
