#include <core/state_tracker.h>

void StateTracker::Set(ST flag){
    user_state |= (1 << (int)flag);
}

void StateTracker::Unset(ST flag){
    user_state &= ~(1 << (int)flag);
}

bool StateTracker::Get(ST flag){
    return (bool)(1 & (user_state >> (int)flag));
}

void StateTracker::Clear(){
    user_state = 0;
}

uint32_t StateTracker::Get_Raw(){
    return user_state;
}
