#version 300 es

precision mediump float;

uniform mat4 CameraMat;

layout(location = 0) in vec4 vert;

out vec2 texCoords;

void main() {
    gl_Position = CameraMat * vec4(vert.xy, 0.0, 1.0);
    texCoords = vert.zw;
}