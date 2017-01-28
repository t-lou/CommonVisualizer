R"(#version 330 core

layout(location = 0) in vec3 pos;
layout(location = 1) in vec4 color;

out vec4 color_fs_in;

uniform mat4 proj;
uniform float point_size;

void main()
{
  gl_Position = proj * vec4(pos, 1.0f);
  gl_Position /= gl_Position.w;
  gl_PointSize = point_size;
  color_fs_in = color;
}
)"