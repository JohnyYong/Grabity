/*!****************************************************************
\file:      GLHelper.h
\author:    Lee Yu Jie Brandon , l.yujiebrandon , 2301232

\brief:     Implementation of the InputManager class, which provides a centralized 
            system for handling OpenGL context initialization, window creation, 
            and input management.

Copyright (C) 2024 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents
without the prior written consent of DigiPen Institute of
Technology is prohibited.
 *******************************************************************/
#include "glhelper.h"

#ifdef _IMGUI
#include <iostream>
#endif // _IMGUI
#include <pch.h>
#include "engine.h"
#include "ImGuiConsole.h"

// Initialize static member variables
GLint InputManager::width = 0;
GLint InputManager::height = 0;
GLdouble InputManager::fps = 0.0;
GLdouble InputManager::deltaTime = 0.0;
std::string InputManager::title;
GLFWwindow* InputManager::ptrWindow = nullptr;

// New members for input state tracking
std::unordered_map<int, bool> InputManager::keyStates;
std::unordered_map<int, bool> InputManager::prevKeyStates;
std::unordered_map<int, bool> InputManager::mouseButtonStates;
std::unordered_map<int, bool> InputManager::justPressedKeys;

double InputManager::mouseX = 0.0;
double InputManager::mouseY = 0.0;
double InputManager::scrollX = 0.0;
double InputManager::scrollY = 0.0;

bool InputManager::Init(GLint newWidth, GLint newHeight, std::string newTitle) {
    InputManager::width = newWidth;
    InputManager::height = newHeight;
    InputManager::title = newTitle;

    if (!glfwInit()) {
#ifdef _LOGGING
        ImGuiConsole::Cout("GLFW init has failed - abort program!!!");
#endif // _LOGGING
        return false;
    }



    glfwSetErrorCallback(InputManager::ErrorCB);

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    glfwWindowHint(GLFW_DOUBLEBUFFER, GLFW_TRUE);
    glfwWindowHint(GLFW_DEPTH_BITS, 24);
    glfwWindowHint(GLFW_RED_BITS, 8); glfwWindowHint(GLFW_GREEN_BITS, 8);
    glfwWindowHint(GLFW_BLUE_BITS, 8); glfwWindowHint(GLFW_ALPHA_BITS, 8);

    // Ridhwan: Create mode for fullscreen
    LuaManager luaManager("Assets/Lua/config.lua");
    if (luaManager.LuaReadFromWindow<bool>("Fullscreen")) {
        // Get the primary monitor
        GLFWmonitor* primaryMonitor = glfwGetPrimaryMonitor();
        const GLFWvidmode* mode = glfwGetVideoMode(primaryMonitor);

        width = mode->width;
        height = mode->height;

        InputManager::ptrWindow = glfwCreateWindow(width, height, title.c_str(), primaryMonitor, NULL);
    }
    else {
#ifndef _IMGUI
        glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
#endif // !_IMGUI
        // Create a windowed mode window
        InputManager::ptrWindow = glfwCreateWindow(width, height, title.c_str(), NULL, NULL);


    }

    if (!InputManager::ptrWindow) {
		ImGuiConsole::Cout("GLFW unable to create OpenGL context - abort program");
        glfwTerminate();
        return false;
    }

    glfwMakeContextCurrent(InputManager::ptrWindow);

    InputManager::SetUpEventCallBacks();

    glfwSetInputMode(InputManager::ptrWindow, GLFW_CURSOR, GLFW_CURSOR_NORMAL);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
#ifdef _LOGGING
        std::cout << "Unable to initialize GLAD - abort program"<< std::endl;
#endif // _LOGGING
        return false;
    }
    if (GLAD_GL_VERSION_4_5) {
#ifdef _LOGGING
        std::cout << "Using GLAD with OpenGL version: 4.5"<< std::endl;
        std::cout << "Driver supports OpenGL 4.5\n"<< std::endl;
#endif // _LOGGING
    }
    else {
#ifdef _LOGGING
        std::cout << "Warning: The driver may lack full compatibility with OpenGL 4.5, potentially limiting access to advanced features."<< std::endl;
#endif // _LOGGING
    }

    // Initialize input states
    for (int key = GLFW_KEY_SPACE; key <= GLFW_KEY_LAST; ++key) {
        keyStates[key] = false;
        justPressedKeys[key] = false;
        prevKeyStates[key] = false;
    }
    for (int button = GLFW_MOUSE_BUTTON_1; button <= GLFW_MOUSE_BUTTON_LAST; ++button) {
        mouseButtonStates[button] = false;
    }

    return true;
}

void InputManager::Cleanup() {
    if (InputManager::ptrWindow) {
        glfwDestroyWindow(InputManager::ptrWindow);
        InputManager::ptrWindow = nullptr;
    }

    glfwTerminate();
}

void InputManager::Update() {

    //ImGuiConsole::Cout("Prev Esc: " << prevKeyStates[256]);
    //ImGuiConsole::Cout("curr Esc: " << keyStates[256] << std::endl);
    for (std::pair<const int, bool>& it : keyStates)
        prevKeyStates[it.first] = it.second;
    //glfwPollEvents();
    //UpdateTime();
}

void InputManager::UpdateTime(double fps_calc_interval) {
    static double prev_time = glfwGetTime();
    double curr_time = glfwGetTime();
    deltaTime = curr_time - prev_time;
    prev_time = curr_time;

    static double count = 0.0;
    static double start_time = glfwGetTime();
    double elapsed_time = curr_time - start_time;

    ++count;

    fps_calc_interval = (fps_calc_interval < 0.0) ? 0.0 : fps_calc_interval;
    fps_calc_interval = (fps_calc_interval > 10.0) ? 10.0 : fps_calc_interval;
    if (elapsed_time > fps_calc_interval) {
        InputManager::fps = count / elapsed_time;
        start_time = curr_time;
        count = 0.0;
    }
}

void InputManager::PrintSpecs() {
    //GLubyte const* str_ven = glGetString(GL_VENDOR);
    //ImGuiConsole::Cout("GPU Vendor: " << str_ven);
    //GLubyte const* str_renderer = glGetString(GL_RENDERER);
    //ImGuiConsole::Cout("GL Renderer: " << str_renderer);
    //GLubyte const* str_version = glGetString(GL_VERSION);
    //ImGuiConsole::Cout("GL Version: " << str_version);
    //GLubyte const* str_shader_version = glGetString(GL_SHADING_LANGUAGE_VERSION);
    //ImGuiConsole::Cout("GL Shader Version: " << str_shader_version);
    //GLint major_version, minor_version;
    //glGetIntegerv(GL_MAJOR_VERSION, &major_version);
    //glGetIntegerv(GL_MINOR_VERSION, &minor_version);
    //ImGuiConsole::Cout("GL Major Version: " << major_version);
    //ImGuiConsole::Cout("GL Minor Version: " << minor_version);
    //GLint double_buffered;
    //glGetIntegerv(GL_DOUBLEBUFFER, &double_buffered);
    //if (double_buffered) {
    //    ImGuiConsole::Cout("Current OpenGL Context is double-buffered");
    //}
    //else {
    //    ImGuiConsole::Cout("Current OpenGL Context is not double-buffered");
    //}
    //GLint max_vertex_count;
    //glGetIntegerv(GL_MAX_ELEMENTS_VERTICES, &max_vertex_count);
    //ImGuiConsole::Cout("Maximum Vertex Count: " << max_vertex_count);
    //GLint max_indices_count;
    //glGetIntegerv(GL_MAX_ELEMENTS_INDICES, &max_indices_count);
    //ImGuiConsole::Cout("Maximum Indices Count: " << max_indices_count);
    //GLint max_texture_size;
    //glGetIntegerv(GL_MAX_TEXTURE_SIZE, &max_texture_size);
    //ImGuiConsole::Cout("GL Maximum texture size: " << max_texture_size);
    //GLint max_viewport_dims[2];
    //glGetIntegerv(GL_MAX_VIEWPORT_DIMS, max_viewport_dims);
    //ImGuiConsole::Cout("Maximum Viewport Dimensions: " << max_viewport_dims[0] << " x " << max_viewport_dims[1]);
    //GLint max_vertex_attribs;
    //glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &max_vertex_attribs);
    //ImGuiConsole::Cout("Maximum generic vertex attributes: " << max_vertex_attribs);
    //GLint max_vertex_bindings;
    //glGetIntegerv(GL_MAX_VERTEX_ATTRIB_BINDINGS, &max_vertex_bindings);
    //ImGuiConsole::Cout("Maximum vertex buffer bindings: " << max_vertex_bindings);
}

void InputManager::SetUpEventCallBacks() {
    glfwSetFramebufferSizeCallback(InputManager::ptrWindow, InputManager::FBSizeCB);
    glfwSetKeyCallback(InputManager::ptrWindow, InputManager::KeyCB);
    glfwSetMouseButtonCallback(InputManager::ptrWindow, InputManager::MouseButtonCB);
    glfwSetCursorPosCallback(InputManager::ptrWindow, InputManager::MousePosCB);
    glfwSetScrollCallback(InputManager::ptrWindow, InputManager::MouseScrollCB);
}

bool InputManager::IsKeyDown(int key) {
    return keyStates[key];
}

//By Johny
bool InputManager::IsKeyPressed(int key)
{
    /* if (justPressedKeys[key]) {
         justPressedKeys[key] = false;
         return true;
     }
     return false;*/
    if (!prevKeyStates[key] && keyStates[key])
        return true;
    return false;
}

//By Johny
bool InputManager::IsKeyReleased(int key) {
    // Check if the key was just released
    //if (!keyStates[key] && justPressedKeys[key]) {
    //    justPressedKeys[key] = false; // Reset state
    //    return true;
    //}
    //return keyStates[key];

    if (prevKeyStates[key] && !keyStates[key])
        return true;
    return false;
}



bool InputManager::IsMouseButtonPressed(int button) {
    return mouseButtonStates[button];
}

void InputManager::GetMousePosition(double& x, double& y) {
    x = mouseX;
    y = mouseY;
}

void InputManager::GetScrollOffset(double& x, double& y) {
    x = scrollX;
    y = scrollY;
    // Reset scroll offset after reading
    scrollX = 0.0;
    scrollY = 0.0;
}

void InputManager::ErrorCB(int error, char const* description) {
	ImGuiConsole::Cout("GLFW Error %d: %s\n", error, description);
}

void InputManager::FBSizeCB(GLFWwindow* ptr_win, int newWidth, int newHeight) {
    (void)ptr_win;
#ifdef _IMGUI
    InputManager::width = newWidth;
    InputManager::height = newHeight;
#else
    (void)newWidth;
    (void)newHeight;
#endif // _IMGUI
}

void InputManager::KeyCB(GLFWwindow* pwin, int key, int scancode, int action, int mod) {
    (void)scancode;
    (void)mod;
    (void)pwin;

    if (action == GLFW_PRESS) {
        keyStates[key] = true;
        //By Johny
        if (!justPressedKeys[key]) {
            justPressedKeys[key] = true;
#ifdef _LOGGING
			ImGuiConsole::Cout("Key just pressed: %d", key);
#endif // _LOGGING
        }
#ifdef _LOGGING
		ImGuiConsole::Cout("Key held down: %d", key);
#endif // _LOGGING
    }
    else if (action == GLFW_RELEASE) {
        keyStates[key] = false;

        //By Johny
        justPressedKeys[key] = true; 
#ifdef _LOGGING
		ImGuiConsole::Cout("Key released: %d", key);
#endif // _LOGGING
    }

    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS && !Engine::GetInstance().isInGameScene) {
        //glfwSetWindowShouldClose(pwin, GLFW_TRUE);
    }
}

void InputManager::MouseButtonCB(GLFWwindow* pwin, int button, int action, int mod) {
    (void)pwin;
    (void)mod;

    if (action == GLFW_PRESS) {
        mouseButtonStates[button] = true;
#ifdef _LOGGING
		ImGuiConsole::Cout("Mouse button %d pressed", button);
#endif // _LOGGING
    }
    else if (action == GLFW_RELEASE) {
        mouseButtonStates[button] = false;
#ifdef _LOGGING
		ImGuiConsole::Cout("Mouse button %d released", button);
#endif // _LOGGING
    }
}

void InputManager::MousePosCB(GLFWwindow* pwin, double xpos, double ypos) {
    (void)pwin;
    mouseX = xpos;
    mouseY = ypos;
   // ImGuiConsole::Cout("Mouse Position: " << xpos << ", " << ypos);
}

void InputManager::MouseScrollCB(GLFWwindow* pwin, double xoffset, double yoffset) {
    (void)pwin;
    scrollX += xoffset;
    scrollY += yoffset;
//#ifdef _LOGGING
//	//ImGuiConsole::Cout("Mouse scroll: ( %f, %f )", xoffset, yoffset);
//#endif // _LOGGING
}

//By Jeremy
std::unordered_map<int, bool> InputManager::GetKeyStates() {
    return keyStates;
}
void InputManager::EnableInput() {
    glfwSetInputMode(ptrWindow, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
#ifdef _LOGGING
    ImGuiConsole::Cout("Input handling enabled.");
#endif // _LOGGING
}
void InputManager::DisableInput() {
    glfwSetInputMode(ptrWindow, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
#ifdef _LOGGING
    ImGuiConsole::Cout("Input handling disabled.");
#endif // _LOGGING
}
