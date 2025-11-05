#include "imgui.h"
#include <updaters/renderer.h>
#include <iostream>


Renderer::Renderer(RendererState& state) : m_cfg(state) {}

Renderer::~Renderer() {
    // Destructor implementation
}

bool Renderer::init() {
   
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER | SDL_INIT_GAMECONTROLLER) != 0)
    {
        printf("Error: %s\n", SDL_GetError());
        return false;
    }

    // TODO: Defend against mangled data entries
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, m_cfg.sdl_flags);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, m_cfg.sdl_profile_mask);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, m_cfg.sdl_major_version);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, m_cfg.sdl_minor_version);

    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, m_cfg.sdl_double_buffer);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, m_cfg.sdl_depth_size);
    SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, m_cfg.sdl_stencil_size);

    main_scale = ImGui_ImplSDL2_GetContentScaleForDisplay(0); 

    win_width = static_cast<int>(m_cfg.sdl_window_width * m_cfg.imgui_scale);
    win_height = static_cast<int>(m_cfg.sdl_window_height * m_cfg.imgui_scale);

    window = SDL_CreateWindow(m_cfg.sdl_window_title.c_str(),
                              SDL_WINDOWPOS_CENTERED,
                              SDL_WINDOWPOS_CENTERED,
                              win_width, 
                              win_height, 
                              (SDL_WindowFlags)(SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI));
    if (window == nullptr)
    {
        printf("Error: SDL_CreateWindow(): %s\n", SDL_GetError());
        return false;
    }

    gl_context = SDL_GL_CreateContext(window);
    if (gl_context == nullptr)
    {
        printf("Error: SDL_GL_CreateContext(): %s\n", SDL_GetError());
        return false;
    }

    SDL_GL_MakeCurrent(window, gl_context);
    SDL_GL_SetSwapInterval(1); // Enable vsync

    // Setup Dear ImGui context
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= m_cfg.imgui_config_flags;

    // Setup scaling
    style = ImGui::GetStyle();
    style.ScaleAllSizes(main_scale);        // Bake a fixed style scale. (until we have a solution for dynamic style scaling, changing this requires resetting Style + calling this again)
    style.FontScaleDpi = main_scale;        // Set initial font scale. (using io.ConfigDpiScaleFonts=true makes this unnecessary. We leave both here for documentation purpose)
    io.ConfigDpiScaleFonts = true;          // [Experimental] Automatically overwrite style.FontScaleDpi in Begin() when Monitor DPI changes. This will scale fonts but _NOT_ scale sizes/padding for now.

    std::string path = PROJECT_SOURCE_DIR + std::string("/assets/Roboto/static/Roboto-Light.ttf");
    ImFont* roboto = io.Fonts->AddFontFromFileTTF(path.c_str(), 12);
    io.FontDefault = roboto;

    std::cout << "\r" << &style << std::flush;
    
    io.ConfigDpiScaleViewports = true;      // [Experimental] Scale Dear ImGui and Platform Windows when Monitor DPI changes.
    ImGui::StyleColorsDark();

    // When viewports are enabled we tweak WindowRounding/WindowBg so platform windows can look identical to regular ones.
    //if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
    //{
    //    style.WindowRounding = 0.0f;
    //    style.Colors[ImGuiCol_WindowBg].w = 1.0f;
    //}

    // Setup Platform/Renderer backends
    ImGui_ImplSDL2_InitForOpenGL(window, gl_context);
    ImGui_ImplOpenGL3_Init(m_cfg.glsl_version.c_str());

    return true;
}

bool Renderer::update_state_via_event(SDL_Event &event) {
    // Renderer state itself doesn't do major updates on event
    return true;
}

void Renderer::build_imgui_frame() {
    /* Build the ImGui frame

    Bottom Row -> Slider for Timeline 
    Top Row -> Buttons for Play, Pause, Stop
    Right Side -> Info Panel with Input Box, Text Box, Checkbox
    Left Side -> Main Viewport with Rendered Content
    */

    bool* sidebar = &m_cfg.sidebar_visible;
    bool* bottom = &m_cfg.bottom_visible;
    bool* ribbon = &m_cfg.ribbon_visible;
    bool* viewport = &m_cfg.viewport_visible;


    ImGuiIO& io = ImGui::GetIO();
    /*Manual Configuration of all of the windows*/




    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplSDL2_NewFrame();
    ImGui::NewFrame();
    ImGui::DockSpaceOverViewport();

    ImVec2 full_window_size = io.DisplaySize;
    
    /*
    _______________________
    |______________|      | <- Ribbon (0,0) and (w * 1, h * 0.1)
    |              |      |
    |              |      | <- Main Viewport      | Sidebar
    |              |      |    (0, h * 0.1)       | (w * 0.7, h * 0)
    |              |      |    (w * 0.7, h * 0.8) | (w * 0.3, h * 1)
    |______________|      |
    |______________|______| <- Bottom (0, h * 0.9) and (w * 1, h * 0.1)
    
    
    
    */

    //ImGui::ShowStyleEditor(&style);
    // Main viewport takes up left 70% of the window
    //ImGui::SetNextWindowPos(ImVec2(0, 0.1 * full_window_size.y), ImGuiCond_FirstUseEver);
    //ImGui::SetNextWindowSize(ImVec2(full_window_size.x * 0.7, full_window_size.y * 0.8), ImGuiCond_FirstUseEver);
    //ImGui::Begin("Main Window", viewport);
    //ImGui::Text("Rendered Content Goes Here");
    //ImGui::Text("Event Count: %d", m_cfg.event_count);
    //ImGui::End();

    // Sidebar takes up all of the right side
    // and extends to 30% of the way along the x side on initial layout
    ImGui::SetNextWindowPos(ImVec2(full_window_size.x * 0.7, 0), ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowSize(ImVec2(full_window_size.x * 0.3, full_window_size.y), ImGuiCond_FirstUseEver);
    ImGui::Begin("Sidebar", sidebar);
    ImGui::Text("Info Panel");
    ImGui::Text("%s", PROJECT_SOURCE_DIR);
    ImGui::End();

    ImGui::SetNextWindowPos(ImVec2(0, full_window_size.y * 0.9), ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowSize(ImVec2(full_window_size.x, full_window_size.y * 0.1), ImGuiCond_FirstUseEver);
    ImGui::Begin("Bottom Panel", bottom);
    ImGui::Text("Timeline Slider Here");
    ImGui::End();

    ImGui::SetNextWindowPos(ImVec2(0, 0), ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowSize(ImVec2(full_window_size.x, full_window_size.y * 0.1), ImGuiCond_FirstUseEver);
    ImGui::Begin("Ribbon", ribbon);
    ImGui::Text("Play | Pause | Stop Buttons Here");
    ImGui::End();
    
    ImGui::Begin("Style", viewport);
    std::cout << "\r"<< &style << std::flush;
    ImGui::ShowStyleEditor(&style);
    ImGui::End();
}

bool Renderer::update_state_via_dT(float dT) {
    static int frame_count = 0;
    frame_count++;
    
    // Log every 60 frames (about once per second at 60fps)
    if (frame_count % 60 == 0) {
        
    }
    
    if (SDL_GetWindowFlags(window) & SDL_WINDOW_MINIMIZED)
    {
        
        SDL_Delay(10);
        return true;
    }

    
    build_imgui_frame();
    
    
    
    ImGuiIO& io = ImGui::GetIO();
    ImGui::Render();
    
    
    glViewport(0, 0, (int)io.DisplaySize.x, (int)io.DisplaySize.y);
    glClearColor(m_cfg.gl_clear_color.r * m_cfg.gl_clear_color.a, 
                 m_cfg.gl_clear_color.g * m_cfg.gl_clear_color.a, 
                 m_cfg.gl_clear_color.b * m_cfg.gl_clear_color.a, 
                 m_cfg.gl_clear_color.a);
    glClear(GL_COLOR_BUFFER_BIT);
    
    
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    
    
    
    
    SDL_GL_SwapWindow(window);
    
    
    return true;
}

bool Renderer::shutdown() {

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    ImGui::DestroyContext();

    SDL_GL_DeleteContext(gl_context);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return true;

}

nlohmann::json read_json_from_file(const std::string& file_path) {
    nlohmann::json json = nlohmann::json::parse(file_path);
    return json;
}
