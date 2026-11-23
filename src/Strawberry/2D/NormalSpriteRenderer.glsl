
layout (set=0, binding=0) uniform DrawConstants
{
    mat4 ProjectionMatrix;
    vec3 LightDirection;
};

layout (set=1,binding=0) uniform sampler2DArray DiffuseTexture;
layout (set=2,binding=0) uniform sampler2DArray NormalTexture;
