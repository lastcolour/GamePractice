#version 300 es

precision mediump float;

uniform sampler2D tex;

in vec2 texCoords;
in vec4 colorMult;

out vec4 FragColor;

void main() {
    FragColor = texture(tex, texCoords) * colorMult;
}