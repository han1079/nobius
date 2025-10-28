#include <iostream>
#include <atomic>
#include <vector>
#include <optional>
#include <glm/glm.hpp>
#include <glad/glad.h>
#include "../render/renderer.h"

enum class SegmentType {
    ARC,
    CUBIC_BEZIER
};

struct Vertex {
    glm::vec3 pos; // Position in 3D space. For most stuff, z=0.0f, but this allows for 3-D shapes later.
    float triangle_type; // Encodes the type of triangle for shader use
};

struct Segment {
    /*Path Segment Object - A single segment of a path. Can be a line, arc, or Bezier curve*/
    glm::vec3 start_point = {0.0f, 0.0f, 0.0f};
    glm::vec3 end_point = {0.0f, 0.0f, 0.0f};
    std::optional<glm::vec3> control_point_1; // For non-line segments
    std::optional<glm::vec3> control_point_2; // For cubic Beziers only

    SegmentType type = SegmentType::CUBIC_BEZIER;;

    float length = 0.0f; // The length of the segment, in normalized Bezier space.
    int32_t list_location = -1; // The index of this segment in the primitive's segment list.
    
    Segment* next_segment = nullptr; // Pointer to the next segment in the path. nullptr if this is the last segment.
    Segment* prev_segment = nullptr; // Pointer to the previous segment in the path. nullptr if this is the first segment.
};


/*Buffers that update based on state:
The Primitive is a collection of flags and values that act to "glue" the segments together. It includes:
    1. Whether or not the path is closed.
    2. The number of segments.
    3. The type of segments used.
    4. The normalized length of each segment.
    5. The total path length
    6. Whether or not the primitive is filled.
    7. Color, opacity, stroke width, fancy color options if necessary.
    8. A matrix transform that convert the primitive from normalized "Bezier space" to "world space"
    9. The bounding box of the primitive in world space.

A primitive state update is triggered on any world frame change, or any segment change. It is a CONTEXT-AWARE summary of the complete
state of the primitive. Thus, it encodes a COMPLETE description of the primitive, but is relatively cheap to compute.

Thus, the function is updated on ANY change to the primitive or world frame. This way, multiple changes can occur between
render calls, but the vertex buffer will only need to be recalculated once.

The vertex buffer is a cache of the vertices that make up the primitive. It is recalculated whenever the FRAME UPDATE is called.

It will use the LATEST primitive state to recalculate.*/

struct Primitive {
    size_t numSegments; // The number of segments in the primitive.
    Segment* headSegment = nullptr; // Pointer to the first segment in the path / linked list.
    Segment* tailSegment = nullptr; // Pointer to the last segment in the path / linked list. (If it's the same as the head, then the pointers are the same!).

    bool isClosed() 
        {return headSegment == tailSegment;} // Whether or not the path is closed.

    std::vector<Segment*> segmentList; // A list of pointers to each segment, in order. Order is enforced by linked list structure. The vector is populated by LL traversal on update.

    float totalLengh = 0.0f; // Total length of the curve normalized in Bezier space.
    

    glm::vec3 boundingBoxBottomLeft = {0.0f, 0.0f, 0.0f}; // The minimum x and y of the bounding box that contains the entire primitive.
    glm::vec3 boundingBoxTopRight = {0.0f, 0.0f, 0.0f}; // The maximum x and y of the bounding box that contains the entire primitive.

    glm::mat4 BezierToWorld = glm::mat4(1.0f); // The matrix that performs an affine transform from Bezier space to world space.
    std::vector<glm::vec3> equidistantPoints; // A list of equidistant points along the entire primitive, in world space.
    uint32_t numEquidistantPoints = 0; // The number of equidistant points in the list.

    bool isFilled = false;

};


/*Object that gives handlers for renderer and state machines to interact with primitives*/
class PrimitiveController {
    private:
        Primitive primitive; // The current state of the primitive.

        /* Primitive Behavior: Summary
        
        1. The primitive is made up of a list of segments. Each segment is a Segment object (or derived class).
        2. Since there are multiple segments - there are a few rules that need to be followed:
            a. The end point of a segment must be the start point of the next segment. Disconnected segments should just be separate primitives.
            b. If the primitive is filled, the last segment's end point must be the same as the first segment's start point,
                i. If this isn't the case, then fillling is disabled.
                ii. Filling can be forced - but a segment will be forcefully created to close the path.
            c. Since connectivity is important, primitive modification is constrained to:
                i. Adding or removing a point. This can either append or prepend a segment, insert a segment in between two existing segments,
                   or convert 2 adjacent segments into one segment.
                ii. Modifying segment control points / endpoints through a public function.
                iii. Changing fill state (which may add a segment if needed).
                iv. Specifically - "T" junctions are not allowed. If a segment needs to branch off, it should be a separate primitive.
        3. Any modification to the primitive that changes segment endpoints or control points marks the vertex buffer as dirty, and it will be recalculated on the next render call.
        4. Inherited Shape Objects (Circle, Rectangle, Polygon, Polyline) will have public functions to modify their shape, which will in turn call the appropriate primitive functions to modify segments.
        */


        mutable std::vector<Vertex> vertex_buffer_cache;
        mutable std::atomic<bool> is_the_primitive_state_locked; // Puts a fence around the primitive update functions. Force a check at the beginning and end of the update function.
        mutable std::atomic<bool> does_the_buffer_needs_update; // Whether or not the vertex buffer needs to be recalculated.



        /*Below are all of the GPU rendering handles and states I need to manage*/

        GLuint vao_handle, vbo_handle, ebo_handle; // GPU buffer handles
        mutable bool do_the_gpu_buffers_have_addresses_yet; // Whether or not the GPU buffers have been bound / VAO configured yet.


    public:
        PrimitiveController();

        bool changePose(const glm::mat4& newPose); // Changes the Bezier to World matrix. Marks the buffer as dirty. Returns 0 on success, -1 on error.
        
        /*Below are all of the public functions that give some level of access to the low level GPU rendering handles*/
        GLuint getVaoHandle() const;
        GLuint getVboHandle() const;
        GLuint getEboHandle() const;
        const std::vector<Vertex>& getVertexBufferCache() const;
        bool getBufferDirtyState() const;
        
        // void force_VertexBufferUpdate(); // Forces the vertex buffer to be recalculated on the next render call. Usually, the buffer only updates when configuration is changed, but this can force an update. TODO - maybe not needed?

        /* This is basically GPU Init. It points the buffer handles I own to addresses the GPU assigns. Once I do this, every render call:
            1. Grabs the buffer handles that now have the GPU addresses assigned to this unique segment.
            2. Checks if the state is dirty, and updates the cached vertex buffer.
            3. Grabs the buffered data. The actual command to send the data to the GPU is done by the renderer object, which takes THIS object as an argument.

            Basically - all the object is acting as is a fancy bucket of data with an address on it.
        */
        void bindGPUBufferHandles(GLuint vao, GLuint vbo, GLuint ebo); // Binds the VAO, VBO, and EBO if they haven't been bound yet. Sets "do_the_gpu_buffers_have_addressed_yet" to true. If they have, does nothing.

        // When the frame update is called, render is called. If no object changes are made, we can just skip this segment's call. Otherwise, we recalculate and render.
        // This actually minimizes the amount of times we have to access the GPU pipeline, and allows static images to work quite efficiently.

        void render(const Renderer& renderer, const glm::mat4& model_matrix) const; // Renders the segment using the provided renderer and model matrix.
        ~PrimitiveController();
};