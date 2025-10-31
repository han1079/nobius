#pragma once
#include <pch.h>
#include <core/common.h>
/* Base class for state tracking objects.
 *
 * Provides generic get/set methods for member variables by using their literal
 * names as seen in the class definition.
 * 
 * Keep all member variables PRIVATE to prevent accidental writes from subsystems
 * that shouldn't have authority. Only objects that were initialized with the
 * object have access to the getter/setter methods.
 * 
 * Usage:
 * class Tracker : public BaseStateTracker {
 * private:
 *     int some_integer;
 *     std::string some_string;
 * };
 * 
 * MyStateTracker tracker;
 * SET(tracker, some_integer, 42);
 * int val = GET(tracker, some_integer);
 * 
 */

class BaseState {

// Wishlist:
// - Helper to get a list of all member names and types for iteration / serialization

public:
    BaseState() = default;
    ~BaseState() = default;

    template <typename member_type>
    auto get(member_type member_name) -> decltype(this->*member_name) {
        return this->*member_name;
    }

    template <typename member_type, typename value_type>
    void set(member_type member_name, value_type val) {
        this->*member_name = val; 
    }

protected:

    nlohmann::json read_json_from_file(const std::string& file_path);
    virtual void write_json_to_file(const std::string& file_path, const nlohmann::json& j);
};
