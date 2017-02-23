R"(#version 330 core

struct CenteredEnd
{
  vec3 pos;
  vec3 origin;
  vec4 color;
  float radius;
  float length;
  float length_nor; // length for computing normal direction
  vec3 dir;
};

layout(location = 0) in vec3 pos;
layout(location = 1) in vec4 color;
layout(location = 2) in float radius;

out CenteredEnd vs_out_centered_end;

uniform mat4 transform;
uniform mat4 proj;

void main()
{
  vs_out_centered_end.pos = (transform * vec4(pos, 1.0f)).xyz;
  vs_out_centered_end.color = color;
  vs_out_centered_end.radius = radius;
}
)"