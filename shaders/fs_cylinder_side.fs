R"(#version 330 core

struct CenteredEnd
{
  vec3 pos;
  vec3 center;
  vec4 color;
  float radius;
  float ratio;
};

struct PixelNormal
{
  vec3 pos;
  vec3 nor;
  vec4 color;
  float depth;
};
vec4 light_phong(PixelNormal pixel);

in CenteredEnd fs_in_centered_end;
out vec4 color_out;

uniform mat4 proj;
uniform float radius;
uniform vec3 pos_eye;
uniform vec4 color_light;

void main()
{
  if(fs_in_centered_end.ratio > 0.0f && fs_in_centered_end.ratio < 1.0f)
  {
    PixelNormal to_light;
    to_light.nor = normalize(fs_in_centered_end.pos - fs_in_centered_end.center);
    to_light.pos = fs_in_centered_end.center + to_light.nor * fs_in_centered_end.radius;
    to_light.color = fs_in_centered_end.color;
    color_out = light_phong(to_light);

    vec4 pos_screen = proj * vec4(to_light.pos, 1.0f);
    gl_FragDepth = pos_screen.z / pos_screen.w;
  }
  else
  {
    discard;
  }
}
)"