#include <QOpenGLContext>
#include <QOpenGLFunctions>
#include <stdexcept>
#include "shader.h"

Shader::Shader(const char *src, bool is_fragment)
{
    GLint success;
    QOpenGLFunctions *f = QOpenGLContext::currentContext()->functions();

    // Carica e compila lo shader
    shader_id = f->glCreateShader(is_fragment ? GL_FRAGMENT_SHADER : GL_VERTEX_SHADER);

    f->glShaderSource(shader_id, 1, &src, NULL);
    f->glCompileShader(shader_id);

    // Verifica se ci sono stati errori nella compilazione
    f->glGetShaderiv(shader_id, GL_COMPILE_STATUS, &success);

    if (success == GL_FALSE) {
        int info_log_length;
        f->glGetShaderiv(shader_id, GL_INFO_LOG_LENGTH, &info_log_length);

        GLchar *message = new char[info_log_length];
        f->glGetShaderInfoLog(shader_id, info_log_length, NULL, message);

        std::runtime_error exception(message);
        delete[] message;
        throw exception;
    }
}

Shader::~Shader()
{
    QOpenGLFunctions *f = QOpenGLContext::currentContext()->functions();
    f->glDeleteShader(shader_id);
}

inline GLuint Shader::get_shader_id() const
{
    return shader_id;
}

// Nota: 0 rappresenta l'assenza di un programma in OpenGL.
GLuint ShaderProgram::current_program_id = 0;

ShaderProgram::ShaderProgram(const char *vertex_shader, const char *fragment_shader)
{
    GLint success;
    QOpenGLFunctions *f = QOpenGLContext::currentContext()->functions();
    Shader vertex(vertex_shader, false), fragment(fragment_shader, true);

    program_id = f->glCreateProgram();

    // Seleziona gli shader e fa il link
    f->glAttachShader(program_id, vertex.get_shader_id());
    f->glAttachShader(program_id, fragment.get_shader_id());
    f->glLinkProgram(program_id);

    // Verifica se ci sono stati errori

    f->glGetProgramiv(program_id, GL_LINK_STATUS, &success);

    if (success == GL_FALSE) {
        int info_log_length;
        f->glGetProgramiv(program_id, GL_INFO_LOG_LENGTH, &info_log_length);

        GLchar *message = new char[info_log_length];
        f->glGetProgramInfoLog(program_id, info_log_length, NULL, message);

        std::runtime_error exception(message);
        delete[] message;
        throw exception;
    }

    f->glDetachShader(program_id, vertex.get_shader_id());
    f->glDetachShader(program_id, fragment.get_shader_id());
}

ShaderProgram::~ShaderProgram()
{
    QOpenGLFunctions *f = QOpenGLContext::currentContext()->functions();
    f->glDeleteProgram(program_id);
}

void ShaderProgram::use() const
{
    // È meglio non usare glUseProgram se non è necessario.
    if (current_program_id != program_id) {
        QOpenGLFunctions *f = QOpenGLContext::currentContext()->functions();
        f->glUseProgram(program_id);
        current_program_id = program_id;
    }
}

BasicShaderProgram::BasicShaderProgram(const char *vertex_shader,
                                       const char *fragment_shader,
                                       const char *model_matrix_uniform,
                                       const char *view_matrix_uniform,
                                       const char *proj_matrix_uniform)
    : ShaderProgram(vertex_shader, fragment_shader)
{
    QOpenGLFunctions *f = QOpenGLContext::currentContext()->functions();

    model_matrix_uniform_id = f->glGetUniformLocation(get_program_id(), model_matrix_uniform);
    view_matrix_uniform_id = f->glGetUniformLocation(get_program_id(), view_matrix_uniform);
    proj_matrix_uniform_id = f->glGetUniformLocation(get_program_id(), proj_matrix_uniform);
}

void BasicShaderProgram::set_4x4_matrix(GLint uniform_id, const Matrix<GLfloat, 4, 4>& matrix)
{
    QOpenGLFunctions *f = QOpenGLContext::currentContext()->functions();

    use();
    f->glUniformMatrix4fv(uniform_id, 1, true, matrix.data_ptr());
}
