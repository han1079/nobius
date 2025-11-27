#pragma once

#ifndef PCH_H_INCLUDED
#include <SDL_opengl.h>
#include <glad/glad.h>
#include <iostream>
#include <vector>
#include <string>
#endif

// Debug hook macros - controlled by DEBUG flag
#ifdef DEBUG
    // Function debug setup - creates static variables and hooks
    #define DEBUG_HOOK_FUNCTION() \
        static auto debug_timer = Instrumentor::getInstance().beginFunction(__PRETTY_FUNCTION__); \
        auto& debug_console = DebugConsole::getInstance();
    
    // Alternative with custom name override
    #define DEBUG_HOOK_FUNCTION_AS(func_name) \
        static auto debug_timer = Instrumentor::getInstance().beginFunction(#func_name); \
        auto& debug_console = DebugConsole::getInstance();
    
    #define DEBUG_HOOK_FUNCTION_NO_TIMER() \
        auto& debug_console = DebugConsole::getInstance();
    
    // Hook a variable with automatic naming
    #define DEBUG_HOOK_VAR(var) \
        debug_console.hookVariable(#var, std::ref(var))
    
    // Hook a variable with custom name
    #define DEBUG_HOOK_VAR_AS(var, name) \
        debug_console.hookVariable(name, std::ref(var))
    
    // Log debug message
    #define DEBUG_LOG(msg) \
        debug_console.addMessage(std::string(__LINE__),"DEBUG: " + std::string(msg))
    
#else
    // Release mode - all debug macros become no-ops
    #define DEBUG_HOOK_FUNCTION() ((void)0)
    #define DEBUG_HOOK_FUNCTION_AS(func_name) ((void)0)
    #define DEBUG_HOOK_FUNCTION_NO_TIMER() ((void)0)
    #define DEBUG_HOOK_VAR(var) ((void)0)
    #define DEBUG_HOOK_VAR_AS(var, name) ((void)0)
    #define DEBUG_LOG(msg) ((void)0)
#endif
// Portable debug break macro

#if defined(_MSC_VER)
    #define DEBUG_BREAK() __debugbreak()
    #define __FUNCTION_NAME__ __FUNCSIG__
#elif defined(__clang__)
    #define DEBUG_BREAK() __builtin_debugtrap()
    #define __FUNCTION_NAME__ __PRETTY_FUNCTION__
#elif defined(__GNUC__)
    #define DEBUG_BREAK() __builtin_trap()
    #define __FUNCTION_NAME__ __FUNCTION__
#else
    #include <signal.h>
    #define DEBUG_BREAK() raise(SIGTRAP)
    #define __FUNCTION_NAME__ __FUNCTION__
#endif

// Macro helpers for stringifying file and line
#define STRINGIFY(x) #x
#define TOSTRING(x) STRINGIFY(x)
#define FILE_LINE __FILE__ ":" TOSTRING(__LINE__)

#define TIME_FUNCTION() InstrumentationTimer itimer##__LINE__(__FUNCTION_NAME__)
#define TIME_LOCATION() InstrumentationTimer itimer##__LINE__(FILE_LINE)

#ifdef DEBUG
    #define DEBUG_LOG(msg) Debug::log(msg, DebugLevel::TRACE)
    #define DEBUG_VAR(var) Debug::log(#var " = " + std::to_string(var), DebugLevel::TRACE)
#else
    #define DEBUG_LOG(msg)
    #define DEBUG_VAR(var)
#endif

#define ASSERT(x) if (!(x)) DEBUG_BREAK();


#define GLCall(x) GLClearError();\
x;\
ASSERT(GLLogCall(#x, __FILE__, __LINE__))

inline bool GLLogCall(const char* function, const char* file, int line) {
    bool success = true;
    GLenum error;
    while ((error = glGetError()) != GL_NO_ERROR) {
        std::cout << "[OpenGL Error] (" << error << "): " << function <<
            " " << file << ":" << line << std::endl;
        success = false;
    }
    
    // Debug: Uncomment this line to see ALL GL calls
    // std::cout << "[OpenGL Call] " << function << " at " << file << ":" << line << std::endl;
    
    return success;
}

inline void GLClearError() {
    int max_errors = 100; // Prevent infinite loop
    int cleared_count = 0;
    GLenum error;
    while ((error = glGetError()) != GL_NO_ERROR && --max_errors > 0) {
        std::cout << "[OpenGL] Clearing error: " << error << std::endl;
        cleared_count++;
    }
    if (max_errors <= 0) {
        std::cout << "[OpenGL] Warning: Too many errors, stopped clearing after " << cleared_count << " errors" << std::endl;
    } else if (cleared_count > 0) {
        std::cout << "[OpenGL] Cleared " << cleared_count << " errors before GL call" << std::endl;
    }
}


enum DebugLevel { TRACE, INFO, WARN, ERROR, FATAL};

class Debug {
    public:
    Debug() = delete;
    ~Debug() = delete;

    static void log(const std::string& msg, DebugLevel level = DebugLevel::INFO) {
        switch (level) {
            case TRACE:
                std::cout << "[TRACE]: " << msg << std::endl;
                break;
            case INFO:
                std::cout << "[INFO]: " << msg << std::endl;
                break;
            case WARN:
                std::cout << "[WARN]: " << msg << std::endl;
                break;
            case ERROR:
                std::cout << "[ERROR]: " << msg << std::endl;
                break;
            case FATAL:
                std::cout << "[FATAL]: " << msg << std::endl;
                abort();
                break;
        }
    }
};