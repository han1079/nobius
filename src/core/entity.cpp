#include <core/entity.h>

void Geometry::CreateCircle(Entity& circle) {
    std::vector<RichVertex> vertices;

    vertices.reserve(4);

    glm::vec3 start = {0.0f, 0.0f, 0.0f};

    RichVertex v1;
    v1.bound = glm::vec3{-0.5f, 0.5f, 0.0f};
    v1.start_pt = start;
    v1.filled = 1;
    v1.thickness = 0.05f;

    RichVertex v2;
    v2.bound = glm::vec3{0.5f, 0.5f, 0.0f};
    v2.start_pt = start;
    v2.filled = 1;
    v2.thickness = 0.05f;

    RichVertex v3;
    v3.bound = glm::vec3{0.5f, -0.5f, 0.0f};
    v3.start_pt = start;
    v3.filled = 1;
    v3.thickness = 0.05f;

    RichVertex v4;
    v4.bound = glm::vec3{-0.5f, -0.5f, 0.0f};
    v4.start_pt = start;
    v4.filled = 1;
    v4.thickness = 0.05f;

    vertices.push_back(v1);
    vertices.push_back(v2);
    vertices.push_back(v3);
    vertices.push_back(v4);

    circle.set_base_vertex_data(vertices);
    circle.set_vertex_data(vertices);

    std::vector<int> indices = {
        0, 1, 2,
        2, 3, 0
    };

    circle.set_indices(indices);

}

glm::mat4 Transform::Translate(const glm::vec3& translation) {
    glm::mat4 translation_mat = glm::translate(glm::mat4(1.0f), translation);
    return translation_mat;
}

glm::mat4 Transform::Scale(const glm::vec3& scale_factors) {
    glm::mat4 scale_mat = glm::scale(glm::mat4(1.0f), scale_factors);
    return scale_mat;
}

glm::mat4 Transform::Zoom(float zoom_factor) {
    glm::mat4 zoom_mat = glm::scale(glm::mat4(1.0f), glm::vec3(zoom_factor, zoom_factor, 1.0f));
    return zoom_mat;
}

glm::mat4 Transform::Rotate2D(float angle_in_radians) {
    glm::mat4 rotation_mat = glm::rotate(glm::mat4(1.0f), angle_in_radians, glm::vec3(0.0f, 0.0f, 1.0f));
    return rotation_mat;
}

void Affine::TransformCircle(Entity& circle) {

    if (!circle.pose.transform_dirty) {
        return; // No transformation needed
    }

    // Get a copy of the vertices to modify
    std::vector<RichVertex> vertices = circle.get_base_vertex_data();
    
    glm::mat4 transform_mat = circle.pose.update_transform_matrix();

    float new_radius = circle.pose.size;
    glm::vec2 new_start = circle.pose.position;

    for (auto& vertex : vertices) {
        // Apply transform to the bounding box quad
        // Note: bound is vec3, need to convert to vec4 for matrix mult, then back
        glm::vec4 transformed_bound = transform_mat * glm::vec4(vertex.bound, 1.0f);
        vertex.bound = glm::vec3(transformed_bound);
        
        vertex.start_pt = glm::vec3(new_start, 0.0f); // Center
        vertex.end_pt[0] = new_radius * 0.5f; // Store radius in x component of end_pt
        vertex.control_pt_1[0] = 0.0f; // Control pt 1 carries "bounding box or not" flag
    }
    
    circle.set_vertex_data(vertices);
    return;
}