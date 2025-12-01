#include <updaters/imgui_updater.h>
#include <iostream>
#include <updaters/orchestrator.h>

#define STYLEPATH (PROJECT_SOURCE_DIR + std::string("/configs/imgui_style_default.json")).c_str()


bool ImGuiUpdater::init() {

    // Setup Dear ImGui context
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    ImGuiConfigFlags imgui_config_flags = (ImGuiConfigFlags_NavEnableKeyboard | 
                                            ImGuiConfigFlags_DockingEnable);
    io.ConfigFlags |= imgui_config_flags;

    load_imgui_style_from_json(STYLEPATH);

    ImGuiStyle style = ImGui::GetStyle();

    float main_scale = Orchestrator::get()->get_renderer().scale;

    style.ScaleAllSizes(main_scale);        // Bake a fixed style scale. (until we have a solution for dynamic style scaling, changing this requires resetting Style + calling this again)
    style.FontScaleDpi = main_scale;        // Set initial font scale. (using io.ConfigDpiScaleFonts=true makes this unnecessary. We leave both here for documentation purpose)
    io.ConfigDpiScaleFonts = true;          // [Experimental] Automatically overwrite style.FontScaleDpi in Begin() when Monitor DPI changes. This will scale fonts but _NOT_ scale sizes/padding for now.

    std::string path = PROJECT_SOURCE_DIR + std::string("/assets/Roboto/static/Roboto-Light.ttf");
    ImFont* roboto = io.Fonts->AddFontFromFileTTF(path.c_str(), 12);
    io.FontDefault = roboto;
    
    io.ConfigDpiScaleViewports = true;      // [Experimental] Scale Dear ImGui and Platform Windows when Monitor DPI changes.
    ImGui::StyleColorsDark();

    // When viewports are enabled we tweak WindowRounding/WindowBg so platform windows can look identical to regular ones.
    //if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
    //{
    //    style.WindowRounding = 0.0f;
    //    style.Colors[ImGuiCol_WindowBg].w = 1.0f;
    //}

    // Setup Platform/Renderer backends
    SDL_Window* window = Orchestrator::get()->get_renderer().sdl_window;
    SDL_GLContext gl_context = Orchestrator::get()->get_renderer().gl_context; 

    ImGui_ImplSDL2_InitForOpenGL(window, gl_context);
    ImGui_ImplOpenGL3_Init("#version 130");

    // Renders a warmup frame to ensure style/font settings are applied
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplSDL2_NewFrame();
    // ImGui::NewFrame();

    // ImGui::Render();

    return true;
}

bool ImGuiUpdater::update_state_via_event() {
    // ImGuiUpdater no longer handles event ingestion - this is now handled by EventIngester
    // This method can be used for ImGui-specific event responses (theme changes, etc.)
    auto input = Orchestrator::get()->get_input();

    if (input.just_pressed(SDL_SCANCODE_SPACE)) {
        Debug::log("Toggling Debug Console Visibility", DebugLevel::INFO);
        debug_console_state.visible = !debug_console_state.visible;
    }
    return true;
}

bool ImGuiUpdater::update_state_via_dT(float dT) {
    // Synchronize ImGui window states with InputSystem's timestamped values 
    auto& input = Orchestrator::get()->get_input();

    input.sidebar_x.update_dT(sidebar_state.pos.x, dT);
    input.sidebar_y.update_dT(sidebar_state.pos.y, dT);
    input.sidebar_width.update_dT(sidebar_state.size.x, dT);
    input.sidebar_height.update_dT(sidebar_state.size.y, dT);

    input.bottom_x.update_dT(bottom_panel_state.pos.x, dT);
    input.bottom_y.update_dT(bottom_panel_state.pos.y, dT);
    input.bottom_width.update_dT(bottom_panel_state.size.x, dT);
    input.bottom_height.update_dT(bottom_panel_state.size.y, dT);

    input.ribbon_x.update_dT(ribbon_state.pos.x, dT);
    input.ribbon_y.update_dT(ribbon_state.pos.y, dT);
    input.ribbon_width.update_dT(ribbon_state.size.x, dT);
    input.ribbon_height.update_dT(ribbon_state.size.y, dT);

    input.viewport_x.update_dT(main_window_state.pos.x, dT);
    input.viewport_y.update_dT(main_window_state.pos.y, dT);
    input.viewport_width.update_dT(main_window_state.size.x, dT);
    input.viewport_height.update_dT(main_window_state.size.y, dT);

    return true;
}

bool ImGuiUpdater::submit_render_commands() {
    auto& renderer = Orchestrator::get()->get_renderer();

    RenderCommand build_sidebar = {RenderCommandType::ImGuiWindow, [this](){this->build_sidebar();}, "build_sidebar"};
    RenderCommand build_bottom_panel = {RenderCommandType::ImGuiWindow, [this](){this->build_bottom_panel();}, "build_bottom_panel"};
    RenderCommand build_ribbon = {RenderCommandType::ImGuiWindow, [this](){this->build_ribbon();}, "build_ribbon"};
    RenderCommand build_main_window = {RenderCommandType::ImGuiWindow, [this](){this->build_main_window();}, "build_main_window"};
    RenderCommand build_debug_console = {RenderCommandType::ImGuiWindow, [this](){this->build_debug_console();}, "build_debug_console"};

    if(sidebar_state.visible) { renderer.submit_render_request(build_sidebar); }
    if(bottom_panel_state.visible) { renderer.submit_render_request(build_bottom_panel); }
    if(ribbon_state.visible) { renderer.submit_render_request(build_ribbon); }
    if(main_window_state.visible) { renderer.submit_render_request(build_main_window); }
    if(debug_console_state.visible) { renderer.submit_render_request(build_debug_console); }

    return true;
}

void ImGuiUpdater::build_sidebar() {

    ImGuiIO& io = ImGui::GetIO();
    ImVec2 full_window_size = io.DisplaySize;

    // Sidebar takes up all of the right side
    // and extends to 30% of the way along the x side on initial layout
    ImGui::SetNextWindowPos(ImVec2(full_window_size.x * 0.7, 0), ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowSize(ImVec2(full_window_size.x * 0.3, full_window_size.y), ImGuiCond_FirstUseEver);
    ImGui::Begin(sidebar_state.texts["name"].c_str(), &sidebar_state.visible);
    sidebar_state.pos = ImGui::GetWindowPos();
    sidebar_state.size = ImGui::GetWindowSize();
    ImGui::Text("Info Panel");
    ImGui::Text("%s", PROJECT_SOURCE_DIR);
    ImGui::End();
    
}

void ImGuiUpdater::build_bottom_panel() {

    ImGuiIO& io = ImGui::GetIO();
    ImVec2 full_window_size = io.DisplaySize;

    ImGui::SetNextWindowPos(ImVec2(0, full_window_size.y * 0.9), ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowSize(ImVec2(full_window_size.x, full_window_size.y * 0.1), ImGuiCond_FirstUseEver);
    ImGui::Begin(bottom_panel_state.texts["name"].c_str(), &bottom_panel_state.visible);
    bottom_panel_state.pos = ImGui::GetWindowPos();
    bottom_panel_state.size = ImGui::GetWindowSize();
    ImGui::Text("Timeline Slider Here");
    ImGui::End();
    
}

void ImGuiUpdater::build_ribbon() {

    ImGuiIO& io = ImGui::GetIO();
    ImVec2 full_window_size = io.DisplaySize;

    ImGui::SetNextWindowPos(ImVec2(0, 0), ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowSize(ImVec2(full_window_size.x, full_window_size.y * 0.1), ImGuiCond_FirstUseEver);
    ImGui::Begin(ribbon_state.texts["name"].c_str(), &ribbon_state.visible);
    ribbon_state.pos = ImGui::GetWindowPos();
    ribbon_state.size = ImGui::GetWindowSize();
    ImGui::Text("Play | Pause | Stop Buttons Here");
    ImGui::End();
    
}

void ImGuiUpdater::build_main_window() {

    ImGuiIO& io = ImGui::GetIO();
    ImVec2 full_window_size = io.DisplaySize;

    ImGui::SetNextWindowPos(ImVec2(0, 0.1 * full_window_size.y), ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowSize(ImVec2(full_window_size.x * 0.7, full_window_size.y * 0.8), ImGuiCond_FirstUseEver);
    ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.1f, 0.1f, 0.1f, 1.0f));
    ImGui::Begin(main_window_state.texts["name"].c_str(), &main_window_state.visible);
    main_window_state.pos = ImGui::GetWindowPos();
    main_window_state.size = ImGui::GetWindowSize();
    ImGui::PopStyleColor();
    ImGui::End();
    
}

void ImGuiUpdater::build_debug_console() {
    DebugConsole::getInstance().render(&debug_console_state.visible);
}

bool ImGuiUpdater::shutdown() {

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    ImGui::DestroyContext();

    return true;
}

ImGuiUpdater::~ImGuiUpdater() {
    shutdown();
}


