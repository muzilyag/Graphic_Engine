#version 330 core
layout (location = 0) out vec4 gPosition;
layout (location = 1) out vec4 gNormal;

in vec3 FragPosView;
in vec3 NormalView;

void main() {
    vec3 n = normalize(NormalView);
    if (!gl_FrontFacing) {
        n = -n;
    }
    gPosition = vec4(FragPosView, 1.0);
    gNormal = vec4(n, 1.0);
}