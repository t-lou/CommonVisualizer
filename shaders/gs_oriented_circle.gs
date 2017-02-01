R"(#version 330 core

struct CenteredOrientedPoint
{
  vec3 normal;
  vec3 center;
  vec3 pos;
  vec4 color;
  float radius;
};

layout(points) in;
layout(triangle_strip, max_vertices = 4) out;

in CenteredOrientedPoint vs_out_centered_pixel[1];
out CenteredOrientedPoint fs_in_centered_pixel;

uniform mat4 proj;
uniform vec3 pos_eye;

void main()
{
  // perpendicular to up and viewing direction
  vec3 dir_to_eye = normalize(pos_eye - vs_out_centered_pixel[0].center);
  vec3 dir_up_ = normalize(cross(vs_out_centered_pixel[0].normal, dir_to_eye));
  vec3 dir_left = normalize(cross(vs_out_centered_pixel[0].normal, dir_up_));

  // four corners
  vec4 mul_up = vec4(1.0f, -1.0f, 1.0f, -1.0f);
  vec4 mul_left = vec4(1.0f, 1.0f, -1.0f, -1.0f);

  fs_in_centered_pixel.color = vs_out_centered_pixel[0].color;
  fs_in_centered_pixel.center = vs_out_centered_pixel[0].center;
  fs_in_centered_pixel.normal = vs_out_centered_pixel[0].normal;
  fs_in_centered_pixel.radius = vs_out_centered_pixel[0].radius;
  for(int i = 0; i < 4; ++i)
  {
    fs_in_centered_pixel.pos = vs_out_centered_pixel[0].center
        + (dir_up_ * mul_up[i] + dir_left * mul_left[i]) * fs_in_centered_pixel.radius;
    // fs_in_centered_pixel.pos = middle + (dir_up_ * mul_up[i] + dir_left * mul_left[i]) * radius;
    gl_Position = proj * vec4(fs_in_centered_pixel.pos, 1.0f);
    gl_Position /= gl_Position.w;
    EmitVertex();
  }

  EndPrimitive();
}

)"