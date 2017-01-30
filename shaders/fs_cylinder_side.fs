R"(#version 330 core

struct CenteredLine
{
  vec4 color;
  vec4 pos;
  vec3 center;
  float radius;
  float ratio;
};

struct PixelNormal
{
  vec3 pos;
  vec3 nor;
  vec4 color;
};

vec4 light_phong(PixelNormal pixel);

in CenteredLine fs_in_cylinder;
// in CenteredLine vs_out_cylinder;

uniform mat4 proj;
uniform vec3 pos_eye;

void main()
{
  if(fs_in_cylinder.ratio > 0.0f && fs_in_cylinder.ratio < 1.0f)
  {
    gl_FragColor = vec4(1,1,1,1);
  }
  else
  {
    discard;
  }
  // gl_FragColor = vec4(1,1,1,1);
}
)"