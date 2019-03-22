#version 300 es

precision mediump float;

uniform vec4 color;
uniform sampler2D tex;

in vec2 texCoords;

out vec4 FragColor;

void main()
{
    vec4 sampled = vec4(1.0, 1.0, 1.0, texture(tex, texCoords).r);
    FragColor = color * sampled;
}