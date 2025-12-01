#pragma once

template<typename T>
struct TimeStampedValue {
    T value{};
    T last_value{};
    T delta_value{};

    bool different_from_poll_start = false;
    float last_timestamp = 0.0f;
    float timestamp = 0.0f;
    int changes_in_current_poll = 0;
    int poll_changes = 0;
    
    inline void update(const T& new_value, float time) {
        if (new_value != value) {
            value = new_value;
            changes_in_current_poll += 1;
            timestamp = time;
        }
    }

    inline void update_dT(const T& new_value, float dT) {
        update(new_value, timestamp + dT);
    }
    
    inline void save_accumulated_changes() {
        if constexpr (std::is_same<T, bool>::value) {
            delta_value = (value == last_value) ? false : true;
            different_from_poll_start = delta_value;
        } else {
            delta_value = value - last_value;
            if (value == last_value) {
                different_from_poll_start = false;
            } else {
                different_from_poll_start = true;
            }
        }   
        poll_changes = changes_in_current_poll;
        changes_in_current_poll = 0;
        last_timestamp = timestamp;
        last_value = value;
    }
    
    inline bool changed_this_frame() const { 
        return poll_changes > 0 && different_from_poll_start; 
    }
    
    inline bool just_became_true() const {
        if constexpr (std::is_same<T, bool>::value) {
            return value && (poll_changes % 2) && changed_this_frame();
        }
        return false;
    }
    
    inline bool just_became_false() const {
        if constexpr (std::is_same<T, bool>::value) {
            return !value && (poll_changes % 2) && changed_this_frame();
        }
        return false;
    }
};

using TimeStampedBool = TimeStampedValue<bool>;
using TimeStampedFloat = TimeStampedValue<float>;
using TimeStampedInt = TimeStampedValue<int>;
using TimeStampedVec2 = TimeStampedValue<glm::vec2>;
using TimeStampedVec3 = TimeStampedValue<glm::vec3>;
using TimeStampedVec4 = TimeStampedValue<glm::vec4>;
using TimeStampedMat4 = TimeStampedValue<glm::mat4>;