#version 300 es

precision mediump float;

uniform float alpha;

in vec4 vertColor;

out vec4 FragColor;

void main() {
    FragColor = vertColor;
    FragColor.a *= alpha;
}