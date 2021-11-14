struct VSOutput
{
	float3 color : Color;
	float4 position : SV_Position;
};

VSOutput main (float2 position : Position, float3 color : Color)
{
	VSOutput vs_output;
	vs_output.position = float4(position.x, position.y, 0.0F, 1.0F);
	vs_output.color = color;

	return vs_output;
}