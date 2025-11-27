#include <core/shader.h>

Shader::Shader(const std::string& vertex_src, const std::string& fragment_src, const std::string& name) :
    m_vertex_source(vertex_src), m_fragment_source(fragment_src), m_name(name)
{
    initialize_shader();
}

Shader::Shader(const std::string& vertex_src, const std::string& fragment_src) :
    m_vertex_source(vertex_src), m_fragment_source(fragment_src), m_name("UnnamedShader")
{
    initialize_shader();
}

Shader::~Shader() {
    cleanup();
}

void Shader::initialize_shader() {
    // Create shader pointers and compile
    char errlog[512];
    int success = 0;

    // No GLCall wrapper since it has no defined return value. TODO.
    unsigned int _vs = glCreateShader(GL_VERTEX_SHADER);
    unsigned int _fs = glCreateShader(GL_FRAGMENT_SHADER);

    // Actual lvalues need to be assigned in order for the shader source 
    // function to have an appropriate argument
    const char* vertex_string = m_vertex_source.c_str();
    const char* fragment_string = m_fragment_source.c_str();

    GLCall(glShaderSource(_vs, 1, &vertex_string, NULL));
    GLCall(glCompileShader(_vs));

    GLCall(glGetShaderiv(_vs, GL_COMPILE_STATUS, &success));

    if (!success) {
        GLCall(glGetShaderInfoLog(_vs, 512, NULL, errlog));
        Debug::log("Vertex Shader Compilation Error", DebugLevel::ERROR);
        Debug::log(errlog, DebugLevel::ERROR);
    }

    GLCall(glShaderSource(_fs, 1, &fragment_string, NULL));
    GLCall(glCompileShader(_fs));


    GLCall(glGetShaderiv(_fs, GL_COMPILE_STATUS, &success));

    if (!success) {
        Debug::log("Fragment Shader Compilation Error", DebugLevel::ERROR);
        GLCall(glGetShaderInfoLog(_fs, 512, NULL, errlog));
        Debug::log(errlog, DebugLevel::ERROR);
    }


    // Create Shader Program Object and Link Compiled Shader Code
    
    // No GLCall wrapper due to return issues. TODO.
    m_program_id = glCreateProgram(); 
    GLCall(glAttachShader(m_program_id, _vs));
    GLCall(glAttachShader(m_program_id, _fs));
    GLCall(glLinkProgram(m_program_id));

    GLCall(glGetProgramiv(m_program_id, GL_LINK_STATUS, &success));

    if (!success) {
        Debug::log("Shader Linking Error", DebugLevel::ERROR);
        GLCall(glGetProgramInfoLog(_fs, 512, NULL, errlog));
        Debug::log(errlog, DebugLevel::ERROR);
    }

    GLCall(glDeleteShader(_vs));
    GLCall(glDeleteShader(_fs));

    m_program_is_valid = (success && validate_program());
}

void Shader::cleanup() {
    unbind();
    GLCall(glDeleteProgram(m_program_id)); // This should fail gracefully with default 0.
}

int Shader::validate_program() const {
    int valid = 0;
    GLCall(glValidateProgram(m_program_id));
    GLCall(glGetProgramiv(m_program_id, GL_VALIDATE_STATUS, &valid));
    return valid;
}

bool Shader::bind() const {
    if (!m_program_is_valid) return false;
    GLCall(glUseProgram(m_program_id));
    return true; 
}

void Shader::unbind() const {
    GLCall(glUseProgram(0));
}


int Shader::lookup_loc_name(const std::string& name) {
    // Default crash out if gpu program isn't ready. Then:
    // * Default return value to failure
    // * Look up location in cache if it's registered
    // * If it isn't registered, ask GPU for the location 
    // * If GPU returns a valid location - register in cache 
    // * Otherwise - we fall back to default and keep loc as -1
    
    if (!m_program_is_valid) { return -1; }


    int loc = -1;
    if (m_name_to_loc_cache.count(name) != 0) {
        loc = m_name_to_loc_cache[name];
    } else {
        loc = glGetUniformLocation(m_program_id, name.c_str());

        if (loc >= 0) {
            m_name_to_loc_cache[name] = loc;
        } else {
            Debug::log("Shader Loc Not Found", DebugLevel::ERROR);
        }
    }

    return loc;
}

bool Shader::set_uniform_bool(const std::string& name, bool value) {
    // Bool gets naively converted to 1i for shader.
    if (!m_program_is_valid) return false;
    int loc = lookup_loc_name(name); 
    int v = value ? 1 : 0;
    if (loc != -1) { 
        glUniform1i(loc, v); 
        return true;
    }
    return false;
}

bool Shader::set_uniform_1i(const std::string& name, int value) {
    if (!m_program_is_valid) return false;
    int loc = lookup_loc_name(name); 
    if (loc != -1) { 
        glUniform1i(loc, value);
        return true;
    }
    return false;
}

bool Shader::set_uniform_1f(const std::string& name, float value) {
    if (!m_program_is_valid) return false;
    int loc = lookup_loc_name(name); 
    if (loc != -1) { 
        glUniform1f(loc, value);
        return true;
    }
    return false;
}

bool Shader::set_uniform_4f(const std::string& name, glm::vec4 value){
    if (!m_program_is_valid) return false;
    int loc = lookup_loc_name(name); 
    if (loc != -1) { 
        glUniform4fv(loc, 1, glm::value_ptr(value));
        return true;
    }
    return false;
}

bool Shader::set_uniform_4f(const std::string& name, float w, float x, float y, float z){
    if (!m_program_is_valid) return false;
    int loc = lookup_loc_name(name); 
    if (loc != -1) { 
        glUniform4f(loc, w, x, y, z);
        return true;
    }
    return false;
}

bool Shader::set_uniform_mat4(const std::string& name, const glm::mat4& matrix) {
    if (!m_program_is_valid) return false;
    int loc = lookup_loc_name(name); 
    if (loc != -1) { 
        glUniformMatrix4fv(loc, 1, GL_FALSE, glm::value_ptr(matrix));
        return true;
    }
    return false;
}

