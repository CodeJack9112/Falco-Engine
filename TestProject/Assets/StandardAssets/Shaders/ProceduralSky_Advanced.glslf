uniform float _SunSize;
uniform float _SunSizeConvergence;
uniform vec4 _WorldSpaceLightPos0;

#define MIE_G (-0.990)
#define MIE_G2 0.9801

#define SKY_GROUND_THRESHOLD 0.02

varying vec4 outVertex;
varying vec3 outRayDir;
varying vec3 groundColor;
varying vec3 skyColor;
varying vec3 sunColor;
varying float skyGroundFactor;
varying mat3 modelMat;

// Calculates the Mie phase function
float getMiePhase(float eyeCos, float eyeCos2)
{
	float temp = 1.0 + MIE_G2 - 2.0 * MIE_G * eyeCos;
	temp = pow(temp, pow(_SunSize,0.65) * 10.0);
	temp = max(temp,1.0e-4); // prevent division by zero, esp. in float precision
	temp = 1.5 * ((1.0 - MIE_G2) / (2.0 + MIE_G2)) * (1.0 + eyeCos2) / temp;
	
	temp = pow(temp, .454545);
	
	return temp;
}

// Calculates the sun shape
float calcSunAttenuation(vec3 lightPos, vec3 ray)
{
	float focusedEyeCos = pow(clamp(dot(lightPos, ray), 0.0, 1.0), _SunSizeConvergence);
	return getMiePhase(-focusedEyeCos, focusedEyeCos * focusedEyeCos);
}

void main()
{
	vec3 col = vec3(0.0, 0.0, 0.0);

	vec3 ray = normalize(modelMat * outVertex.xyz);
	float y = ray.y / SKY_GROUND_THRESHOLD;

	vec3 sun = sunColor * calcSunAttenuation(_WorldSpaceLightPos0.xyz, -ray);
	col = mix(skyColor + sun, groundColor, clamp(y, 0.0, 1.0));
	
	//if(y < 0.0)
	//{
		//col += sunColor * calcSunAttenuation(_WorldSpaceLightPos0.xyz, -ray);
	//}

	gl_FragColor = vec4(col, 1.0);
}