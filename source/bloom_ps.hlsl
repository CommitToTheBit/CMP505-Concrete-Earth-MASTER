Texture2D textures[2];
SamplerState SampleType;

struct InputType
{
    float4 position : SV_POSITION;
    float2 tex : TEXCOORD0;
};

float4 main(InputType input) : SV_TARGET
{
    // STEP 1: Blur the colour from the bright pass...
    float r = 15.0f;
    float4 brightness = (textures[1].Sample(SampleType, input.tex)
        +textures[1].Sample(SampleType, input.tex+r*float2(1.0f/1920.0f, 0.0f))
            +textures[1].Sample(SampleType, input.tex-r*float2(1.0f/1920.0f, 0.0f))
                +textures[1].Sample(SampleType, input.tex+r*float2(0.0f, 1.0f/1080.0f))
                    +textures[1].Sample(SampleType, input.tex-r*float2(0.0f, 1.0f/1080.0f)))/5.0f;

    // STEP 2: Add this to the original texture...
    return textures[0].Sample(SampleType, input.tex)+brightness;
}