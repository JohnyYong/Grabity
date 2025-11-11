/*!****************************************************************
\file: AIStateBase.h
\author: Jeremy Lim Ting Jie, jeremytingjie.lim, 2301370
\brief: Abstract base class for defining AI states in the state 
        machine. Each AI state (e.g., Idle, Chase, Flee) inherits 
        from this class and implements its behavior in the 
        Enter, Update, and Exit methods. This allows for modular 
        and extensible state management in AI systems.

Copyright (C) 2024 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents
without the prior written consent of DigiPen Institute of
Technology is prohibited.
*******************************************************************!*/
#pragma once
#include "GameObject.h"
#include "GameObjectFactory.h"

// Forward declare AIStateMachineComponent
class AIStateMachineComponent;

/**
 * @class AIStateBase
 * @brief Abstract base class for AI states in the state machine.
 *
 * The AIStateBase class defines the interface for different states in the AI state machine.
 * Each state should implement the `Enter`, `Update`, and `Exit` methods to handle specific behavior
 * when entering, updating, and exiting the state.
 */
class AIStateBase {
public:
    /**
     * @brief Destructor for AIStateBase.
     *
     * Virtual destructor to ensure proper cleanup of derived classes.
     */
    virtual ~AIStateBase() {}

    /**
     * @brief Method called when the AI enters this state.
     *
     * This method is responsible for any initialization or setup that is required
     * when the AI enters this state.
     *
     * @param aiComponent A pointer to the AIStateMachineComponent managing the AI.
     */
    virtual void Enter(AIStateMachineComponent* aiComponent) = 0;

    /**
     * @brief Method called every frame to update the AI's behavior while in this state.
     *
     * This method should contain the logic for the AI's actions while in this state.
     * It is called during each update cycle of the game.
     *
     * @param aiComponent A pointer to the AIStateMachineComponent managing the AI.
     */
    virtual void Update(AIStateMachineComponent* aiComponent) = 0;

    /**
     * @brief Method called when the AI exits this state.
     *
     * This method is responsible for cleaning up or performing any final actions before
     * the AI exits the state.
     *
     * @param aiComponent A pointer to the AIStateMachineComponent managing the AI.
     */
    virtual void Exit(AIStateMachineComponent* aiComponent) = 0;
};