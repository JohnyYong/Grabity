/*!****************************************************************
\file: Camera.cpp
\author: Jeremy Lim Ting Jie, jeremytingjie.lim, 2301370
\brief: Implements the Camera class for managing an orthogonal editor camera.
        This class handles camera transformations, including position,
        scale (zoom), rotation, and screen shake effects. It uses a 
        projection matrix for rendering and a view matrix for transforming 
        world coordinates to camera coordinates. The file contains methods
        for updating the camera's view matrix based on its transformations
        and user input, including camera movement, zooming, rotation, 
        and shake effects.

Copyright (C) 2024 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents
without the prior written consent of DigiPen Institute of
Technology is prohibited.
*******************************************************************!*/
#pragma once
#include "Camera.h"
#include "MathUtils.h"
#include "GameObjectFactory.h"
#include <cmath>

/**
 * @brief Constructs a Camera object with an orthographic projection matrix and initializes default properties.
 *
 * @param left The left boundary of the orthographic projection.
 * @param right The right boundary of the orthographic projection.
 * @param bottom The bottom boundary of the orthographic projection.
 * @param top The top boundary of the orthographic projection.
 */
Camera::Camera(float left, float right, float bottom, float top)
    : m_Center(0.0f, 0.0f),   // Initialize the camera center at the origin (0, 0)
    m_Scale(1.0f),            // Initialize the scale level to 1 (no zoom)
    m_Angle(0.0f)            // Initialize the rotation angle to 0 degrees
    //m_ShakeIntensity(0.0f),   // Initialize shake intensity to 0
    //m_ShakeDuration(0.0f),    // Initialize shake duration to 0
    //m_ShakeElapsedTime(0.0f)         // Initialize shake time to 0
{
    // Create an orthographic projection matrix for 2D rendering
    m_ProjMatrix = Matrix4x4::Ortho(left, right, bottom, top, -1.f, 1.f);
    m_ViewingRange = m_OriginalViewingRange = { right, top };

    // Calculate the initial view matrix based on the default center, scale, and angle
    UpdateViewMatrix();
}

/**
 * @brief Sets the center position of the camera.
 *
 * @param center The new center position of the camera.
 */
void Camera::SetCenter(const Vector2& center)
{
    // Update the camera's center with the new value
    m_Center = center;
    // Update the view matrix to reflect the new center
    UpdateViewMatrix();
}

/**
 * @brief Sets the zoom level of the camera.
 *
 * @param scaleLevel The desired zoom level, clamped between 0.1 and 5.0.
 */
void Camera::SetScale(float scaleLevel)
{
    // Clamp the scale level to prevent extreme zoom
    m_Scale = Clamp(scaleLevel, 0.1f, 5.0f);
    // Update the view matrix to account for the new scale level
    UpdateViewMatrix();
}

/**
 * @brief Sets the rotation angle of the camera.
 *
 * @param angle The desired rotation angle in degrees.
 */
void Camera::SetAngle(float angle)
{
    // Update the camera's rotation angle with the new value
    m_Angle = angle;
    // Update the view matrix to reflect the new angle
    UpdateViewMatrix();
}

/**
 * @brief Resets the camera to a specified default position, scale, and angle.
 *
 * @param defaultCenter The default center position to reset the camera to.
 */
void Camera::Reset(const Vector2& defaultCenter) {
    // Retrieve player object using the new helper function
    GameObject* playerObject = GameObjectFactory::GetInstance().GetPlayerObject();

    if (playerObject) {
        // Check if the player has a TransformComponent
        auto playerTransform = playerObject->GetComponent<TransformComponent>(TypeOfComponent::TRANSFORM);
        if (playerTransform) {
            m_Center = playerTransform->GetPosition(); // Center on player's position
#ifdef _LOGGING
			ImGuiConsole::Cout("Camera centered on player position.");
#endif // _LOGGING
        }
        else {
            // Log a warning if the TransformComponent is missing
#ifdef _LOGGING
			ImGuiConsole::Cout("Warning: Player TransformComponent not found. Resetting to default center.");
#endif // _LOGGING
            m_Center = defaultCenter;
        }
    }
    else {
        // Log a warning if the player object is missing
#ifdef _LOGGING
		ImGuiConsole::Cout("Warning: Player object not found. Resetting to default center.");
#endif // _LOGGING
        m_Center = defaultCenter;
    }

    // Reset scale and angle
    m_Scale = 1.0f;
    m_Angle = 0.0f;

    // Update the view matrix to reflect the reset values
    UpdateViewMatrix();
}


/**
 * @brief Initiates a shake effect on the camera.
 *
 * @param intensity The intensity of the shake effect.
 * @param duration The duration of the shake effect in seconds.
 */
void Camera::Shake(float intensity, float duration) {
    (void)intensity;
    (void)duration;
    //if (IsPlayerAvailable()) {
    //    m_InitialShakeIntensity = intensity;
    //    m_ShakeIntensity = intensity;
    //    m_ShakeDuration = duration;
    //    m_ShakeElapsedTime = 0.0f;  // Reset the shake time
    //}
}

/**
 * @brief Updates the camera shake
 *
 * Continues to apply a shaking effect to the camera until the timer is up
 */
void Camera::UpdateShake(float deltaTime)
{

    if (!m_IsSuctionShaking && !m_IsShootingShaking) {
        return; // Exit early if no shake is active
    }

    float totalShakeIntensity = m_SuctionShakeIntensity + m_ShootingShakeIntensity;

    if (totalShakeIntensity > 0.0f) {
        m_ShakeElapsedTime += deltaTime;

        // Generate random shake offsets with reduced magnitude
        float shakeOffsetX = RNGRange(-0.9f, 0.9f) * totalShakeIntensity;
        float shakeOffsetY = RNGRange(-0.9f, 0.9f) * totalShakeIntensity;
        float shakeAngle = RNGRange(-0.2f, 0.2f) * totalShakeIntensity * 0.015f;

        m_Center.x += shakeOffsetX;
        m_Center.y += shakeOffsetY;
        m_Angle += shakeAngle;
    }

    // Apply decay separately for suction and shooting
    if (!m_IsSuctionShaking && m_SuctionShakeIntensity > 0.0f) {
        m_SuctionShakeIntensity -= m_ShakeDecayRate * deltaTime;
        m_SuctionShakeIntensity = std::max(m_SuctionShakeIntensity, 0.0f);
    }

    if (m_IsShootingShaking && m_ShootingShakeIntensity > 0.0f) {
        m_ShootingShakeIntensity -= (m_ShakeDecayRate * 8.0f) * deltaTime; // Faster decay for shooting
        m_ShootingShakeIntensity = std::max(m_ShootingShakeIntensity, 0.0f);

        //ImGuiConsole::Cout("Shooting Shake Intensity: %f", m_ShootingShakeIntensity);
        //ImGuiConsole::Cout("Minusing: %f", (m_ShakeDecayRate * 10.0f)* deltaTime);

    }

    if (m_ShootingShakeIntensity <= 0.0f) {
        m_IsShootingShaking = false;
    }
}



/**
 * @brief Updates the view matrix to apply transformations and handle active shake effects.
 *
 * @param deltaTime The time elapsed since the last update, used for shake animations.
 */
void Camera::UpdateViewMatrix(float deltaTime)
{
    (void)deltaTime;
    UpdateShake((float)InputManager::GetDeltaTime());

    Vector2 cameraTranslate = m_Center;
    Matrix4x4 translation(Matrix4x4::Translation(cameraTranslate.x, cameraTranslate.y, 0.0f));
    Matrix4x4 rotation(Matrix4x4::RotationZ(DegreesToRadians(m_Angle)));
    Matrix4x4 scaling(Matrix4x4::Scale(m_Scale, m_Scale, 1.0f));
    Matrix4x4 combinedTransform = translation * rotation * scaling;
    m_ViewMatrix = combinedTransform.Inverse();
    m_ViewingRange.x = m_OriginalViewingRange.x * m_Scale;
    m_ViewingRange.y = m_OriginalViewingRange.y * m_Scale;
}


/**
 * @brief Toggles the camera mode between player-bound and free-roam.
 */
void Camera::ToggleBindingToPlayer()
{
    m_IsBoundToPlayer = !m_IsBoundToPlayer; // Toggle the camera mode

    if (m_IsBoundToPlayer)
    {
        // Bind to the player (assume player ID or reference is already known)
        const auto& playerPosition = GameObjectFactory::GetInstance().GetAllGameObjects().at(3)
            ->GetComponent<TransformComponent>(TypeOfComponent::TRANSFORM)
            ->GetPosition();
        SetCenter(Vector2(playerPosition.x, playerPosition.y)); // Center the camera on the player
		ImGuiConsole::Cout("Bind mode activate ");
    }
    else
    {
        ImGuiConsole::Cout("Camera is now in free roam mode");
    }
}

/**
 * @brief Adjusts the camera position based on mouse dragging.
 *
 * This function calculates the mouse movement delta and adjusts
 * the camera's position accordingly. The view matrix is updated
 * to reflect the new position.
 *
 * @param mouseX Current mouse X position.
 * @param mouseY Current mouse Y position.
 * @param lastMouseX Last recorded mouse X position.
 * @param lastMouseY Last recorded mouse Y position.
 * @param isDragging Boolean flag indicating whether the mouse is being dragged.
 */
void Camera::UpdateViewMatrix()
{
    // Call the shaking version with zero deltaTime (no shaking applied)
    UpdateViewMatrix(0.0f);
}

/**
 * @brief Processes camera movement based on keyboard input.
 *
 * Handles keyboard inputs for W, A, S, D keys to move the camera
 * vertically and horizontally. Updates the view matrix after movement.
 *
 * @param deltaTime Time elapsed since the last frame.
 */
void Camera::HandleMouseDrag(double mouseX, double mouseY, double& lastMouseX, double& lastMouseY, bool& isDragging)
{
    if (isDragging)
    {
        // Calculate the mouse delta (movement since the last frame)
        double deltaX = mouseX - lastMouseX;
        double deltaY = mouseY - lastMouseY;

        // Update the camera's position based on the mouse movement
        m_Center.x -= static_cast<float>(deltaX) * 2.0f;  // Move camera horizontally
        m_Center.y += static_cast<float>(deltaY) * 2.0f;  // Move camera vertically (inverted Y-axis)

        // Update the view matrix to reflect the new position
        UpdateViewMatrix();

        // Update the last mouse position
        lastMouseX = mouseX;
        lastMouseY = mouseY;
    }
}

/**
 * @brief Processes camera movement based on keyboard input.
 *
 * Handles keyboard inputs for W, A, S, D keys to move the camera
 * vertically and horizontally. Updates the view matrix after movement.
 *
 * @param deltaTime Time elapsed since the last frame.
 */

void Camera::HandleMovement(float deltaTime) {
    if (IsPlayerAvailable()) {
        float cameraSpeed = 600.0f * deltaTime;

        if (InputManager::IsKeyDown(GLFW_KEY_W)) {
            m_Center.y += cameraSpeed;
        }
        if (InputManager::IsKeyDown(GLFW_KEY_S)) {
            m_Center.y -= cameraSpeed;
        }
        if (InputManager::IsKeyDown(GLFW_KEY_A)) {
            m_Center.x -= cameraSpeed;
        }
        if (InputManager::IsKeyDown(GLFW_KEY_D)) {
            m_Center.x += cameraSpeed;
        }

        UpdateViewMatrix();
    }
}

/**
 * @brief Adjusts the camera's zoom level based on key input.
 *
 * Increases or decreases the zoom level using Q and E keys, respectively.
 * The zoom level is clamped within predefined bounds.
 *
 * @param zoomSpeed Speed at which the zoom level changes.
 */

void Camera::HandleZoom(float zoomSpeed) {
    if (InputManager::IsKeyDown(GLFW_KEY_Q)) {
        SetScale(m_Scale + zoomSpeed);
    }
    if (InputManager::IsKeyDown(GLFW_KEY_E)) {
        SetScale(m_Scale - zoomSpeed);
    }

    // Clamp the scale to reasonable bounds
    m_Scale = std::clamp(m_Scale, 0.1f, 5.0f);
}

/**
 * @brief Adjusts the camera's rotation angle based on key input.
 *
 * Rotates the camera clockwise or counterclockwise using R and T keys.
 *
 * @param rotateSpeed Speed at which the rotation angle changes.
 */

void Camera::HandleRotation(float rotateSpeed) {
    if (InputManager::IsKeyDown(GLFW_KEY_R)) {
        SetAngle(m_Angle + rotateSpeed);
    }
    if (InputManager::IsKeyDown(GLFW_KEY_T)) {
        SetAngle(m_Angle - rotateSpeed);
    }
}

/**
 * @brief Resets the camera to its default settings.
 *
 * Resets the camera's position, scale, rotation, and other settings
 * to their default values when the Z key is pressed.
 */
void Camera::HandleReset() {
    if (InputManager::IsKeyDown(GLFW_KEY_Z)) {
        Reset();
    }
}

/**
 * @brief Triggers a screen shake effect.
 *
 * Applies a shaking effect to the camera when player shoot / suction
 */
void Camera::HandleShake(bool isSuctionActive, bool isShooting) {

    if (!isSuctionActive && !isShooting) {
        // Completely stop all shaking immediately
        m_SuctionShakeIntensity = 0.0f;
        m_ShootingShakeIntensity = 0.0f;
        m_IsSuctionShaking = false;
        m_IsShootingShaking = false;
    }

    if (isShooting) {
        // Apply a small instant shake for shooting without interfering with suction
        m_ShootingShakeIntensity = m_MaxShootingIntensity;
        m_IsShootingShaking = true;
        m_ShakeElapsedTime = 0.0f;
    }
    else
    {
        m_IsShootingShaking = false;
    }

    if (isSuctionActive) {
        // Gradually increase suction shake over time
        if (!m_IsSuctionShaking) {
            m_ShakeElapsedTime = 0.0f;
            m_IsSuctionShaking = true;
        }

        float increaseRate = 3.0f * (float)InputManager::GetDeltaTime(); // Smooth buildup
        m_SuctionShakeIntensity = std::min(m_SuctionShakeIntensity + increaseRate, m_MaxSuctionIntensity);
    }
    else {
        // Stop suction shake naturally
        m_IsSuctionShaking = false;
    }

    UpdateShake((float)InputManager::GetDeltaTime());
}


/**
 * @brief Checks if the player object is available.
 *
 * Determines whether the player object exists and has a valid
 * TransformComponent.
 *
 * @return `true` if the player object and its TransformComponent are available; `false` otherwise.
 */
bool Camera::IsPlayerAvailable() const {
    GameObject* playerObject = GameObjectFactory::GetInstance().GetPlayerObject();
    return playerObject != nullptr &&
        playerObject->GetComponent<TransformComponent>(TypeOfComponent::TRANSFORM) != nullptr;
}
