#include "meshrenderer.h"

MeshRenderer::MeshRenderer(const Mesh &mesh, ShaderProgram &program)
    : program(program)
    , position_buffer(GL_FLOAT, 3)
    , normal_buffer(GL_FLOAT, 3)
    , uv_buffer(GL_FLOAT, 2)
    , triangle_count(mesh.face_map().size() * 3)
{
    update(mesh);
}

void MeshRenderer::update(const Mesh &mesh)
{
    std::vector<float> positions = mesh.position_vector(),
                       normals = mesh.normal_vector(),
                       uvs = mesh.uv_vector();
    position_buffer.reset(positions.data(), positions.size() * sizeof(positions[0]));
    normal_buffer.reset(normals.data(), normals.size() * sizeof(normals[0]));
    uv_buffer.reset(uvs.data(), uvs.size() * sizeof(uvs[0]));
    triangle_count = mesh.face_map().size() * 3;
    vao.set_attributes(0, {&position_buffer, &normal_buffer, &uv_buffer});
}

void MeshRenderer::render()
{
    program.use();
    vao.draw(triangle_count);
}
