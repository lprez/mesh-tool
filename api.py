class Vertex:
    def __init__(self, position, uv):
        self.position = position
        self.uv = uv

class Face:
    def __init__(self, v1, v2, v3):
        self.v1 = v1
        self.v2 = v2
        self.v3 = v3

class Mesh:
    def __init__(self, vertices={}, faces={}):
        self.vertices = vertices
        self.faces = faces

class Plugin:
    def __init__(self, name):
        self.name = name

    def __call__(self, mesh):
        raise NotImplementedError
