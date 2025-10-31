#pragma once
#include <pch.h>
#include <core/common.h>

class Entity {
private:
    int UUID;
    glm::vec3 origin;
    float size;

    bool selectable;
    bool draggable;
    bool movable;
    bool visible;

    friend class WorldData;
    friend class WorldUpdater;
};

