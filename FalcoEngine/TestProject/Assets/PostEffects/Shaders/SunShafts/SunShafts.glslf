uniform float exposure;
uniform float decay;
uniform float density;
uniform float weight;
uniform vec3 lightPositionOnScreen;
uniform vec2 iResolution;

uniform sampler2D rt0;
uniform sampler2D firstPass;

varying vec2 texCoord;

const int NUM_SAMPLES = 100;

void main()
{
	vec4 color = vec4(0.0, 0.0, 0.0, 0.0);
	
	if (lightPositionOnScreen.z < 1.0)
	{
		vec2 mouse = lightPositionOnScreen.xy;

		mouse.y = iResolution.y - mouse.y;
		mouse /= iResolution;
		
		vec2 deltaTextCoord = vec2(texCoord - mouse);
		vec2 textCoo = texCoord;
		deltaTextCoord *= 1.0 /  float(NUM_SAMPLES) * density;
		float illuminationDecay = 1.0;

		for(int i=0; i < NUM_SAMPLES; i++)
		{
			textCoo -= deltaTextCoord;
			vec4 sample = texture2D(firstPass, textCoo);
			sample *= illuminationDecay * weight;
			color += sample;
			illuminationDecay *= decay;
		}
		
		color *= exposure;
	}
	
	gl_FragColor = texture2D(rt0, texCoord) + color;
	//gl_FragColor = texture2D(firstPass, texCoord);
}