/*!****************************************************************
\file:      ContentBrowser.h
\author:    Lee Yu Jie Brandon , l.yujiebrandon , 2301232
\brief:     Header for the game editor's content browser system
\details:   Defines the content browser interface providing:
            - Asset management and organization
            - File system navigation and previews
            - Drag-and-drop asset manipulation
            - Multi-format asset support (textures/prefabs/audio)
            - Editor viewport integration
            - Real-time asset manipulation tools
            - Error handling and user notifications

Copyright (C) 2024 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents
without the prior written consent of DigiPen Institute of
Technology is prohibited.
*******************************************************************!*/

#pragma once
#include <pch.h>

#ifdef _IMGUI
#include <iostream>
#endif // _IMGUI
#include <filesystem>
#include "Texture.h"
#include "SpriteAnimation.h"
#include "GameObject.h"
#include "pch.h"

/**
 * @class ContentBrowserPanel
 * @brief Game editor's asset management interface
 * @details Provides:
 *          - Singleton-based asset management system
 *          - File system navigation and previews
 *          - Asset drag-and-drop operations
 *          - Multi-format asset support
 *          - Level editor integration
 *          - Error handling and notifications
 */
class ContentBrowserPanel {
public:
    /** @brief Initialize content browser with default state */
    ContentBrowserPanel();

    /**
     * @brief Get singleton instance
     * @return Reference to content browser instance
     */
    static ContentBrowserPanel& GetInstance();

    /**
     * @brief Render ImGui content browser interface
     * @details Manages:
     *          - Asset browser window with navigation
     *          - Grid-based asset display
     *          - Asset previews and thumbnails
     *          - Drag-drop interactions
     *          - Level editor viewport
     */
    void OnImGuiRender();

    /**
     * @brief Convert screen to world coordinates
     * @param normalizedX X coordinate (0-1)
     * @param normalizedY Y coordinate (0-1)
     * @return World space position
     */
    Vector2 ImVec2toVector2(float normalizedX, float normalizedY);

    /**
     * @brief Check mouse-object intersection
     * @param mouseWorldPos Mouse position in world space
     * @param gameObj Target GameObject
     * @return True if mouse intersects object bounds
     */
    static bool IsMouseOverGameObject(const Vector2& mouseWorldPos, GameObject* gameObj);

    /**
     * @brief Parse numeric ID from filename
     * @param input Filename format: "NUMBER_name"
     * @return Extracted number or -1 if invalid
     * @details Handles edge cases:
     *          - Empty string
     *          - Missing underscore
     *          - Non-numeric prefix
     *          - Parse failures
     */
    static int extractIndex(const std::string& input);

    /**
     * @brief Display UI notification
     * @param message Notification text
     * @param isError True for error styling
     */
    void ShowNotification(const char* message, bool isError = false) {
        if (isError) {
            showErrorPopup = true;
        }
        else {
            showSuccessPopup = true;
        }
        popupMessage = message;
        popupTimer = POPUP_DURATION;
    }

    bool showErrorPopup = false;          ///< Error popup visibility flag
    bool showSuccessPopup = false;        ///< Success popup visibility flag
    std::string popupMessage;             ///< Current notification message
    float popupTimer = 0.0f;              ///< Notification display timer
    static constexpr float POPUP_DURATION = 5.0f;  ///< Default display duration

private:
    static std::unique_ptr<ContentBrowserPanel> instance;  ///< Singleton instance
    std::filesystem::path m_CurrentDir;                    ///< Active directory path
};