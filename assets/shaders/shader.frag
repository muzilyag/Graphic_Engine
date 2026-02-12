#version 330 core
out vec4 FragColor;

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoord;

struct Material {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float shininess;
    sampler2D diffuseMap;
    sampler2D specularMap;
    sampler2D normalMap;
};

struct PointLight {
    vec3 position;
    vec3 color;
    
    float constant;
    float linear;
    float quadratic;
};

#define NR_POINT_LIGHTS 4 // Количество ламп
uniform PointLight pointLights[NR_POINT_LIGHTS];

uniform Material material;
uniform vec3 viewPos;

vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir, vec3 diffTex, vec3 specTex) {
    vec3 lightDir = normalize(light.position - fragPos);
    
    float diff = max(dot(normal, lightDir), 0.0);
    
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    
    float distance = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));    

    vec3 ambient  = light.color * material.ambient * diffTex;
    vec3 diffuse  = light.color * (diff * material.diffuse) * diffTex;
    vec3 specular = light.color * (spec * material.specular) * specTex;
    
    return (ambient + diffuse + specular) * attenuation;
}

vec3 getNormalFromMap() {
    vec3 tangentNormal = texture(material.normalMap, TexCoord).xyz * 2.0 - 1.0;

    vec3 Q1  = dFdx(FragPos);
    vec3 Q2  = dFdy(FragPos);
    vec2 st1 = dFdx(TexCoord);
    vec2 st2 = dFdy(TexCoord);

    vec3 N   = normalize(Normal);
    vec3 T   = normalize(Q1*st2.t - Q2*st1.t);
    vec3 B   = -normalize(cross(N, T));
    mat3 TBN = mat3(T, B, N);

    return normalize(TBN * tangentNormal);
}

void main() {
    vec3 norm = getNormalFromMap();
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 diffTex = texture(material.diffuseMap, TexCoord).rgb;
    vec3 specTex = texture(material.specularMap, TexCoord).rgb;

    vec3 result = vec3(0.0);
    
    for(int i = 0; i < NR_POINT_LIGHTS; i++) {
        result += CalcPointLight(pointLights[i], norm, FragPos, viewDir, diffTex, specTex);
    }
    
    FragColor = vec4(result, 1.0);
}