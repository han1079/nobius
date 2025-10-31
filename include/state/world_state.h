#pragma once
#include <pch.h>
#include <core/common.h>
#include <core/base_state.h>
#include <core/entity.h>

class WorldState : public BaseState {
public:
    WorldState() = default;
    ~WorldState() = default;

private:
    int num_entities;
    glm::mat4 clip_transform_mat;
    std::unique_ptr<std::unordered_map<int, Entity>> EntityRegister;    

};
