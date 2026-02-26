uniform mat4 worldViewProj;
attribute vec4 vertex;
varying vec2 oUv0;

void main()
{
	gl_Position = worldViewProj * vertex;

	// Clean up inaccuracies for the UV coords
	vec2 uv = sign(vertex).xy;
	// Convert to image space
	uv = (vec2(uv.x, -uv.y) + 1.0) * 0.5;
	oUv0 = uv;
}