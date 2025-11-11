/*!****************************************************************
\file:      UISystem.h
\author:    Ridhwan Mohamed Afandi, mohamedridhwan.b, 2301367
\brief:     UI System for updating  UI elements in the game world.
\details:   Implements the UpdateUI function to update UI elements
			such as timer, health bar, and FPS counter.

Copyright (C) 2024 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents
without the prior written consent of DigiPen Institute of
Technology is prohibited.
*******************************************************************!*/

#pragma once
#include "ButtonComponent.h"
#include "UIComponent.h"
#include "EventSystem.h"

namespace variables {
    extern bool isRunning;
    extern bool isRunningBack;
    extern bool isInteractable;

    extern bool isPopupAnimating;
    extern bool isMovingUp;
    extern float popupTimer;

    extern bool isPopupAnimatingSide;
    extern bool isMovingSide;
    extern float popupTimerSide;

    extern bool volumeMenuOn;
    extern bool runFadeIntoCutscene;
}


/*!****************************************************************
\func  UpdateUI
\brief Updates the UI elements in the game world.
*******************************************************************!*/
void UpdateUI();

/*!****************************************************************
\func  triggerUpDown
\brief Starts the popup animation when triggered.
\details This function is responsible for initiating the popup animation
         by setting the relevant state variables (`isPopupAnimating` and
         `isMovingUp`) and resetting the timer for the animation.
*******************************************************************!*/
void TriggerUpDown();

/*!****************************************************************
\func  textChange
\brief Updates the text on specific UI elements based on the enemy type.
\param num The current value to be displayed.
\param tagname The tag name for identifying the target UI element.
\param total The total value to be displayed alongside the current number.
\details This function updates the text on specific UI elements for
         different enemy types (light, heavy, and bomb enemies).
         It uses the `tagname` to identify which enemy's text component
         to update with the given values (`num` and `total`).
*******************************************************************!*/
void SetGreen(EnemyType enemytype);

/*!****************************************************************
\func  SetGreen
\brief Sets the color of a specific enemy's sprite to green.
\param enemytype The type of enemy to be updated.
\details This function changes the color of a specified enemy type (light, heavy, or bomb)
         to green (indicating a completed or successful state).
         It uses the `enemytype` to determine which enemy to update and sets the color
         of its sprite accordingly.
*******************************************************************!*/
void TextChange(int num, std::string tagname, int total);

/*!****************************************************************
\func  UpDownPopup
\brief Moves a popup UI element up and down with a pause in between.
\param selectedGO The GameObject representing the popup.
\param maxY The maximum Y position (top position).
\param minY The minimum Y position (bottom position).
\param pauseTime The time to pause at the maxY position before moving down.
\details This function moves a popup UI element up towards `minY` and then,
         after a pause, moves it back down towards `maxY`. The animation
         logic ensures that the popup moves smoothly and stays at the top
         for a defined period (`pauseTime`) before moving back down.
*******************************************************************!*/
void UpDownPopup(GameObject* selectedGO, float maxY, float minY, float pauseTime);


/*!****************************************************************
\func  LeftRightPopup
\brief Moves a popup UI element left and right.
\param selectedGO The GameObject representing the popup.
\param maxX The maximum X position (right position).
\param minX The minimum X position (left position).
\details This function moves a popup UI element left and right between the
         `maxX` (right) and `minX` (left) positions based on user input.
         The popup will move outwards and inwards based on the state of
         `isMovingOut` and the `Tab` key press. It also adjusts the positions
         of any child UI elements to stay synchronized with the parent.
*******************************************************************!*/
void LeftRightPopup(GameObject* selectedGO, float maxX, float minX);

