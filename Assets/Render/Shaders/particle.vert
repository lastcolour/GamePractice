#version 300 es

precision mediump float;

uniform mat4 CameraMat;

layout(location = 0) in vec3 vert;
layout(location = 1) in vec2 textPos;
layout(location = 2) in vec4 InstanceColorMult;
layout(location = 3) in mat4 InstanceModelMat;

out vec2 texCoords;
out vec4 colorMult;

void main() {
    gl_Position = CameraMat * InstanceModelMat * vec4(vert, 1.0);
    texCoords = textPos;
    colorMult = InstanceColorMult;
}