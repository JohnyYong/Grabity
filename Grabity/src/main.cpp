/*!****************************************************************
\file:      main.cpp
\author:    Ridhwan Mohamed Afandi, mohamedridhwan.b, 2301367
\brief:     Entry point for the application. Initializes the
            OpenGL context, engine, and handles the game loop.
\details:   This file sets up the application by reading
            configuration settings, initializing required
            components, and executing the main loop that updates
            and renders the game.

Copyright (C) 2024 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents
without the prior written consent of DigiPen Institute of
Technology is prohibited.
*******************************************************************!*/


/*                                                                   includes
----------------------------------------------------------------------------- */
#define _CRTDBG_MAP_ALLOC

#include <pch.h>
#include <chrono>
#include <thread>
#include <main.h>
#include <GLHelper.h>
#include <engine.h>
#include <LuaConfig.h>
#include <InterruptionHandler.h>  // Include the header for InterruptionHandler
#ifdef _IMGUI
#include <imguimanager.h>
#endif // _IMGUI

/*                                                      function declarations
----------------------------------------------------------------------------- */
static void Draw();
static void Update();
static void Init(GLint width, GLint height, std::string title);
static void Cleanup();

int main() {
    _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF); //Memory Leak


    auto initStart = std::chrono::high_resolution_clock::now();
    LuaManager luaManager("Assets/Lua/config.lua");
    Init(   luaManager.LuaReadFromWindow<int>("Width"),
            luaManager.LuaReadFromWindow<int>("Height"),
            luaManager.LuaReadFromWindow<std::string>("Name"));

    int targetFPS = luaManager.LuaReadFromWindow<int>("TargetFramerate");
    const double frameDuration = 1000.0 / targetFPS;
    

    // Initialize the interruption handler with the GLFW window
    InterruptionHandler::Init(InputManager::ptrWindow);  // Pass the GLFW window to the handler

    while (!glfwWindowShouldClose(InputManager::ptrWindow)) {
        auto frameStart = std::chrono::high_resolution_clock::now();
        InputManager::Update();

        Update();

        Draw();

        auto frameEnd = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double, std::milli> elapsed = frameEnd - frameStart;

		// sleep if the frame was rendered too quickly
        while (elapsed.count() < frameDuration) {
            frameEnd = std::chrono::high_resolution_clock::now();
            elapsed = frameEnd - frameStart;
        }

        auto initEnd = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> initElapsed = initEnd - initStart;
        //std::cout << "Initialization Time: " << initElapsed.count() << " seconds\n";
    }
    Cleanup();
    //int* a = new int{ 5 };
}

/*  _________________________________________________________________________ */
/*! init
@param none
@return none

Get handle to OpenGL context through GLHelper::GLFWwindow*.
*/
static void Init(GLint width, GLint height, std::string title) {
    if (!InputManager::Init(width, height, title)) {
        std::cout << "Unable to create OpenGL context";
        std::exit(EXIT_FAILURE);
    }

    Engine& engine = Engine::GetInstance();
    engine.Init(width, height);
//#ifdef _DEBUG
//    InputManager::PrintSpecs();
//#endif
}


/*  _________________________________________________________________________ */
/*! update
@param none
@return none

Uses GLHelper::GLFWWindow* to get handle to OpenGL context.
For now, there are no objects to animate nor keyboard, mouse button click,
mouse movement, and mouse scroller events to be processed.
*/
static void Update() {
    glfwPollEvents();

    InputManager::UpdateTime(1.0);
    Engine& engine = Engine::GetInstance();
    engine.Update();
}

/*  _________________________________________________________________________ */
/*! draw
@param none
@return none43

Uses GLHelper::GLFWWindow* to get handle to OpenGL context.
For now, there's nothing to draw - just paint color buffer with constant color
*/
static void Draw() {
    Engine& engine = Engine::GetInstance();
    engine.Draw();
    glfwSwapBuffers(InputManager::ptrWindow);
}

/*  _________________________________________________________________________ */
/*! cleanup
@param none
@return none

Return allocated resources for window and OpenGL context thro GLFW back
to system.
Return graphics memory claimed through
*/
void Cleanup() {
    // Part 2
    InputManager::Cleanup();
    Engine().GetInstance().Exit();
}