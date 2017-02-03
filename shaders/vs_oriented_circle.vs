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

uniform mat4 transform;

void main()
{
  vs_out_centered_pixel.pos = (transform * vec4(pos, 1.0f)).xyz;
  vs_out_centered_pixel.center = vs_out_centered_pixel.pos;
  vs_out_centered_pixel.normal = (transform * vec4(nor, 1.0f)).xyz;
  vs_out_centered_pixel.color = color;
  vs_out_centered_pixel.radius = radius;
}
)"