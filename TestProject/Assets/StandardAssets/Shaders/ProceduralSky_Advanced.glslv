attribute vec4 vertex;

uniform mat4 model;

uniform mat4 worldViewProj;
uniform vec3 _SkyTint;
uniform float _Exposure;
uniform vec3 _GroundColor;
uniform float _SunSize;
uniform float _SunSizeConvergence;
uniform float _AtmosphereThickness;
uniform vec3 _LightColor0;
uniform vec4 _WorldSpaceLightPos0;

varying vec4 outVertex;
varying vec3 outRayDir;
varying vec3 groundColor;
varying vec3 skyColor;
varying vec3 sunColor;
varying float skyGroundFactor;
varying mat3 modelMat;

#define OUTER_RADIUS 1.025
#define kSUN_BRIGHTNESS 20.0    // Sun brightnes

#define kRAYLEIGH (mix(0.0, 0.0025, pow(_AtmosphereThickness,2.5)))      // Rayleigh constant

#define kMIE 0.0010             // Mie constant
#define kSUN_BRIGHTNESS 20.0    // Sun brightness

#define kMAX_SCATTER 50.0 // Maximum scattering value, to prevent math overflows on Adrenos
#define SKY_GROUND_THRESHOLD 0.02

const float kCameraHeight = 0.0001;

// RGB wavelengths
// .35 (.62=158), .43 (.68=174), .525 (.75=190)
const vec3 kDefaultScatteringWavelength = vec3(.65, .57, .475);
const vec3 kVariableRangeForScatteringWavelength = vec3(.15, .15, .15);

const float kOuterRadius = OUTER_RADIUS;
const float kOuterRadius2 = OUTER_RADIUS*OUTER_RADIUS;
const float kInnerRadius = 1.0;
const float kInnerRadius2 = 1.0;

const float kSunScale = 400.0 * kSUN_BRIGHTNESS;
const float kKmESun = kMIE * kSUN_BRIGHTNESS;
const float kKm4PI = kMIE * 4.0 * 3.14159265;
const float kScale = 1.0 / (OUTER_RADIUS - 1.0);
const float kScaleDepth = 0.25;
const float kScaleOverScaleDepth = (1.0 / (OUTER_RADIUS - 1.0)) / 0.25;
const float kSamples = 2.0; // THIS IS UNROLLED MANUALLY, DON'T TOUCH
 
const float kHDSundiskIntensityFactor = 15.0;

float scale(float inCos)
{
	float x = 1.0 - inCos;
	return 0.25 * exp(-0.00287 + x*(0.459 + x*(3.83 + x*(-6.80 + x*5.25))));
}

vec3 COLOR_2_GAMMA(vec3 col)
{
	return vec3(pow(col.x,1.0/2.2), pow(col.y,1.0/2.2), pow(col.z,1.0/2.2));
}

vec3 COLOR_2_LINEAR(vec3 col)
{
	return col;
}

// Calculates the Rayleigh phase function
float getRayleighPhase(float eyeCos2)
{
	return 0.75 + 0.75 * eyeCos2;
}

float getRayleighPhase(vec3 light, vec3 ray)
{
	float eyeCos = dot(light, ray);
	return getRayleighPhase(eyeCos * eyeCos);
}

mat3 mat3_emu(mat4 m4)
{
  return mat3(
      m4[0][0], m4[0][1], m4[0][2],
      m4[1][0], m4[1][1], m4[1][2],
      m4[2][0], m4[2][1], m4[2][2]);
}

void main()
{
	modelMat = mat3_emu(model);
	gl_Position = worldViewProj * vertex;

	vec3 kSkyTintInGammaSpace = COLOR_2_GAMMA(_SkyTint); // convert tint from Linear back to Gamma
	vec3 kScatteringWavelength = mix (
		kDefaultScatteringWavelength-kVariableRangeForScatteringWavelength,
		kDefaultScatteringWavelength+kVariableRangeForScatteringWavelength,
		vec3(1,1,1) - kSkyTintInGammaSpace); // using Tint in sRGB gamma allows for more visually linear interpolation and to keep (.5) at (128, gray in sRGB) point
	vec3 kInvWavelength = 1.0 / vec3(pow(kScatteringWavelength.x, 4.0), pow(kScatteringWavelength.y, 4.0), pow(kScatteringWavelength.z, 4.0));

	float kKrESun = kRAYLEIGH * kSUN_BRIGHTNESS;
	float kKr4PI = kRAYLEIGH * 4.0 * 3.14159265;

	vec3 cameraPos = vec3(0,kInnerRadius + kCameraHeight,0);    // The camera's current position

	// Get the ray from the camera to the vertex and its length (which is the far point of the ray passing through the atmosphere)
	vec3 eyeRay = normalize(modelMat * vertex.xyz);

	float far = 0.0;
	vec3 cIn, cOut;

	if(eyeRay.y >= 0.0)
	{
		// Sky
		// Calculate the length of the "atmosphere"
		far = sqrt(kOuterRadius2 + kInnerRadius2 * eyeRay.y * eyeRay.y - kInnerRadius2) - kInnerRadius * eyeRay.y;

		vec3 pos = cameraPos + far * eyeRay;

		// Calculate the ray's starting position, then calculate its scattering offset
		float height = kInnerRadius + kCameraHeight;
		float depth = exp(kScaleOverScaleDepth * (-kCameraHeight));
		float startAngle = dot(eyeRay, cameraPos) / height;
		float startOffset = depth*scale(startAngle);

		// Initialize the scattering loop variables
		float sampleLength = far / kSamples;
		float scaledLength = sampleLength * kScale;
		vec3 sampleRay = eyeRay * sampleLength;
		vec3 samplePoint = cameraPos + sampleRay * 0.5;

		// Now loop through the sample rays
		vec3 frontColor = vec3(0.0, 0.0, 0.0);

		//for(int i=0; i<int(kSamples); i++)
		{
			float height = length(samplePoint);
			float depth = exp(kScaleOverScaleDepth * (kInnerRadius - height));
			float lightAngle = dot(_WorldSpaceLightPos0.xyz, samplePoint) / height;
			float cameraAngle = dot(eyeRay, samplePoint) / height;
			float scatter = (startOffset + depth*(scale(lightAngle) - scale(cameraAngle)));
			vec3 attenuate = exp(-clamp(scatter, 0.0, kMAX_SCATTER) * (kInvWavelength * kKr4PI + kKm4PI));

			frontColor += attenuate * (depth * scaledLength);
			samplePoint += sampleRay;
		}
		{
			float height = length(samplePoint);
			float depth = exp(kScaleOverScaleDepth * (kInnerRadius - height));
			float lightAngle = dot(_WorldSpaceLightPos0.xyz, samplePoint) / height;
			float cameraAngle = dot(eyeRay, samplePoint) / height;
			float scatter = (startOffset + depth*(scale(lightAngle) - scale(cameraAngle)));
			vec3 attenuate = exp(-clamp(scatter, 0.0, kMAX_SCATTER) * (kInvWavelength * kKr4PI + kKm4PI));

			frontColor += attenuate * (depth * scaledLength);
			samplePoint += sampleRay;
		}

		// Finally, scale the Mie and Rayleigh colors and set up the varying variables for the pixel shader
		cIn = frontColor * (kInvWavelength * kKrESun);
		cOut = frontColor * kKmESun;
	}
	else
	{
		// Ground
		far = (-kCameraHeight) / (min(-0.001, eyeRay.y));

		vec3 pos = cameraPos + far * eyeRay;

		// Calculate the ray's starting position, then calculate its scattering offset
		float depth = exp((-kCameraHeight) * (1.0/kScaleDepth));
		float cameraAngle = dot(-eyeRay, pos);
		float lightAngle = dot(_WorldSpaceLightPos0.xyz, pos);
		float cameraScale = scale(cameraAngle);
		float lightScale = scale(lightAngle);
		float cameraOffset = depth*cameraScale;
		float temp = (lightScale + cameraScale);

		// Initialize the scattering loop variables
		float sampleLength = far / kSamples;
		float scaledLength = sampleLength * kScale;
		vec3 sampleRay = eyeRay * sampleLength;
		vec3 samplePoint = cameraPos + sampleRay * 0.5;

		// Now loop through the sample rays
		vec3 frontColor = vec3(0.0, 0.0, 0.0);
		vec3 attenuate;
//      for(int i=0; i<int(kSamples); i++) // Loop removed because we kept hitting SM2.0 temp variable limits. Doesn't affect the image too much.
		{
			float height = length(samplePoint);
			float depth = exp(kScaleOverScaleDepth * (kInnerRadius - height));
			float scatter = depth*temp - cameraOffset;
			attenuate = exp(-clamp(scatter, 0.0, kMAX_SCATTER) * (kInvWavelength * kKr4PI + kKm4PI));
			frontColor += attenuate * (depth * scaledLength);
			samplePoint += sampleRay;
		}

		cIn = frontColor * (kInvWavelength * kKrESun + kKmESun);
		cOut = clamp(attenuate, 0.0, 1.0);
	}

	outVertex          = -vertex;

	groundColor = _Exposure * (cIn + COLOR_2_LINEAR(_GroundColor) * cOut);
	skyColor    = _Exposure * (cIn * getRayleighPhase(_WorldSpaceLightPos0.xyz, -eyeRay));

	float lightColorIntensity = clamp(length(_LightColor0.xyz), 0.25, 1.0);
	sunColor    = kHDSundiskIntensityFactor * clamp(cOut, 0.0, 1.0) * _LightColor0.xyz / lightColorIntensity;
}