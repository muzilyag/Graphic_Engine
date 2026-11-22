#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoord;
layout (location = 3) in vec3 aTangent;
layout (location = 4) in vec3 aBitangent;

out vec3 FragPos;
out vec2 TexCoord;
out mat3 TBN;
out vec4 FragPosLightSpace;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform mat4 lightSpaceMatrix;

void main() {
    FragPos = vec3(model * vec4(aPos, 1.0));
    TexCoord = aTexCoord;
    FragPosLightSpace = lightSpaceMatrix * vec4(FragPos, 1.0);

    mat3 normalMatrix = mat3(transpose(inverse(model)));
    mat3 modelMat3 = mat3(model);

    vec3 N = normalize(normalMatrix * aNormal);
    vec3 T = normalize(modelMat3 * aTangent);
    T = normalize(T - dot(T, N) * N);
    vec3 B = normalize(modelMat3 * aBitangent);
    float handedness = (dot(cross(N, T), B) < 0.0) ? -1.0 : 1.0;
    B = cross(N, T) * handedness;

    TBN = mat3(T, B, N);
    
    gl_Position = projection * view * vec4(FragPos, 1.0);
}