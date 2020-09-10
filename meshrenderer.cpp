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
    position_buffer.reset(mesh.position_buffer(), mesh.position_buffer_size());
    normal_buffer.reset(mesh.normal_buffer(), mesh.normal_buffer_size());
    uv_buffer.reset(mesh.uv_buffer(), mesh.uv_buffer_size());
    triangle_count = mesh.face_map().size() * 3;
    vao.set_attributes(0, {&position_buffer, &normal_buffer, &uv_buffer});
}

void MeshRenderer::render()
{
    program.use();
    vao.draw(triangle_count);
}
