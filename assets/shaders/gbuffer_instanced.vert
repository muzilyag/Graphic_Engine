#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 5) in mat4 aInstanceModel;

out vec3 FragPosView;
out vec3 NormalView;

uniform mat4 view;
uniform mat4 projection;

void main() {
    mat4 mv = view * aInstanceModel;
    vec4 viewPos = mv * vec4(aPos, 1.0);
    FragPosView = viewPos.xyz;
    NormalView = mat3(transpose(inverse(mv))) * aNormal;
    gl_Position = projection * viewPos;
}