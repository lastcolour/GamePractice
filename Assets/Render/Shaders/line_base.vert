#version 300 es

precision mediump float;

uniform mat4 CameraMat;

layout(location = 0) in vec2 vert;
layout(location = 1) in vec4 color;

out vec4 vertColor;

void main() {
    gl_Position = CameraMat * vec4(vert, 0.f, 1.f);
    vertColor = color;
}