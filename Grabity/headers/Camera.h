/*!****************************************************************
\file: Camera.h
\author: Jeremy Lim Ting Jie, jeremytingjie.lim , 2301370
\brief: Header file for the Camera class, defining camera properties
        and functions for handling movement, rotation, zoom,
        shaking effects.

Copyright (C) 2024 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents
without the prior written consent of DigiPen Institute of
Technology is prohibited.
*******************************************************************!*/

#pragma once
#include "Maths.h"
#include "Vector3.h"
#include "Matrix4x4.h"

class Camera
{
public:
    // Constructor to initialize the camera with given orthographic bounds
    Camera(float left, float right, float bottom, float top);

    //Handling of camera movements
    void HandleMouseDrag(double mouseX, double mouseY, double& lastMouseX, double& lastMouseY, bool& isDragging); // mouse dragging
    void HandleMovement(float deltaTime); // W A S D movements
    void HandleZoom(float zoomSpeed); // key Q ,R zooming
    void HandleRotation(float rotateSpeed); // key R ,T rotation
    void HandleReset(); // key Z rotate
    void HandleShake(bool isActive, bool isShooting); // key X shake

    //Camera binding stuff
    bool m_IsBoundToPlayer = false; // Camera binding state
    void ToggleBindingToPlayer();

    // Getters
    float GetAngle() const { return m_Angle; } // get angle (rotation)
    inline const Vector2& GetViewingRange() const { return m_ViewingRange; } // get viewing range
    const Matrix4x4& GetViewMatrix() const { return m_ViewMatrix; } // get view matrix
    const Matrix4x4& GetProjectionMatrix() const { return m_ProjMatrix; } // get projection matrix
    inline const Vector2& GetCenter() const { return m_Center; } // get center of camera
    float GetScale() const { return m_Scale; } // get scale (zoom)

    // Setters
    void SetAngle(float angle); // set angle (rotation)
    void SetCenter(const Vector2& center); // set center
    void SetScale(float scaleLevel); // set scale (zoom)

    // Function to update the view matrix
    void UpdateViewMatrix();
    void UpdateViewMatrix(float deltaTime);

    //Reset camera to player or default center
    void Reset(const Vector2& defaultCenter = Vector2(1.0f, 1.0f));
    //Camera shake
    void Shake(float intensity, float duration);
    void UpdateShake(float deltaTime);
    bool IsPlayerAvailable() const;
private:
    Matrix4x4 m_ViewMatrix;                 // Matrix for the camera's view
    Matrix4x4 m_ProjMatrix;                 // Matrix for the camera's projection
    Vector2 m_Center;                       // Center position of the camera
    Vector2 m_OriginalViewingRange;         // The original setting of the viewing range of the game world
    Vector2 m_ViewingRange;                 // The Viewing range of the game world
    float m_Scale;                          // Scale factor (zoom level)
    float m_Angle;                          // Rotation angle in degrees
    
    
    float m_ShakeIntensity;                 // Intensity of the shake
    float m_ShakeDuration;                  // Duration of the shake in seconds
    float m_ShakeElapsedTime;               // keeps track of the elapsed time since the shake effect started
    float m_InitialShakeIntensity;          // 
    bool m_IsShaking;

    float m_SuctionShakeIntensity = 0.0f;
    float m_ShootingShakeIntensity = 0.0f;
    float m_MaxSuctionIntensity = 4.0f; // Cap suction shake at 5.0
    float m_MaxShootingIntensity = 15.0f; // Cap shooting shake at 5
    float m_ShakeDecayRate = 3.0f; // General decay rate for all shake types
    bool m_IsSuctionShaking = false;
    bool m_IsShootingShaking = false;
};