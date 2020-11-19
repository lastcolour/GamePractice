#version 300 es

precision mediump float;

uniform sampler2D tex;
uniform vec2 texSize;
uniform int vertical;

float offset[3] = float[](0.0, 1.3846153846, 3.2307692308);
float weight[3] = float[](0.2270270270, 0.3162162162, 0.0702702703);

in vec2 texCoords;

out vec4 FragColor;

void main() {
    FragColor = texture(tex, texCoords) * weight[0];
    if(vertical == 1) {
        for(int i = 1; i < 3; ++i) {
            vec2 shift = vec2(offset[i], 0.0) / texSize;
            FragColor += texture(tex, texCoords + shift) * weight[i];
            FragColor += texture(tex, texCoords - shift) * weight[i];
        }
    } else {
        for(int i = 1; i < 3; ++i) {
            vec2 shift = vec2(0.0, offset[i]) / texSize;
            FragColor += texture(tex, texCoords + shift) * weight[i];
            FragColor += texture(tex, texCoords - shift) * weight[i];
        }
    }
}