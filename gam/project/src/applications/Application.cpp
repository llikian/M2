/***************************************************************************************************
 * @file  Application.cpp
 * @brief Implementation of the Application class
 **************************************************************************************************/

#include "applications/Application.hpp"

#include "assets/Shader.hpp"
#include "engine/EventHandler.hpp"
#include "engine/Window.hpp"
#include "glad/glad.h"
#include "maths/constants.hpp"
#include "maths/transforms.hpp"
#include "Mesh.hpp"

Application::Application()
    : camera(vec3(0.0f, 0.0f, 5.0f), PI_HALF_F, 0.1f, 1024.0f),
      are_axes_drawn(false),
      sky_color_low(0.0f, 0.105f, 0.191f),
      sky_color_high(0.123f, 0.285f, 0.583f) {
    /* ---- Event Handler ---- */
    EventHandler::set_active_camera(&camera);
    EventHandler::associate_action_to_key(GLFW_KEY_Q, false, [this] { are_axes_drawn = !are_axes_drawn; });

    /* ---- Other ---- */
    // glfwSwapInterval(0); // disable vsync
}

Application::~Application() {}

void Application::run() {
    Shader shader({ "shaders/default.vert", "shaders/default.frag" }, "Default");

    Mesh mesh;

    mesh.load_from_off("data/queen.off");
    // mesh.load_from_off("data/r2.off");

    mesh.compute_normals();
    mesh.bind_buffers();

    mesh.save_to_off("data/out.off");

    /* Main Loop */
    while(!Window::should_close()) {
        EventHandler::poll_and_handle_events();
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        shader.use();
        shader.set_uniform("u_mvp", camera.get_view_projection_matrix() * scale(10.0f));
        shader.set_uniform("u_color", vec4(1.0f));

        mesh.draw();

        Window::swap_buffers();
    }
}
