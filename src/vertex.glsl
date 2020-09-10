#version 450 core

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 uv;

void main() {
    gl_Position.xyz = position;
    gl_Position.w = 1.0;
}
