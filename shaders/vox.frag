#version 410 core

flat in vec3 frag_normal;
in vec3 frag_pos;

out vec4 frag_color;

uniform sampler2DArray texture_array;

void main() {
  vec3 light_dir = normalize(vec3(0.2, -1.0, 0.0));
  vec3 light_color = vec3(1.0, 1.0, 1.0);
  float kdiff = max(0.0, dot(frag_normal, -light_dir));

  vec2 tileUV = vec2(dot(frag_normal.zxy, frag_pos), 
                   dot(frag_normal.yzx, frag_pos));
  vec2 uv = fract(tileUV);
  vec3 ambient_color = vec4(texture(texture_array, vec3(uv, float(2.0)))).rgb;

  vec3 color = 0.1 * (kdiff * light_color) + 0.9 * ambient_color;
  frag_color = vec4(color, 1.0);
}
