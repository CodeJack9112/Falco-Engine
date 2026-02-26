attribute vec4 vertex;
attribute vec2 uv0;

uniform mat4 worldViewProj;
uniform float rt_w;
uniform float rt_h;

varying vec4 posPos;
varying vec2 uv;

float FXAA_SUBPIX_SHIFT = 1.0/4.0;

void main(void)
{
	gl_Position = worldViewProj * vertex;
	uv = uv0;
	
	vec2 rcpFrame = vec2(1.0/rt_w, 1.0/rt_h);
	posPos.xy = uv;
	posPos.zw = uv - (rcpFrame * (0.5 + FXAA_SUBPIX_SHIFT));
}