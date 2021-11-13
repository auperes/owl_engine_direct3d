float4 main (float4 position : POSITION) : SV_POSITION
{
	return float4(position.x, position.y, 0.0F, 1.0F);
}