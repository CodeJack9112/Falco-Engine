#version 120    
uniform sampler2D tex0;
uniform sampler2D tex1;
varying vec2 texCoord;

void main (void)
{
	vec4 smp1 = texture2D(tex0, texCoord);
	float smp2 = texture2D(tex1, texCoord).a;
	
	if (smp2 > 0)
		gl_FragColor = vec4(0.0, 0.0, 0.0, 0.0);
	else
		gl_FragColor = smp1;
		
	//gl_FragColor = smp1;
}