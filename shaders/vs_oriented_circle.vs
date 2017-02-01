R"(#version 330 core

struct CenteredOrientedPoint
{
  vec3 normal;
  vec3 center;
  vec3 pos;
  vec4 color;
  float radius;
};

layout(location = 0) in vec3 pos;
layout(location = 1) in vec3 nor;
layout(location = 2) in vec4 color;
layout(location = 3) in float radius;

out CenteredOrientedPoint vs_out_centered_pixel;

void main()
{
  vs_out_centered_pixel.center = pos;
  vs_out_centered_pixel.pos = pos;
  vs_out_centered_pixel.normal = nor;
  vs_out_centered_pixel.color = color;
  vs_out_centered_pixel.radius = radius;
}
)"