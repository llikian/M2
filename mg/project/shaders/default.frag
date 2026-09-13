/***************************************************************************************************
 * @file  default.frag
 * @brief Default fragment shader
 **************************************************************************************************/

#version 460 core

in vec3 v_normal;

out vec4 frag_color;

uniform vec3 u_color;

const vec3 light_direction = normalize(vec3(0.0f, 1.0f, 1.0f));

void main() {
    vec3 color = u_color * max(0.2f, dot(normalize(v_normal), light_direction));
    // vec3 color = u_color.rgb;
    frag_color = vec4(color, 1.0f);
}
