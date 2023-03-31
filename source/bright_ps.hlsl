Texture2D textures[1];
SamplerState SampleType;

cbuffer StressBuffer : register(b5)
{
    float stress;
};

struct InputType
{
    float4 position : SV_POSITION;
    float2 tex : TEXCOORD0;
};

float4 main(InputType input) : SV_TARGET
{
    // STEP 1: Get the greyscale value of the texture...
    float4 colour = textures[0].Sample(SampleType, input.tex);
    float greyscale = (colour.r+colour.g+colour.b)/3.0f;

    // STEP 2: Display colour, if it's bright enough...
    return (greyscale >= 0.95f) ? colour : float4(0.0f, 0.0f, 0.0f, 1.0f);
}