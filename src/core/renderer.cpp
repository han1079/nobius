#include <core/renderer.h>
#include <updaters/orchestrator.h>

// Static callback wrapper for ImGui
static void CustomCallback(const ImDrawList* parent_list, const ImDrawCmd* cmd) {
    // Retrieve the shader pointer from UserCallbackData


    CommandBundle* bundle = static_cast<CommandBundle*>(cmd->UserCallbackData);

    Shader* shader = bundle->shader.get();
    Renderer* renderer = bundle->renderer;
    RenderCommand render_cmd = bundle->command;
    
    glScissor(static_cast<GLint>(renderer->get_main_window_params().x),
               static_cast<GLint>(renderer->get_main_window_params().y),
               static_cast<GLsizei>(renderer->get_main_window_params().z),
               static_cast<GLsizei>(renderer->get_main_window_params().w));
                    

    if (shader) {
        shader->bind();

        shader->set_uniform_mat4("u_ViewProj", render_cmd.transform);
        shader->set_uniform_4f("u_baseColor", render_cmd.color);
    }

    if (renderer) {
        std::string current_updater = render_cmd.updater_name;
        renderer->m_vertex_buffer.bind();
        renderer->m_vertex_buffer.sync_data(renderer->get_vertex_allocator());
        renderer->m_vertex_array.bind();
        Debug::log("Rendering with index buffer for updater: " + current_updater, DebugLevel::TRACE);
        renderer->m_index_buffers[current_updater].bind();
        renderer->m_index_buffers[current_updater].upload_data();
        renderer->m_index_buffers[current_updater].send_draw_command();
    }
}




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

    m_shader_manager.load_shader_from_files("Circle", "shaders/vertex_shader_2D.glsl", "shaders/fragment_shader_circle2D.glsl");
    std::shared_ptr<Shader> copy_of_shader = m_shader_manager.get_shader("Circle");
    std::cout << copy_of_shader->m_fragment_source << std::endl;

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

    CommandBundle cmd_bundle;
    cmd_bundle.command = std::move(request);
    cmd_bundle.renderer = this;
    std::shared_ptr<Shader> shader = m_shader_manager.get_shader(cmd_bundle.command.shader_name);
    cmd_bundle.shader = shader;

    if(request.type == RenderCommandType::ImGuiWindow) {
        Debug::log("Command wrapped: " + std::to_string((int)cmd_bundle.command.type), DebugLevel::TRACE);
    }

    Debug::log("Submitting render request of type: " + std::to_string((int)cmd_bundle.command.type), DebugLevel::TRACE);
    Debug::log("Command pointer pre-submission: " + std::to_string(reinterpret_cast<uintptr_t>(&(cmd_bundle.command))), DebugLevel::TRACE);
    command_queue.push(cmd_bundle);
}

void Renderer::set_up_gpu_buffers() {
    GLCall(glClearColor(gl_clear_color.r * gl_clear_color.a, 
                 gl_clear_color.g * gl_clear_color.a, 
                 gl_clear_color.b * gl_clear_color.a, 
                 gl_clear_color.a));

    VertexAllocator& allocator = get_vertex_allocator();
    m_vertex_buffer.initialize(allocator);
    m_vertex_buffer.bind();
    m_vertex_array.initialize();
    m_vertex_array.bind();
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

    auto& in = Orchestrator::get()->get_input();


    
    if (should_render) {

        Debug::log("Renderer: Starting Frame", DebugLevel::TRACE);

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplSDL2_NewFrame();
        ImGui::NewFrame();
        ImGui::DockSpaceOverViewport();
        gl_main_window_params = {
            in.windows.viewport.value.x,
            in.windows.viewport.value.y,
            in.windows.viewport.value.z,
            in.windows.viewport.value.w,
        };

        ImVec2 display_size = ImGui::GetIO().DisplaySize;
        ImVec2 scale = ImGui::GetIO().DisplayFramebufferScale;
        int w = static_cast<int>(display_size.x) * static_cast<int>(scale.x);
        int h = static_cast<int>(display_size.y);        

        gl_viewport_params = {
            0.0f,
            0.0f,
            static_cast<float>(w),
            static_cast<float>(h)
        };

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
    std::vector<CommandBundle> commands_to_process;

    while (!command_queue.empty()) {
        commands_to_process.push_back(std::move(command_queue.front()));
        command_queue.pop();
        Debug::log("Processing command of type: " + std::to_string((int)commands_to_process.back().command.type), DebugLevel::TRACE);
        process(commands_to_process.back());
    }
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    commands_to_process.clear();
}

void Renderer::process(CommandBundle& cmd) {

    if (cmd.command.type == RenderCommandType::Entity) {
        Debug::log("Renderer: Processing Entity Render Command: " + std::to_string((int)cmd.command.type), DebugLevel::TRACE);

        ImDrawList* draw_list = ImGui::GetWindowDrawList();
        
        // Set viewport and scissor BEFORE adding callbacks so ResetRenderState knows what to restore
        // GLCall(glEnable(GL_DEPTH_TEST));
        // GLCall(glEnable(GL_BLEND));
        // GLCall(glEnable(GL_SCISSOR_TEST));

        // GLCall(glViewport(
        //     static_cast<GLint>(gl_main_window_params.x),
        //     static_cast<GLint>(gl_main_window_params.y),
        //     static_cast<GLsizei>(gl_main_window_params.z),
        //     static_cast<GLsizei>(gl_main_window_params.w)
        // ));
       
        
        // Resolve the shader pointer. 
        // Note: We rely on ShaderManager keeping the shared_ptr alive for the duration of the frame.
        
        if (cmd.shader) {
            // Pass the raw pointer to ImGui. 
            draw_list->AddCallback(CustomCallback, &cmd);
        } else {
            Debug::log("Renderer: Failed to find shader for Entity: " + cmd.command.shader_name, DebugLevel::ERROR);
        }

        draw_list->AddCallback(ImDrawCallback_ResetRenderState, nullptr);
        

    } else if (cmd.command.type == RenderCommandType::ImGuiWindow) {
        Debug::log("Renderer: Processing ImGuiWindow Render Command: " + cmd.command.debug_name, DebugLevel::TRACE);
        cmd.command.execute_func();
    } else {
        Debug::log("Renderer: Unknown Render Command Type", DebugLevel::ERROR);
        Debug::log("Renderer: Command Debug Name: " + std::to_string((int)cmd.command.type), DebugLevel::ERROR);
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
