#version 410 core

flat in vec3 frag_normal;
in vec3 frag_pos;

out vec4 frag_color;

uniform sampler2DArray texture_array;

void main() {
  vec2 tileUV = vec2(dot(frag_normal.zxy, frag_pos), 
                   dot(frag_normal.yzx, frag_pos));
  vec2 uv = fract(tileUV);
  //frag_color = vec4(texture(texture_array, vec3(frag_pos.xy, float(1.0))));
  frag_color = vec4(frag_normal, 1.0);

}
