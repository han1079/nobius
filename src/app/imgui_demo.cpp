/* The purpose of this file is to replicate the ImGui Demo, but with much fewer frills*/

#include "SDL_video.h"
#include <imgui.h>
#include <imgui_impl_sdl2.h>
#include <imgui_impl_opengl3.h>

#include <SDL_opengl.h>

#include <iostream>
int main(int argv, char* argc[]) {

    // Initialize of SDL_Init
    if (!SDL_Init()) {

        std::cout << "Unsuccessful SDL Init" << std::endl;
        return 0;
    }

    /*FORGOT: SDL Attribute sets*/
    const char* glsl_version = "#version 460";

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, 0);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
    // Create SDL_Window
    SDL_Window window = 
    // Create glContext

    // Create holders for vertex buffers an e buffers
    
    // Create holders for shaders

    // Bind shaders
    
    // Start Loop

    return 1;
}
