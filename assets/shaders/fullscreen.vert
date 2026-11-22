#version 330 core

out vec2 TexCoord;

void main() {
    vec2 positions[3] = vec2[3](
        vec2(-1.0, -1.0),
        vec2( 3.0, -1.0),
        vec2(-1.0,  3.0)
    );
    vec2 uvs[3] = vec2[3](
        vec2(0.0, 0.0),
        vec2(2.0, 0.0),
        vec2(0.0, 2.0)
    );

    TexCoord = uvs[gl_VertexID];
    gl_Position = vec4(positions[gl_VertexID], 0.0, 1.0);
}