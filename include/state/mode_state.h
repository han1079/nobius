#pragma once

#include <core/base_state.h>
#include <core/common.h>
#include <pch.h>

class ModeState : public BaseState { 
public:

    ModeState() = default;
    ModeState(const std::optional<std::string>& fpath);
    ~ModeState() = default;


protected:

    void load_from_json(const std::optional<std::string>& fpath = std::nullopt) override;
private:
    // Mode Informatin
    
    uint32_t user_mode = MODE_SELECT;
    template <typename member_type>
    auto get(member_type member_name) -> decltype(this->*member_name) {
        return (this)->*member_name;
    }

    template <typename member_type, typename value_type>
    void set(member_type member_name, value_type val) {
        (this)->*member_name = val; 
    }

};
