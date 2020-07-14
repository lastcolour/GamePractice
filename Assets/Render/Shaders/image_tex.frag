#version 300 es

precision mediump float;

uniform sampler2D tex;
uniform float alpha;

in vec2 texCoords;

out vec4 FragColor;

void main() {
    FragColor = texture(tex, texCoords);
    FragColor.a *= alpha;
}