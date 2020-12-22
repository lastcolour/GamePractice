#version 300 es

precision mediump float;

uniform mat4 CameraMat;
uniform mat4 ModelMat;

layout(location = 0) in vec2 vert;
layout(location = 1) in vec2 textPos;
layout(location = 2) in vec4 InstanceColorMult;
layout(location = 3) in mat3x2 InstanceModelMat;

out vec2 texCoords;
out vec4 colorMult;

void main() {
    vec3 pt = mat3(InstanceModelMat) * vec3(vert, 1.0);
    gl_Position = CameraMat * ModelMat * vec4(pt.x, pt.y, 0.0, 1.0);
    texCoords = textPos;
    colorMult = InstanceColorMult;
}