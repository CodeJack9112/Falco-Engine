#version 120

uniform vec4 skytop;
uniform vec4 skyhorizon;

varying vec3 position;
varying mat3 world;

void main()
{  
    vec3 pointOnSphere = normalize(world * position);
    float a = pointOnSphere.y;
    gl_FragColor = mix(skyhorizon, skytop, a);
}