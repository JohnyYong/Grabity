/*!****************************************************************
\file: PlayerCamera.cpp
\author: Jeremy Lim Ting Jie, jeremytingjie.lim, 2301370
\brief: This file implements the PlayerCamera class, which is a specialized 
        camera designed to follow the player in a 2D game environment. 
        It inherits from the base Camera class and manages the camera's 
        position and orientation based on the player's current position.

Copyright (C) 2024 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents
without the prior written consent of DigiPen Institute of
Technology is prohibited.
*******************************************************************!*/
#pragma once
#include "PlayerCamera.h"
#include "GameObjectFactory.h"
#include "PlayerSceneControls.h"
/**
 * @brief Constructs a PlayerCamera object with specified boundaries.
 *
 * Initializes the camera with orthographic boundaries and binds it to track the player.
 * The player is identified using the GameObjectFactory.
 *
 * @param left Left boundary of the orthographic projection.
 * @param right Right boundary of the orthographic projection.
 * @param bottom Bottom boundary of the orthographic projection.
 * @param top Top boundary of the orthographic projection.
 */
PlayerCamera::PlayerCamera(float left, float right, float bottom, float top)
    : Camera(left, right, bottom, top),
    m_PlayerID(-1),
    m_Offset(0.3f),    
    m_Damping(0.04f)  
{
#ifdef _LOGGING
    std::cout << "PlayerCamera created." << std::endl;
#endif // _LOGGING
}

/**
 * @brief Updates the camera's position and view matrix to follow the player.
 *
 * Retrieves the player's position from the GameObjectFactory and updates the camera's
 * center to match the player's position. If the player object is unavailable, the
 * camera center remains unchanged. The view matrix is updated accordingly.
 */
void PlayerCamera::Update() {
    GameObject* playerObject = GameObjectFactory::GetInstance().GetPlayerObject();
    if (playerObject != nullptr)
    {
        // Get player position
        const auto& playerPosition = playerObject
            ->GetComponent<TransformComponent>(TypeOfComponent::TRANSFORM)
            ->GetPosition();

        // Get cursor position in screen space
        double cursorX, cursorY;
        InputManager::GetMousePosition(cursorX, cursorY);

        Vector2 cursorPosition;

        if (Engine::GetInstance().cameraManager.GetCurrentMode() == CameraManager::CameraMode::PlayerCamera)
        {
            cursorPosition = PlayerSceneControls::GetInstance().GetMousePositionInGameWorld();
        }
        else
        {
            Vector2 viewportSize = Vector2(1280.0f, 720.0f);
            Vector2 cursorScreenPos = Vector2(static_cast<float>(cursorX), static_cast<float>(cursorY));

            // Calculate cursor position in world space
            cursorPosition.x = ((cursorScreenPos.x / viewportSize.x) * 2.0f - 1.0f) * GetViewingRange().x + GetCenter().x;
            cursorPosition.y = (-(cursorScreenPos.y / viewportSize.y) * 2.0f + 1.0f) * GetViewingRange().y + GetCenter().y;
        }

        // Calculate direction vector from player to cursor
        Vector2 direction = cursorPosition - playerPosition;

        // Normalize the direction if it's not zero
        float length = direction.Length();
        if (length > 0.001f) {
            direction = direction / length;
        }

        // Calculate the look-ahead distance
        float lookAheadDistance = std::min(length * 0.5f, 200.0f);

        // Calculate target position
        // Using m_Offset to control how far the camera should be between player and cursor
        Vector2 targetPosition = playerPosition + direction * lookAheadDistance * m_Offset;

        // Check if player is within dead zone
        Vector2 currentCenter = GetCenter();
        Vector2 playerToCenter = playerPosition - currentCenter;

        // Calculate dead zone dimensions as a percentage of the viewing range
        float deadZoneWidth = GetViewingRange().x * m_DeadZoneSize;
        float deadZoneHeight = GetViewingRange().y * m_DeadZoneSize;

        bool withinDeadZone =
            fabs(playerToCenter.x) < deadZoneWidth * 0.5f &&
            fabs(playerToCenter.y) < deadZoneHeight * 0.5f;

        // Only update camera position if player is outside dead zone
        Vector2 newCenter;
        if (withinDeadZone) {
            // Player is within dead zone, keep current camera position
            newCenter = currentCenter;
        }
        else {
            // Player is outside dead zone, smoothly move camera towards target
            newCenter = Lerp(currentCenter, targetPosition, m_Damping);
        }

        // Perform border checks
        auto TopBorder = GameObjectFactory::GetInstance().FindGameObjectsByTag("TopBorder");
        if (!TopBorder.empty())
        {
            TransformComponent* position = TopBorder[0]->GetComponent<TransformComponent>(TypeOfComponent::TRANSFORM);
            RectColliderComponent* collider = TopBorder[0]->GetComponent<RectColliderComponent>(TypeOfComponent::RECTCOLLIDER);
            std::vector<std::pair<Vector2, Vector2>>& colliderTop = collider->GetColliderData();
            if (newCenter.y + GetViewingRange().y >= position->GetPosition().y + colliderTop[0].second.y - colliderTop[0].first.y * 0.5f)
            {
                newCenter.y = position->GetPosition().y + colliderTop[0].second.y - colliderTop[0].first.y * 0.5f - GetViewingRange().y;
            }
        }

        auto LowerBorder = GameObjectFactory::GetInstance().FindGameObjectsByTag("LowerBorder");
        if (!LowerBorder.empty())
        {
            TransformComponent* position = LowerBorder[0]->GetComponent<TransformComponent>(TypeOfComponent::TRANSFORM);
            RectColliderComponent* collider = LowerBorder[0]->GetComponent<RectColliderComponent>(TypeOfComponent::RECTCOLLIDER);
            std::vector<std::pair<Vector2, Vector2>>& colliderLower = collider->GetColliderData();
            if (newCenter.y - GetViewingRange().y <= position->GetPosition().y + colliderLower[0].second.y + colliderLower[0].first.y * 0.5f)
            {
                newCenter.y = position->GetPosition().y + colliderLower[0].second.y + colliderLower[0].first.y * 0.5f + GetViewingRange().y;
            }
        }

        auto LeftBorder = GameObjectFactory::GetInstance().FindGameObjectsByTag("LeftBorder");
        if (!LeftBorder.empty())
        {
            TransformComponent* position = LeftBorder[0]->GetComponent<TransformComponent>(TypeOfComponent::TRANSFORM);
            RectColliderComponent* collider = LeftBorder[0]->GetComponent<RectColliderComponent>(TypeOfComponent::RECTCOLLIDER);
            std::vector<std::pair<Vector2, Vector2>>& colliderLeft = collider->GetColliderData();
            if (newCenter.x - GetViewingRange().x <= position->GetPosition().x + colliderLeft[0].second.x + colliderLeft[0].first.x * 0.5f)
            {
                newCenter.x = position->GetPosition().x + colliderLeft[0].second.x + colliderLeft[0].first.x * 0.5f + GetViewingRange().x;
            }
        }

        auto RightBorder = GameObjectFactory::GetInstance().FindGameObjectsByTag("RightBorder");
        if (!RightBorder.empty())
        {
            TransformComponent* position = RightBorder[0]->GetComponent<TransformComponent>(TypeOfComponent::TRANSFORM);
            RectColliderComponent* collider = RightBorder[0]->GetComponent<RectColliderComponent>(TypeOfComponent::RECTCOLLIDER);
            std::vector<std::pair<Vector2, Vector2>>& colliderRight = collider->GetColliderData();
            if (newCenter.x + GetViewingRange().x >= position->GetPosition().x + colliderRight[0].second.x - colliderRight[0].first.x * 0.5f)
            {
                newCenter.x = position->GetPosition().x + colliderRight[0].second.x - colliderRight[0].first.x * 0.5f - GetViewingRange().x;
            }
        }

        // Update the camera's center position
        SetCenter(newCenter);
    }

    // Call the inherited method to update the view matrix based on the new center
    UpdateViewMatrix();
}

/**
 * @brief Linearly interpolates between two vectors.
 *
 * This function performs a linear interpolation between the start and end vectors
 * based on the interpolation factor t. The interpolation factor t should be in the
 * range [0, 1], where 0 returns the start vector and 1 returns the end vector.
 *
 * @param start The starting vector.
 * @param end The ending vector.
 * @param t The interpolation factor, typically in the range [0, 1].
 * @return The interpolated vector.
 */
Vector2 PlayerCamera::Lerp(const Vector2& start, const Vector2& end, float t) {
    return start + (end - start) * t;
}

/**
 * @brief Converts screen coordinates to world coordinates.
 *
 * This function converts screen coordinates (in pixels) to world coordinates
 * based on the camera's viewing range and center. The screen coordinates are
 * first converted to normalized device coordinates (NDC) in the range [-1, 1],
 * and then transformed to world coordinates.
 *
 * @param screenX The x-coordinate in screen space (pixels).
 * @param screenY The y-coordinate in screen space (pixels).
 * @return The corresponding world coordinates.
 */
Vector2 PlayerCamera::ScreenToWorldCoordinates(double screenX, double screenY) {
    // Get the viewport size
    Vector2 viewportSize = Vector2(1280.0f, 720.0f);

    // Convert screen coordinates to normalized device coordinates (-1 to 1)
    float ndcX = (float)(screenX / viewportSize.x) * 2.0f - 1.0f;
    float ndcY = -((float)(screenY / viewportSize.y) * 2.0f - 1.0f); // Y is inverted

    // Convert to world coordinates using the camera's viewing range and center
    Vector2 worldPos;
    worldPos.x = ndcX * GetViewingRange().x + GetCenter().x;
    worldPos.y = ndcY * GetViewingRange().y + GetCenter().y;

    return worldPos;
}