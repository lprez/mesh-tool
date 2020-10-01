#ifndef MESHVIEW_H
#define MESHVIEW_H

#include "mesh.h"
#include "attributebuffer.h"
#include "vao.h"
#include "shader.h"

// Gestisce i buffer nella GPU associati alla mesh
class MeshBuffers
{
public:
    MeshBuffers(const Mesh& mesh);

    // Sostituisce con una nuova mesh
    void update(const Mesh& mesh);

    const AttributeBuffer & position_buffer() const { return positions; }
    const AttributeBuffer & normal_buffer() const { return normals; }
    const AttributeBuffer & uv_buffer() const { return uvs; }
    const AttributeBuffer & vertex_id_buffer() const { return vertex_ids; }

    // Numero di triangoli
    unsigned int triangle_count() const { return elems; }

private:
    AttributeBuffer positions, normals, uvs, vertex_ids;
    unsigned int elems;
};

// Contiene lo stato necessario al rendering di una Mesh
class MeshRenderer
{
public:
    void render(bool points = false);

protected:
    MeshRenderer(const MeshBuffers& mesh_buffers, const ShaderProgram& program) :
        buffers(mesh_buffers), program(program) {}

    const MeshBuffers &buffers;
    const ShaderProgram &program;
    VAO vao;
};

// Renderer per la vista
class MeshRendererView : public MeshRenderer
{
public:
    MeshRendererView(const MeshBuffers& mesh, const ShaderProgram& program);
};

// Renderer per il 3D picking
class MeshRendererPick : public MeshRenderer
{
public:
    MeshRendererPick(const MeshBuffers& mesh, const ShaderProgram& program);
};


#endif // MESHVIEW_H
