#version 460


#include "NormalSpriteRenderer.glsl"


layout (location=0) in      vec2 inDiffuseTextureCoord;
layout (location=1) in flat uint inDiffuseTexturePage;
layout (location=2) in      vec2 inNormalTextureCoord;
layout (location=3) in flat uint inNormalTexturePage;
layout (location=4) in flat float inRotation;


layout (location=0) out vec4 outColor;


const float AMBIENT = 0.1;
const vec4 LIGHT_COLOR = vec4(1.0, 0.5, 0.5, 1.0);

void main()
{
    const vec3 LIGHT_DIRECTION = normalize(vec3(-1, -1, -0.8));

    vec4 diffuse = texture(
        DiffuseTexture,
        vec3(inDiffuseTextureCoord, inDiffuseTexturePage));

    vec3 normal = texture(
        NormalTexture,
        vec3(inNormalTextureCoord, inNormalTexturePage)).xyz;
    normal.x = 2.0 * normal.x - 1.0;
    normal.y = 2.0 * normal.y- 1.0;
    mat2 rotation = mat2(cos(inRotation), sin(inRotation), -sin(inRotation), cos(inRotation));
    normal = vec3(inverse(rotation) * normal.xy, normal.z);
    normal = normalize(normal);


    const float LAMBERTIAN = (1.0 - AMBIENT) * clamp(dot(normal, -LIGHT_DIRECTION), 0.0, 1.0);

    outColor = AMBIENT * diffuse + pow(LAMBERTIAN, 1) * diffuse * LIGHT_COLOR;
}