#ifndef ATTRIBUTEBUFFER_H
#define ATTRIBUTEBUFFER_H

#include <cstddef>
#include <GL/gl.h>

// Gestisce un buffer con gli attributi dei vertici
class AttributeBuffer
{
public:
    AttributeBuffer(const GLenum type, const GLint component_size, const void *data = nullptr, size_t len = 0);
    ~AttributeBuffer();

    // Ricarica il buffer con dei nuovi dati
    void reset(const void *data, size_t len);
    // Seleziona il buffer per essere usato come attributo
    void bind(GLuint index) const;

    AttributeBuffer& operator=(const AttributeBuffer&) = delete;
    AttributeBuffer(const AttributeBuffer&) = delete;

    const GLint size;
    const GLenum type;

private:
    GLuint buffer_id;
};

#endif // ATTRIBUTEBUFFER_H
