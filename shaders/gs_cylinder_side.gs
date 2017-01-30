R"(#version 330 core

struct CenteredLine
{
  vec4 color;
  vec4 pos;
  vec3 center;
  float radius;
  float ratio;
};

layout(lines) in;
layout(triangle_strip, max_vertices = 4) out;

in CenteredLine vs_out_cylinder[2];
out CenteredLine fs_in_cylinder;

uniform mat4 proj;
uniform vec3 pos_eye;

void main()
{
  vec3 dir_cylinder = vs_out_cylinder[1].pos.xyz - vs_out_cylinder[0].pos.xyz;
  float height = length(dir_cylinder);
  dir_cylinder = normalize(dir_cylinder);
  float total_height = height + 2.0f * vs_out_cylinder[1].radius;
  vec3 dir_to_eye = normalize(pos_eye - vs_out_cylinder[0].pos.xyz);
  vec3 dir_up = normalize(cross(dir_to_eye, dir_cylinder));
  vec4 center = vec4(dir_cylinder * vs_out_cylinder[1].radius + vs_out_cylinder[0].pos.xyz, 1.0f);

  vec4 corner0 = center + vec4(dir_up, 0.0f);
  vec4 corner1 = center - vec4(dir_up, 0.0f);

  dir_to_eye = normalize(pos_eye - vs_out_cylinder[1].pos.xyz);
  dir_up = normalize(cross(dir_to_eye, dir_cylinder));
  center = vec4(dir_cylinder * vs_out_cylinder[1].radius + vs_out_cylinder[1].pos.xyz, 1.0f);

  vec4 corner2 = center + vec4(dir_up, 0.0f);
  vec4 corner3 = center - vec4(dir_up, 0.0f);

  fs_in_cylinder.color = vs_out_cylinder[0].color;
  // corner0
  fs_in_cylinder.ratio = -vs_out_cylinder[0].radius / total_height;
  fs_in_cylinder.pos = corner0;
  fs_in_cylinder.radius = vs_out_cylinder[0].radius;
  fs_in_cylinder.center = vs_out_cylinder[0].center - vs_out_cylinder[0].radius * dir_cylinder;
  gl_Position = proj *fs_in_cylinder.pos;
  EmitVertex();

  // corner1
  fs_in_cylinder.pos = corner1;
  fs_in_cylinder.center = vs_out_cylinder[0].center - vs_out_cylinder[0].radius * dir_cylinder;
  gl_Position = proj *fs_in_cylinder.pos;
  EmitVertex();

  // corner2
  fs_in_cylinder.ratio = vs_out_cylinder[1].radius / total_height + 1.0f;
  fs_in_cylinder.pos = corner2;
  fs_in_cylinder.radius = vs_out_cylinder[1].radius;
  fs_in_cylinder.center = vs_out_cylinder[1].center + vs_out_cylinder[1].radius * dir_cylinder;
  gl_Position = proj *fs_in_cylinder.pos;
  EmitVertex();

  // corner3
  fs_in_cylinder.pos = corner3;
  fs_in_cylinder.center = vs_out_cylinder[1].center + vs_out_cylinder[1].radius * dir_cylinder;
  gl_Position = proj *fs_in_cylinder.pos;
  EmitVertex();

  EndPrimitive();
}

)"