R"(#version 330 core

struct PixelNormal
{
  vec3 pos;
  vec3 nor;
  vec4 color;
  float depth;
};

vec4 light_phong(PixelNormal pixel); // in fs_sub_phong.fs

in PixelNormal posnorcolor_in_fs;
out vec4 color_out;

uniform vec4 color_light;

void main()
{
  if(color_light[3] > 0.0f)
  {
    color_out = light_phong(posnorcolor_in_fs);
  }
  else
  {
    color_out = posnorcolor_in_fs.color;
  }
  gl_FragDepth = posnorcolor_in_fs.depth;
}
)"