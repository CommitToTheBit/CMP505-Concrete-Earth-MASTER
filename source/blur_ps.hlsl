Texture2D textures[1];
SamplerState SampleType;

struct InputType
{
    float4 position : SV_POSITION;
    float2 tex : TEXCOORD0;
};

float4 main(InputType input) : SV_TARGET
{
    // STEP 1: Blur the colour from the bright pass...
    float r = 3.0f*pow(2.0f,-0.5f)*length(2.0f*input.tex-float2(1.0f,1.0f));
    float4 brightness = (textures[0].Sample(SampleType, input.tex)
        +textures[0].Sample(SampleType, input.tex+r*float2(1.0f/1920.0f, 0.0f))
            +textures[0].Sample(SampleType, input.tex-r*float2(1.0f/1920.0f, 0.0f))
                +textures[0].Sample(SampleType, input.tex+r*float2(0.0f, 1.0f/1080.0f))
                    +textures[0].Sample(SampleType, input.tex-r*float2(0.0f, 1.0f/1080.0f)))/5.0f;

    // STEP 2: Add this to the original texture...
    return brightness;
}