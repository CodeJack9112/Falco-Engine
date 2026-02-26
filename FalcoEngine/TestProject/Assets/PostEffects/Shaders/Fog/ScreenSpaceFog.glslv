attribute vec4 normal;
attribute vec2 uv0;
attribute vec4 vertex;

varying vec2 texCoord;

uniform mat4 worldViewProj;

void main()
{
	gl_Position = worldViewProj * vertex;
	texCoord = uv0;
}