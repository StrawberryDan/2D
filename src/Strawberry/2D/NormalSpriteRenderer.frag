#version 460


#include "NormalSpriteRenderer.glsl"


layout (location=0) in      vec2 inDiffuseTextureCoord;
layout (location=1) in flat uint inDiffuseTexturePage;
layout (location=2) in      vec2 inNormalTextureCoord;
layout (location=3) in flat uint inNormalTexturePage;


layout (location=0) out vec4 outColor;


const float AMBIENT = 0.1;

void main()
{
    const vec3 lightDirection = vec3(-1, -1, -1);

    vec4 diffuse = texture(
        DiffuseTexture,
        vec3(inDiffuseTextureCoord, inDiffuseTexturePage));

    vec3 normal = texture(
        NormalTexture,
        vec3(inNormalTextureCoord, inNormalTexturePage)).xyz;
    normal.x = 2.0 * normal.x - 1.0;
    normal.y = 2.0 * normal.y- 1.0;


    const float LAMBERTIAN = clamp(dot(normal, -lightDirection), 0.0, 1.0);

    outColor = AMBIENT * diffuse + LAMBERTIAN * diffuse;
}