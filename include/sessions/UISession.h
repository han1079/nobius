#pragma once
#include <pch.h>
#include <engines/ui.h>
#include <core/session.h>

class UISession : public Session {
public:
    UISession() = delete; // No default constructor. Create constructor for overridden classes only.
    UISession(const UISession& ) = delete; // No copy constructor
    UISession& operator=(const UISession& ) = delete; // No copy assignment
    UISession(UISession&& ) = default; // Allow move constructor
    UISession& operator=(UISession&& ) = default; // Allow move assignment

    UISession(
        const std::string& session_name,
        EntityManager& entity_manager,
        Renderer& renderer,
        AggregateManager& aggregate_manager,
        InputSystem& input_system
    )
        : Session(
            session_name,
            entity_manager,
            renderer,
            aggregate_manager,
            input_system
        )
    {}
    ~UISession() = default;

    friend class Orchestrator;

protected:

    void on_enter() override;
    void on_exit() override;
    void process_input_deltas(DispatchHistory& h) override;
    void process_time_deltas(float dT) override;
    void submit_render_request() override;

private:

    std::unordered_map<std::string, WindowState> window_states;
    void build_windows();
};  
