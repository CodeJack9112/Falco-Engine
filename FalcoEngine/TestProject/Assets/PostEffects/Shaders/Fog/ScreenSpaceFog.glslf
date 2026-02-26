uniform sampler2D ColorBuffer;
uniform sampler2D ZBuffer;
varying vec2 texCoord;

uniform float DistanceOffset;
uniform float Density;
uniform float LinearGradient;
//uniform float _LinearOffs;
uniform vec4 FogColor;
uniform float Near;
uniform float Far;

// Applies one of standard fog formulas, given fog coordinate (i.e. distance)
float ComputeFogFactor(float coord)
{
	float fog = 0.0;
//#if FOG_LINEAR
	fog = coord * LinearGradient;// + _LinearOffs;
// #elif FOG_EXP
	//fog = Density * coord * LinearGradient;
	//fog = 1.0 - exp2(-fog);
// #endif
	return clamp(fog, 0.0, Density);
}

// Distance-based fog
float ComputeDistance(float depth)
{
	float dist = depth * Far;
	dist -= Near;
	
	return dist;
}
	
void main()
{
	vec4 zbuffer = texture2D(ZBuffer, texCoord);
	vec4 color = texture2D(ColorBuffer, texCoord);

	// Reconstruct world space position & direction towards this screen pixel.
	float zsample = zbuffer.w;
	float depth = clamp(zsample, 0.0, 1.0);

	// Compute fog amount.
	float g = ComputeDistance(depth) - DistanceOffset;
	float fog = ComputeFogFactor(max(0.0, g));
	
	if (zbuffer.w == 0.0)
		fog = Density;

	// Lerp between source color to fog color with the fog amount.
	gl_FragColor = mix(color, FogColor, fog);
}