#include "meshrenderer.h"

MeshBuffers::MeshBuffers(const Mesh &mesh)
    : positions(GL_FLOAT, 3)
    , normals(GL_FLOAT, 3)
    , uvs(GL_FLOAT, 2)
    , vertex_ids(GL_UNSIGNED_INT, 1)
    , elems(mesh.face_map().size() * 3)
{
    update(mesh);
}

void MeshBuffers::update(const Mesh &mesh)
{
    std::vector<float> pos_vec = mesh.position_vector(),
                       norm_vec = mesh.normal_vector(),
                       uv_vec = mesh.uv_vector();
    std::vector<VertexID> ids_vec = mesh.vertex_id_vector();
    positions.reset(pos_vec.data(), pos_vec.size() * sizeof(pos_vec[0]));
    normals.reset(norm_vec.data(), norm_vec.size() * sizeof(norm_vec[0]));
    uvs.reset(uv_vec.data(), uv_vec.size() * sizeof(uv_vec[0]));
    vertex_ids.reset(ids_vec.data(), ids_vec.size() * sizeof(ids_vec[0]));
    elems = mesh.face_map().size() * 3;
}

void MeshRenderer::render(bool points)
{
    program.use();
    vao.render(buffers.triangle_count(), points);
}

MeshRendererView::MeshRendererView(const MeshBuffers &mesh, const ShaderProgram &program)
    : MeshRenderer(mesh, program)
{
    vao.set_attributes(0, {&buffers.position_buffer(), &buffers.normal_buffer(), &buffers.uv_buffer()});
}

MeshRendererPick::MeshRendererPick(const MeshBuffers &mesh, const ShaderProgram &program)
    : MeshRenderer(mesh, program)
{
    vao.set_attributes(0, {&buffers.position_buffer(), &buffers.vertex_id_buffer()});
}
