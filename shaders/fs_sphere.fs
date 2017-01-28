R"(#version 330 core

struct CenteredPixel
{
  vec3 center;
  vec4 pos;
  vec4 color;
};

struct PixelNormal
{
  vec3 pos;
  vec3 nor;
  vec4 color;
};

vec4 light_phong(PixelNormal pixel);

in CenteredPixel fs_in_centered_pixel;

uniform float radius;
uniform vec3 pos_eye;

void main()
{
  vec3 eye_to_center = pos_eye - fs_in_centered_pixel.center;
  vec3 dir = normalize(fs_in_centered_pixel.pos.xyz - pos_eye);

  // radius == length(pos_eye + dir * d - center). this leads to a second order equation
  float b = dot(eye_to_center, dir) * 2.0f;
  float c = dot(eye_to_center, eye_to_center) - radius * radius;
  float under_sqrt = b * b - 4.0f * c;
  if(under_sqrt > 0.0f)
  {
    PixelNormal real_pos;
    real_pos.pos = dir * 0.5f * (-b - sqrt(under_sqrt)) + pos_eye;
    real_pos.nor = normalize(real_pos.pos - fs_in_centered_pixel.center);
    real_pos.color = fs_in_centered_pixel.color;
    gl_FragColor = light_phong(real_pos);
  }
  else
  {
    discard;
  }
}
)"