/// GeoBlur -- Sebastian Mestre 2018

varying vec2 texCoord;

uniform sampler2D DepthTexture;
uniform sampler2D ReflectanceTexture;
uniform sampler2D ColorTexture;

uniform float Kernel_scale;
uniform float Kernel_radius;

uniform float ReflectanceTextureWidth;
uniform float ReflectanceTextureHeight;

float width = ReflectanceTextureWidth;
float height = ReflectanceTextureHeight;

const float pi = 3.14159265359;

//#######################################

float znear = 1.0;
float zfar = 1000.0;

//#######################################

float aspectratio = width / height;
float fovratio    = 45.0;//  / fov;

//#######################################

float getDepth(vec2 coord) {
  float zdepth = texture2D(DepthTexture, coord).w;
  return -zfar * znear / (zdepth * (zfar - znear) - zfar);
}

vec3 getViewPosition(vec2 coord) {
  vec3 pos = vec3((coord.x * 2.0 - 1.0) / fovratio, (coord.y * 2.0 - 1.0) / aspectratio / fovratio, 1.0);
  return pos * getDepth(coord);
}

vec3 getViewNormal(vec2 coord) {
  float pW = 1.0 / width;
  float pH = 1.0 / height;

  vec3 p1  = getViewPosition(coord + vec2(pW, 0.0)).xyz;
  vec3 p2  = getViewPosition(coord + vec2(0.0, pH)).xyz;
  vec3 p3  = getViewPosition(coord + vec2(-pW, 0.0)).xyz;
  vec3 p4  = getViewPosition(coord + vec2(0.0, -pH)).xyz;

  vec3 vP  = getViewPosition(coord);

  vec3 dx  = vP - p1;
  vec3 dy  = p2 - vP;
  vec3 dx2 = p3 - vP;
  vec3 dy2 = vP - p4;

  if (length(dx2) < length(dx) && coord.x - pW >= 0.0 || coord.x + pW > 1.0) {
    dx = dx2;
  }
  if (length(dy2) < length(dy) && coord.y - pH >= 0.0 || coord.y + pH > 1.0) {
    dy = dy2;
  }

  return normalize(cross(dx, dy));
  //return texture2D(NormalTexture, coord).xyz;
}

vec4 getColor(vec2 coord){
	return texture2D(ReflectanceTexture, coord);
}

void main() {
    int k_r = int(Kernel_radius);
	
    vec4 color = vec4(0.0);
    float amount = 0.0;
    
    vec2 pixely = vec2(0.0, 1.0 / height);
    vec2 pixelx = vec2(1.0 / width,  0.0);
    
    vec3 normal = getViewNormal(texCoord);
    vec3 posit = getViewPosition(texCoord);
    
    for (int i = -k_r; i <= k_r; i += 1)
	{
        for (int j = -k_r; j <= k_r; j += 1)
		{
            vec2 coord = texCoord + Kernel_scale * (pixelx * float(i) + pixely * float(j));
            vec4 sample = getColor(coord);
            vec3 nsample = getViewNormal(coord);
            vec3 psample = getViewPosition(coord);
            
            vec3 dpos = posit - psample;
            
            float weight = pow(max(dot(nsample, normal),0.0),32.0) / (1.0 + 10.0 * dot(dpos, dpos));
            color += sample * weight;
            amount += weight;
            
        }
    }
    
	vec4 col = texture2D(ColorTexture, texCoord);
    gl_FragColor = col + (color / amount);
	
	//gl_FragColor = texture2D(ReflectanceTexture, texCoord);
}