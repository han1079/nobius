#pragma once

template<typename T>
struct TimeStampedValue {
    T value{};
    T last_value{};
    float timestamp = 0.0f;
    int changes_since_poll = 0;
    
    inline void update(const T& new_value, float time) {
        if (new_value != value) {
            last_value = value;
            value = new_value;
            changes_since_poll += 1;
            timestamp = time;
        }
    }

    inline void update_dT(const T& new_value, float dT) {
        update(new_value, timestamp + dT);
    }
    
    inline void reset_changes() {
        changes_since_poll = 0;
        last_value = value;
    }
    
    inline bool changed_this_frame() const { 
        return changes_since_poll > 0; 
    }
    
    inline bool just_became_true() const {
        return value && !last_value && changed_this_frame();
    }
    
    inline bool just_became_false() const {
        return !value && last_value && changed_this_frame();
    }
};

using TimeStampedBool = TimeStampedValue<bool>;
using TimeStampedFloat = TimeStampedValue<float>;
using TimeStampedInt = TimeStampedValue<int>;