#include <iostream>
#include <glad/glad.h>
#include <SDL2/SDL.h>

#define FRAME_RATE 16 // Approx ~60 FPS

/* Annoying, but necessary. A shader is a program that runs in a completely different language.
The way that this works is that you literally write the entire shader program as a string in C++,
then compile it at runtime, and then link it to the OpenGL pipeline.

Basically - this variable will be sent into OpenGL API calls as a argument, which will eventually
pass it to the GPU, along with a command to "compile this code". The GPU driver sees this command,
and the string, and tells the GPU to compile it. Basically we're controlling a smaller computer inside
our computer and uploading a file to it.
*/

// The vertex shader here basically just takes in a 3D position and outputs its projection into 4D space without
// and changes. The fourth dimension is usually used to turn translations in 3-D space into multiplications.
const char* vertexShaderSource = "#version 330 core\n"
    "layout (location = 0) in vec3 aPos;\n"
    "void main()\n"
    "{\n"
    "   gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
    "}\0";


const char* fragmentShaderSource = "#version 330 core\n"
    "out vec4 FragColor;\n"
    "void main()\n"
    "{\n"
    "   FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);\n"
    "}\0";


void calculate_mouse_point_vertices(float* vertices, int mouse_x, int mouse_y, int x_window_size, int y_window_size) {
    // Convert mouse coordinates to OpenGL normalized device coordinates
    float x_ndc = (2.0f * mouse_x) / x_window_size - 1.0f;
    float y_ndc = 1.0f - (2.0f * mouse_y) / y_window_size; // Invert y-axis

    float point_size = 0.01f; // Size of the point in NDC

    // Define a small triangle around the mouse position
    vertices[0] = x_ndc;               vertices[1] = y_ndc + point_size; vertices[2] = 0.0f; // Top vertex
    vertices[3] = x_ndc - point_size;  vertices[4] = y_ndc - point_size; vertices[5] = 0.0f; // Bottom left vertex
    vertices[6] = x_ndc + point_size;  vertices[7] = y_ndc - point_size; vertices[8] = 0.0f; // Bottom right vertex
}

int main() {
    std::cout << "Program Start" << std::endl;
    
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        std::cerr << "Failed to initialize SDL: " << SDL_GetError() << std::endl;
        return -1;
    }


    // This tells the SDL library that we're about to make an OpenGL context for OpenGL version 3.3
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);

    // Initializes a window object that's centered, 800x600, uses OpenGL, and is shown the moment it's created.
    int x_window_size = 800;
    int y_window_size = 600;
    SDL_Window *window = SDL_CreateWindow("Test_Window",
                     SDL_WINDOWPOS_CENTERED,
                     SDL_WINDOWPOS_CENTERED,
                     800, 600,
                     SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);


    if (!window) {
        std::cerr << "Failed to create window: " << SDL_GetError() << std::endl;
        SDL_Quit();
        return -1;
    }
    SDL_GL_CreateContext(window);

    // This line initializes GLAD with the process address provided by SDL.
    // Apparently you need to give it a recasted function pointer.

    if (!gladLoadGLLoader((GLADloadproc)SDL_GL_GetProcAddress)) {
        std::cerr << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    GLint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);

    GLint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);

    int shader_compile_success = 0;
    char infoLog[512];
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &shader_compile_success);
    if (!shader_compile_success) {
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        std::cerr << "Error: Vertex shader compilation failed\n" << infoLog << std::endl;
    }

    shader_compile_success = 0;    
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &shader_compile_success);
    if (!shader_compile_success) {
        glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
        std::cerr << "Error: Fragment shader compilation failed\n" << infoLog << std::endl;
    }

    GLint shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &shader_compile_success);
    if (!shader_compile_success) {
        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
        std::cerr << "Error: Shader program linking failed\n" << infoLog << std::endl;
    }

    glUseProgram(shaderProgram);
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    bool running = true;
    SDL_Event event;

    int prev_x_window_size = x_window_size;
    int prev_y_window_size = y_window_size;
    GLint viewport[4];
    GLint pre_viewport[4];
    glGetIntegerv(GL_VIEWPORT, viewport);
    

    int mouse_x = 0;
    int mouse_y = 0;
    glClearColor(0.2f, 0.3f, 0.8f, 1.0f);

    // Set up initial VAO configuration for drawing a small triangle point
    GLuint vao_handle, vbo_handle;
    glGenVertexArrays(1, &vao_handle);
    glGenBuffers(1, &vbo_handle);

    // Very rudimentary way of drawing a point where the mouse clicked
    float mouse_point_vertices[9];
    calculate_mouse_point_vertices(mouse_point_vertices, mouse_x, mouse_y, x_window_size, y_window_size);

    // Perform an initial binding sequence with buffer upload of 0,0 coordinates
    // This is just to set up the VAO and VBO; we won't actually draw this yet.
    // This way, VAO is set up so we don't have to call most of this every frame.
    glBindBuffer(GL_ARRAY_BUFFER, vbo_handle);
    glBufferData(GL_ARRAY_BUFFER, sizeof(mouse_point_vertices), mouse_point_vertices, GL_STATIC_DRAW);
    glBindVertexArray(vao_handle);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

    while (running) {

        // This sets the "screen clear" color to blue, and then clears the screen to that color.
        glClear(GL_COLOR_BUFFER_BIT);
        int event_count = 0;
        bool resized = false;

        /* The variable "event" is a struct that contains a reference to a stack
           that holds all of the events since the last time the stack was cleared.
           The stack gets popped when SDL_PollEvent is called
           
           Therefore, it's important to make sure that the actions in this while loop
           happen faster than the user can pipe in more events. Otherwise, you
           never get to the rendering part of the loop. */

        while (SDL_PollEvent(&event)) {
            event_count++;
            if (event.type == SDL_QUIT) {
                running = false;
                std::cout << "Event Code: " << event.type << " received" << std::endl;
            }
            
            if (event.type == SDL_WINDOWEVENT && event.window.event == SDL_WINDOWEVENT_RESIZED) {
                prev_x_window_size = x_window_size;
                prev_y_window_size = y_window_size;
                glGetIntegerv(GL_VIEWPORT, pre_viewport);
                glViewport(0, 0, event.window.data1, event.window.data2);
                glGetIntegerv(GL_VIEWPORT, viewport);
                x_window_size = event.window.data1;
                y_window_size = event.window.data2;
                bool resized = true;   
            }

            // This is a very rudimentary implementation of "if mouse clicked, draw a dot there"
            if (event.type == SDL_MOUSEBUTTONDOWN) {
                mouse_x = event.button.x;
                mouse_y = event.button.y;
                
               
            }
        }
        calculate_mouse_point_vertices(mouse_point_vertices, mouse_x, mouse_y, x_window_size, y_window_size);
        glBindVertexArray(vao_handle);
        glBindBuffer(GL_ARRAY_BUFFER, vbo_handle);
        glBufferData(GL_ARRAY_BUFFER, sizeof(mouse_point_vertices), mouse_point_vertices, GL_STATIC_DRAW);
        // Draw the triangle point at the mouse click location
        glDrawArrays(GL_TRIANGLES, 0, 3);



        SDL_GL_SwapWindow(window); // Update the window with OpenGL rendering
        SDL_GL_SetSwapInterval(1); // Enable V-Sync

        std::cout << "\033[2J\033[H";  // Clear screen, cursor to top
        // Debug output
        if (event_count > 0) {
            std::cout << "Events processed this frame: " << event_count << std::endl;
        } else {
            std::cout << "No events this frame." << std::endl;
        }

        if (resized) {
            std::cout << "Window resized to " << x_window_size << "x" << y_window_size << std::endl;
            std::cout << "Current viewport: " << viewport[0] << ", " << viewport[1] 
                      << ", " << viewport[2] << ", " << viewport[3] << std::endl;
            std::cout << "Previous viewport: " << pre_viewport[0] << ", " << pre_viewport[1]
                      << ", " << pre_viewport[2] << ", " << pre_viewport[3] << std::endl;
        } else {
            std::cout << "Window size: " << x_window_size << "x" << y_window_size << std::endl;
            std::cout << "Viewport size: " << viewport[2] << "x" << viewport[3] << std::endl;
            std::cout << "Previous viewport: " << pre_viewport[0] << ", " << pre_viewport[1]
                      << ", " << pre_viewport[2] << ", " << pre_viewport[3] << std::endl;

        }
        std::cout << "Mouse last clicked at: " << mouse_x << "," << mouse_y << std::endl;
        std::cout << "Mouse point vertices: ";
        for (int i = 0; i < 9; i++) {
            std::cout << mouse_point_vertices[i] << " ";
        }
        std::cout << std::endl; 
        SDL_Delay(FRAME_RATE);
    }


    // Put a quit at the end because teardown
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}