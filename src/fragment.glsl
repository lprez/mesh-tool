#version 450 core

layout(location = 0, index = 0) out vec4 color;
in vec3 vertexcolor;

void main() {
    color = vec4(vertexcolor, 1.0);
}
