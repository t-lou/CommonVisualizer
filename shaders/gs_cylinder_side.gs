R"(#version 330 core

struct CenteredEnd
{
  vec3 pos;
  vec3 center;
  vec4 color;
  float radius;
  float ratio;
};

layout(lines) in;
layout(triangle_strip, max_vertices = 4) out;

in CenteredEnd vs_out_centered_end[2];
out CenteredEnd fs_in_centered_end;

uniform mat4 proj;
uniform vec3 dir_up;
uniform vec3 pos_eye;

void main()
{
  float radius_ = vs_out_centered_end[0].radius;
  vec3 dir_along = vs_out_centered_end[1].center - vs_out_centered_end[0].center;
  vec3 dir0_look = normalize(vs_out_centered_end[0].pos - pos_eye);
  vec3 dir1_look = normalize(vs_out_centered_end[1].pos - pos_eye);
  vec3 dir0_up = normalize(cross(dir_along, dir0_look));
  vec3 dir1_up = normalize(cross(dir_along, dir1_look));

  vec3 pos0 = vs_out_centered_end[0].pos + dir0_up * radius_;
  vec3 pos1 = vs_out_centered_end[0].pos - dir0_up * radius_;
  vec3 pos2 = vs_out_centered_end[1].pos + dir1_up * radius_;
  vec3 pos3 = vs_out_centered_end[1].pos - dir1_up * radius_;

  vec3 edge0_to = normalize(pos0 - pos2);
  vec3 edge1_to = normalize(pos1 - pos3);

  pos0 += edge0_to * radius_;
  pos2 -= edge0_to * radius_;
  pos1 += edge1_to * radius_;
  pos3 -= edge1_to * radius_;

  float full_ratio = length(dir_along) + 2.0f * radius_;
  dir_along = normalize(dir_along);

  fs_in_centered_end.radius = radius_;
  fs_in_centered_end.color = vs_out_centered_end[0].color;
  fs_in_centered_end.ratio = -radius_ / full_ratio;
  fs_in_centered_end.center = vs_out_centered_end[0].center - dir_along * radius_;
  fs_in_centered_end.pos = pos0;
  gl_Position = proj * vec4(pos0, 1.0f);
  EmitVertex();

  fs_in_centered_end.pos = pos1;
  gl_Position = proj * vec4(pos1, 1.0f);
  EmitVertex();

  fs_in_centered_end.ratio = (full_ratio - radius_) / full_ratio;
  fs_in_centered_end.center = vs_out_centered_end[1].center + dir_along * radius_;
  fs_in_centered_end.pos = pos2;
  gl_Position = proj * vec4(pos2, 1.0f);
  EmitVertex();

  fs_in_centered_end.pos = pos3;
  gl_Position = proj * vec4(pos3, 1.0f);
  EmitVertex();

  EndPrimitive();
}

)"