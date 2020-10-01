#include "framebuffer.h"

#include <QOpenGLFunctions>

Framebuffer::Framebuffer(GLsizei width, GLsizei height, GLenum depth_format, std::initializer_list<GLenum> color_formats)
{
    QOpenGLFunctions *f = QOpenGLContext::currentContext()->functions();

    f->glGenFramebuffers(1, &framebuffer_id);
    f->glBindFramebuffer(GL_FRAMEBUFFER, framebuffer_id);

    n_color_attachments = color_formats.size();
    renderbuffer_ids = new GLenum[n_color_attachments + 1];
    f->glGenRenderbuffers(n_color_attachments + 1, renderbuffer_ids);

    f->glBindRenderbuffer(GL_RENDERBUFFER, renderbuffer_ids[0]);
    f->glRenderbufferStorage(GL_RENDERBUFFER, depth_format, width, height);
    f->glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, renderbuffer_ids[0]);

    int color_idx = 0;

    for (GLenum color_format : color_formats) {
        f->glBindRenderbuffer(GL_RENDERBUFFER, renderbuffer_ids[color_idx + 1]);
        f->glRenderbufferStorage(GL_RENDERBUFFER, color_format, width, height);
        f->glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + color_idx,
                                     GL_RENDERBUFFER, renderbuffer_ids[color_idx + 1]);
        color_idx++;
    }

    f->glBindRenderbuffer(GL_RENDERBUFFER, 0);
    f->glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

Framebuffer::~Framebuffer()
{
    QOpenGLFunctions *f = QOpenGLContext::currentContext()->functions();

    f->glDeleteFramebuffers(1, &framebuffer_id);
    f->glDeleteRenderbuffers(n_color_attachments + 1, renderbuffer_ids);
    delete[] renderbuffer_ids;
}

void Framebuffer::enable(bool read)
{
    QOpenGLFunctions *f = QOpenGLContext::currentContext()->functions();

    f->glBindFramebuffer(read ? GL_READ_FRAMEBUFFER : GL_DRAW_FRAMEBUFFER, framebuffer_id);
}

void Framebuffer::disable(bool read)
{
    QOpenGLFunctions *f = QOpenGLContext::currentContext()->functions();

    f->glBindFramebuffer(read ? GL_READ_FRAMEBUFFER : GL_DRAW_FRAMEBUFFER, 0);
}

void Framebuffer::read_pixels_red(int x, int y, int width, int height, unsigned int *data)
{
    QOpenGLFunctions *f = QOpenGLContext::currentContext()->functions();
    f->glReadPixels(x, y, width, height, GL_RED_INTEGER, GL_UNSIGNED_INT, data);
}

void Framebuffer::read_pixels_red(int x, int y, int width, int height, float *data)
{
    QOpenGLFunctions *f = QOpenGLContext::currentContext()->functions();
    f->glReadPixels(x, y, width, height, GL_RED, GL_FLOAT, data);
}

void Framebuffer::read_pixels_depth(int x, int y, int width, int height, float *data)
{
    QOpenGLFunctions *f = QOpenGLContext::currentContext()->functions();
    f->glReadPixels(x, y, width, height, GL_DEPTH_COMPONENT, GL_FLOAT, data);
}
