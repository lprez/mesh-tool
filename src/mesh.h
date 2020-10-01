/*
 * mesh.h
 *
 *  Created on: 14 lug 2020
 *      Author: user
 */

#ifndef MESH_H_
#define MESH_H_

#include <cstdint>
#include <map>
#include <vector>
#include "linear.h"

using Vec2 = Vector<float, 2>;
using Vec3 = Vector<float, 3>;
using Vec4 = Vector<float, 4>;
using Mat4 = Matrix<float, 4, 4>;
using VertexID = uint32_t;
using FaceID = uint32_t;

// Rappresenta un vertice (in senso geometrico)
class Vertex {
public:
    Vertex() {}
    Vertex(Vec3 position) : position(position) {}

    Vec3 position;
};

// Rappresenta una particolare istanza di un vertice con gli attributi
// necessari al rendering. Possono esserci più SubVertex per lo stesso
// vertice (per esempio, lo stesso vertice di un cubo
// può far riferimento a parti diverse della texture in base alla faccia
// che si sta renderizzando, per cui servono coordinate UV diverse, ma duplicare
// il vertice non va bene perché algoritmi come Catmull-Clark richiedono la
// conoscenza della topologia della mesh).
class SubVertex {
public:
    SubVertex(VertexID v) : vertex_id(v) {}
    SubVertex(VertexID v, Vec3 normal, Vec2 uv) : vertex_id(v), uv(uv), normal(normal) {}
    VertexID vertex_id;
    Vec2 uv;
    Vec3 normal;
};

// Un triangolo
class Face {
public:
    Face(VertexID v1, VertexID v2, VertexID v3) : sv1(v1), sv2(v2), sv3(v3) {}
    Face(SubVertex sv1, SubVertex sv2, SubVertex sv3) : sv1(sv1), sv2(sv2), sv3(sv3) {}

    // Imposta la stessa normale per tutti i SubVertex
    void set_normal(Vec3 normal);
    // Imposta una normale diversa per ogni SubVertex
    void set_normals(Vec3, Vec3, Vec3);
    // Calcola la normale alla superficie
    static Vec3 surface_normal(const Vertex &v1, const Vertex &v2, const Vertex &v3);

    SubVertex sv1, sv2, sv3;
};

class Mesh {
public:
    Mesh() : vertices(std::map<VertexID, Vertex>()), faces(std::map<FaceID, Face>()), smooth(false) {}
    Mesh(const std::map<VertexID, Vertex>& vertices, const std::map<FaceID, Face>& faces,
         bool smooth = false, bool recalculate_normals = true);

    // Ricalcola le normali dei SubVertex
    void recalculate_normals();

    // Cambia il tipo di shading (smooth/flat)
    void set_smooth(bool smooth, bool recalculate_normals = true);
    bool is_smooth() const { return smooth; }

    // Operazioni sui vertici
    void set_vertex(VertexID id, Vertex vertex) { vertices[id] = vertex; }
    void remove_vertex(VertexID id) { vertices.erase(id); }

    // Genera un vettore con tutte le posizioni usate da tutte le facce
    std::vector<float> position_vector() const;
    std::vector<float> normal_vector() const;
    std::vector<float> uv_vector() const;
    std::vector<VertexID> vertex_id_vector() const;

    // Vertici e facce con ID associati
    const std::map<VertexID, Vertex> vertex_map() const { return vertices; }
    const std::map<FaceID, Face> face_map() const { return faces; }
private:
    std::map<VertexID, Vertex> vertices;
    std::map<FaceID, Face> faces;
    bool smooth;
};

#endif /* MESH_H_ */
