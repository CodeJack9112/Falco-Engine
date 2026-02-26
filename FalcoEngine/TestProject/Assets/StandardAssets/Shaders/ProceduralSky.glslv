attribute vec4 vertex;

varying vec3 position;
varying mat3 world;

uniform mat4 model;
uniform mat4 worldViewProj;

mat3 mat3_emu(mat4 m4)
{
  return mat3(
      m4[0][0], m4[0][1], m4[0][2],
      m4[1][0], m4[1][1], m4[1][2],
      m4[2][0], m4[2][1], m4[2][2]);
}

void main()
{
    world = mat3_emu(model);
	position = vertex.xyz;

	gl_Position = worldViewProj * vertex;
}