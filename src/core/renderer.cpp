#include <core/renderer.h>
#include <updaters/orchestrator.h>

Renderer::Renderer() = default;

void Renderer::init() {
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER | SDL_INIT_GAMECONTROLLER) != 0)
    {
        printf("Error: %s\n", SDL_GetError());
        return;
    }

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, 0);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);

    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
    SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);

    SDL_WindowFlags sdl_window_flags = (SDL_WindowFlags)(SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI);

    scale = ImGui_ImplSDL2_GetContentScaleForDisplay(0);
    sdl_window = SDL_CreateWindow(window_name.c_str(), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                                  1280, 800,
                                  sdl_window_flags);

    if (sdl_window == nullptr)
    {
        printf("Error: SDL_CreateWindow(): %s\n", SDL_GetError());
        return;
    }

    gl_context = SDL_GL_CreateContext(sdl_window);
    if (gl_context == nullptr)
    {
        printf("Error: SDL_GL_CreateContext(): %s\n", SDL_GetError());
        return;
    }

    if (!gladLoadGLLoader((GLADloadproc)SDL_GL_GetProcAddress)) {
        Debug::log("Failed to initialize GLAD in Renderer", DebugLevel::FATAL);
        return;
    }


    std::string glsl_version = "#version 130";
    set_up_gpu_buffers();
}

VertexAllocator& Renderer::get_vertex_allocator() {
    auto o = Orchestrator::get();
    return o->get_vertex_allocator();
}

void Renderer::submit_render_request(RenderCommand request) {
    if(request.type == RenderCommandType::None) { 
        Debug::log("Not a proper command.", DebugLevel::ERROR);
        return; 
    }
    command_queue.push(request);
}

void Renderer::set_up_gpu_buffers() {
    auto& vb = get_vertex_allocator().get_vertex_buffer_data();
    GLCall(glClearColor(gl_clear_color.r * gl_clear_color.a, 
                 gl_clear_color.g * gl_clear_color.a, 
                 gl_clear_color.b * gl_clear_color.a, 
                 gl_clear_color.a));

    GLCall(glGenVertexArrays(1, &m_VAO_ID));
    GLCall(glGenBuffers(1, &m_VBO_ID));
    GLCall(glGenBuffers(1, &m_IBO_ID));
    GLCall(glBindVertexArray(m_VAO_ID));
    GLCall(glBindBuffer(GL_ARRAY_BUFFER, m_VBO_ID));
    GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_IBO_ID));
    GLCall(glBufferData(GL_ARRAY_BUFFER, vb.size() * sizeof(RichVertex), vb.data(), GL_DYNAMIC_DRAW));
    
    GLCall(glVertexAttribPointer(RichVertexAttr::BOUNDINGBOX_LOC, RichVertexAttr::BOUNDINGBOX_SIZE, GL_FLOAT, GL_FALSE, RichVertexAttr::STRIDE, (void*)RichVertexAttr::BOUNDINGBOX_OFFSET));
    GLCall(glEnableVertexAttribArray(RichVertexAttr::BOUNDINGBOX_LOC));
    GLCall(glVertexAttribPointer(RichVertexAttr::STARTPT_LOC, RichVertexAttr::STARTPT_SIZE, GL_FLOAT, GL_FALSE, RichVertexAttr::STRIDE, (void*)RichVertexAttr::STARTPT_OFFSET));
    GLCall(glEnableVertexAttribArray(RichVertexAttr::STARTPT_LOC));
    GLCall(glVertexAttribPointer(RichVertexAttr::ENDPT_LOC, RichVertexAttr::ENDPT_SIZE, GL_FLOAT, GL_FALSE, RichVertexAttr::STRIDE, (void*)RichVertexAttr::ENDPT_OFFSET));
    GLCall(glEnableVertexAttribArray(RichVertexAttr::ENDPT_LOC));
    GLCall(glVertexAttribPointer(RichVertexAttr::CONTROLPT1_LOC, RichVertexAttr::CONTROLPT1_SIZE, GL_FLOAT, GL_FALSE, RichVertexAttr::STRIDE, (void*)RichVertexAttr::CONTROLPT1_OFFSET));
    GLCall(glEnableVertexAttribArray(RichVertexAttr::CONTROLPT1_LOC));
    GLCall(glVertexAttribPointer(RichVertexAttr::CONTROLPT2_LOC, RichVertexAttr::CONTROLPT2_SIZE, GL_FLOAT, GL_FALSE, RichVertexAttr::STRIDE, (void*)RichVertexAttr::CONTROLPT2_OFFSET));
    GLCall(glEnableVertexAttribArray(RichVertexAttr::CONTROLPT2_LOC));
    GLCall(glVertexAttribPointer(RichVertexAttr::TEXCOORDS_LOC, RichVertexAttr::TEXCOORDS_SIZE, GL_FLOAT, GL_FALSE, RichVertexAttr::STRIDE, (void*)RichVertexAttr::TEXCOORDS_OFFSET));
    GLCall(glEnableVertexAttribArray(RichVertexAttr::TEXCOORDS_LOC));

    GLCall(glVertexAttribIPointer(RichVertexAttr::THICKNESS_LOC, 1, GL_INT, RichVertexAttr::STRIDE, (void*)RichVertexAttr::THICKNESS_OFFSET));
    GLCall(glEnableVertexAttribArray(RichVertexAttr::THICKNESS_LOC));
    
    GLCall(glVertexAttribIPointer(RichVertexAttr::FILLED_LOC, 1, GL_INT, RichVertexAttr::STRIDE, (void*)RichVertexAttr::FILLED_OFFSET));
    GLCall(glEnableVertexAttribArray(RichVertexAttr::FILLED_LOC));
}

void Renderer::render() {
    if(start_frame()){
        process_render_submissions();
        end_frame(true);
    } else {
        end_frame(false);
    }
}

bool Renderer::start_frame() {
    uint32_t window_flags = SDL_GetWindowFlags(sdl_window);
    bool should_render = !(window_flags & (SDL_WINDOW_MINIMIZED | SDL_WINDOW_HIDDEN));
    
    if (should_render) {

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplSDL2_NewFrame();
        ImGui::NewFrame();
        ImGui::DockSpaceOverViewport();
        ImVec2 display_size = ImGui::GetIO().DisplaySize;
        ImVec2 scale = ImGui::GetIO().DisplayFramebufferScale;
        int w = static_cast<int>(display_size.x) * static_cast<int>(scale.x);
        int h = static_cast<int>(display_size.y);        
        GLCall(glViewport(0,0,w,h)); 
        GLCall(glClearColor(gl_clear_color.r * gl_clear_color.a, 
                     gl_clear_color.g * gl_clear_color.a, 
                     gl_clear_color.b * gl_clear_color.a, 
                     gl_clear_color.a));
        GLCall(glClear(GL_COLOR_BUFFER_BIT));
    }
    return should_render;
}

void Renderer::process_render_submissions() {
    while (!command_queue.empty()) {
        RenderCommand cmd = command_queue.front();
        command_queue.pop();
        process(cmd);
    }
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void Renderer::process(RenderCommand& cmd) {
    if (cmd.type == RenderCommandType::Entity) {
        //NOT YET IMPLEMENTED
        return;
    } else if (cmd.type == RenderCommandType::ImGuiWindow) {
        cmd.execute_func();
    }
}

void Renderer::end_frame(bool should_swap) {
    if (should_swap) {
        SDL_GL_SwapWindow(sdl_window);
    } else {
        SDL_Delay(5);
    }
}

void Renderer::cleanup() {
    SDL_GL_DeleteContext(gl_context);
    SDL_DestroyWindow(sdl_window);
    SDL_Quit();
}
