#version 300 es

precision mediump float;

uniform sampler2D tex;

in vec2 texCoords;

out vec4 FragColor;

void main() {
    FragColor = texture(tex, texCoords);
}