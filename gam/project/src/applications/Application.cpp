/***************************************************************************************************
 * @file  Application.cpp
 * @brief Implementation of the Application class
 **************************************************************************************************/

#include "applications/Application.hpp"
#include <cmath>

#include "assets/Shader.hpp"
#include "engine/EventHandler.hpp"
#include "engine/Window.hpp"
#include "glad/glad.h"
#include "maths/constants.hpp"
#include "maths/transforms.hpp"
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "Mesh.hpp"

Application::Application()
    : camera(vec3(0.0f, 0.0f, 5.0f), PI_HALF_F, 0.1f, 1024.0f),
      are_axes_drawn(false),
      sky_color_low(0.0f, 0.105f, 0.191f),
      sky_color_high(0.123f, 0.285f, 0.583f) {
    /* ---- Event Handler ---- */
    EventHandler::set_active_camera(&camera);
    EventHandler::associate_action_to_key(GLFW_KEY_Q, false, [this] { are_axes_drawn = !are_axes_drawn; });

    /* ---- ImGui ---- */
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui::StyleColorsDark();
    ImGui::GetIO().IniFilename = "imgui.ini";
    ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    ImGui_ImplGlfw_InitForOpenGL(Window::get_glfw(), true);
    ImGui_ImplOpenGL3_Init();

    /* ---- Other ---- */
    // glfwSwapInterval(0); // disable vsync
}

Application::~Application() {
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}

void Application::run() {
    Shader shader({ "shaders/default.vert", "shaders/default.frag" }, "Default");

    Mesh mesh;

    // mesh.load_from_off("data/queen.off");
    // mesh.load_from_off("data/r2.off");

    float radius = 2.0f;
    vec3 top(0.0f, radius, 0.0f);
    vec3 A = radius * vec3(std::cos(0.0f), 0.0f, std::sin(0.0f));
    vec3 B = radius * vec3(std::cos(4.0f * PI_F / 3.0f), 0.0f, std::sin(4.0f * PI_F / 3.0f));
    vec3 C = radius * vec3(std::cos(2.0f * PI_F / 3.0f), 0.0f, std::sin(2.0f * PI_F / 3.0f));
    mesh.make_tetrahedron(top, A, B, C);

    mesh.compute_normals();
    mesh.bind_buffers();

    // mesh.save_to_off("data/out.off");

    glLineWidth(3.0f);
    /* Main Loop */
    while(!Window::should_close()) {
        EventHandler::poll_and_handle_events();

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
        ImGui::DockSpaceOverViewport(0, nullptr, ImGuiDockNodeFlags_PassthruCentralNode);

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        shader.use();
        shader.set_uniform("u_mvp", camera.get_view_projection_matrix() * scale(10.0f));
        shader.set_uniform("u_color", vec4(1.0f));
        shader.set_uniform("u_camera_front", camera.get_direction());

        mesh.draw();

        ImGui::Begin("Debug");

        mesh.draw_imgui_table();

        if(ImGui::Button("Split Face")) { mesh.edge_split(0, 0, (top + A) / 2.0f); }

        ImGui::End();

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        Window::swap_buffers();
    }
}
