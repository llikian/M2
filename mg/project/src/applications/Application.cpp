/***************************************************************************************************
 * @file  Application.cpp
 * @brief Implementation of the Application class
 **************************************************************************************************/

#include "applications/Application.hpp"

#include "engine/EventHandler.hpp"
#include "engine/Window.hpp"
#include "glad/glad.h"
#include "maths/constants.hpp"
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

Application::Application()
    : camera(vec3(0.0f, 10.0f, 0.0f), PI_HALF_F, 0.1f, 1024.0f),
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
    /* Main Loop */
    while(!Window::should_close()) {
        EventHandler::poll_and_handle_events();

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        draw();

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        Window::swap_buffers();
    }
}

void Application::draw() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}
