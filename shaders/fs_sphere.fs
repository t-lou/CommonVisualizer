R"(#version 330 core

struct CenteredPixel
{
  vec3 center;
  vec4 pos;
  vec4 color;
};

struct PixelNormal
{
  vec3 pos;
  vec3 nor;
  vec4 color;
  float depth;
};
vec4 light_phong(PixelNormal pixel);

in CenteredPixel fs_in_centered_pixel;
out vec4 color_out;

uniform mat4 proj;
uniform float radius;
uniform vec3 pos_eye;
uniform vec4 color_light;

void main()
{
  vec3 eye_to_center = pos_eye - fs_in_centered_pixel.center;
  vec3 ray = fs_in_centered_pixel.pos.xyz - pos_eye;

  // Solve the ray/sphere intersection without normalizing the ray.
  float a = dot(ray, ray);
  float b = dot(eye_to_center, ray) * 2.0f;
  float c = dot(eye_to_center, eye_to_center) - radius * radius;
  float under_sqrt = b * b - 4.0f * a * c;
  if(under_sqrt > 0.0f)
  {
    float t = (-b - sqrt(under_sqrt)) / (2.0f * a);
    vec4 pos_coll = vec4(ray * t + pos_eye, 1.0);
    if(color_light[3] > 0.0f)
    {
      PixelNormal real_pos;
      real_pos.pos = pos_coll.xyz;
      real_pos.nor = normalize(real_pos.pos - fs_in_centered_pixel.center);
      real_pos.color = fs_in_centered_pixel.color;

      vec4 pos_screen = proj * pos_coll;
      gl_FragDepth = pos_screen.z / pos_screen.w;
      color_out = light_phong(real_pos);
    }
    else
    {
      vec4 pos_screen = proj * pos_coll;
      gl_FragDepth = pos_screen.z / pos_screen.w;
      color_out = fs_in_centered_pixel.color;
    }
  }
  else
  {
    discard;
  }
}
)"