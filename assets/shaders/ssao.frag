#version 330 core
out vec4 FragColor;

in vec2 TexCoord;

uniform sampler2D gPosition;
uniform sampler2D gNormal;
uniform mat4 projection;

const int KERNEL_SIZE = 32;
const float RADIUS = 0.85;
const float BIAS = 0.025;
const float POWER = 2.0;

const float bayer16[16] = float[16](
     0.0,  8.0,  2.0, 10.0,
    12.0,  4.0, 14.0,  6.0,
     3.0, 11.0,  1.0,  9.0,
    15.0,  7.0, 13.0,  5.0
);

void main() {
    vec4 posData = texture(gPosition, TexCoord);
    if (posData.a < 0.5) {
        FragColor = vec4(1.0);
        return;
    }

    vec3 fragPos = posData.xyz;
    vec3 normal = normalize(texture(gNormal, TexCoord).xyz);

    ivec2 p = ivec2(gl_FragCoord.xy) & 3;
    float rotAngle = (bayer16[p.y * 4 + p.x] / 16.0) * 6.2831853;
    vec3 randomVec = vec3(cos(rotAngle), sin(rotAngle), 0.0);

    vec3 tRaw = randomVec - normal * dot(randomVec, normal);
    if (dot(tRaw, tRaw) < 1e-5) {
        tRaw = cross(normal, vec3(0.0, 0.0, 1.0));
    }
    vec3 tangent = normalize(tRaw);
    vec3 bitangent = cross(normal, tangent);
    mat3 TBN = mat3(tangent, bitangent, normal);

    float occlusion = 0.0;
    for (int i = 0; i < KERNEL_SIZE; ++i) {
        float fi = (float(i) + 0.5) / float(KERNEL_SIZE);
        float angle = float(i) * 2.39996323;
        float z = 0.12 + 0.88 * fract(sin(float(i) * 12.9898 + 1.234) * 43758.5453);
        float rXY = sqrt(max(1.0 - z * z, 0.0));
        vec3 sampleDir = vec3(cos(angle) * rXY, sin(angle) * rXY, z);
        float scale = mix(0.1, 1.0, fi * fi);

        vec3 samplePos = fragPos + TBN * (sampleDir * scale * RADIUS);

        vec4 offset = projection * vec4(samplePos, 1.0);
        offset.xyz /= offset.w;
        offset.xy = offset.xy * 0.5 + 0.5;

        if (offset.x < 0.0 || offset.x > 1.0 || offset.y < 0.0 || offset.y > 1.0) {
            continue;
        }

        vec4 sampleGeo = texture(gPosition, offset.xy);
        if (sampleGeo.a < 0.5) {
            continue;
        }

        float sampleDepth = sampleGeo.z;
        float rangeCheck = smoothstep(0.0, 1.0, RADIUS / max(abs(fragPos.z - sampleDepth), 0.0001));
        occlusion += ((sampleDepth >= samplePos.z + BIAS) ? 1.0 : 0.0) * rangeCheck;
    }

    float ao = 1.0 - (occlusion / float(KERNEL_SIZE));
    ao = pow(clamp(ao, 0.0, 1.0), POWER);
    FragColor = vec4(ao, ao, ao, 1.0);
}