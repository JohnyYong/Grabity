/*!****************************************************************
\file:      GLHelper.h
\author:    Lee Yu Jie Brandon , l.yujiebrandon , 2301232

\brief:     This file defines the InputManager class, which provides a centralized
            system for handling OpenGL context initialization, window creation,
            and input management.

Copyright (C) 2024 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents
without the prior written consent of DigiPen Institute of
Technology is prohibited.
 *******************************************************************/
#pragma once

#include <string>
#include <unordered_map>
#include <pch.h>

 /**
  * @class InputManager
  * @brief Manages OpenGL context, window creation, and input handling.
  *
  * This class provides functions for initializing GLFW, setting up callback
  * functions for various input events (keyboard, mouse, window resizing),
  * and tracking input states. It also manages timing-related functions for
  * FPS calculation and delta time.
  */
class InputManager {
public:
    /**
     * @brief Initializes the GLFW window and OpenGL context.
     * @param newWidth The width of the window to create.
     * @param newHeight The height of the window to create.
     * @param newTitle The title of the window to create.
     * @return true if initialization was successful, false otherwise.
     */
    static bool Init(GLint width, GLint height, std::string title);

    /**
    * @brief Cleans up GLFW resources.
    */
    static void Cleanup();

    /**
     * @brief Updates input states and time-related variables.
     */
    static void Update();

    /**
     * @brief Updates time-related variables and calculates FPS.
     * @param fps_calc_interval The interval for FPS calculation (default is 1.0 second).
     */
    static void UpdateTime(double fps_calc_interval = 1.0);

    /**
     * @brief Prints OpenGL specifications (currently commented out).
     */
    static void PrintSpecs();

    // Input state queries
    /**
     * @brief Checks if a specific key is currently pressed.
     * @param key The key code to check.
     * @return true if the key is pressed, false otherwise.
     */
    static bool IsKeyDown(int key);


    //By Johny
    /**
     * @brief Checks the immediate instance if a specific key is pressed.
     * @param key The key code to check.
     * @return true if the key is pressed once, false otherwise.
     */
    static bool IsKeyPressed(int key);


    /**
    * @brief Checks if a specific mouse button is currently pressed.
    * @param button The mouse button code to check.
    * @return true if the mouse button is pressed, false otherwise.
    */
    static bool IsMouseButtonPressed(int button);

    /**
     * @brief Gets the current mouse position.
     * @param[out] x The x-coordinate of the mouse cursor.
     * @param[out] y The y-coordinate of the mouse cursor.
     */
    static void GetMousePosition(double& x, double& y);

    /**
     * @brief Gets the current scroll offset.
     * @param[out] x The horizontal scroll offset.
     * @param[out] y The vertical scroll offset.
     */
    static void GetScrollOffset(double& x, double& y);

    // Getters for window properties and timing information
    static GLint GetWidth() { return width; }
    static GLint GetHeight() { return height; }
    static GLdouble GetFPS() { return fps; }
    static GLdouble GetDeltaTime() { return deltaTime; }
    static const std::string& GetTitle() { return title; }
    static GLFWwindow* GetWindow() { return ptrWindow; }
    /// Time elapsed since last frame
    static GLdouble deltaTime;
    /// Pointer to the GLFW window
    static GLFWwindow* ptrWindow;


    //By Jeremy
    //Retrieves the current state of all keys.
    static std::unordered_map<int, bool> GetKeyStates();
    //Enables input handling for the application.
    static void EnableInput();
    //Disables input handling for the application.
    static void DisableInput();
    
    //By JOhny
    static bool IsKeyReleased(int key);

private:

    /**
     * @brief Sets up event callbacks for GLFW.
     */
    static void SetUpEventCallBacks();

    // Callback functions
    /**
     * @brief Callback function for GLFW errors.
     * @param error The error code.
     * @param description A description of the error.
     */
    static void ErrorCB(int error, char const* description);

    /**
     * @brief Callback function for framebuffer size changes.
     * @param ptr_win Pointer to the GLFW window.
     * @param newWidth The new width of the framebuffer.
     * @param newHeight The new height of the framebuffer.
     */
    static void FBSizeCB(GLFWwindow* ptr_win, int width, int height);

    /**
     * @brief Callback function for keyboard events.
     * @param pwin Pointer to the GLFW window.
     * @param key The key that was pressed or released.
     * @param scancode The system-specific scancode of the key.
     * @param action GLFW_PRESS, GLFW_RELEASE or GLFW_REPEAT.
     * @param mod Bit field describing which modifier keys were held down.
     */
    static void KeyCB(GLFWwindow* pwin, int key, int scancode, int action, int mod);

    /**
     * @brief Callback function for mouse button events.
     * @param pwin Pointer to the GLFW window.
     * @param button The mouse button that was pressed or released.
     * @param action GLFW_PRESS or GLFW_RELEASE.
     * @param mod Bit field describing which modifier keys were held down.
     */
    static void MouseButtonCB(GLFWwindow* pwin, int button, int action, int mod);

    /**
     * @brief Callback function for cursor position changes.
     * @param pwin Pointer to the GLFW window.
     * @param xpos The new x-coordinate of the cursor.
     * @param ypos The new y-coordinate of the cursor.
     */
    static void MousePosCB(GLFWwindow* pwin, double xpos, double ypos);

    /**
     * @brief Callback function for scroll events.
     * @param pwin Pointer to the GLFW window.
     * @param xoffset The scroll offset along the x-axis.
     * @param yoffset The scroll offset along the y-axis.
     */
    static void MouseScrollCB(GLFWwindow* pwin, double xoffset, double yoffset);

    // Static member variables
    /// Window width
    static GLint width;
    /// Window height
    static GLint height;
    /// Frames per second
    static GLdouble fps;
    /// Window title
    static std::string title;
    
    //by Johny
    static std::unordered_map<int, bool> justPressedKeys;

    static std::unordered_map<int, bool> prevKeyStates;

    // Input state tracking
    /// Map to store the state of each key
    static std::unordered_map<int, bool> keyStates;
    /// Map to store the state of each mouse button
    static std::unordered_map<int, bool> mouseButtonStates;
    /// Current x-coordinate of the mouse cursor
    static double mouseX;
    /// Current y-coordinate of the mouse cursor
    static double mouseY;
    /// Accumulated horizontal scroll offset
    static double scrollX;
    /// Accumulated vertical scroll offset
    static double scrollY;
};