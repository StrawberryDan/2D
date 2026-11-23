#version 460


#include "NormalSpriteRenderer.glsl"


layout (location=0) in      vec2 inDiffuseTextureCoord;
layout (location=1) in flat uint inDiffuseTexturePage;
layout (location=2) in      vec2 inNormalTextureCoord;
layout (location=3) in flat uint inNormalTexturePage;
layout (location=4) in flat float inRotation;


layout (location=0) out vec4 outColor;


const float AMBIENT = 0.1;
const vec3 LIGHT_COLOR = vec3(2.0, 1.0, 1.0);

void main()
{
    vec4 diffuse = texture(
        DiffuseTexture,
        vec3(inDiffuseTextureCoord, inDiffuseTexturePage));

    vec4 normal = texture(
        NormalTexture,
        vec3(inNormalTextureCoord, inNormalTexturePage));

    outColor.x = normal.x;
    outColor.y = normal.y;
    outColor.z = 1.0;
    outColor.w = length(normal.xy);
    // return;

    if (normal.a > 0)
    {
        normal.x = 2.0 * normal.x - 1.0;
        normal.y = 2.0 * normal.y - 1.0;
        mat2 rotation = mat2(cos(-inRotation), sin(-inRotation), -sin(-inRotation), cos(-inRotation));
        normal.xy = rotation * normal.xy;
        normal = normalize(normal);

        float dotProduct = dot(normal.xyz, -LightDirection);
        float LAMBERTIAN = 10.0 * pow((1.0 - AMBIENT) * clamp(dotProduct, 0.0, 1.0), 1);
        LAMBERTIAN = clamp(LAMBERTIAN * -LightDirection.z, 0.0, 1.0);

        outColor.xyz = (AMBIENT * diffuse.xyz + LAMBERTIAN * diffuse.xyz) * LIGHT_COLOR;
        outColor.w = diffuse.w;
    }
    else
    {
        outColor.xyz = diffuse.xyz;
        outColor.w = diffuse.w;
    }
}
