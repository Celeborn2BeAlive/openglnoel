#version 330

layout(location = 3) in vec2 aVertexPosition;
layout(location = 8) in vec3 aVertexColor;

out vec3 vColor;

void main() {
    vColor = aVertexColor;
    gl_Position = vec4(aVertexPosition, 0, 1);
}
