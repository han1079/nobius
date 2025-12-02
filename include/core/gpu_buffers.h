#pragma once

#include <pch.h>
#include <core/common.h>
#include <core/entity.h>
#include <core/vertex_allocator.h>

// Vertex attribute constants
struct RichVertexAttr {
    static constexpr int BOUNDINGBOX_LOC = 0;
    static constexpr int STARTPT_LOC = 1;
    static constexpr int ENDPT_LOC = 2;
    static constexpr int CONTROLPT1_LOC = 3;
    static constexpr int CONTROLPT2_LOC = 4;
    static constexpr int TEXCOORDS_LOC = 5;
    static constexpr int THICKNESS_LOC = 6;
    static constexpr int FILLED_LOC = 7;

    static constexpr int TOTAL_ATTRS = 8;
    static constexpr int STRIDE = sizeof(RichVertex);

    static constexpr int BOUNDINGBOX_SIZE = 3;
    static constexpr int STARTPT_SIZE = 3;
    static constexpr int ENDPT_SIZE = 3;
    static constexpr int CONTROLPT1_SIZE = 3;  
    static constexpr int CONTROLPT2_SIZE = 3;
    static constexpr int TEXCOORDS_SIZE = 2;
    static constexpr int THICKNESS_SIZE = 1;
    static constexpr int FILLED_SIZE = 1;

    static constexpr size_t BOUNDINGBOX_OFFSET = 0;
    static constexpr size_t STARTPT_OFFSET = BOUNDINGBOX_OFFSET + BOUNDINGBOX_SIZE * sizeof(float);
    static constexpr size_t ENDPT_OFFSET = STARTPT_OFFSET + STARTPT_SIZE * sizeof(float);
    static constexpr size_t CONTROLPT1_OFFSET = ENDPT_OFFSET + ENDPT_SIZE * sizeof(float);  
    static constexpr size_t CONTROLPT2_OFFSET = CONTROLPT1_OFFSET + CONTROLPT1_SIZE * sizeof(float);
    static constexpr size_t TEXCOORDS_OFFSET = CONTROLPT2_OFFSET + CONTROLPT2_SIZE * sizeof(float);
    static constexpr size_t THICKNESS_OFFSET = TEXCOORDS_OFFSET + TEXCOORDS_SIZE * sizeof(float);
    static constexpr size_t FILLED_OFFSET = THICKNESS_OFFSET + THICKNESS_SIZE * sizeof(float);
};

class IndexBuffer {
public:
    IndexBuffer() = default;
    IndexBuffer(const IndexBuffer&) = delete;
    IndexBuffer& operator=(const IndexBuffer&) = delete;

    IndexBuffer(IndexBuffer&&) = default;
    IndexBuffer& operator=(IndexBuffer&&) = default;
    ~IndexBuffer() =  default;

    unsigned int m_IBO_ID;
    std::vector<unsigned int> m_indices;

    unsigned int* get_IBO_ptr() { return &m_IBO_ID; }
    unsigned int get_IBO_id() const { return m_IBO_ID; }

    void initialize() {
        GLCall(glGenBuffers(1, &m_IBO_ID));
        bind();
    }

    void bind() {
        GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_IBO_ID));
    }
    void unbind() {
        GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0));
    }
    void upload_data() {
        if (m_indices.empty()) {
            Debug::log("IndexBuffer: No indices to upload.", DebugLevel::WARN);
            return;
        }
        Debug::log("IndexBuffer: Uploading " + std::to_string(m_indices.size()) + " indices.", DebugLevel::TRACE);
        GLCall(glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_indices.size() * sizeof(int), (void*)(m_indices.data()), GL_DYNAMIC_DRAW));
    }

    void send_draw_command() {
        Debug::log("IndexBuffer: Sending draw command with " + std::to_string(m_indices.size()) + " indices.", DebugLevel::TRACE);
        GLCall(glDrawElements(GL_TRIANGLES, static_cast<unsigned int>(m_indices.size()), GL_UNSIGNED_INT, (void*)0));
    }
};

class VertexArray {
public:
    VertexArray() = default;
    VertexArray(const VertexArray&) = delete;
    VertexArray& operator=(const VertexArray&) = delete;
    ~VertexArray() = default;

    unsigned int m_VAO_ID;

    void initialize() {
        GLCall(glGenVertexArrays(1, &m_VAO_ID));
        bind();
        GLCall(glVertexAttribPointer(RichVertexAttr::BOUNDINGBOX_LOC, RichVertexAttr::BOUNDINGBOX_SIZE, GL_FLOAT, GL_FALSE, RichVertexAttr::STRIDE, (void*)RichVertexAttr::BOUNDINGBOX_OFFSET));
        GLCall(glEnableVertexAttribArray(RichVertexAttr::BOUNDINGBOX_LOC));
        GLCall(glVertexAttribPointer(RichVertexAttr::STARTPT_LOC, RichVertexAttr::STARTPT_SIZE, GL_FLOAT, GL_FALSE, RichVertexAttr::STRIDE, (void*)RichVertexAttr::STARTPT_OFFSET));
        GLCall(glEnableVertexAttribArray(RichVertexAttr::STARTPT_LOC));
        GLCall(glVertexAttribPointer(RichVertexAttr::ENDPT_LOC, RichVertexAttr::ENDPT_SIZE, GL_FLOAT, GL_FALSE, RichVertexAttr::STRIDE, (void*)RichVertexAttr::ENDPT_OFFSET));
        GLCall(glEnableVertexAttribArray(RichVertexAttr::ENDPT_LOC));
        GLCall(glVertexAttribPointer(RichVertexAttr::CONTROLPT1_LOC, RichVertexAttr::CONTROLPT1_SIZE, GL_FLOAT, GL_FALSE, RichVertexAttr::STRIDE, (void*)RichVertexAttr::CONTROLPT1_OFFSET));
        GLCall(glEnableVertexAttribArray(RichVertexAttr::CONTROLPT1_LOC));
        GLCall(glVertexAttribPointer(RichVertexAttr::CONTROLPT2_LOC, RichVertexAttr::CONTROLPT2_SIZE, GL_FLOAT, GL_FALSE, RichVertexAttr::STRIDE, (void*)RichVertexAttr::CONTROLPT2_OFFSET));
        GLCall(glEnableVertexAttribArray(RichVertexAttr::CONTROLPT2_LOC));
        GLCall(glVertexAttribPointer(RichVertexAttr::TEXCOORDS_LOC, RichVertexAttr::TEXCOORDS_SIZE, GL_FLOAT, GL_FALSE, RichVertexAttr::STRIDE, (void*)RichVertexAttr::TEXCOORDS_OFFSET));
        GLCall(glEnableVertexAttribArray(RichVertexAttr::TEXCOORDS_LOC));

        GLCall(glVertexAttribPointer(RichVertexAttr::THICKNESS_LOC, RichVertexAttr::THICKNESS_SIZE, GL_FLOAT, GL_FALSE, RichVertexAttr::STRIDE, (void*)RichVertexAttr::THICKNESS_OFFSET));
        GLCall(glEnableVertexAttribArray(RichVertexAttr::THICKNESS_LOC));
        
        GLCall(glVertexAttribIPointer(RichVertexAttr::FILLED_LOC, RichVertexAttr::FILLED_SIZE, GL_INT, RichVertexAttr::STRIDE, (void*)RichVertexAttr::FILLED_OFFSET));
        GLCall(glEnableVertexAttribArray(RichVertexAttr::FILLED_LOC));
    }

    void bind() {
        GLCall(glBindVertexArray(m_VAO_ID));
    }
    void unbind() {
        GLCall(glBindVertexArray(0));
    }
};

class VertexBuffer {
public:
    VertexBuffer() = default;
    VertexBuffer(const VertexBuffer&) = delete;
    VertexBuffer& operator=(const VertexBuffer&) = delete;
    ~VertexBuffer() = default;

    unsigned int m_VBO_ID;

    void initialize(VertexAllocator& allocator) {
        const std::vector<RichVertex>& vertices = allocator.get_vertex_buffer_data();
        GLCall(glGenBuffers(1, &m_VBO_ID));
        bind();
        GLCall(glBufferData(GL_ARRAY_BUFFER, (GLint)(vertices.size() * sizeof(RichVertex)), (void*)vertices.data(), GL_DYNAMIC_DRAW));
    }

    void bind() {
        GLCall(glBindBuffer(GL_ARRAY_BUFFER, m_VBO_ID));
    }
    void unbind() {
        GLCall(glBindBuffer(GL_ARRAY_BUFFER, 0));
    }
    void sync_data(VertexAllocator& allocator) {
        DEBUG_HOOK_FUNCTION_NO_TIMER();
        static float debug_vertex;
        bind();
        std::vector<AllocatedMem> allocations = allocator.get_dirty_allocations();

        const std::vector<RichVertex>& vertices = allocator.get_vertex_buffer_data();

        for (const auto& alloc : allocations) {
            GLCall(glBufferSubData(GL_ARRAY_BUFFER, (GLint)(alloc.offset * sizeof(RichVertex)), (GLsizei)(alloc.vertex_count * sizeof(RichVertex)), (void*)(vertices.data() + alloc.offset)));
            allocator.mark_uuid_clean(alloc.uuid);
            debug_vertex = vertices[alloc.offset].bound.x;
            DEBUG_HOOK_VAR_AS(debug_vertex, "First Vertex After Sync");
            
        }
    }
};