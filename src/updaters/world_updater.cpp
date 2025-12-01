#include <updaters/world_updater.h>
#include <updaters/orchestrator.h>
#include <core/entity.h>

float clamp(float value, float min = 0.001f, float max = 1000.0f) {
    if (value < min) return min;
    if (value > max) return max;
    return value;
}

bool WorldUpdater::init() {
    auto& renderer = Orchestrator::get()->get_renderer();
    renderer.create_index_buffer_for("WorldUpdater");
    clip_transform_mat = glm::mat4(1.0f);

    DEBUG_HOOK_FUNCTION_NO_TIMER();
    DEBUG_HOOK_VAR_AS(current_zoom, "WORLD_UPDATER_CURRENT_ZOOM");
    DEBUG_HOOK_VAR_AS(camera_position.x, "WORLD_UPDATER_CAMERA_POS_X");
    DEBUG_HOOK_VAR_AS(camera_position.y, "WORLD_UPDATER_CAMERA_POS_Y");
    return true;
}

bool WorldUpdater::shutdown() {
    return true;
}

bool WorldUpdater::update_state_via_event() {
    // Get orchestrator instance
    auto& in = Orchestrator::get()->get_input();
    UserMode current_mode = static_cast<UserMode>(in.get_mode_flags());
    DEBUG_HOOK_FUNCTION_NO_TIMER();

    static glm::vec2 translation;

    DEBUG_HOOK_VAR_AS(translation.x, "WORLD_UPDATER_TRANSLATION_X");
    DEBUG_HOOK_VAR_AS(translation.y, "WORLD_UPDATER_TRANSLATION_Y");
    float delta_zoom = 1.0f;

    ImVec2 viewport_dims = ImVec2(in.viewport_width.value, in.viewport_height.value);
    float aspect_ratio = viewport_dims.x / viewport_dims.y;

    float pixels_per_world_unit = viewport_dims.y / world_units_per_screen_height;
    float effective_pixels_per_world_unit = pixels_per_world_unit * current_zoom;

    float half_screen_height_eff_pixel = world_units_per_screen_height / 2.0f * effective_pixels_per_world_unit;
    float half_screen_width_eff_pixel = half_screen_height_eff_pixel * aspect_ratio;

    // Projection Matrix: Defines the viewing volume in WORLD UNITS.
    // We do not use pixels here. The mapping from World -> NDC -> Pixels happens later.
    float half_world_height = world_units_per_screen_height / 2.0f;
    float half_world_width = half_world_height * aspect_ratio;

    projection_mat = glm::ortho(-half_world_width, half_world_width,
                             -half_world_height, half_world_height,
                             -1.0f, 1.0f);


    if ((uint32_t)current_mode & (uint32_t)UserMode::MODE_SELECT) {
        if (in.mouse_wheel_dy.delta_value != 0.0f) {
            float zoom_delta = in.mouse_wheel_dy.delta_value * 0.1f; // temporary sensitivity factor
            float new_zoom = clamp(current_zoom * (1.0f + zoom_delta));
            current_zoom = new_zoom;
        }

        if ((uint32_t)current_mode & (uint32_t)UserMode::MODE_DRAG) {
            // Handle camera pan logic here
            translation = in.get_mouse_delta_position();
            if (translation != glm::vec2(0.0f)) {
                glm::vec2 camera_delta = -translation / effective_pixels_per_world_unit;
                glm::vec2 new_camera_position = camera_position + camera_delta;
                camera_position = new_camera_position;
            }
        }
    }

    glm::mat4 zoom_mat = Transform::Zoom(current_zoom);
    glm::mat4 translation_mat = Transform::Translate(glm::vec3(-camera_position, 0.0f));

    // projection_mat is already set above in World Units.
    // We do NOT overwrite it with pixel coordinates.

    view_mat = zoom_mat * translation_mat;
    clip_transform_mat = projection_mat * view_mat;

    // if (in.key_states[SDL_SCANCODE_A].just_became_true()) {
    //     // Reset clip transform
    //     add_circle(glm::vec3(0.0f, 0.0f, 0.0f), 0.3f, glm::vec4(1.0f, 0.0f, 0.0f, 1.0f));
    // }

    return true;
}

bool WorldUpdater::update_state_via_dT(float dT) {
    time_since_startup += dT;
    static int count = 0;
    if (count == 0){

        uint64_t id = m_entity_manager.generate_circle(1.0f, glm::vec3(0.0f, 0.0f, 0.0f));
        m_entity_manager.EntityRegister.at(id).set_movable(true);
        m_entity_manager.EntityRegister.at(id).set_visible(true);
        count++;
    }

    for (auto& [uuid, entity] : m_entity_manager.EntityRegister) {
        if (entity.is_movable()) {
            m_entity_manager.translate_entity(uuid, glm::vec3(0.002*glm::sin(time_since_startup), 0.002*glm::cos(time_since_startup), 0.0f));
        }
    }
    return true;
}

bool WorldUpdater::submit_render_commands() {
    auto& renderer = Orchestrator::get()->get_renderer();
    auto& index_buffer_mirror = renderer.m_index_buffers["WorldUpdater"].m_indices;

    // Sync the Vertex Buffer with changes to Entity State
    BatchedMemoryMap map = m_entity_manager.update_gpu_buffer();
    std::vector<unsigned int> all_indices;

    for (auto& [uuid, allocated_mem]: map) {

        Debug::log("Submitting render command for entity UUID: " + std::to_string(uuid), DebugLevel::INFO);
        Entity& entity = m_entity_manager.EntityRegister.at(uuid);
        if (entity.is_visible()) {
            Debug::log("Submitting render command for visible entity UUID: " + std::to_string(uuid), DebugLevel::INFO);
            std::vector<int> shifted_indices = entity.get_indices();
            std::cout << "Indices" << std::endl;
            for (const auto& idx : shifted_indices) {
                std::cout << idx << " ";
            }
            std::cout << std::endl;
            // Shift indices based on allocated_mem.offset
            for (auto& index : shifted_indices) {
                index += allocated_mem.offset;
            }
            all_indices.insert(all_indices.end(), shifted_indices.begin(), shifted_indices.end());

            RenderCommand cmd;
            cmd.type = RenderCommandType::Entity;
            cmd.updater_name = "WorldUpdater";
            cmd.shader_name = "Circle";
            cmd.transform = clip_transform_mat;
            renderer.submit_render_request(cmd);
        }
    }

    index_buffer_mirror = all_indices;

    return true;
}

