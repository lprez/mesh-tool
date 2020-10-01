#include <cstring>
#include <numeric>
#include "mesh.h"

Vec3 Face::surface_normal(const Vertex &v1, const Vertex &v2, const Vertex &v3)
{
    // Due lati del triangolo.
    Vec3 u = v2.position - v1.position,
         v = v3.position - v1.position;

    // La normale è il prodotto scalare dei due lati.
    return cross3(u, v).normalized();
}


void Face::set_normal(Vec3 normal)
{
    sv1.normal = normal;
    sv2.normal = normal;
    sv3.normal = normal;
}


void Face::set_normals(Vec3 n1, Vec3 n2, Vec3 n3)
{
    sv1.normal = n1;
    sv2.normal = n2;
    sv3.normal = n3;
}

Mesh::Mesh(const std::map<VertexID, Vertex> &vertices, const std::map<FaceID, Face> &faces,
           bool smooth, bool recalculate)
    : vertices(vertices), faces(faces), smooth(smooth)
{
    if (recalculate) recalculate_normals();
}

void Mesh::recalculate_normals()
{
    std::map<VertexID, Vec3> normal_sums;
    std::map<VertexID, float> normal_div;

    for (auto it = faces.begin(); it != faces.end(); it++) {
        Vec3 face_normal = Face::surface_normal(vertices.at(it->second.sv1.vertex_id),
                                                vertices.at(it->second.sv2.vertex_id),
                                                vertices.at(it->second.sv3.vertex_id));
        // Se è richiesto lo smooth shading, le normali vengono raccolte e associate ad ogni
        // vertice, altrimenti vengono direttamente aggiunte al sottovertice
        if (smooth) {
            for (SubVertex subvertex : {it->second.sv1, it->second.sv2, it->second.sv3}) {
                auto previous_normal = normal_sums.find(subvertex.vertex_id);

                if (previous_normal != normal_sums.end()) {
                    normal_sums[subvertex.vertex_id] = face_normal + previous_normal->second;;
                    normal_div[subvertex.vertex_id] += 1;
                } else {
                    normal_sums[subvertex.vertex_id] = face_normal;
                    normal_div[subvertex.vertex_id] = 1;
                }
            }
        } else {
            it->second.set_normal(face_normal);
        }
    }


    if (smooth) {
        // Aggiunge al sottovertice la media delle normali precedentemente calcolate
        for (auto it = faces.begin(); it != faces.end(); it++) {
            for (SubVertex *subvertex : {&it->second.sv1, &it->second.sv2, &it->second.sv3}) {
                Vec3 normal = normal_sums[subvertex->vertex_id] * (1 / normal_div[subvertex->vertex_id]);
                subvertex->normal = normal;
            }
        }
    }
}

void Mesh::set_smooth(bool smooth, bool recalculate)
{
    this->smooth = smooth;
    if (recalculate) recalculate_normals();
}

std::vector<float> Mesh::position_vector() const
{
    std::vector<float> positions;

    for (auto it = faces.begin(); it != faces.end(); it++) {
        for (SubVertex subvertex : {it->second.sv1, it->second.sv2, it->second.sv3}) {
            Vertex vertex = vertices.at(subvertex.vertex_id);
            for (size_t i = 0; i < vertex.position.count; i++) {
                positions.push_back(vertex.position[i]);
            }
        }
    }

    return positions;
}

std::vector<float> Mesh::normal_vector() const
{
    std::vector<float> normals;

    for (auto it = faces.begin(); it != faces.end(); it++) {
        for (SubVertex subvertex : {it->second.sv1, it->second.sv2, it->second.sv3}) {
            for (size_t i = 0; i < subvertex.normal.count; i++) {
                normals.push_back(subvertex.normal[i]);
            }
        }
    }

    return normals;
}

std::vector<float> Mesh::uv_vector() const
{
    std::vector<float> uvs;

    for (auto it = faces.begin(); it != faces.end(); it++) {
        for (SubVertex subvertex : {it->second.sv1, it->second.sv2, it->second.sv3}) {
            for (size_t i = 0; i < subvertex.uv.count; i++) {
                uvs.push_back(subvertex.uv[i]);
            }
        }
    }

    return uvs;
}

std::vector<VertexID> Mesh::vertex_id_vector() const
{
    std::vector<VertexID> ids;

    for (auto it = faces.begin(); it != faces.end(); it++) {
        for (SubVertex subvertex : {it->second.sv1, it->second.sv2, it->second.sv3}) {
            ids.push_back(subvertex.vertex_id);
        }
    }

    return ids;
}
