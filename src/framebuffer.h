#ifndef FRAMEBUFFER_H
#define FRAMEBUFFER_H

#include <GL/gl.h>
#include <initializer_list>

// Rappresenta un framebuffer con renderbuffer associati
class Framebuffer
{
public:
    Framebuffer(GLsizei width, GLsizei height, GLenum depth_format, std::initializer_list<GLenum> color_formats);
    ~Framebuffer();

    void enable(bool read = false);
    void disable(bool read = false);

    // Legge la componente rossa da un rettangolo di pixel del framebuffer in uso
    static void read_pixels_red(int x, int y, int width, int height, unsigned int *data);
    static void read_pixels_red(int x, int y, int width, int height, float *data);
    // Legge la profondità (z)
    static void read_pixels_depth(int x, int y, int width, int height, float *data);

    Framebuffer& operator=(const Framebuffer&) = delete;
    Framebuffer(const Framebuffer&) = delete;

private:
    GLuint framebuffer_id;
    GLuint * renderbuffer_ids;
    int n_color_attachments = 0;
};

#endif // FRAMEBUFFER_H
