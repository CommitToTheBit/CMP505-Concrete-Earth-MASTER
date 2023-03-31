Texture2D textures[2];
SamplerState SampleType;

struct InputType
{
    float4 position : SV_POSITION;
    float2 tex : TEXCOORD0;
};

float4 main(InputType input) : SV_TARGET
{
    // STEP 1: Get alpha value of the vignette... 
    float4 alphaMap = textures[1].Sample(SampleType, input.tex);
    float alpha = (alphaMap.r+alphaMap.g+alphaMap.b)/3.0f;

    // STEP 2: Mix physical and vignette textures...
    return (1.0f-alpha)*textures[0].Sample(SampleType, input.tex)+alpha*float4(1.0f, 0.0f, 0.0f, 1.0f);
}