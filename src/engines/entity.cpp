#include <iostream>
#include <atomic>
#include <vector>
#include <utility>
#include <optional>
#include <glm/glm.hpp>
#include <glad/glad.h>
#include <stack>
#include "../render/renderer.h"
#include "entity.h"


/* Pre-declarations of functions that get calculates important segment information:
1. get_segment_length - Returns the length of a segment in normalized Bezier space.
2. calc_segment_length - Calculates the length of a segment in normalized Bezier space and updates the segment's length field.
3. calc_t_given_point_on_seg - Calculates the t value in [0,1] for a given point on a single Bezier segment. 
4. calc_point_given_t_on_seg - Calculates a point on a single Bezier segment given a t value in [0,1].
5. calc_t_given_point_on_primitive- Calculates the t value in [0,1] treating the entire primitive as a single Bezier curve. t=0 is the start of the first segment, t=1 is the end of the last segment. 
6. calc_point_given_t_on_primitive- Calculates normalized Bezier coordinates given a t in [0,1], treating the entire primitive as a single Bezier curve. 
7. get_segment_given_t_on_primitive- Returns a pointer to the segment and local t value in [0,1] for a given global t in [0,1] treating the entire primitive as a single Bezier curve.
8. get_segment_given_point_on_primitive- Returns a pointer to the segment and local t value in [0,1] for a given point on the primitive treating the entire primitive as a single Bezier curve.
*/

float getSegmentLength(const Segment& segment);
float calc_segment_length(Segment& segment);
float calc_t_given_point_on_seg(const Segment& segment, const glm::vec3& point);
glm::vec3 calc_point_given_t_on_seg(const Segment& segment, float t);
float calc_t_given_point_on_primitive(const Primitive& primitive, const glm::vec3&);
glm::vec3 calc_point_given_t_on_primitive(const Primitive& primitive, float t);
std::pair<Segment*, float> get_segment_given_t_on_primitive(const Primitive& primitive, float t);
std::pair<Segment*, float> get_segment_given_point_on_primitive(const Primitive& primitive, const glm::vec3& point);

/*Pre-declarations of functions for segment manipulations
1. setSegmentCriticalPoints - Low level access to endpoints and control points of a segment.
2. appendSegment - Appends a segment to the end of the linked list. Updates the primitive state.
3. prependSegment - Prepends a segment to the start of the linked list. Updates the primitive state.
4. splitSegment - Splits a segment into two segments at a given point. Updates the primitive state.
5. mergeSegments - Merges two adjacent segments into one segment. Updates the primitive state.
*/


bool setSegmentCriticalPoints(Segment& segment, const glm::vec3& start, const glm::vec3& end, const std::optional<glm::vec3>& cp1 = std::nullopt, const std::optional<glm::vec3>& cp2 = std::nullopt);
bool appendSegment(Primitive& primitive, const Segment& segment);
bool prependSegment(Primitive& primitive, const Segment& segment);
bool splitSegment(Primitive& primitive, const glm::vec3& point);
bool mergeSegments(Primitive& primitive, Segment* first, Segment* second);

void cleanUpSegmentList(Segment* head);

float getCubicBezierLength(const glm::vec3& p0, const glm::vec3& p1, const glm::vec3& p2, const glm::vec3& p3, float tolerance = 0.01f) {
    float length = 0.0f;
    
    glm::vec3 u0 = glm::vec3(1, 1, 1) - p0;
    glm::vec3 u1 = glm::vec3(1, 1, 1) - p1;
    glm::vec3 u2 = glm::vec3(1, 1, 1) - p2;
    glm::vec3 u3 = glm::vec3(1, 1, 1) - p3;
    return length;
}

float getSegmentLength(const Segment& segment) {
    if (segment.type == SegmentType::CUBIC_BEZIER) {
        glm::vec3 p0 = segment.start_point;
        glm::vec3 p1 = segment.control_point_1.value_or(segment.start_point);
        glm::vec3 p2 = segment.control_point_2.value_or(segment.end_point);
        glm::vec3 p3 = segment.end_point;

        glm::vec3 first_derivative = 
    } else if (segment.type == SegmentType::ARC) {
        float radius = glm::length(segment.start_point - segment.control_point_1);
        float angle = glm::atan(glm::length(segment.start_point - segment.end_point) / (2 * radius), radius) * 2.0f;
        return radius * angle;
    } else {
        // Unsupported segment type
        return 0.0f;
    }
}

void cleanUpSegmentList(Segment* head) {
    if (head == nullptr) return; // Nothing to clean up
    
    std::stack<Segment*> stack_of_seen_segments;

    // Looks for a basic loop in the linked list, and breaks it if found.
    Segment* before_head = head->prev_segment;
    Segment* current_segment = head;

    // Head only has a non-null prev if there's a loop, so we break it.
    if (before_head != nullptr) {
        before_head->next_segment = nullptr;
        head->prev_segment = nullptr
    };


    while (current_segment != nullptr) {
        stack_of_seen_segments.push(current_segment);
        current_segment = current_segment->next_segment;
    }   

    while (!stack_of_seen_segments.empty()) {
        Segment* toDelete = stack_of_seen_segments.top();
        stack_of_seen_segments.pop();
        delete toDelete;
    }

    return;
}

/*Constructor of the Primitive Class*/
PrimitiveController::PrimitiveController() : 
            primitive(), 
            is_the_primitive_state_locked(false), 
            does_the_buffer_needs_update(true),
            vao_handle(0), 
            vbo_handle(0), 
            ebo_handle(0), 
            do_the_gpu_buffers_have_addresses_yet(false) {
}

/*Destructor of the Primitive Class.*/
PrimitiveController::~PrimitiveController() {
    // Cleanup GPU resources if they were allocated
    if (do_the_gpu_buffers_have_addresses_yet) {
        glDeleteVertexArrays(1, &vao_handle);
        glDeleteBuffers(1, &vbo_handle);
        if (ebo_handle != 0) {
            glDeleteBuffers(1, &ebo_handle);
        }
    }

    // State saves the head of the linked list. Use that to break any loops and clean.
    cleanUpSegmentList(primitive.headSegment);
}