#pragma once
#include <pch.h>
#include <core/common.h>
#include <core/entity.h>
#include <core/vertex_allocator.h>



// Vertex attribute constants
struct RichVertexAttr {
    static constexpr int BOUNDINGBOX_LOC = 0;
    static constexpr int STARTPT_LOC = 1;
    static constexpr int ENDPT_LOC = 2;
    static constexpr int CONTROLPT1_LOC = 3;
    static constexpr int CONTROLPT2_LOC = 4;
    static constexpr int TEXCOORDS_LOC = 5;
    static constexpr int THICKNESS_LOC = 6;
    static constexpr int FILLED_LOC = 7;

    static constexpr int TOTAL_ATTRS = 8;
    static constexpr int STRIDE = sizeof(RichVertex) / sizeof(float);

    static constexpr int BOUNDINGBOX_SIZE = 3;
    static constexpr int STARTPT_SIZE = 3;
    static constexpr int ENDPT_SIZE = 3;
    static constexpr int CONTROLPT1_SIZE = 3;  
    static constexpr int CONTROLPT2_SIZE = 3;
    static constexpr int TEXCOORDS_SIZE = 3;
    static constexpr int THICKNESS_SIZE = 1;
    static constexpr int FILLED_SIZE = 1;

    static constexpr int BOUNDINGBOX_OFFSET = 0;
    static constexpr int STARTPT_OFFSET = BOUNDINGBOX_OFFSET + BOUNDINGBOX_SIZE;
    static constexpr int ENDPT_OFFSET = STARTPT_OFFSET + STARTPT_SIZE;
    static constexpr int CONTROLPT1_OFFSET = ENDPT_OFFSET + ENDPT_SIZE;  
    static constexpr int CONTROLPT2_OFFSET = CONTROLPT1_OFFSET + CONTROLPT1_SIZE;
    static constexpr int TEXCOORDS_OFFSET = CONTROLPT2_OFFSET + CONTROLPT2_SIZE;
    static constexpr int THICKNESS_OFFSET = TEXCOORDS_OFFSET + TEXCOORDS_SIZE;
    static constexpr int FILLED_OFFSET = THICKNESS_OFFSET + THICKNESS_SIZE;
};

enum class RenderCommandType {
    None,
    Entity,
    ImGuiWindow
};

struct RenderCommand {
    RenderCommandType type = RenderCommandType::None;
    std::function<void()> execute_func = {};
    std::string debug_name = "";
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

public:
    // OpenGL resource IDs
    unsigned int m_VAO_ID{};
    unsigned int m_VBO_ID{};
    unsigned int m_IBO_ID{};

public:
    // Public interface

    void init();
    void render();
    void submit_render_request(RenderCommand request);

    std::queue<RenderCommand>& get_command_queue() { return command_queue; }

private:

    void set_up_gpu_buffers();

    //TO IMPLEMENT - Helper functions to register and deregister shader classes.
    void add_shader();
    void delete_shader();
    //TO IMPLEMENT - Needs an arg to specify which shader. Shader itself takes care of binding
    void use_shader();

    //TO IMPLEMENT - Check Vertex Allocator for dirty regions. Gl Sub Buffer reload those regions
    void update_vertex_buffer();


    //TO IMPLEMENT - Submit new draw - gets a "batch of indices" and a "shader with config" as a command. This is 
    //run as one of the branches of the process() command
    void submit_draw_call();

private:

    bool start_frame();
    void process_render_submissions();
    void process(RenderCommand& cmd);
    void end_frame(bool should_swap);
    void cleanup();

    VertexAllocator& get_vertex_allocator();

private:

    glm::vec4 gl_clear_color = {0.45f, 0.55f, 0.60f, 1.00f};
    inline static std::queue<RenderCommand> command_queue;

};
