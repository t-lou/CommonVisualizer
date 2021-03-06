R"(#version 330 core

struct PixelNormal
{
  vec3 pos;
  vec3 nor;
  vec4 color;
  float depth;
};

uniform vec3 pos_light;
uniform vec4 color_light;
uniform vec3 pos_eye;
uniform vec4 phong; // ambient, diffuse, specular, power

float saturate(float f)
{
  return min(max(f, 0.0f), 1.0f);
}

vec4 light_phong(PixelNormal pixel)
{
  vec3 l = normalize(pos_light - pixel.pos);
  vec3 h = normalize(normalize(pos_eye - pixel.pos) + l);
  vec4 c = (phong.y * saturate(dot(l, pixel.nor)) + phong.z * pow(saturate(dot(h, pixel.nor)), phong.w))
    * color_light + phong.x * pixel.color;
  return vec4(c.rgb, pixel.color.a);
}
)"