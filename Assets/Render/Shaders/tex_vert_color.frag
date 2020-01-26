#version 300 es

precision mediump float;

in vec4 vertColor;

out vec4 FragColor;

void main() {
    FragColor = vertColor;
}