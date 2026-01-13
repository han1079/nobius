#include <engines/ui.h>
#include <utils/imgui_style_loader.h>
#ifndef STYLEPATH
#define STYLEPATH (PROJECT_SOURCE_DIR + std::string("/configs/imgui_style_default.json")).c_str()
#endif

void UI::initialize_imgui(SDL_Window* window, SDL_GLContext gl_context, float main_scale) {

    // Setup Dear ImGui context
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    ImGuiConfigFlags imgui_config_flags = (ImGuiConfigFlags_NavEnableKeyboard | 
                                            ImGuiConfigFlags_DockingEnable);
    io.ConfigFlags |= imgui_config_flags;

    load_imgui_style_from_json(STYLEPATH);

    ImGuiStyle style = ImGui::GetStyle();

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

    // Setup Platform/Renderer backends=
    ImGui_ImplSDL2_InitForOpenGL(window, gl_context);
    ImGui_ImplOpenGL3_Init("#version 130");

    // Renders a warmup frame to ensure style/font settings are applied
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplSDL2_NewFrame();
    // ImGui::NewFrame();
    // ImGui::Render();
    // ImGui::EndFrame();
}

void UI::push_event_to_imgui(const SDL_Event* event) {
    ImGui_ImplSDL2_ProcessEvent(event);
}   

void UI::update_imgui_state() {
    // This actually processes the event queue and updates ImGui state
    // ImGui_ImplOpenGL3_NewFrame();
    // ImGui_ImplSDL2_NewFrame();
    // ImGui::NewFrame();
}

void UI::build_bottom_panel(WindowState& state) {

    ImGuiIO& io = ImGui::GetIO();
    ImVec2 full_window_size = io.DisplaySize;

    ImGui::SetNextWindowPos(ImVec2(0, full_window_size.y * 0.9), ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowSize(ImVec2(full_window_size.x, full_window_size.y * 0.1), ImGuiCond_FirstUseEver);
    ImGui::Begin(state.window_name.c_str(), &state.visible);
    state.pos = ImGui::GetWindowPos();
    state.size = ImGui::GetWindowSize();
    ImGui::Text("Timeline Slider Here");
    ImGui::End();
    
}

void UI::build_ribbon(WindowState& state) {

    ImGuiIO& io = ImGui::GetIO();
    ImVec2 full_window_size = io.DisplaySize;

    ImGui::SetNextWindowPos(state.initial_pos, ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowSize(state.initial_size, ImGuiCond_FirstUseEver);
    ImGui::Begin(state.window_name.c_str(), &state.visible);
    state.pos = ImGui::GetWindowPos();
    state.size = ImGui::GetWindowSize();
    ImGui::Text("Play | Pause | Stop Buttons Here");
    ImGui::End();
    
}

void UI::build_main_window(WindowState& state) {

    ImGuiIO& io = ImGui::GetIO();
    ImVec2 full_window_size = io.DisplaySize;

    ImGui::SetNextWindowPos(state.initial_pos, ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowSize(state.initial_size, ImGuiCond_FirstUseEver);
    ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.1f, 0.1f, 0.1f, 1.0f));
    ImGui::Begin(state.window_name.c_str(), &state.visible);
    state.pos = ImGui::GetWindowPos();
    state.size = ImGui::GetWindowSize();

    ImGui::PopStyleColor();
    ImGui::End();
    
}
void UI::build_sidebar(WindowState& state) {

    ImGuiIO& io = ImGui::GetIO();
    ImVec2 full_window_size = io.DisplaySize;

    // Sidebar takes up all of the right side
    // and extends to 30% of the way along the x side on initial layout
    ImGui::SetNextWindowPos(ImVec2(full_window_size.x * 0.7, 0), ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowSize(ImVec2(full_window_size.x * 0.3, full_window_size.y), ImGuiCond_FirstUseEver);
    ImGui::Begin(state.window_name.c_str(), &state.visible);
    state.pos = ImGui::GetWindowPos();
    state.size = ImGui::GetWindowSize();
    ImGui::Text("Info Panel");
    ImGui::Text("%s", PROJECT_SOURCE_DIR);
    ImGui::End();
    
}


void UI::build_debug_console(WindowState& state) {
    bool show = true;
    DebugConsole::getInstance().render(&show);
}

void UI::shutdown_imgui() {

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    ImGui::DestroyContext();
}

