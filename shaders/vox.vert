#version 410 core
layout (location = 0) in vec4 vert_pos; //xyz = pos, w = face siding
layout (location = 1) in vec2 vert_attrib; //x = texture_id, y = biome_id

uniform mat4 MVP;
uniform mat4 M;

flat out float texture_id;
flat out vec3 frag_normal;
flat out vec3 frag_tangent;
flat out vec3 frag_bitangent;
flat out vec3 frag_biome_color;
out vec3 frag_pos;


const vec3 normal_table[6] = vec3[6](
  vec3(0.0, 0.0, 1.0),
  vec3(0.0, 0.0, -1.0),
  vec3(1.0, 0.0, 0.0),
  vec3(-1.0, 0.0, 0.0),
  vec3(0.0, -1.0, 0.0),
  vec3(0.0, 1.0, 0.0)
);

const vec3 tangent_table[6] = vec3[6](
  vec3(-1.0, 0.0, 0.0),
  vec3(1.0, 0.0, 0.0),
  vec3(0.0, 0.0, 1.0),
  vec3(0.0, 0.0, -1.0),
  vec3(1.0, 0.0, 0.0),
  vec3(-1.0, 0.0, 0.0)
);
const vec3 biome_color_lookup[6] = vec3[6](
  vec3(0.2, 0.5, 0.05),
  vec3(0.3, 0.5, 0.05),
  vec3(0.4, 0.5, 0.05),
  vec3(0.5, 0.5, 0.05),
  vec3(0.6, 0.5, 0.05),
  vec3(0.7, 0.5, 0.05)
);
	

void main() {
  frag_normal = normal_table[uint(vert_pos.w)];
  frag_tangent = tangent_table[uint(vert_pos.w)];
  frag_bitangent = cross(frag_normal, frag_tangent);
  texture_id = vert_attrib.x;
	frag_biome_color = biome_color_lookup[uint(vert_attrib.y)];
  gl_Position = MVP * vec4(vert_pos.xyz, 1.0);
  frag_pos = vec3(M * vec4(vert_pos.xyz, 1.0));
}
