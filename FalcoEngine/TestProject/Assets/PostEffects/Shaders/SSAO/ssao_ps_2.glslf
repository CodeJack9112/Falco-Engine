uniform sampler2D gbuffer;  // Depth texture
uniform sampler2D rand; // Random texture

uniform float nearClip;
uniform float farClip;

uniform float width;
uniform float height;

varying vec2 oUv0;

vec2 camerarange = vec2(nearClip, farClip);
float pw = 1.0/width*0.5;
float ph = 1.0/height*0.5;

float readDepth(in vec2 coord)
{
	float posZ = texture2D(gbuffer, coord).w;
	return posZ;
}

float compareDepths(in float depth1, in float depth2, inout int far)  
{
	float diff = (depth1 - depth2)*100; //depth difference (0-100)
	float gdisplace = 0.2; //gauss bell center
	float garea = 2.0; //gauss bell width 2

	//reduce left bell width to avoid self-shadowing
	if (diff<gdisplace){ 
		garea = 0.1;
	}else{
		far = 1;
	}
	float gauss = pow(2.7182,-2*(diff-gdisplace)*(diff-gdisplace)/(garea*garea));

	return gauss;
}

float calAO(float depth,float dw, float dh)  
{
	float temp = 0;
	float temp2 = 0;
	float coordw = oUv0.x + dw/depth;
	float coordh = oUv0.y + dh/depth;
	float coordw2 = oUv0.x - dw/depth;
	float coordh2 = oUv0.y - dh/depth;

	if (coordw  < 1.0 && coordw  > 0.0 && coordh < 1.0 && coordh  > 0.0)
	{
		vec2 coord = vec2(coordw , coordh);
		vec2 coord2 = vec2(coordw2, coordh2);
		int far = 0;
		temp = compareDepths(depth, readDepth(coord),far);

		//DEPTH EXTRAPOLATION:
		if (far > 0){
			temp2 = compareDepths(readDepth(coord2),depth,far);
			temp += (1.0-temp)*temp2; 
		}
	}

	return temp;
}

void main(void)
{
	//randomization texture:
	vec2 fres = vec2(20,20);
	vec3 random = texture2D(rand, oUv0.st*fres.xy);
	random = random*2.0-vec3(1.0);

	//initialize stuff:
	float depth = readDepth(oUv0);
	float ao = 0.0;

	for(int i=0; i<2; ++i) 
	{  
		//calculate color bleeding and ao:
		ao+=calAO(depth,  pw, ph);
		ao+=calAO(depth,  pw, -ph);
		ao+=calAO(depth,  -pw, ph);
		ao+=calAO(depth,  -pw, -ph);

		ao+=calAO(depth,  pw*1.2, 0);
		ao+=calAO(depth,  -pw*1.2, 0);
		ao+=calAO(depth,  0, ph*1.2);
		ao+=calAO(depth,  0, -ph*1.2);

		//sample jittering:
		pw += random.x*0.0007;
		ph += random.y*0.0007;

		//increase sampling area:
		pw *= 1.7;
		ph *= 1.7;
	}

	//final values, some adjusting:
	vec3 finalAO = vec3(1.0-(ao/32.0));

	gl_FragColor = vec4(0.3+finalAO*0.7,1.0);
}