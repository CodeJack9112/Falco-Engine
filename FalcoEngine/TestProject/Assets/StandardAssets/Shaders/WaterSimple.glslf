varying vec3 position;
varying vec3 worldNormal;
varying vec3 eyeNormal;
uniform vec3 eyePos;
uniform samplerCube envMap;
uniform float opacity;
uniform vec3 waterColor;

mat3 rotationX(float angle ) {
	return mat3(1.0,	0,		0,
			 	0, 	cos(angle),	-sin(angle),
				0, 	sin(angle),	 cos(angle));
}

mat3 rotationY(float angle ) {
	return mat3(cos(angle),		0,	sin(angle),
			 			0,		1.0,		 0,
				-sin(angle),	0,	cos(angle));
}

mat3 rotationZ(float angle){
	return mat3(cos(angle),	-sin(angle),	0,
				sin(angle),	cos(angle),		0,
						0,			0,		1);
}

void main() {
     vec3 eye = normalize(position - eyePos);
     vec3 r = reflect(eye, worldNormal);
     vec4 color = textureCube(envMap, r * rotationY(90.0) * rotationX(-90.0) * rotationZ(90.0));
     color.a = opacity;
     gl_FragColor = (color * 0.85) * vec4(waterColor, 1.0);
}