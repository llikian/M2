/***************************************************************************************************
 * @file  default.frag
 * @brief Default fragment shader
 **************************************************************************************************/

#version 460 core

in vec3 v_normal;

out vec4 frag_color;

uniform vec4 u_color;
uniform vec3 u_camera_front;

// const vec3 light_direction = normalize(vec3(0.0f, 1.0f, 1.0f));

void main() {
    vec3 light_direction = normalize(-u_camera_front);
    float cos_theta = max(0.0f, dot(normalize(v_normal), light_direction));
    float ambient = (cos_theta * (1.0f - 0.3f) + 0.3f);

    vec3 color = u_color.rgb * ambient;
    frag_color = vec4(color, u_color.a);
}
