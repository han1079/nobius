#pragma once

#include <core/base_state.h>
#include <core/common.h>
#include <pch.h>

class SystemState : public BaseState { 
public:

    SystemState() = default;
    ~SystemState() = default;


private:
    // Mode Information
    
    uint32_t user_mode = MODE_SELECT;

};
