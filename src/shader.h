
#ifndef SHADER_H
#define SHADER_H

#include <GL/gl.h>
#include "linear.h"

// Gestisce gli shader OpenGL
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

// Gestisce i programmi OpenGL. Di per sè non permette di impostare gli uniform,
// che potrebbero cambiare da programma a programma. Sono le classi derivate
// a definire quali uniform sono disponibili e fornire i metodi per impostarli
class ShaderProgram
{
public:
    ShaderProgram(const char *vertex_shader, const char *fragment_shader);
    ~ShaderProgram();

    // Seleziona il programma per usarlo nel rendering
    void use() const;

    ShaderProgram& operator=(const ShaderProgram&) = delete;
    ShaderProgram(const ShaderProgram&) = delete;

protected:
    GLuint get_program_id() const {return program_id;}

private:
    // ID di questo programma
    GLuint program_id;
    // ID del programma attualmente in uso
    static GLuint current_program_id;
};

// Rappresenta uno shader program che usa uniform rappresentanti le matrici
// di trasformazione, vista e proiezione
class BasicShaderProgram : public ShaderProgram
{
public:
    BasicShaderProgram(const char *vertex_shader,
                       const char *fragment_shader,
                       const char *model_matrix_uniform,
                       const char *view_matrix_uniform,
                       const char *proj_matrix_uniform
                       );

    void set_model_matrix(const Matrix<GLfloat, 4, 4>& matrix) {
        set_4x4_matrix(model_matrix_uniform_id, matrix);
    }

    void set_view_matrix(const Matrix<GLfloat, 4, 4>& matrix) {
        set_4x4_matrix(view_matrix_uniform_id, matrix);
    }

    void set_proj_matrix(const Matrix<GLfloat, 4, 4>& matrix) {
        set_4x4_matrix(proj_matrix_uniform_id, matrix);
    }
private:
    void set_4x4_matrix(GLint uniform_id, const Matrix<GLfloat, 4, 4>& matrix);
    GLint model_matrix_uniform_id;
    GLint view_matrix_uniform_id;
    GLint proj_matrix_uniform_id;
};


#endif // SHADER_H
