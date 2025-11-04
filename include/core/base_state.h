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
    BaseState();
    BaseState(const std::optional<std::string>& fpath);
    ~BaseState();

    
protected:
    
    const std::optional<std::string> file_path = "";

protected:

    virtual void load_from_json(const std::optional<std::string>& fpath);
    virtual void write_json_to_file(const std::string& fpath, const nlohmann::json& j);
};

