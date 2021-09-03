#version 300 es

precision mediump float;

uniform sampler2D tex;
uniform float alpha;

in vec2 texCoords;
in vec4 colorMult;

out vec4 FragColor;

void main() {
    vec4 sampled = texture(tex, texCoords);
    float a = sampled.a * colorMult.a * alpha;
    FragColor.rgb = colorMult.rgb * a;
    FragColor.a = a;
}