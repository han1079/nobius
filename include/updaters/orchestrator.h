
#pragma once
#include <pch.h>
#include <core/common.h>


#include <updaters/world_updater.h>
#include <updaters/imgui_updater.h>
#include <updaters/input_system.h>
#include <core/vertex_allocator.h>

#include <core/renderer.h>



class Orchestrator {

public:

    //Constructor - can be default or with LoadSpec
    Orchestrator() = delete;
    Orchestrator(const LoadSpec& load_spec);

    //TODO: make actual destructor. For now, I'm leaving it empty because
    // quitting Orchestrator means main ends.
    ~Orchestrator();

    //Getter Function for the Orchestrator
    static Orchestrator* get();

    void init();
    void run();
    void sig_exit_loop();
    void shutdown();
    
    // Getter methods
    const WorldUpdater& get_world() const { return m_world_updater; }
    const InputSystem& get_input() const { return m_input_system; }
    const Renderer& get_renderer() const { return m_renderer;}
    
    // Mutable getters for systems that need to be modified
    InputSystem& get_input() { return m_input_system; }
    VertexAllocator& get_vertex_allocator() { return m_vertex_allocator; }
    Renderer& get_renderer() { return m_renderer; }
    
    // Convenience accessors for common operations
    glm::vec2 get_mouse_position() const { return m_input_system.get_mouse_position(); }
    bool is_mode(UserMode mode) const { return m_input_system.is_mode(mode); }
    bool is_key_pressed(SDL_Scancode key) const { return m_input_system.is_key_pressed(key); }


    
    // Window visibility check for renderer
    bool is_window_visible() const;
    
private:

    bool m_running = true;

    // Inline static pointer -> one global instance. Shared across everything that
    // includes Orchestrator.h or needs to access this object. 
    inline static Orchestrator* m_orchestrator_ptr = nullptr;

    /*The Orchestrator owns the following control objects:
     * 
     * InputSystem (was EventIngester + ModeUpdater)
     * WorldUpdater
     * ImGuiUpdater
     * 
     * */
    InputSystem m_input_system;
    WorldUpdater m_world_updater;
    ImGuiUpdater m_imgui_updater;

    Renderer m_renderer;
    VertexAllocator m_vertex_allocator;
    friend class Renderer;
};

