#ifndef ATTRIBUTEBUFFER_H
#define ATTRIBUTEBUFFER_H

#include <cstddef>
#include <GL/gl.h>


class AttributeBuffer
{
public:
    AttributeBuffer(const GLenum type, const GLint component_size, const void *data = nullptr, size_t len = 0);
    ~AttributeBuffer();

    void reset(const void *data, size_t len);
    void bind(GLuint index) const;

    AttributeBuffer& operator=(const AttributeBuffer&) = delete;
    AttributeBuffer(const AttributeBuffer&) = delete;

    const GLint size;
    const GLenum type;

private:
    GLuint buffer_id;
};

#endif // ATTRIBUTEBUFFER_H
