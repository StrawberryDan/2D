
layout (set=0, binding=0) uniform DrawConstants
{
    mat4 ProjectionMatrix;
};

layout (set=1,binding=0) uniform sampler2DArray DiffuseTexture;