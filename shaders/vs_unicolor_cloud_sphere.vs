R"(#version 330 core

struct CenteredPixel
{
  vec3 center;
  vec4 pos;
  vec4 color;
};

layout(location = 0) in vec3 pos;

out CenteredPixel vs_out_centered_pixel;

uniform mat4 transform;
uniform vec4 color;

void main()
{
  vs_out_centered_pixel.pos = transform * vec4(pos, 1.0f);
  vs_out_centered_pixel.center = vs_out_centered_pixel.pos.xyz;
  vs_out_centered_pixel.color = color;
}
)"