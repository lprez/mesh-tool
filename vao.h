#ifndef VAO_H
#define VAO_H

#include <GL/gl.h>
#include <initializer_list>
#include "attributebuffer.h"

class VAO
{
public:
    VAO();
    ~VAO();

    void set_attributes(GLuint start_index, std::initializer_list<const AttributeBuffer*> buffers);
    void draw(GLsizei triangle_count);

    VAO& operator=(const VAO&) = delete;
    VAO(const VAO&) = delete;

private:
    GLuint vao_id;
};

#endif // VAO_H
