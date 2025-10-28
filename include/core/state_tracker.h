#ifndef STATE_TRACKER_H
#define STATE_TRACKER_H

#include <cstdint>

enum class ST : uint32_t {
    CTRL_HELD =  0,
    SHIFT_HELD,
    LEFT_CLICK_HELD,
    RIGHT_CLICK_HELD,
    IN_DEBUG_OVERLAY
};

class StateTracker {
private:
    uint32_t user_state = 0;
public:
    StateTracker() = default;
    ~StateTracker() = default;
    void Set(ST flag);
    void Unset(ST flag);
    bool Get(ST flag);
    void Clear();
    uint32_t Get_Raw();
};

#endif //STATE_TRACKER_H
