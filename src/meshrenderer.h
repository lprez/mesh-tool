#ifndef MESHVIEW_H
#define MESHVIEW_H

#include "mesh.h"
#include "attributebuffer.h"
#include "vao.h"
#include "shader.h"

class MeshRenderer
{
public:
    MeshRenderer(const Mesh& mesh, ShaderProgram& program);

    void update(const Mesh& mesh);
    void render();

private:
    ShaderProgram &program;
    VAO vao;
    AttributeBuffer position_buffer, normal_buffer, uv_buffer;
    unsigned int triangle_count;
};

#endif // MESHVIEW_H
