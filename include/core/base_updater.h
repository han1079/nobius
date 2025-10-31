#pragma once
#include <pch.h>
#include <string>

#define MAKE_CALLBACK_FCN_USING(fcn) [this](auto&& arg) -> bool { return this->fcn(arg); }
#define GET(state_obj, member_name) state_obj.get(&std::remove_reference_t<decltype(state_obj)>::member_name)
#define SET(state_obj, member_name, val) state_obj.set(&std::remove_reference_t<decltype(state_obj)>::member_name, val)


class BaseUpdater {
private:
    std::string m_layer_name;
public:
    virtual bool init();
    virtual bool shutdown();
    virtual bool update_state_via_event(SDL_Event &event);
    virtual bool update_state_via_dT(float dT);
};


class EventRouter {
private:
    SDL_Event& m_event;
public:
    EventRouter(SDL_Event& e) : m_event(e) {}
    ~EventRouter() = default;

    template <typename fcn_t, SDL_Event& event_class_name>
    bool match_event_to_fcn(const fcn_t& callback){
        if (m_event.type == event_class_name.type){
            return callback(m_event);
        }
        return false;
    }

};
