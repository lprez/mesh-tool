#include <QOpenGLContext>
#include <QOpenGLFunctions>
#include <QOpenGLFunctions_4_5_Core>

#include "vao.h"

VAO::VAO()
{
    QOpenGLFunctions_4_5_Core *f = QOpenGLContext::currentContext()->versionFunctions<QOpenGLFunctions_4_5_Core>();

    f->glGenVertexArrays(1, &vao_id);
}

VAO::~VAO()
{
    QOpenGLFunctions_4_5_Core *f = QOpenGLContext::currentContext()->versionFunctions<QOpenGLFunctions_4_5_Core>();

    f->glDeleteVertexArrays(1, &vao_id);
}

void VAO::set_attributes(GLuint index, std::initializer_list<const AttributeBuffer*> buffers)
{
    QOpenGLFunctions_4_5_Core *f = QOpenGLContext::currentContext()->versionFunctions<QOpenGLFunctions_4_5_Core>();

    f->glBindVertexArray(vao_id);

    for (auto buffer : buffers) {
        buffer->bind(index++);
    }

    f->glBindVertexArray(0);
}

void VAO::render(GLsizei count)
{
    QOpenGLFunctions_4_5_Core *f = QOpenGLContext::currentContext()->versionFunctions<QOpenGLFunctions_4_5_Core>();

    f->glBindVertexArray(vao_id);
    f->glDrawArrays(GL_TRIANGLES, 0, count);
    f->glBindVertexArray(0);
}

