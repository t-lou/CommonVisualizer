R"(#version 330 core

struct CenteredEnd
{
  vec3 pos;
  vec3 center;
  vec4 color;
  float radius;
  float ratio;
};

layout(location = 0) in vec3 pos;
layout(location = 1) in vec4 color;
layout(location = 2) in float radius;

out CenteredEnd vs_out_centered_end;

uniform mat4 proj;

void main()
{
  vs_out_centered_end.pos = pos;
  vs_out_centered_end.center = pos;
  vs_out_centered_end.color = color;
  vs_out_centered_end.radius = radius;

  //gl_Position = proj * vec4(pos, 1.0f);
  //gl_Position /= gl_Position.w;
}
)"