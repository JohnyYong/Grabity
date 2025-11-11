/*!****************************************************************
\file: IdleState.h
\author: Jeremy Lim Ting Jie, jeremytingjie.lim, 2301370
\brief: AI Script file for Idle state behavior. This state
        represents a passive or non-active state for AI entities.

Copyright (C) 2024 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents
without the prior written consent of DigiPen Institute of
Technology is prohibited.
*******************************************************************/
#pragma once
#include "AIStateBase.h"

#ifdef _IMGUI
#include <iostream>
#endif // _IMGUI

/**
 * @class IdleState
 * @brief A state in the AI state machine where the AI remains idle.
 *
 * The IdleState is a concrete implementation of the `AIStateBase` class.
 * In this state, the AI does not perform any actions and remains idle.
 * The state includes entering, updating, and exiting behaviors for idling.
 */
class IdleState : public AIStateBase {
public:

    /**
     * @brief Called when entering the idle state.
     *
     * This method is responsible for logging the transition to the idle state.
     * It may also trigger any initialization or setup needed when the AI
     * enters the idle state.
     *
     * @param aiComponent A pointer to the AIStateMachineComponent managing the AI.
     */
    void Enter(AIStateMachineComponent* aiComponent) override {
        if (aiComponent) {
#ifdef _LOGGING
			ImGuiConsole::Cout("Entering Idle State for GameObject: %s", 
                aiComponent->GetParentGameObject()->GetName().c_str());
#endif // _LOGGING
        }
    }

    /**
     * @brief Updates the idle state, performing no actions.
     *
     * This method is responsible for any ongoing behavior while the AI is idle.
     * In this state, no action is taken, but additional monitoring or logging
     * could be added.
     *
     * @param aiComponent A pointer to the AIStateMachineComponent managing the AI.
     */
    void Update(AIStateMachineComponent* aiComponent) override {
        if (aiComponent) {
            // Optionally add monitoring or log GameObject status
#ifdef _LOGGING
			ImGuiConsole::Cout("GameObject is idling.");
#endif // _LOGGING
        }
    }

    /**
     * @brief Called when exiting the idle state.
     *
     * This method is responsible for logging the transition out of the idle state.
     * It may also perform any necessary cleanup or state-specific logic.
     *
     * @param aiComponent A pointer to the AIStateMachineComponent managing the AI.
     */
    void Exit(AIStateMachineComponent* aiComponent) override {
        if (aiComponent) {
#ifdef _LOGGING
			ImGuiConsole::Cout("Exiting Idle State for GameObject: %s", 
                aiComponent->GetParentGameObject()->GetName().c_str());
#endif // _LOGGING
        }
    }
};
