#version 450

vec3 colors[3] = vec3[](
    vec3(1.0, 0.0, 0.0),
    vec3(0.0, 1.0, 0.0),
    vec3(0.0, 0.0, 1.0)
);

layout(location = 0) in vec2 inPosition;

layout(location = 0) out vec3 vertexColor;

void main() {
    gl_Position = vec4(inPosition, 0.0, 1.0);

    vertexColor = colors[gl_VertexIndex];
}
