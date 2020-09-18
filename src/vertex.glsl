#version 450 core

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 uv;

uniform mat4 model_matrix;
uniform mat4 view_matrix;
uniform mat4 proj_matrix;

out vec3 int_normal;
out vec4 int_eye;

void main() {
    int_normal = vec3(transpose(inverse(view_matrix * model_matrix)) * vec4(normal, 0.0));
    int_eye = - (view_matrix * model_matrix * vec4(position + vec3(0.5), 1.0));
    gl_Position =  proj_matrix * view_matrix * model_matrix * vec4(position, 1.0);
}
