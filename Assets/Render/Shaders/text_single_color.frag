#version 300 es

precision mediump float;

uniform sampler2D tex;
uniform vec4 color;
uniform float alpha;

in vec2 texCoords;

out vec4 FragColor;

void main() {
    vec4 sampled = vec4(vec3(1.f), texture(tex, texCoords).r);
    FragColor = color * sampled;
    FragColor.a *= alpha;
}