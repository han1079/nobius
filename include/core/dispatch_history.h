#pragma once
#include <pch.h>
#include <core/common.h>

struct DispatchHistory {
    
    std::pair<std::string, bool> hard_lockout = {"", false}; // Pair of (reason, is_locked)
    bool mouse_claimed = false;
    bool keyboard_claimed = false;
    std::set<SDL_Scancode> claimed_keys;

    std::set<UUID_t> claimed_aggregates;

    bool try_claim_aggregate(const UUID_t& uuid) {
        if (claimed_aggregates.find(uuid) != claimed_aggregates.end()) {
            return false;
        } else {
            claimed_aggregates.insert(uuid);
            return true;
        }
    }

    bool check_lockout() const {
        return hard_lockout.second;
    }

    bool set_lockout(const std::string& reason) {
        if (!hard_lockout.second) {
            hard_lockout = {reason, true};
            return true;
        }
        return false;
    }
};