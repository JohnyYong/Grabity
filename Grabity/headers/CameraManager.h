/*!****************************************************************
\file: CameraManager.h
\author: Jeremy Lim Ting Jie, jeremytingjie.lim, 2301370
\brief: This file defines the CameraManager class, which manages multiple camera modes, 
specifically the PlayerCamera and EditorCamera.

Copyright (C) 2024 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents
without the prior written consent of DigiPen Institute of
Technology is prohibited.
*******************************************************************!*/
#pragma once
#include "Camera.h"
#include "PlayerCamera.h"
#include <GLHelper.h>

class CameraManager {
public:
    enum class CameraMode {
        PlayerCamera,
        EditorCamera,
        None
    };

    // Constructor to initialize the camera manager with parameters for the cameras
    CameraManager(float playerLeft, float playerRight, float playerBottom, float playerTop,
        float debugLeft, float debugRight, float debugBottom, float debugTop);

    // Method to set the camera mode directly
    void SetCameraMode(CameraMode mode);

    // Method to toggle between PlayerCamera and EditorCamera
    void ToggleCameraMode();

    // New method to handle key input for toggling the camera mode
    void HandleCameraToggleInput(GLFWwindow* window);

    //Method to get the current camera mode
    CameraMode GetCurrentMode() const;
    Camera& GetEditorCamera() { return EditorCamera; }
    PlayerCamera& GetPlayerCamera() { return playerCamera; }
    Camera* GetCurrentCamera() {
        if (currentMode == CameraMode::PlayerCamera) {
            return &playerCamera; // Return player camera
        }
        else {
            return &EditorCamera; // Return debug camera
        }
    }

private:
    CameraMode currentMode; // Stores the current camera mode
    PlayerCamera playerCamera; // Player camera instance
    Camera EditorCamera; // Debug camera instance
    bool isCameraTogglePressed = false; // Track the camera toggle state
};
