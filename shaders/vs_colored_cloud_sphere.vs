R"(#version 330 core

struct CenteredPixel
{
  vec3 center;
  vec4 pos;
  vec4 color;
};

layout(location = 0) in vec3 pos;
layout(location = 1) in vec4 color;

out CenteredPixel vs_out_centered_pixel;

void main()
{
  vs_out_centered_pixel.center = pos;
  vs_out_centered_pixel.pos.xyz = pos;
  vs_out_centered_pixel.pos.w = 1.0f;
  vs_out_centered_pixel.color = color;
}
)"