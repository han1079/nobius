#pragma once
#include <pch.h>
#include <core/common.h>
#include <core/entity.h>

namespace Geometry {

    namespace Build {
        void CreateCircle(Entity& circle);
    } // namespace Build

    namespace Affine {
        void TransformCircle(Entity& circle);
    } // namespace Affine

    namespace Transform {
        glm::mat4 Translate(const glm::vec3& translation);
        glm::mat4 Scale(const glm::vec3& scale_factors);
        glm::mat4 Zoom(float zoom_factor);
        glm::mat4 Rotate2D(float angle_in_radians);
    } // namespace Transform

} // namespace Geometry
