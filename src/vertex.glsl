#version 450 core

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 uv;
out vec3 vertexcolor;

void main() {
    gl_Position.xyz = position;
    gl_Position.w = 1.0;
    vertexcolor = normal * 0.5 + 0.5;
}
