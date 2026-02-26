#version 120    

uniform float sunSize;
uniform vec3 lightPositionOnScreen;
uniform vec2 iResolution;

varying vec2 texCoord;

void main (void)
{
	vec3 lPos = lightPositionOnScreen;
	
	if (lPos.z < 1.0)
	{
		vec2 mouse = lPos.xy;

		mouse.y = iResolution.y - mouse.y;
		mouse /= iResolution;

		vec2 midpoint = (mouse * iResolution);
		float radius = min(iResolution.x, iResolution.y) * sunSize;
		float dist = length(gl_FragCoord.xy - midpoint);
		float circle = smoothstep(radius-1.0, radius+1.0, dist);
		
		gl_FragColor = vec4(1.0 - circle);
	}
	else
	{
		gl_FragColor = vec4(0.0, 0.0, 0.0, 0.0);
	}
}