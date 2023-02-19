#version 300 es

precision mediump float;

uniform float alpha;
uniform vec4 color;

in vec4 vertColor;

out vec4 FragColor;

void main() {
    FragColor = vec4(mix(vertColor.rgb, color.rgb, color.a), vertColor.a * alpha);
}