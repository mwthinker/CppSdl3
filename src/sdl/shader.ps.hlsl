struct VSOutput
{
    float4 position   : SV_Position;
    float2 tex        : TEXCOORD0;
    float4 color      : COLOR0;
};

Texture2D<float4> Texture : register(t0, space2);
SamplerState Sampler : register(s0, space2);

float4 main(VSOutput input) : SV_Target
{
    if (input.tex.x >= 0.0 && input.tex.y >= 0.0)
    {
        return Texture.Sample(Sampler, input.tex) * input.color;
    }
    return input.color;
}
