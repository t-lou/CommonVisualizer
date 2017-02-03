R"(#version 330 core

in float depth_fs_in;
in vec4 color_fs_in;
out vec4 color_out;

void main()
{
  color_out = color_fs_in;
  gl_FragDepth = depth_fs_in;
}
)"