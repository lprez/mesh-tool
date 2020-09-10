/*
 * mesh.cpp
 *
 *  Created on: 14 lug 2020
 *      Author: user
 */

#include <cstring>
#include <numeric>
#include "mesh.h"



Vec3 Face::normal(const Vertex &v1, const Vertex &v2, const Vertex &v3)
{
    // Due lati del triangolo.
    Vec3 u = v2.position - v1.position,
         v = v3.position - v1.position;

    // La normale è il prodotto scalare dei due lati.
    return cross3(u, v);
}


/*
inline const void *Mesh::element_buffer() const
{
    return elements.data();
}

size_t Mesh::element_buffer_size() const
{
    return elements.size() * sizeof(elements[0]);
}
*/

void Mesh::populate_buffers(bool smooth)
{
    std::map<VertexID, Vec3> normal_sums;
    std::map<VertexID, float> normal_div;

    positions.clear();
    normals.clear();
    uvs.clear();

    //std::iota(std::begin(elements), std::end(elements), 0);

    for (auto it = faces.begin(); it != faces.end(); it++) {
        Vec3 normal = Face::normal(vertices.at(it->second.v1), vertices.at(it->second.v2), vertices.at(it->second.v3));

        for (VertexID vertex_id : {it->second.v1, it->second.v2, it->second.v3}) {
            Vertex vertex = vertices.at(vertex_id);


            for (size_t j = 0; j < vertex.position.count; j++) {
                positions.push_back(vertex.position[j]);
            }


            if (smooth) {
                auto previous_normal = normal_sums.find(vertex_id);

                if (previous_normal != normal_sums.end()) {
                    normal = normal + previous_normal->first;
                    normal_div[vertex_id] += 1;
                } else {
                    normal_div[vertex_id] = 1;
                }

                normal_sums[vertex_id] = normal;
            } else {
                for (size_t j = 0; j < normal.count; j++) {
                    normals.push_back(normal[j]);
                }
            }

            for (size_t j = 0; j < vertex.uv.count; j++) {
                uvs.push_back(vertex.uv[j]);
            }
        }
    }

    if (smooth) {
        for (auto it = faces.begin(); it != faces.end(); it++) {
            for (VertexID vertex_id : {it->second.v1, it->second.v2, it->second.v3}) {
                Vec3 normal = normal_sums[vertex_id] * (1 / normal_div[vertex_id]);
                for (size_t j = 0; j < normal.size(); j++) {
                    normals.push_back(normal[j]);
                }
            }
        }
    }
}
