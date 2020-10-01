#include <QOpenGLContext>
#include <QOpenGLFunctions>
#include <QOpenGLFunctions_4_5_Core>

#include "attributebuffer.h"

AttributeBuffer::AttributeBuffer(const GLenum component_type, const GLint component_size,
                                 const void *data, size_t len)
    : size(component_size), type(component_type)
{
    QOpenGLFunctions *f = QOpenGLContext::currentContext()->functions();

    f->glGenBuffers(1, &buffer_id);

    if (data != nullptr && len > 0) {
        reset(data, len);
    }
}

AttributeBuffer::~AttributeBuffer()
{
    QOpenGLFunctions *f = QOpenGLContext::currentContext()->functions();
    f->glDeleteBuffers(1, &buffer_id);
}

void AttributeBuffer::reset(const void *data, size_t len)
{
    QOpenGLFunctions *f = QOpenGLContext::currentContext()->functions();

    f->glBindBuffer(GL_ARRAY_BUFFER, buffer_id);
    f->glBufferData(GL_ARRAY_BUFFER, len, data, GL_STATIC_DRAW);
}

void AttributeBuffer::bind(GLuint index) const
{
    QOpenGLFunctions_4_5_Core *f = QOpenGLContext::currentContext()->versionFunctions<QOpenGLFunctions_4_5_Core>();

    f->glBindBuffer(GL_ARRAY_BUFFER, buffer_id);
    f->glEnableVertexAttribArray(index);
    switch(type) {
    case GL_UNSIGNED_SHORT:
    case GL_INT:
    case GL_UNSIGNED_INT:
        f->glVertexAttribIPointer(index, size, type, 0, (void *) 0);
        break;
    default:
        f->glVertexAttribPointer(index, size, type, GL_FALSE, 0, (void *) 0);
    }
}
