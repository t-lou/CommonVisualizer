R"(#version 330 core

struct CenteredOrientedPoint
{
  vec3 normal;
  vec3 center;
  vec3 pos;
  vec4 color;
  float radius;
};

struct PixelNormal
{
  vec3 pos;
  vec3 nor;
  vec4 color;
  float depth;
};
vec4 light_phong(PixelNormal pixel);

in CenteredOrientedPoint fs_in_centered_pixel;
out vec4 color_out;

uniform mat4 proj;
uniform vec3 pos_eye;
uniform vec4 color_light;

void main()
{
  if(length(fs_in_centered_pixel.pos - fs_in_centered_pixel.center) < fs_in_centered_pixel.radius)
  {
    if(color_light[3] > 0.0f)
    {
      PixelNormal real_pos;
      real_pos.pos = fs_in_centered_pixel.pos;
      real_pos.nor = fs_in_centered_pixel.normal;
      real_pos.color = fs_in_centered_pixel.color;

      color_out = light_phong(real_pos);
    }
    else
    {
      color_out = fs_in_centered_pixel.color;
    }
    vec4 pos_screen = proj * vec4(fs_in_centered_pixel.pos, 1.0f);
    gl_FragDepth = pos_screen.z / pos_screen.w;
  }
  else
  {
    discard;
  }
}
)"