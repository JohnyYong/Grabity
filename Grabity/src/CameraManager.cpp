/*!****************************************************************
\file: CameraManager.cpp
\author: Jeremy Lim Ting Jie, jeremytingjie.lim, 2301370
\brief: This file contains the implementation of the CameraManager class,
        which is responsible for managing the active camera.
        The class allows switching between a PlayerCamera, which follows 
        the player, and an EditorCamera, which assists in level design.

Copyright (C) 2024 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents
without the prior written consent of DigiPen Institute of
Technology is prohibited.
*******************************************************************!*/
#pragma once
#include "CameraManager.h"

#ifdef _IMGUI
/*!
\brief Constructs a CameraManager with parameters for player and editor cameras in debug mode.

\param playerLeft The left boundary of the player camera's orthographic projection.
\param playerRight The right boundary of the player camera's orthographic projection.
\param playerBottom The bottom boundary of the player camera's orthographic projection.
\param playerTop The top boundary of the player camera's orthographic projection.
\param debugLeft The left boundary of the editor camera's orthographic projection.
\param debugRight The right boundary of the editor camera's orthographic projection.
\param debugBottom The bottom boundary of the editor camera's orthographic projection.
\param debugTop The top boundary of the editor camera's orthographic projection.
*/
CameraManager::CameraManager(float playerLeft, float playerRight, float playerBottom, float playerTop,
    float debugLeft, float debugRight, float debugBottom, float debugTop)
    : currentMode(CameraMode::EditorCamera),
    playerCamera(playerLeft, playerRight, playerBottom, playerTop), // Initialize player camera
    EditorCamera(debugLeft, debugRight, debugBottom, debugTop)       // Initialize debug camera with different parameters
{
    // Additional initialization if necessary
}
#else
/*!
\brief Constructs a CameraManager with parameters for player and editor cameras in release mode.

\param playerLeft The left boundary of the player camera's orthographic projection.
\param playerRight The right boundary of the player camera's orthographic projection.
\param playerBottom The bottom boundary of the player camera's orthographic projection.
\param playerTop The top boundary of the player camera's orthographic projection.
\param debugLeft The left boundary of the editor camera's orthographic projection.
\param debugRight The right boundary of the editor camera's orthographic projection.
\param debugBottom The bottom boundary of the editor camera's orthographic projection.
\param debugTop The top boundary of the editor camera's orthographic projection.
*/
CameraManager::CameraManager(float playerLeft, float playerRight, float playerBottom, float playerTop,
    float debugLeft, float debugRight, float debugBottom, float debugTop)
    : currentMode(CameraMode::PlayerCamera),
    playerCamera(playerLeft, playerRight, playerBottom, playerTop), // Initialize player camera
    EditorCamera(debugLeft, debugRight, debugBottom, debugTop)       // Initialize debug camera with different parameters
{
    // Additional initialization if necessary
}
#endif // _IMGUI

/*!
\brief Sets the camera to the specified mode.

\param mode The camera mode to set, either PlayerCamera or EditorCamera.
*/
void CameraManager::SetCameraMode(CameraMode mode) {
    currentMode = mode; // Change the current camera mode
}

/*!
\brief Toggles between PlayerCamera and EditorCamera modes.
*/
void CameraManager::ToggleCameraMode() {
    // Toggle between player and debug camera modes
    if (currentMode == CameraMode::EditorCamera) {
        currentMode = CameraMode::PlayerCamera;
    }
    else {
        currentMode = CameraMode::EditorCamera;
    }
}

/*!
\brief Retrieves the current camera mode.

\return The current camera mode (PlayerCamera or EditorCamera).
*/
CameraManager::CameraMode CameraManager::GetCurrentMode() const {
    return currentMode; // Return the current camera mode
}

/*!
\brief Handles input for toggling between camera modes using the 'C' key.

\param window The GLFW window pointer for detecting input.
*/
void CameraManager::HandleCameraToggleInput(GLFWwindow* window) {
    (void)window;
    // Check for camera switch
    if (InputManager::IsKeyPressed(GLFW_KEY_C)) {
        // If the key was previously not pressed, toggle the camera mode
        if (!isCameraTogglePressed) {
            ToggleCameraMode(); // Toggle the camera mode
            isCameraTogglePressed = true; // Set the state to pressed
        }
    }
    else {
        // Reset the toggle state when the key is released
        isCameraTogglePressed = false;
    }
}
