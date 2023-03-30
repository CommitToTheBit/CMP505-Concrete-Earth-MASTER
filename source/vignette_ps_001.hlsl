Texture2D textures[2];
SamplerState SampleType;

struct InputType
{
    float4 position : SV_POSITION;
    float2 tex : TEXCOORD0;
};

float4 main(InputType input) : SV_TARGET
{
    return textures[0].Sample(SampleType, input.tex)+textures[1].Sample(SampleType, input.tex);
}