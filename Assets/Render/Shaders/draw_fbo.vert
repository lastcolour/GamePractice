#version 300 es

precision mediump float;

layout(location = 0) in vec3 vert;
layout(location = 1) in vec2 inTexCoords;

out vec2 texCoords;

void main() {
    gl_Position = vec4(vert, 1.f);
    texCoords = inTexCoords;
}