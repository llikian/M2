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
#include "utility/Random.hpp"
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "Surface.hpp"

Application::Application() : camera(vec3(0.0f, 0.0f, 3.0f), PI_HALF_F, 0.1f, 1024.0f) {
    /* ---- Event Handler ---- */
    EventHandler::set_active_camera(&camera);

    /* ---- ImGui ---- */
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui::StyleColorsDark();
    ImGui::GetIO().IniFilename = "data/imgui.ini";
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
    Surface surface(0.5f);

    // surface.add<SphereBlob>(vec3(-0.6f, 0.0f, 0.0f), 1.0f);
    // surface.add<SphereBlob>(vec3(0.6f, 0.0f, 0.0f), 1.0f);
    // surface.add<CapsuleBlob>(vec3(0.0f, -2.0f, 0.0f), vec3(0.0f, 2.0f, 0.0f), 1.0f);
    // SphereBlob* sphere = surface.add<SphereBlob>(vec3(0.0f, 4.0f, 0.0f), 1.0f);

    int n = 50;
    vec3 min(-5.0f);
    vec3 max(5.0f);
    for(int i = 0; i < n; ++i) { surface.add<SphereBlob>(Random::get_vec3(min, max), Random::get_float(1.0f, 5.0f)); }

    Mesh surface_mesh = surface.compute_mesh();

    Shader shader({ "shaders/default.vert", "shaders/default.frag" }, "Default");

    /* Main Loop */
    while(!Window::should_close()) {
        EventHandler::poll_and_handle_events();

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        shader.use();
        shader.set_uniform("u_mvp", camera.get_view_projection_matrix());
        shader.set_uniform("u_color", vec3(0.541, 0.529, 0.8));

        // sphere->center.x = 2.0f * std::cos(EventHandler::get_time());
        // Mesh surface_mesh = surface.compute_mesh();
        surface_mesh.draw();

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        Window::swap_buffers();
    }
}
