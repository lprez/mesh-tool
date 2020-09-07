#ifndef SHADER_H
#define SHADER_H

#include <GL/gl.h>

class Shader
{
friend class ShaderProgram;

public:
    Shader(const char *src, bool is_fragment);
    ~Shader();

    // Impedisce la copia di oggetti Shader.
    Shader& operator=(const Shader&) = delete;
    Shader(const Shader&) = delete;

protected:
    GLuint get_shader_id() const;

private:
    GLuint shader_id;
};

class ShaderProgram
{
public:
    ShaderProgram(const char *vertex_shader, const char *fragment_shader);
    ~ShaderProgram();
    void use() const;

    ShaderProgram& operator=(const ShaderProgram&) = delete;
    ShaderProgram(const ShaderProgram&) = delete;

protected:
    GLuint get_program_id() const {return program_id;}

private:
    GLuint program_id;
    static GLuint current_program_id;
};


#endif // SHADER_H
