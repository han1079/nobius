#include <core/entity.h>
#include <engines/geometry.h>

glm::mat4 Entity::Pose::update_transform_matrix() {
    if (transform_dirty) {
        glm::mat4 translation_mat = Geometry::Transform::Translate(position);
        glm::mat4 rotation_mat = Geometry::Transform::Rotate2D(rotation);
        glm::mat4 size_mat = Geometry::Transform::Zoom(size);
        cached_transform = translation_mat * rotation_mat * size_mat;

        mark_clean();
    }
    return cached_transform;
}