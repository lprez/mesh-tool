/*
 * mesh.h
 *
 *  Created on: 14 lug 2020
 *      Author: user
 */

#ifndef MESH_H_
#define MESH_H_

// TODO: spostare in un altro modulo
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

class Vertex {
public:
    //Vertex(Vec3 position, Vec3 uv, Vec3 normal, unsigned int id) :
    //    position(position), uv(uv), normal(normal), id(id) {}
    Vertex(Vec3 position, Vec2 uv) : position(position), uv(uv) {}

    Vec3 position; //, normal;
    Vec2 uv;
    //unsigned int id;
};

class Face {
public:
    Face(VertexID v1, VertexID v2, VertexID v3) : v1(v1), v2(v2), v3(v3) {}

    // Calcola la normale alla superficie.
    static Vec3 normal(const Vertex& v1, const Vertex& v2, const Vertex& v3);

    VertexID v1, v2, v3;
};

class Mesh {
public:
    Mesh() : vertices(std::map<VertexID, Vertex>()), faces(std::map<FaceID, Face>()), smooth(false) {}
    Mesh(const std::map<VertexID, Vertex>& vertices, const std::map<FaceID, Face>& faces, bool smooth = false) :
        vertices(vertices), faces(faces), smooth(smooth) {
        populate_buffers(smooth);
    }

    bool is_smooth() const { return smooth; }

    const void *position_buffer() const;
    size_t position_buffer_size() const;
    const void *normal_buffer() const;
    size_t normal_buffer_size() const;
    const void *uv_buffer() const;
    size_t uv_buffer_size() const;
    //const void *element_buffer() const;
    //size_t element_buffer_size() const;

    const std::map<VertexID, Vertex> vertex_map() const { return vertices; }
    const std::map<FaceID, Face> face_map() const { return faces; }
private:
    std::map<VertexID, Vertex> vertices;
    std::map<FaceID, Face> faces;
    bool smooth;
    //std::multimap<VertexID, FaceID> connected_faces;

    std::vector<float> positions;
    std::vector<float> normals;
    std::vector<float> uvs;

    void populate_buffers(bool smooth);
};


inline const void *Mesh::position_buffer() const
{
    return positions.data();
}

inline size_t Mesh::position_buffer_size() const
{
    return positions.size() * sizeof(positions[0]);
}

inline const void *Mesh::normal_buffer() const
{
    return normals.data();
}

inline size_t Mesh::normal_buffer_size() const
{
    return normals.size() * sizeof(normals[0]);
}

inline const void *Mesh::uv_buffer() const
{
    return uvs.data();
}

inline size_t Mesh::uv_buffer_size() const
{
    return uvs.size() * sizeof(uvs[0]);
}


#endif /* MESH_H_ */
