#version 300 es

precision mediump float;

uniform sampler2D tex;
uniform float alpha;
uniform vec4 color;

in vec2 texCoords;

out vec4 FragColor;

void main() {
    vec4 resColor = texture(tex, texCoords);
    FragColor = vec4(mix(resColor.rgb, color.rgb, color.a), resColor.a);
    FragColor.a *= alpha;
}