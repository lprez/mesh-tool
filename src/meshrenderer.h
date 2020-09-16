#ifndef MESHVIEW_H
#define MESHVIEW_H

#include "mesh.h"
#include "attributebuffer.h"
#include "vao.h"
#include "shader.h"

// Contiene lo stato necessario al rendering di una Mesh
class MeshRenderer
{
public:
    MeshRenderer(const Mesh& mesh, ShaderProgram& program);

    // Sostituisce con una nuova mesh
    void update(const Mesh& mesh);

    void render();

private:
    ShaderProgram &program;
    VAO vao;
    AttributeBuffer position_buffer, normal_buffer, uv_buffer;
    unsigned int triangle_count;
};

#endif // MESHVIEW_H
