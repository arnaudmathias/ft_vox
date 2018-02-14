#version 410 core
layout (location = 0) in vec3 vert_pos;
layout (location = 1) in uint vert_data;

uniform mat4 MVP;

out vec3 color;

const vec3 normal_table[6] = vec3[6](
  vec3(0.0, 0.0, 1.0),
  vec3(0.0, 0.0, -1.0),
  vec3(1.0, 0.0, 0.0),
  vec3(-1.0, 0.0, 0.0),
  vec3(0.0, -1.0, 0.0),
  vec3(0.0, 1.0, 0.0)
);

void main() {
  //uint side_unpack = (vert_data & uint(0xff000000)) >> 24;
  uint side_unpack = (vert_data & uint(0x0000ff00)) >> 8;
  //color = vec3(side_unpack);
  //color = vec3(vert_data);
  //vec3 normal = normal_table[side_unpack];
  //vec3 normal = normal_table[vert_data];
  //color = normal;
  color = vec3(1.0, 0.0, 0.0);
  gl_Position = MVP * vec4(vert_pos, 1.0);
}
