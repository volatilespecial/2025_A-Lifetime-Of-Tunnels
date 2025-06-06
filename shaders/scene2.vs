#version 330

layout(location = 0) in vec3 pos;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 texCoord;

uniform mat4 proj;
uniform mat4 modview;
uniform vec4 Lp;

uniform float tex_scale;

out float intensite_diffus;

out vec2 tc;

void main() {
  const float dmax = 6.0f;
  vec4 p = modview * vec4(pos, 1.0);
  vec4 n = transpose(inverse(modview)) * vec4(normal, 0.0);
  vec3 Ldo = p.xyz - Lp.xyz;
  float d = length(Ldo);
  float attenuation = pow(1.0 - clamp(d / dmax, 0.0, 1.0), 0.5);
  vec3 Ld = Ldo / d;
  intensite_diffus = attenuation * clamp(dot(normalize(n.xyz), -Ld), 0.0, 1.0);
  gl_Position = proj * p;
  tc = texCoord * tex_scale;
}
