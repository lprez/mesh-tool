#version 450 core

layout(location = 0, index = 0) out vec4 out_color;
in vec3 int_normal;
in vec4 int_eye;

void main() {
    vec3 color = vec3(0.5, 0.5, 0.8);
    float diffuse_intensity = max(0, dot(normalize(int_normal), normalize(vec3(0.2, 0.2, 1))));
    out_color = vec4(color * min(diffuse_intensity + 0.3, 1), 1.0);
}
