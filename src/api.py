class Vertex:
    def __init__(self, position, uv):
        self.position = position
        self.uv = uv

    def __repr__(self):
        return "Vertex(({}, {}, {}), ({}, {}))".format(*self.position, *self.uv)

    # Somma delle posizioni e delle coordinate UV
    def __add__(self, other):
        position = ( self.position[0] + other.position[0],
                     self.position[1] + other.position[1],
                     self.position[2] + other.position[2]
                   )
        uv = ( self.uv[0] + other.uv[0],
               self.uv[1] + other.uv[1]
             )
        return Vertex(position, uv)

    # Moltiplicazione delle posizioni per un fattore
    def scale(self, factor):
        position = ( self.position[0] * factor,
                     self.position[1] * factor,
                     self.position[2] * factor
                   )
        uv = ( self.uv[0] * factor,
               self.uv[1] * factor
             )
        return Vertex(position, uv)

class Face:
    def __init__(self, v1, v2, v3):
        self.v1 = v1
        self.v2 = v2
        self.v3 = v3

    def __repr__(self):
        return "Face({}, {}, {})".format(self.v1, self.v2, self.v3)

    # Converte una faccia con quattro lati in due facce con tre lati
    def quad_to_triangles(vertices):
        return [ Face(vertices[0], vertices[1], vertices[2]),
                 Face(vertices[0], vertices[2], vertices[3]) ]

class Mesh:
    def __init__(self, vertices={}, faces={}, smooth=False):
        self.vertices = vertices
        self.faces = faces
        self.smooth = smooth

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
