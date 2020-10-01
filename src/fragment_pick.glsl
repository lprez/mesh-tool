#version 450 core

layout(location = 0, index = 0) out uint vertex_id;

flat in uint int_vertex_id;

void main() {
    vertex_id = int_vertex_id;
}
