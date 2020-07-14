#version 300 es

precision mediump float;

uniform vec4 color;
uniform float alpha;

out vec4 FragColor;

void main() {
    FragColor = color;
    FragColor.a *= alpha;
}