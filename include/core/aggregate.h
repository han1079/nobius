
#pragma once
#include <pch.h>
#include <core/common.h>

class Aggregate {
public:
    Aggregate() = default;
    ~Aggregate() = default;

    std::set<UUID_t> member_entity_uuids;

    glm::vec3 position = {0.0f, 0.0f, 1.0f};
    glm::vec3 size = {1.0f, 1.0f, 1.0f};
    float rotation = 0.0f; // In radians

    bool is_visible = true;
    bool is_selectable = true;
    bool is_draggable = true;
    bool is_movable = true;

    std::any config;
};