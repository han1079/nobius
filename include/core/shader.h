#pragma once
#include <pch.h>


class Shader {
    private:
        std::string m_vertex_source;
        std::string m_fragment_source;
        std::string m_name;
        GLuint m_program_id = 0;

        bool m_program_is_valid = false;
        std::unordered_map<std::string, int> m_name_to_loc_cache;

        void initialize_shader();
        int validate_program() const;
        void cleanup();
        int lookup_loc_name(const std::string& name);
    public:
        Shader() = delete; // No default constructor. Shaders need source code at minimum.
        Shader(const std::string& vertex_src, const std::string& fragment_src);
        Shader(const std::string& vertex_src, const std::string& fragment_src, const std::string& name);
        ~Shader();

        bool bind() const;
        void unbind() const;

        bool set_uniform_1i(const std::string& name, int value);
        bool set_uniform_1f(const std::string& name, float value);
        bool set_uniform_bool(const std::string& name, bool value);

        // Overloads to have one-at-a-time style or full glm vec imports
        bool set_uniform_4f(const std::string& name, glm::vec4 value);
        bool set_uniform_4f(const std::string& name, float w, float x, float y, float z);

        bool set_uniform_mat4(const std::string& name, const glm::mat4& matrix);

        std::string get_shader_name() const { return m_name; }
        unsigned int get_program_id() const { return m_program_id; }
};
