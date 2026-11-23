#version 330 core
layout (location = 0) out vec4 FragColor;
layout (location = 1) out vec4 BrightColor;

struct Material {
    vec3 albedo;
    float opacity;
};

uniform Material material;
uniform float bloomThreshold;

void main() {
    vec3 color = material.albedo;
    float alpha = clamp(material.opacity, 0.0, 1.0);
    FragColor = vec4(color, alpha);

    float peak = max(color.r, max(color.g, color.b));
    float contrib = max(peak - bloomThreshold, 0.0) / max(peak, 0.0001);
    BrightColor = vec4(color * contrib, alpha);
}