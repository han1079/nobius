#pragma once
#include <pch.h>
#include <core/common.h>
#include <core/entity.h>
#include <core/vertex_allocator.h>
#include <core/shader.h>
#include <core/gpu_buffers.h>

enum class RenderCommandType {
    None = 0,
    Entity = 1,
    ImGuiWindow = 2
};

struct RenderCommand {
    RenderCommandType type = RenderCommandType::None;
    std::function<void()> execute_func;
    
    // Draw Context
    std::string shader_name = "None";
    std::string updater_name = "None";
    // Uniforms / State
    glm::vec4 color = {1.0f, 1.0f, 1.0f, 1.0f};
    glm::mat4 transform = glm::mat4(1.0f);
    
    // Debugging
    std::string debug_name = "None";

};

struct CommandBundle {
    std::shared_ptr<Shader> shader;
    RenderCommand command;
    Renderer* renderer;
};

class Renderer {
public:
    Renderer();

    Renderer(const Renderer&) = delete;
    Renderer& operator=(const Renderer&) = delete;

    Renderer(Renderer&&) = default;
    Renderer& operator=(Renderer&&) = default;

    ~Renderer() { cleanup(); };

public:
    // SDL Handlers
    SDL_Window* sdl_window;
    SDL_GLContext gl_context;
    float scale;
    const std::string window_name = "PLACEHOLDER_TITLE";
    ShaderManager m_shader_manager;
    Shader* active_shader = nullptr;


public:
    // OpenGL resource IDs

    std::unordered_map<std::string, IndexBuffer> m_index_buffers;
    void create_index_buffer_for(const std::string& updater_name) {
        if (m_index_buffers.find(updater_name) != m_index_buffers.end()) {
            Debug::log("IndexBuffer for " + updater_name + " already exists.", DebugLevel::WARN);
            return;
        }

        IndexBuffer new_index_buffer;
        new_index_buffer.initialize();

        m_index_buffers.insert_or_assign(updater_name, std::move(new_index_buffer));
    }

    VertexBuffer m_vertex_buffer;
    VertexArray m_vertex_array;

public:
    // Public interface

    void init();
    void render();
    void submit_render_request(RenderCommand request);

    std::queue<CommandBundle>& get_command_queue() { return command_queue; }

    VertexAllocator& get_vertex_allocator();


private:

    void set_up_gpu_buffers();

    //TO IMPLEMENT - Helper functions to register and deregister shader classes.
    void add_shader(const std::string& name, const std::string& vertex_path, const std::string& fragment_path) {m_shader_manager.load_shader_from_files(name, vertex_path, fragment_path);}
    void delete_shader(const std::string& name) {m_shader_manager.deregister_shader(name);}
    //TO IMPLEMENT - Needs an arg to specify which shader. Shader itself takes care of binding
    void use_shader(const std::string& shader_name) {std::shared_ptr<Shader> shader = m_shader_manager.get_shader(shader_name); if(shader) { shader->bind(); } }

    //TO IMPLEMENT - Check Vertex Allocator for dirty regions. Gl Sub Buffer reload those regions
    void update_vertex_buffer();


    //TO IMPLEMENT - Submit new draw - gets a "batch of indices" and a "shader with config" as a command. This is 
    //run as one of the branches of the process() command
    void submit_draw_call();

private:

    bool start_frame();
    void process_render_submissions();
    void process(CommandBundle& cmd);
    void end_frame(bool should_swap);
    void cleanup();


private:
    void verify_command(RenderCommand& cmd);
    void update_vertex_buffer_data(const uint64_t& dirty_uuid);

private:

    glm::vec4 gl_clear_color = {0.45f, 0.55f, 0.60f, 1.00f};
    inline static std::queue<CommandBundle> command_queue;
    glm::vec4 gl_viewport_params = {0.0f, 0.0f, 800.0f, 600.0f}; // x, y, width, height
    glm::vec4 gl_main_window_params = {0.0f, 0.0f, 800.0f, 600.0f}; // x, y, width, height

};
