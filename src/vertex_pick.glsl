#version 450 core

layout(location = 0) in vec3 position;
layout(location = 1) in uint vertex_id;

uniform mat4 model_matrix;
uniform mat4 view_matrix;
uniform mat4 proj_matrix;

flat out uint int_vertex_id;

void main() {
    int_vertex_id = vertex_id;
    gl_Position = proj_matrix * view_matrix * model_matrix * vec4(position, 1.0);
}
