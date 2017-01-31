R"(#version 330 core

in vec4 color_fs_in;
out vec4 color_out;

void main()
{
  color_out = color_fs_in;
}
)"