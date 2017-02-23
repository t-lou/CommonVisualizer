R"(#version 330 core

struct CenteredEnd
{
  vec3 pos;
  vec3 origin;
  vec4 color;
  float radius;
  float length;
  float length_nor; // length for computing normal direction
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
  // <= x0 = <dir, eye - origin>, x1 = <dir, view>
  // => t0 = x0 + t * x1
  //
  //    (r * (1 - t0 / length))^2 = <pos - center>
  // => (r * (1 - t0 / length))^2 = <(eye + t * view) - (origin + t0 * dir)>
  // => (r * (1 - t0 / length))^2 = <(eye + t * view) - (origin + (x0 + t * x1) * dir)>
  // => (r * (1 - t0 / length))^2 = <(eye - origin - x0 * dir) + t * (view - x1 * dir)>
  // => r^2 - 2*r^2*x0^2/length - 2*r^2*x1*t/length + r^2*x0^2/length^2 + r^2*2*x0*x1*t/length^2+r^2*x1^2*t^2/length^2
  //    = <(eye - origin - x0 * dir) + t * (view - x1 * dir)>
  // <= al*t^2 + bl*t + cl = ar*t^2 + br*t + cr

  vec3 dir_view = normalize(fs_in_centered_end.pos - pos_eye);
  vec3 origin_to_eye = pos_eye - fs_in_centered_end.origin;
  float x0 = dot(fs_in_centered_end.dir, origin_to_eye);
  float x1 = dot(fs_in_centered_end.dir, dir_view);

  vec3 vec_part = origin_to_eye - fs_in_centered_end.dir * x0;
  vec3 scalar_part = dir_view - fs_in_centered_end.dir * x1;

  float length_sq_inv = 1.0f / (fs_in_centered_end.length*fs_in_centered_end.length);
  float radius_sq = fs_in_centered_end.radius*fs_in_centered_end.radius;

  float al = radius_sq * x1*x1 * length_sq_inv;
  float bl = radius_sq * 2.0f * (length_sq_inv * x0 * x1 - x1 / fs_in_centered_end.length);
  float cl = radius_sq * (1.0f + length_sq_inv * x0*x0 - 2.0f * x0 / fs_in_centered_end.length);
  float ar = dot(scalar_part, scalar_part);
  float br = 2.0f * dot(scalar_part, vec_part);
  float cr = dot(vec_part, vec_part);
  float a = al - ar;
  float b = bl - br;
  float c = cl - cr;
  float under_sqrt = b * b - 4.0f * a * c;

  if(under_sqrt >= 0.0f)
  {
    float t = a > 0.0f ? (-b - sqrt(under_sqrt)) / (2.0f * a) : (-b + sqrt(under_sqrt)) / (2.0f * a);
    float t0 = dot(fs_in_centered_end.dir, pos_eye - fs_in_centered_end.origin + t * dir_view);
    float distance = ar * t*t + br * t + cr;
    float radius_local = (1.0f - t0 / fs_in_centered_end.length) * fs_in_centered_end.radius;
    if(t0 > 0.0f && t0 < fs_in_centered_end.length && distance < radius_local)
    {
      PixelNormal to_light;
      to_light.pos = dir_view * t + pos_eye;

      if(color_light[3] > 0.0f)
      {
        float t0 = dot(fs_in_centered_end.dir, to_light.pos - fs_in_centered_end.origin);
        to_light.nor = normalize(to_light.pos - (fs_in_centered_end.origin + fs_in_centered_end.dir
            * (fs_in_centered_end.length - fs_in_centered_end.length_nor
                 + t0 * fs_in_centered_end.length_nor / fs_in_centered_end.length)));
        to_light.color = fs_in_centered_end.color;
        color_out = light_phong(to_light);
      }
      else
      {
        color_out = fs_in_centered_end.color;
      }

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