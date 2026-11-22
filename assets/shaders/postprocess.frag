#version 330 core
out vec4 FragColor;

in vec2 TexCoord;

uniform sampler2D sceneTexture;
uniform sampler2D bloomTexture;
uniform int useBloom;
uniform float exposure;
uniform float bloomStrength;

vec3 acesToneMapping(vec3 x) {
    const float a = 2.51;
    const float b = 0.03;
    const float c = 2.43;
    const float d = 0.59;
    const float e = 0.14;
    return clamp((x * (a * x + b)) / (x * (c * x + d) + e), 0.0, 1.0);
}

void main() {
    vec3 hdrColor = texture(sceneTexture, TexCoord).rgb;

    if (useBloom != 0) {
        vec3 bloomColor = texture(bloomTexture, TexCoord).rgb;
        hdrColor += bloomColor * bloomStrength;
    }

    vec3 mapped = acesToneMapping(hdrColor * exposure);
    mapped = pow(mapped, vec3(1.0 / 2.2));
    float luma = dot(mapped, vec3(0.299, 0.587, 0.114));
    FragColor = vec4(mapped, luma);
}