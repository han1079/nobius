#pragma once
#include <glad/glad.h>
#include <SDL2/SDL.h>
#include <glm/glm.hpp>
#include <vector>
#include <unordered_map>
#include <string>
#include "../core/entity.h"

class Renderer { 
    public:
        Renderer();
        ~Renderer();

        int32_t initialize(GLADloadproc* GLContext, SDL_Window* window); // Initializes the renderer, which requires a GL Context and a Window.

        glm::vec3 backgroundColor = {1.0f, 1.0f, 1.0f}; // The background color of the renderer.
        void setBackgroundColor(const glm::vec3& color);

        void clearScreen(); // Clears the screen to the background color.

        std::unordered_map<std::string, GLint> shaderProgramList; // A list of shader programs that the renderer can use.
        int32_t addShaderProgram(const char* vertexShaderSource, const char* fragmentShaderSource); // Compiles and adds a shader program to the list. Returns the index of the shader program, or -1 on error.
        int32_t useShaderProgram(int32_t index); 
        int32_t currentShaderProgramIndex = -1; // The index of the currently used shader program.

        int32_t drawPrimitive(const Primitive& primitive); // Draws a frame. This is where all the rendering happens.
        /*Helper Functions to get GL and SDL stuff*/

        GLint getXWindowSize() const;
        GLint getYWindowSize() const;

        SDL_Window* getWindow() const;


    private:
        SDL_Window* window; // The SDL window that the renderer is associated with.
        GLint x_window_size; // The width of the window in pixels.
        GLint y_window_size; // The height of the window in pixels.

        GLADloadproc* GLContext; // The OpenGL context that the renderer is associated with.

}