#version 410 core
layout (location = 0) in vec3 vert_pos;
layout (location = 1) in uint vert_data;

uniform mat4 MVP;

out vec3 color;

const vec3 normal_table[6] = vec3[](
  vec3(0.0, 0.0, 1.0),
  vec3(0.0, 0.0, -1.0),
  vec3(1.0, 0.0, 0.0),
  vec3(-1.0, 0.0, 0.0),
  vec3(0.0, -1.0, 0.0),
  vec3(0.0, 1.0, 0.0)
);

void main() {
  color = vec3(cos(vert_data), sin(vert_data), tan(vert_data));
  //uint side_unpack = (vert_data & uint(0xff000000)) >> 24;
  //uint side_unpack = (vert_data & uint(0x000000ff)) >> 0;
  //vec3 normal = normal_table[side_unpack];
  vec3 normal = normal_table[vert_data];
  color = normal;
  gl_Position = MVP * vec4(vert_pos, 1.0);
}
