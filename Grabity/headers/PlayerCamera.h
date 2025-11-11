/*!****************************************************************
\file: PlayerCamera.h
\author: Jeremy Lim Ting Jie, jeremytingjie.lim, 2301370
\brief: Header file for PlayerCamera class in PlayerCamera.cpp
        PlayerCamera inherits from the camera class and specializes
        it to follow the player.

Copyright (C) 2024 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents
without the prior written consent of DigiPen Institute of
Technology is prohibited.
*******************************************************************!*/
#pragma once
#include "Camera.h"
/**
 * @class PlayerCamera
 * @brief A specialized camera class that follows the player's position in the game world.
 *
 * The PlayerCamera class extends the Camera functionality by binding its position to
 * the player object in the game. It automatically updates its position based on the
 * player's movement, ensuring the player remains centered in the view.
 */
class PlayerCamera : public Camera {
public:
    /**
     * @brief Constructs a PlayerCamera with specified orthographic boundaries.
     *
     * @param left The left boundary of the orthographic projection.
     * @param right The right boundary of the orthographic projection.
     * @param bottom The bottom boundary of the orthographic projection.
     * @param top The top boundary of the orthographic projection.
     */
    PlayerCamera(float left, float right, float bottom, float top);

    /**
     * @brief Updates the camera's position to follow the player.
     *
     * This method retrieves the player's position from the GameObjectFactory and
     * adjusts the camera's center accordingly. If the player object is unavailable,
     * the camera center remains unchanged.
     */
    void Update();

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
    Vector2 Lerp(const Vector2& start, const Vector2& end, float t);

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
    Vector2 ScreenToWorldCoordinates(double screenX, double screenY);
private:
    float m_DeadZoneSize = 0.35f; // Size of the central dead zone
    int m_PlayerID; ///< The ID of the player object that the camera tracks.
    float m_Damping;
    float m_Offset;
};