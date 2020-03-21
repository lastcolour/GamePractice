#version 300 es

precision mediump float;

uniform mat4 MVP;
uniform sampler2D tex;

layout(location = 0) in vec3 vert;
layout(location = 1) in vec2 inTexCoords;

out vec4 vertColor;

void main() {
    gl_Position = MVP * vec4(vert, 1.0);
    vertColor = texture(tex, inTexCoords);
}