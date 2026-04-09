#version 460


#include "SpriteRenderer.glsl"


layout (location=0) in mat3  inTransform;
layout (location=3) in float inDepth;

layout (location=4) in vec2  inDiffuseTextureMin;
layout (location=5) in vec2  inDiffuseTextureMax;
layout (location=6) in uint  inDiffuseTexturePage;


layout (location=0) out vec2 outDiffuseTextureCoord;
layout (location=1) out flat uint outDiffuseTexturePage;


const vec2 GEOMETRY[] =
{
    vec2(-0.5, -0.5),
    vec2(0.5, -0.5),
    vec2(-0.5, 0.5),
    vec2(0.5, -0.5),
    vec2(0.5, 0.5),
    vec2(-0.5, 0.5)
};

vec2 GetDiffuseTextureCoordinate()
{
    switch (gl_VertexIndex)
    {
        case 0:
            return inDiffuseTextureMin;
        case 1:
        case 3:
            return vec2(inDiffuseTextureMax.x, inDiffuseTextureMin.y);
        case 4:
            return inDiffuseTextureMax;
        case 2:
        case 5:
            return vec2(inDiffuseTextureMin.x, inDiffuseTextureMax.y);
    }
}

void main()
{
    outDiffuseTexturePage = inDiffuseTexturePage;
    outDiffuseTextureCoord = GetDiffuseTextureCoordinate();

    gl_Position = ProjectionMatrix * vec4(
        vec2(inTransform * vec3(GEOMETRY[gl_VertexIndex], 1.0)),
        0.0f,
        1.0);
        gl_Position.xy *= pow(2, inDepth);
}