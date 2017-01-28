R"(#version 330 core

struct PixelNormal
{
  vec3 pos;
  vec3 nor;
  vec4 color;
};

vec4 light_phong(PixelNormal pixel); // in fs_sub_phong.fs

// in vec4 color_fs_in;
in PixelNormal posnorcolor_in_fs;

uniform vec4 color_light;

void main()
{
  if(color_light[3] > 0.0f)
  {
    gl_FragColor = light_phong(posnorcolor_in_fs);
  }
  else
  {
    gl_FragColor = posnorcolor_in_fs.color;
  }
}
)"