R"(#version 330 core

struct CenteredLine
{
  vec4 color;
  vec4 pos;
  vec3 center;
  float radius;
  float ratio;
};

layout(location = 0) in vec3 pos;
layout(location = 1) in float radius;
layout(location = 2) in vec4 color;

out CenteredLine vs_out_cylinder;

void main()
{
  vs_out_cylinder.pos = vec4(pos, 1.0f);
  vs_out_cylinder.center = pos;
  vs_out_cylinder.radius = radius;
  vs_out_cylinder.color = color;
  vs_out_cylinder.ratio = 0.5f;
}
)"