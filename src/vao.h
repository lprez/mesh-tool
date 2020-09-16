#ifndef VAO_H
#define VAO_H

#include <GL/gl.h>
#include <initializer_list>
#include "attributebuffer.h"

// Gestisce un Vertex Array Object (gruppo di attribute buffers)
class VAO
{
public:
    VAO();
    ~VAO();

    // Assegna dei buffer al VAO con indici crescenti a partire da start_index
    void set_attributes(GLuint start_index, std::initializer_list<const AttributeBuffer*> buffers);

    // Seleziona il VAO per il rendering
    void render(GLsizei triangle_count);

    VAO& operator=(const VAO&) = delete;
    VAO(const VAO&) = delete;

private:
    GLuint vao_id;
};

#endif // VAO_H
