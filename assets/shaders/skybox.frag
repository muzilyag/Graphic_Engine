#version 330 core
layout (location = 0) out vec4 FragColor;
layout (location = 1) out vec4 BrightColor;

in vec3 WorldDir;

uniform vec3 sunDirection;

void main() {
    vec3 dir = normalize(WorldDir);
    vec3 sunDir = normalize(sunDirection);

    float y = dir.y;

    vec3 zenithColor  = vec3(0.04, 0.09, 0.24);
    vec3 horizonColor = vec3(0.32, 0.45, 0.65);
    vec3 groundColor  = vec3(0.03, 0.035, 0.05);

    vec3 sky;
    if (y >= 0.0) {
        float t = pow(y, 0.55);
        sky = mix(horizonColor, zenithColor, t);
    } else {
        float t = pow(clamp(-y, 0.0, 1.0), 0.35);
        sky = mix(horizonColor, groundColor, t);
    }

    float sunAmt = max(dot(dir, sunDir), 0.0);
    vec3 sunGlow = vec3(1.0, 0.65, 0.35) * pow(sunAmt, 12.0) * 0.45;
    vec3 sunDisc = vec3(1.0, 0.92, 0.78) * smoothstep(0.9985, 0.9995, sunAmt) * 4.0;

    vec3 color = sky + sunGlow + sunDisc;
    FragColor = vec4(color, 1.0);

    float peak = max(color.r, max(color.g, color.b));
    float contrib = max(peak - 1.0, 0.0) / max(peak, 0.0001);
    BrightColor = vec4(color * contrib, 1.0);
}