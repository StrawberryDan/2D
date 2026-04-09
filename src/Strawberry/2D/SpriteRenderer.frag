#version 460


#include "SpriteRenderer.glsl"


layout (location=0) in vec2 inDiffuseTextureCoord;
layout (location=1) in flat uint inDiffuseTexturePage;


layout (location=0) out vec4 outColor;


void main()
{
    const vec3 lightDirection = vec3(-1, -1, -1);

    vec4 diffuse = texture(
        DiffuseTexture,
        vec3(inDiffuseTextureCoord, inDiffuseTexturePage));

    outColor = diffuse;
}