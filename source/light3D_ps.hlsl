Texture2D textures[2];
SamplerState SampleType;

cbuffer TimeBuffer : register(b0)
{
    float time;
};

cbuffer LightBuffer : register(b1)
{
    float4 ambientColor;
    float4 diffuseColor;
    float3 lightPosition;
    float sourceStrength;
};

struct InputType
{
    float4 position : SV_POSITION;
    float3 tex3D : TEXCOORD0;
    float3 position3D : TEXCOORD3;
    float3 normal : NORMAL;
    float3 tangent: TANGENT;
    float3 binormal : BINORMAL;
};

float4 main(InputType input) : SV_TARGET
{
    const float PI = 3.14159265;

    // STEP 1: Sample from the base textures to calculate the pixel's base colour
    //float4 textureColor = float4(input.tex3D.x, input.tex3D.y, input.tex3D.z, 1.0f);
    float4 textureColor = float4(1.0f, 1.0f, 1.0f, 1.0f);

    // STEP 2: Calculate lighting hitting pixel
    float3 lightDirection = normalize(input.position3D - lightPosition);
    float3 lightDistance = max(length(input.position3D - lightPosition), pow(sourceStrength, 0.5));
    float lightIntensity = sourceStrength*saturate(dot(input.normal, -lightDirection))/(lightDistance*lightDistance);
    float4 lightColor = ambientColor + diffuseColor * lightIntensity;
    lightColor = saturate(lightColor);

    // STEP 3: Applying lighting to pixel's base colour.
    float4 color = lightColor * textureColor;
    color.a = 0.25f;

    return color;
}
