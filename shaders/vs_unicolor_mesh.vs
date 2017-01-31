R"(#version 330 core

struct PixelNormal
{
  vec3 pos;
  vec3 nor;
  vec4 color;
  float depth;
};

layout(location = 0) in vec3 pos;
layout(location = 1) in vec3 nor;

out PixelNormal posnorcolor_in_fs;

uniform mat4 proj;
uniform vec4 color;

void main()
{
  gl_Position = proj * vec4(pos, 1.0f);
  gl_Position /= gl_Position.w;
  posnorcolor_in_fs.color = color;
  posnorcolor_in_fs.pos = pos;
  posnorcolor_in_fs.nor = nor;
  posnorcolor_in_fs.depth = gl_Position.z;
}
)"