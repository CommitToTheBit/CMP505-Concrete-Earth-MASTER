Texture2D textures[1];
SamplerState SampleType;

struct InputType
{
    float4 position : SV_POSITION;
    float2 tex : TEXCOORD0;
};

float4 main(InputType input) : SV_TARGET
{
    if (input.tex.x > 0.0f && input.tex.x < 1.0f && input.tex.y > 0.0f && input.tex.y < 1.0f)
    {
        float4 col = textures[0].Sample(SampleType, input.tex);
        return col;
    }
    return float4(input.tex.x, input.tex.y, 1.0f, 1.0f);
}