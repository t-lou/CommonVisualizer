R"(#version 330 core

struct CenteredPixel
{
  vec3 center;
  vec4 pos;
  vec4 color;
};

layout(points) in;
layout(triangle_strip, max_vertices = 4) out;

in CenteredPixel vs_out_centered_pixel[1];
out CenteredPixel fs_in_centered_pixel;

uniform mat4 proj;
uniform vec3 dir_up;
uniform vec3 pos_eye;
uniform float radius;

void main()
{
  // perpendicular to up and viewing direction
  vec3 dir_to_eye = normalize(pos_eye - vs_out_centered_pixel[0].center);
  vec3 dir_left = normalize(cross(dir_to_eye, dir_up));
  vec3 dir_up_ = normalize(cross(dir_left, dir_to_eye));
  vec3 middle = vs_out_centered_pixel[0].center + dir_to_eye * radius;

  // four corners
  vec4 mul_up = vec4(1.0f, -1.0f, 1.0f, -1.0f);
  vec4 mul_left = vec4(1.0f, 1.0f, -1.0f, -1.0f);

  fs_in_centered_pixel.color = vs_out_centered_pixel[0].color;
  fs_in_centered_pixel.center = vs_out_centered_pixel[0].center;
  fs_in_centered_pixel.pos.w = 1.0f;
  for(int i = 0; i < 4; ++i)
  {
    fs_in_centered_pixel.pos.xyz = middle + (dir_up_ * mul_up[i] + dir_left * mul_left[i]) * radius;
    gl_Position = proj * fs_in_centered_pixel.pos;
    // gl_Position /= gl_Position.w;
    EmitVertex();
  }

  EndPrimitive();
}

)"