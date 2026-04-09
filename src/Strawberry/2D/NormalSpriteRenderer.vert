#version 460


#include "SpriteRenderer.glsl"


layout (location=0) in vec3 inPosition;
layout (location=1) in vec3 inExtent;

layout (location=2) in vec2  inDiffuseTextureMin;
layout (location=3) in vec2  inDiffuseTextureMax;
layout (location=4) in uint  inDiffuseTexturePage;

layout (location=5) in vec2  inNormalTextureMin;
layout (location=6) in vec2  inNormalTextureMax;
layout (location=7) in uint  inNormalTexturePage;


layout (location=0) out      vec2 outDiffuseTextureCoord;
layout (location=1) out flat uint outDiffuseTexturePage;
layout (location=2) out      vec2 outNormalTextureCoord;
layout (location=3) out flat uint outNormalTexturePage;
layout (location=4) out flat float outRotation;


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

vec2 GetNormalTextureCoordinate()
{
    switch (gl_VertexIndex)
    {
        case 0:
        return inNormalTextureMin;
        case 1:
        case 3:
        return vec2(inNormalTextureMax.x, inNormalTextureMin.y);
        case 4:
        return inNormalTextureMax;
        case 2:
        case 5:
        return vec2(inNormalTextureMin.x, inNormalTextureMax.y);
    }
}

void main()
{
    const float inRotation = inExtent.z;

    outDiffuseTexturePage = inDiffuseTexturePage;
    outDiffuseTextureCoord = GetDiffuseTextureCoordinate();
    outNormalTexturePage = inNormalTexturePage;
    outNormalTextureCoord = GetNormalTextureCoordinate();

    mat2 rotation = mat2(cos(inRotation), sin(inRotation), -sin(inRotation), cos(inRotation));

    gl_Position = ProjectionMatrix * vec4(
        vec2(inPosition.xy + rotation * inExtent.xy * GEOMETRY[gl_VertexIndex]),
        0.0f,
        1.0);
    gl_Position.xy *= pow(2, inPosition.z);
    outRotation = inRotation;
}