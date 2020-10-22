#version 300 es

precision mediump float;

uniform mat4 CameraMat;
uniform mat4 ModelMat;

layout(location = 0) in vec3 vert;

void main() {
    gl_Position = CameraMat * ModelMat * vec4(vert, 1.f);
}