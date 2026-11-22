#version 330 core
out vec4 FragColor;

in vec2 TexCoord;

uniform sampler2D ssaoInput;
uniform sampler2D gPosition;

void main() {
    vec4 centerGeo = texture(gPosition, TexCoord);
    if (centerGeo.a < 0.5) {
        FragColor = vec4(1.0);
        return;
    }

    float centerDepth = centerGeo.z;
    vec2 texelSize = 1.0 / vec2(textureSize(ssaoInput, 0));

    float result = 0.0;
    float weightSum = 0.0;

    for (int x = -2; x <= 2; ++x) {
        for (int y = -2; y <= 2; ++y) {
            vec2 uv = TexCoord + vec2(float(x), float(y)) * texelSize;
            vec4 sampleGeo = texture(gPosition, uv);
            if (sampleGeo.a < 0.5) {
                continue;
            }
            float depthDiff = abs(centerDepth - sampleGeo.z);
            float weight = exp(-depthDiff * 2.5);
            result += texture(ssaoInput, uv).r * weight;
            weightSum += weight;
        }
    }

    float ao = (weightSum > 0.0) ? (result / weightSum) : 1.0;
    FragColor = vec4(ao, ao, ao, 1.0);
}