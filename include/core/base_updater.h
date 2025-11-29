#pragma once
#include <pch.h>
#include <string>
#include <core/common.h>

#define MAKE_CALLBACK_FCN_USING(fcn) [this](auto&& arg) -> bool { return this->fcn(arg); }
#define GET(state_obj, member_name) state_obj.get(&std::remove_reference_t<decltype(state_obj)>::member_name)
#define SET(state_obj, member_name, val) state_obj.set(&std::remove_reference_t<decltype(state_obj)>::member_name, val)


class BaseUpdater {
private:
    std::string m_name;
public:
    BaseUpdater();
    virtual ~BaseUpdater();
protected:
    virtual bool init();
    virtual bool shutdown();
    virtual bool update_state_via_event(EngineEvent &event);
    virtual bool update_state_via_dT(float dT);
    virtual bool submit_render_commands();
};


