/*!****************************************************************
\file:      main.h
\author:    Ridhwan Mohamed Afandi, mohamedridhwan.b, 2301367
\brief:     Header file for the main application entry point and related functions.
\details:   This file contains functions for handling OpenGL initialization, window 
            creation, and frame updates. Additionally, this file provides the setup
            for input handling and managing the main game loop.

Copyright (C) 2024 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents
without the prior written consent of DigiPen Institute of
Technology is prohibited.
*******************************************************************!*/

#pragma once

#ifdef _IMGUI
#include <iostream>
#endif // _IMGUI
#include <glad.h>
#include <glfw3.h>
#ifdef _IMGUI
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <ImGuiConsole.h>
#endif // _IMGUI

/*!****************************************************************
\func  key_callback
\brief Callback function for handling key press and release events.
\details This function is called whenever a key is pressed or released in the GLFW window. It logs the key events if logging is enabled.
\param window The GLFW window that received the event.
\param key The key that was pressed or released.
\param scancode The system-specific scancode of the key.
\param action The action that was performed (GLFW_PRESS or GLFW_RELEASE).
\param mods The modifier keys pressed (e.g., Shift, Control, etc.).
*******************************************************************!*/
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    (void)mods;
    (void)scancode;
    (void)window;
    (void)key;
    if (action == GLFW_PRESS) {
#ifdef _LOGGING
        //ImGuiConsole::Cout("Key pressed: %d", key);
#endif // _LOGGING
    }
    else if (action == GLFW_RELEASE) {
#ifdef _LOGGING
        //ImGuiConsole::Cout("Key released: %d", key);
#endif // _LOGGING
    }
}

/*!****************************************************************
\func  mouse_button_callback
\brief Callback function for handling mouse button press and release events.
\details This function is called whenever a mouse button is pressed or released in the GLFW window. It logs the mouse button events if logging is enabled.
\param window The GLFW window that received the event.
\param button The mouse button that was pressed or released (GLFW_MOUSE_BUTTON_LEFT, GLFW_MOUSE_BUTTON_RIGHT, etc.).
\param action The action that was performed (GLFW_PRESS or GLFW_RELEASE).
\param mods The modifier keys pressed (e.g., Shift, Control, etc.).
*******************************************************************!*/
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
    (void)mods;
    (void)window;
	(void)button;
    if (action == GLFW_PRESS) {
#ifdef _LOGGING
		ImGuiConsole::Cout("Mouse button pressed: %d", button);
#endif // _LOGGING
    }
    else if (action == GLFW_RELEASE) {
#ifdef _LOGGING
		ImGuiConsole::Cout("Mouse button released: %d", button);
#endif // _LOGGING
    }
}

/*!****************************************************************
\func  cursor_position_callback
\brief Callback function for handling mouse cursor position changes.
\details This function is called whenever the mouse cursor moves within the GLFW window. It logs the cursor's new position if logging is enabled.
\param window The GLFW window that received the event.
\param xpos The new x-coordinate of the mouse cursor.
\param ypos The new y-coordinate of the mouse cursor.
*******************************************************************!*/
void cursor_position_callback(GLFWwindow* window, double xpos, double ypos) {
    (void)window;
    (void)xpos;
	(void)ypos;
#ifdef _LOGGING
	ImGuiConsole::Cout("Cursor moved to: (%.2f, %.2f)", xpos, ypos);
#endif // _LOGGING
}