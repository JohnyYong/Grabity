/*!****************************************************************
\file: PlayerSceneControls.h
\author: Teng Shi Heng, shiheng.teng, 2301269
\brief:
    The PlayerSceneControls class provides functionality to
    manage and manipulate GameObject instances within a
    game editor scene. It supports modes such as translation,
    rotation, and scaling, and integrates with a graphical
    user interface (e.g., ImGui) for interactive control.

Copyright (C) 2024 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents
without the prior written consent of DigiPen Institute of
Technology is prohibited.
*******************************************************************!*/
#pragma once

#include <memory>
#include "GameObject.h"
#include "Vector2.h"

/*!****************************************************************
\enum GameObjectEditorMode
\brief
    Represents the different editing modes available for
    manipulating GameObject instances in the scene editor.
*******************************************************************!*/
enum GameObjectEditorMode {
    None,       // No editing mode selected.
    Translate,  // Translation mode for moving objects.
    Rotate,     // Rotation mode for rotating objects.
    Scale       // Scaling mode for resizing objects.
};

/*!****************************************************************
\struct ControlsUI
\brief
    Stores the UI control data used for interacting with
    GameObject instances in the editor.
*******************************************************************!*/
struct ControlsUI
{
    Vector2 pos;           // The position of the UI control.
    Vector2 colliderBox;   // The bounding box of the control for collision detection.
};

/*!****************************************************************
\class PlayerSceneControls
\brief
    Manages scene editor functionality for selecting and
    manipulating GameObject instances. Supports various editor
    modes and integrates with mouse input for interaction.
*******************************************************************!*/
class PlayerSceneControls
{
public:
    /*!****************************************************************
    \brief
        Retrieves the singleton instance of PlayerSceneControls.
    \return
        A reference to the singleton instance.
    *******************************************************************!*/
    static PlayerSceneControls& GetInstance();

    /*!****************************************************************
    \brief
        Constructor for PlayerSceneControls.
    *******************************************************************!*/
    PlayerSceneControls();

    /*!****************************************************************
    \brief
        Destructor for PlayerSceneControls.
    *******************************************************************!*/
    ~PlayerSceneControls();

#ifdef _IMGUI
    /*!****************************************************************
    \brief
        Updates the state of the editor, handling input and object
        interactions.
    *******************************************************************!*/
    void Update();

    /*!****************************************************************
    \brief
        Converts screen-space mouse position to world-space within
        the level editor.
    \param withinRange
        Outputs whether the mouse is within the valid range.
    \return
        The mouse position in world coordinates.
    *******************************************************************!*/
    Vector2 GetMousePositionInLevelEditor(bool& withinRange);

    /*!****************************************************************
    \brief
        Determines if the mouse is hovering over a GameObject.
    \param mouseWorldPos
        The mouse position in world coordinates.
    \param gameObj
        Pointer to the GameObject to check.
    \param distance
        Outputs the distance between the mouse and the object.
    \return
        True if the mouse is over the object; otherwise, false.
    *******************************************************************!*/
    bool IsMouseOverGameObject(const Vector2& mouseWorldPos, GameObject* gameObj, float& distance);

    /*!****************************************************************
    \brief
        Converts screen-space coordinates to world-space coordinates.
    \param mousePos
        The mouse position in screen coordinates.
    \param camera
        The camera used for the conversion.
    \param screenWidth
        The width of the screen.
    \param screenHeight
        The height of the screen.
    \return
        The mouse position in world coordinates.
    *******************************************************************!*/
    Vector2 ScreenToWorld(const Vector2& mousePos, const Camera& camera, float screenWidth, float screenHeight);
#endif // _IMGUI

    /*!****************************************************************
    \brief
        Sets the selected GameObject in the editor.
    \param selected
        Pointer to the GameObject to be selected.
    *******************************************************************!*/
    void SetSelectedGameObject(GameObject* selected);

    /*!****************************************************************
    \brief
        Gets the currently selected GameObject.
    \return
        Pointer to the selected GameObject.
    *******************************************************************!*/
    GameObject* GetSelectedGameObject();

    /*!****************************************************************
    \brief
        Retrieves the current editor mode.
    \return
        A constant reference to the current `GameObjectEditorMode`.
    *******************************************************************!*/
    GameObjectEditorMode const& GetSelectedObjectMode();

    /*!****************************************************************
    \brief
        Retrieves the UI controls for the translation mode.
    \return
        A constant reference to an array of `ControlsUI`.
    *******************************************************************!*/
    std::array<ControlsUI, 2> const& GetTranslateModeUI();

    /*!****************************************************************
    \brief
        Retrieves the UI controls for the scaling mode.
    \return
        A constant reference to an array of `ControlsUI`.
    *******************************************************************!*/
    std::array<ControlsUI, 2> const& GetScaleModeUI();

    /*!****************************************************************
    \brief
        Retrieves the UI controls for the rotation mode.
    \return
        A constant reference to a `ControlsUI` object.
    *******************************************************************!*/
    ControlsUI const& GetRotateModeUI();

    /*!****************************************************************
    \brief
        Retrieves the mouse position in the game world.
    \return
        The mouse position in world coordinates.
    *******************************************************************!*/
    Vector2 GetMousePositionInGameWorld();

private:
    static std::unique_ptr<PlayerSceneControls> instance; // Singleton instance.

    GameObject* selectedObj; // Pointer to the currently selected object.

    bool isFirstClickObject = false; // Tracks the state of the first click.
    bool firstAttempt = true;        // Tracks if this is the first interaction attempt.
    bool isDragging;                 // Tracks if an object is being dragged.
    Vector2 lastGameObjOffset{ 0,0 }; // Offset from the last object position.
    GameObjectEditorMode mode;       // Current editing mode.

    std::array<ControlsUI, 2> translate; // UI controls for translation.
    std::array<ControlsUI, 2> scale;     // UI controls for scaling.
    ControlsUI rotate;                   // UI control for rotation.

    Vector2 mousePositionInGameWorld;    // Mouse position in world coordinates.
};
