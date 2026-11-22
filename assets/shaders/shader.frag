#version 330 core
layout (location = 0) out vec4 FragColor;
layout (location = 1) out vec4 BrightColor;

in vec3 FragPos;
in vec2 TexCoord;
in mat3 TBN;
in vec4 FragPosLightSpace;

struct Material {
    vec3 albedo;
    float metallic;
    float roughness;
    float ao;
    vec3 emissive;
    float opacity;
    float alphaCutoff;
    float transmission;
    float ior;
    float refractionStrength;
    float chromaticAberration;
    sampler2D albedoMap;
    sampler2D normalMap;
    sampler2D metallicMap;
    sampler2D roughnessMap;
    sampler2D aoMap;
};

struct PointLight {
    vec3 position;
    vec3 color;
    float intensity;
    float radius;
};

struct DirectionalLight {
    vec3 direction;
    vec3 color;
    float intensity;
};

struct SpotLight {
    vec3 position;
    vec3 direction;
    vec3 color;
    float intensity;
    float radius;
    float innerCutOff;
    float outerCutOff;
};

#define MAX_POINT_LIGHTS 8
const float PI = 3.14159265359;

uniform mat4 view;
uniform mat4 projection;

uniform int activePointLights;
uniform PointLight pointLights[MAX_POINT_LIGHTS];

uniform int hasDirLight;
uniform DirectionalLight dirLight;

uniform int hasSpotLight;
uniform SpotLight spotLight;

uniform sampler2D shadowMap;
uniform sampler2D ssaoMap;
uniform sampler2D sceneColorMap;
uniform int useSSAO;
uniform int useRefraction;
uniform vec2 screenSize;
uniform vec3 sunDirection;

uniform Material material;
uniform vec3 viewPos;

float distributionGGX(vec3 N, vec3 H, float roughness) {
    float a = roughness * roughness;
    float a2 = a * a;
    float NdotH = max(dot(N, H), 0.0);
    float NdotH2 = NdotH * NdotH;

    float denom = NdotH2 * (a2 - 1.0) + 1.0;
    return a2 / max(PI * denom * denom, 0.000001);
}

float geometrySchlickGGX(float NdotV, float roughness) {
    float r = roughness + 1.0;
    float k = (r * r) / 8.0;
    return NdotV / max(NdotV * (1.0 - k) + k, 0.000001);
}

float geometrySmith(vec3 N, vec3 V, vec3 L, float roughness) {
    float NdotV = max(dot(N, V), 0.0);
    float NdotL = max(dot(N, L), 0.0);
    float ggx2 = geometrySchlickGGX(NdotV, roughness);
    float ggx1 = geometrySchlickGGX(NdotL, roughness);
    return ggx1 * ggx2;
}

vec3 fresnelSchlick(float cosTheta, vec3 F0) {
    return F0 + (1.0 - F0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
}

vec3 fresnelSchlickRoughness(float cosTheta, vec3 F0, float roughness) {
    return F0 + (max(vec3(1.0 - roughness), F0) - F0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
}

vec3 calcBRDF(vec3 N, vec3 V, vec3 L, vec3 albedo, float metallic, float roughness, vec3 F0, float diffuseScale) {
    float NdotL = max(dot(N, L), 0.0);
    if (NdotL <= 0.0) {
        return vec3(0.0);
    }

    vec3 H = normalize(V + L);
    float NDF = distributionGGX(N, H, roughness);
    float G = geometrySmith(N, V, L, roughness);
    vec3 F = fresnelSchlick(max(dot(H, V), 0.0), F0);

    vec3 numerator = NDF * G * F;
    float denominator = 4.0 * max(dot(N, V), 0.0) * NdotL + 0.0001;
    vec3 specular = numerator / denominator;

    vec3 kS = F;
    vec3 kD = (vec3(1.0) - kS) * (1.0 - metallic) * diffuseScale;

    return (kD * albedo / PI + specular) * NdotL;
}

float calcShadow(vec4 fragPosLightSpace, vec3 normal, vec3 lightDir) {
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    projCoords = projCoords * 0.5 + 0.5;

    if (projCoords.z > 1.0 || projCoords.x < 0.0 || projCoords.x > 1.0 || projCoords.y < 0.0 || projCoords.y > 1.0) {
        return 0.0;
    }

    float currentDepth = projCoords.z;
    float bias = max(0.003 * (1.0 - max(dot(normal, lightDir), 0.0)), 0.0005);

    float shadow = 0.0;
    vec2 texelSize = 1.0 / vec2(textureSize(shadowMap, 0));
    for (int x = -1; x <= 1; ++x) {
        for (int y = -1; y <= 1; ++y) {
            float pcfDepth = texture(shadowMap, projCoords.xy + vec2(x, y) * texelSize).r;
            shadow += (currentDepth - bias > pcfDepth) ? 1.0 : 0.0;
        }
    }
    return shadow / 9.0;
}

vec3 sampleProceduralEnv(vec3 dir, vec3 sunDir, float roughness) {
    vec3 zenithColor  = vec3(0.04, 0.09, 0.24);
    vec3 horizonColor = vec3(0.32, 0.45, 0.65);
    vec3 groundColor  = vec3(0.03, 0.035, 0.05);
    vec3 avgSkyColor  = vec3(0.16, 0.22, 0.32);

    vec3 env;
    if (dir.y >= 0.0) {
        env = mix(horizonColor, zenithColor, pow(dir.y, 0.55));
    } else {
        env = mix(horizonColor, groundColor, pow(clamp(-dir.y, 0.0, 1.0), 0.35));
    }

    float sunAmt = max(dot(dir, sunDir), 0.0);
    float specExp = mix(64.0, 4.0, roughness * roughness);
    float specScale = mix(1.4, 0.2, roughness);
    vec3 sunHighlight = vec3(1.0, 0.85, 0.65) * pow(sunAmt, specExp) * specScale;

    return mix(env + sunHighlight, avgSkyColor, roughness * 0.75);
}

vec3 getNormalFromMap() {
    vec3 tangentNormal = texture(material.normalMap, TexCoord).xyz * 2.0 - 1.0;
    return normalize(TBN * tangentNormal);
}

void main() {
    vec4 albedoSample = texture(material.albedoMap, TexCoord);
    float alpha = clamp(albedoSample.a * material.opacity, 0.0, 1.0);
    if (material.alphaCutoff > 0.0 && alpha < material.alphaCutoff) {
        discard;
    }

    vec3 N = getNormalFromMap();
    if (!gl_FrontFacing) {
        N = -N;
    }

    vec3 V = normalize(viewPos - FragPos);
    vec3 albedo = pow(albedoSample.rgb, vec3(2.2)) * pow(max(material.albedo, vec3(0.0)), vec3(2.2));
    float metallic = clamp(material.metallic * texture(material.metallicMap, TexCoord).r, 0.0, 1.0);
    float roughness = clamp(material.roughness * texture(material.roughnessMap, TexCoord).r, 0.04, 1.0);
    float ao = clamp(material.ao * texture(material.aoMap, TexCoord).r, 0.0, 1.0);
    float transmission = clamp(material.transmission, 0.0, 1.0);
    float diffuseScale = 1.0 - transmission;

    float ior = max(material.ior, 1.001);
    float dielectricF0 = pow((ior - 1.0) / (ior + 1.0), 2.0);
    vec3 baseF0 = (transmission > 0.001) ? vec3(dielectricF0) : vec3(0.04);
    vec3 F0 = mix(baseF0, albedo, metallic);

    vec3 Lo = vec3(0.0);

    if (hasDirLight != 0) {
        vec3 L = normalize(-dirLight.direction);
        vec3 radiance = dirLight.color * dirLight.intensity;
        float shadow = calcShadow(FragPosLightSpace, N, L);
        Lo += (1.0 - shadow) * calcBRDF(N, V, L, albedo, metallic, roughness, F0, diffuseScale) * radiance;
    }

    if (hasSpotLight != 0) {
        vec3 toLight = spotLight.position - FragPos;
        float dist = length(toLight);
        vec3 L = toLight / max(dist, 0.0001);
        float theta = dot(L, normalize(-spotLight.direction));
        float epsilon = max(spotLight.innerCutOff - spotLight.outerCutOff, 0.0001);
        float cone = clamp((theta - spotLight.outerCutOff) / epsilon, 0.0, 1.0);

        if (cone > 0.0) {
            float distRatio = dist / max(spotLight.radius, 0.001);
            float smoothFactor = clamp(1.0 - pow(distRatio, 4.0), 0.0, 1.0);
            float attenuation = (smoothFactor * smoothFactor) / (dist * dist + 0.25);
            vec3 radiance = spotLight.color * spotLight.intensity * attenuation * cone;
            Lo += calcBRDF(N, V, L, albedo, metallic, roughness, F0, diffuseScale) * radiance;
        }
    }

    for (int i = 0; i < activePointLights && i < MAX_POINT_LIGHTS; ++i) {
        vec3 toLight = pointLights[i].position - FragPos;
        float dist = length(toLight);
        vec3 L = toLight / max(dist, 0.0001);

        float distRatio = dist / max(pointLights[i].radius, 0.001);
        float smoothFactor = clamp(1.0 - pow(distRatio, 4.0), 0.0, 1.0);
        float attenuation = (smoothFactor * smoothFactor) / (dist * dist + 0.25);

        vec3 radiance = pointLights[i].color * pointLights[i].intensity * attenuation;
        Lo += calcBRDF(N, V, L, albedo, metallic, roughness, F0, diffuseScale) * radiance;
    }

    vec3 sunDir = normalize(sunDirection);
    vec3 kSAmb = fresnelSchlickRoughness(max(dot(N, V), 0.0), F0, roughness);
    vec3 kDAmb = (vec3(1.0) - kSAmb) * (1.0 - metallic);

    vec3 skyIrradiance = mix(vec3(0.04, 0.045, 0.06), vec3(0.18, 0.25, 0.38), N.y * 0.5 + 0.5);
    vec3 diffuseAmbient = kDAmb * albedo * skyIrradiance * diffuseScale;

    vec3 R = reflect(-V, N);
    vec3 envSpec = sampleProceduralEnv(R, sunDir, roughness);
    vec3 specularAmbient = envSpec * kSAmb * (1.0 - roughness * 0.6);

    vec2 screenUV = gl_FragCoord.xy / max(screenSize, vec2(1.0));

    float ssao = 1.0;
    if (useSSAO != 0 && alpha > 0.99 && transmission < 0.001) {
        ssao = texture(ssaoMap, screenUV).r;
    }

    vec3 ambient = (diffuseAmbient + specularAmbient) * ao * ssao;
    vec3 result = ambient + Lo + material.emissive;

    float outAlpha = alpha;
    if (transmission > 0.001) {
        if (useRefraction != 0) {
            vec3 I = -V;
            float eta = 1.0 / ior;
            vec3 refrDir = refract(I, N, eta);
            if (dot(refrDir, refrDir) < 1e-5) {
                refrDir = I;
            }

            vec3 refrPos = FragPos + refrDir * material.refractionStrength;
            vec4 refrClip = projection * view * vec4(refrPos, 1.0);
            vec2 refrUV = (refrClip.xy / max(refrClip.w, 0.0001)) * 0.5 + 0.5;
            vec2 uvOffset = refrUV - screenUV;

            float maxOffset = 0.15;
            float offsetLen = length(uvOffset);
            if (offsetLen > maxOffset) {
                uvOffset *= (maxOffset / offsetLen);
            }

            float lod = roughness * 5.0;
            float ca = material.chromaticAberration;
            vec2 uvR = clamp(screenUV + uvOffset * (1.0 + ca), vec2(0.002), vec2(0.998));
            vec2 uvG = clamp(screenUV + uvOffset,              vec2(0.002), vec2(0.998));
            vec2 uvB = clamp(screenUV + uvOffset * (1.0 - ca), vec2(0.002), vec2(0.998));

            vec3 refractedColor = vec3(
                textureLod(sceneColorMap, uvR, lod).r,
                textureLod(sceneColorMap, uvG, lod).g,
                textureLod(sceneColorMap, uvB, lod).b
            );

            vec3 glassTint = mix(vec3(1.0), albedo, 0.45);
            vec3 kT = (vec3(1.0) - kSAmb) * (1.0 - metallic) * transmission;
            result += refractedColor * glassTint * kT;
            outAlpha = clamp(max(alpha, transmission), 0.0, 1.0);
        } else {
            outAlpha = clamp(alpha * (1.0 - transmission * 0.65), 0.15, 1.0);
        }
    }

    FragColor = vec4(result, outAlpha);

    float peak = max(result.r, max(result.g, result.b));
    float contrib = max(peak - 1.2, 0.0) / max(peak, 0.0001);
    BrightColor = vec4(result * contrib, outAlpha);
}