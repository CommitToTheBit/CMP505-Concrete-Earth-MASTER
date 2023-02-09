Texture2D textures[1];
SamplerState SampleType;

struct InputType
{
    float4 position : SV_POSITION;
    float2 tex : TEXCOORD0;
    float3 position3D : TEXCOORD2;
};

float4 main(InputType input) : SV_TARGET
{
    return float4(input.position3D.x, input.position3D.y, input.position3D.z, 1.0f);
}