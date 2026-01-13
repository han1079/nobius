#pragma once
#include <pch.h>
#include <core/common.h>
#include <core/entity_manager.h>
#include <core/renderer.h>
#include <core/aggregate_manager.h>
#include <core/input_system.h>
#include <core/dispatch_history.h>

enum class SessionType {
    UI,
    Canvas,
    Selection,
};

class Session {
public:
    Session() = delete; // No default constructor. Create constructor for overridden classes only.
    Session(const Session& ) = delete; // No copy constructor
    Session& operator=(const Session& ) = delete; // No copy assignment
    Session(Session&& ) = default; // Allow move constructor
    Session& operator=(Session&& ) = default; // Allow move assignment

    Session(
        const std::string& session_name,
        EntityManager& entity_manager,
        Renderer& renderer,
        AggregateManager& aggregate_manager,
        InputSystem& input_system
    )
        : m_session_name(session_name),
          m_session_id(generate_uuid()),
          p_entity_manager(&entity_manager),
          p_renderer(&renderer),
          p_input_system(&input_system),
          p_aggregate_manager(&aggregate_manager)
    {}
    virtual ~Session();

    bool ready_to_be_popped = false;

protected:
    const std::string& get_session_name() const { return m_session_name; }
    UUID_t get_session_id() const { return m_session_id; }

    InputSystem& inputs() { return *p_input_system; } 
    EntityManager& entity_mgr() { return *p_entity_manager; } 
    Renderer& renderer() { return *p_renderer; } 
    AggregateManager& aggregate_mgr() { return *p_aggregate_manager; } 

protected:

    std::string m_session_name;
    UUID_t m_session_id;

    EntityManager* p_entity_manager;
    Renderer* p_renderer;
    AggregateManager* p_aggregate_manager;
    InputSystem* p_input_system;

    virtual void on_enter();
    virtual void on_exit();
    virtual void process_input_deltas(DispatchHistory& h);
    virtual void process_time_deltas(float dT);
    virtual void submit_render_request();

    friend Orchestrator;
};
