#version 330

in float intensite_diffus;
in vec2 tc;

out vec4 fragColor;
uniform vec4 color;

uniform sampler2D tex;

void main() {
  const float intensite_ambient = 0.175;
  float intensite = mix(intensite_ambient, intensite_diffus, 0.75);
  fragColor = vec4(mix(texture(tex, tc).rgb, intensite * color.rgb, 0.8), 1.0);
  //fragColor = vec4(vec3(gl_FragCoord.z * gl_FragCoord.w), 1.0);
  //fragColor = vec4(vec3(intensite_diffus), 1.0);// * color;
  //fragColor = vec4(intensite_diffus * color.rgb, 1.0);// * color;
}