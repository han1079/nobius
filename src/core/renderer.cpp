#include <core/renderer.h>



bool Renderer::init() {
   
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER | SDL_INIT_GAMECONTROLLER) != 0)
    {
        printf("Error: %s\n", SDL_GetError());
        return 1;
    }

    // TODO: Defend against mangled data entries
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, m_cfg.sdl_flags);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, m_cfg.sdl_profile_mask);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, m_cfg.sdl_major_version);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, m_cfg.sdl_minor_version);

    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, m_cfg.sdl_double_buffer);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, m_cfg.sdl_depth_size);
    SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, m_cfg.sdl_stencil_size);

    float main_scale = ImGui_ImplSDL2_GetContentScaleForDisplay(0); 

    int win_width = static_cast<int>(m_cfg.sdl_window_width * m_cfg.imgui_scale);
    int win_height = static_cast<int>(m_cfg.sdl_window_height * m_cfg.imgui_scale);

    window = SDL_CreateWindow(m_cfg.sdl_window_title,
                              SDL_WINDOWPOS_CENTERED,
                              SDL_WINDOWPOS_CENTERED,
                              win_width, 
                              win_height, 
                              (SDL_WindowFlags)(SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI));
    if (window == nullptr)
    {
        printf("Error: SDL_CreateWindow(): %s\n", SDL_GetError());
        return 1;
    }

    gl_context = SDL_GL_CreateContext(window);
    if (gl_context == nullptr)
    {
        printf("Error: SDL_GL_CreateContext(): %s\n", SDL_GetError());
        return 1;
    }

    SDL_GL_MakeCurrent(window, gl_context);
    SDL_GL_SetSwapInterval(1); // Enable vsync
};
