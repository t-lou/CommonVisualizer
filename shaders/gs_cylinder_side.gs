R"(#version 330 core

struct CenteredEnd
{
  vec3 pos;
  vec3 origin;
  vec4 color;
  float radius;
  float length;
  vec3 dir;
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
  vec3 dir_along = vs_out_centered_end[1].pos - vs_out_centered_end[0].pos;
  vec3 dir0_look = normalize(vs_out_centered_end[0].pos - pos_eye);
  vec3 dir1_look = normalize(vs_out_centered_end[1].pos - pos_eye);
  vec3 dir0_up = normalize(cross(dir_along, dir0_look));
  vec3 dir1_up = normalize(cross(dir_along, dir1_look));
  vec3 dir0_out = normalize(cross(dir_along, dir0_up));
  vec3 dir1_out = normalize(cross(dir_along, dir1_up));

  vec3 pos0 = vs_out_centered_end[0].pos + (dir0_out + dir0_up) * radius_;
  vec3 pos1 = vs_out_centered_end[0].pos + (dir0_out - dir0_up) * radius_;
  vec3 pos2 = vs_out_centered_end[1].pos + (dir1_out + dir1_up) * radius_;
  vec3 pos3 = vs_out_centered_end[1].pos + (dir1_out - dir1_up) * radius_;

  vec3 edge0_to = normalize(pos2 - pos0);
  vec3 edge1_to = normalize(pos3 - pos1);

  // extend by radius_
  pos0 -= edge0_to * radius_;
  pos1 -= edge1_to * radius_;
  pos2 += edge0_to * radius_;
  pos3 += edge1_to * radius_;

  // common
  fs_in_centered_end.length = length(dir_along);
  fs_in_centered_end.dir = normalize(dir_along);
  fs_in_centered_end.color = vs_out_centered_end[0].color;
  fs_in_centered_end.origin = vs_out_centered_end[0].pos;
  fs_in_centered_end.radius = radius_;

  // individual
  fs_in_centered_end.pos = pos0;
  gl_Position = proj * vec4(pos0, 1.0f);
  EmitVertex();

  fs_in_centered_end.pos = pos1;
  gl_Position = proj * vec4(pos1, 1.0f);
  EmitVertex();

  fs_in_centered_end.pos = pos2;
  gl_Position = proj * vec4(pos2, 1.0f);
  EmitVertex();

  fs_in_centered_end.pos = pos3;
  gl_Position = proj * vec4(pos3, 1.0f);
  EmitVertex();

  EndPrimitive();
}

)"