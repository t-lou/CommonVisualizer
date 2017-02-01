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

struct PixelNormal
{
  vec3 pos;
  vec3 nor;
  vec4 color;
  float depth;
};
vec4 light_phong(PixelNormal pixel);

in CenteredEnd fs_in_centered_end;
out vec4 color_out;

uniform mat4 proj;
uniform float radius;
uniform vec3 pos_eye;
uniform vec4 color_light;

void main()
{
  //    t0 = <dir, eye + view * t - origin>
  // => t0 = <dir, eye - origin> + t * <dir, view>
  //
  //    r^2 = <pos - center>
  // => r^2 = <(eye + t * view) - (origin + t0 * dir)>
  // => r^2 = <(eye + t * view) - (origin + (<dir, eye - origin> + t * <dir, view>) * dir)>
  // => r^2 = <(eye - origin - <dir, eye - origin> * dir)
  //           + t * (view - <dir, view> * dir)>
  vec3 dir_view = normalize(fs_in_centered_end.pos - pos_eye);
  vec3 origin_to_eye = pos_eye - fs_in_centered_end.origin;
  vec3 vec_part = origin_to_eye - fs_in_centered_end.dir * dot(origin_to_eye, fs_in_centered_end.dir);
  vec3 scalar_part = dir_view - fs_in_centered_end.dir * dot(fs_in_centered_end.dir, dir_view);
  float a = dot(scalar_part, scalar_part);
  float b = 2.0f * dot(scalar_part, vec_part);
  float c = dot(vec_part, vec_part) - fs_in_centered_end.radius * fs_in_centered_end.radius;
  float under_sqrt = b * b - 4.0f * a * c;

  if(under_sqrt >= 0.0f)
  {
    float t = (-b - sqrt(under_sqrt)) / (2.0f * a);
    float t0 = dot(fs_in_centered_end.dir, pos_eye + t * dir_view - fs_in_centered_end.origin);
    if(t0 > 0.0f && t0 < fs_in_centered_end.length)
    {
      PixelNormal to_light;
      to_light.pos = pos_eye + dir_view * t;
      float t0 = dot(fs_in_centered_end.dir, to_light.pos - fs_in_centered_end.origin);
      to_light.nor = normalize(to_light.pos - (fs_in_centered_end.origin + fs_in_centered_end.dir * t0));
      to_light.color = fs_in_centered_end.color;
      color_out = light_phong(to_light);

      vec4 pos_screen = proj * vec4(to_light.pos, 1.0f);
      gl_FragDepth = pos_screen.z / pos_screen.w;
    }
    else
    {
      discard;
    }
  }
  else
  {
    discard;
  }
}
)"