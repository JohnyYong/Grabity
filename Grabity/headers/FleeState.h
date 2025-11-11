/*!****************************************************************
\file: FleeState.h
\author: Jeremy Lim Ting Jie, jeremytingjie.lim, 2301370
\brief: AI Script file for Fleeing state behavior. Implements the behavior for an AI entity
        to move away from the player when in the "flee" state.

Copyright (C) 2024 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents
without the prior written consent of DigiPen Institute of
Technology is prohibited.
*******************************************************************!*/

/**
 * @class FleeState
 * @brief A state in the AI state machine that makes the AI flee from the player.
 *
 * The FleeState is a concrete implementation of the `AIStateBase` class,
 * where the AI moves away from the player object. The state includes entering,
 * updating, and exiting behaviors for fleeing from the player.
 */
class FleeState : public AIStateBase {
public:
    /**
     * @brief Called when entering the flee state.
     *
     * This method is responsible for logging or initializing any variables
     * needed when transitioning to the flee state, including resetting the flee timer.
     *
     * @param aiComponent A pointer to the AIStateMachineComponent managing the AI.
     */
    void Enter(AIStateMachineComponent* aiComponent) override {
        if (!aiComponent) return;

        // Initialize flee timer
        fleeTimer = 0.0f;

        // Log or trigger flee behavior
#ifdef _LOGGING
		ImGuiConsole::Cout("Entering Flee State");
#endif // _LOGGING
    }

    /**
     * @brief Updates the flee state, moving the AI away from the player.
     *
     * This method continuously updates the AI's position to move away from the player.
     * It checks the AI's flee timer and updates the position accordingly.
     *
     * @param aiComponent A pointer to the AIStateMachineComponent managing the AI.
     */
    void Update(AIStateMachineComponent* aiComponent) override {
		Engine& engine = Engine::GetInstance();
        if (!aiComponent) return;

        GameObject* player = GameObjectFactory::GetInstance().GetPlayerObject();
        if (!player) return;

        // Move away from the player
        MoveAwayFromPlayer(aiComponent, player);

        // Update flee timer
		// fleeTimer += static_cast<float>(InputManager::deltaTime);
        fleeTimer += static_cast<float>(engine.fixedDT * (long long)engine.currentNumberOfSteps);
    }

    /**
     * @brief Called when exiting the flee state.
     *
     * This method is responsible for logging or performing any cleanup
     * needed when transitioning out of the flee state.
     *
     * @param aiComponent A pointer to the AIStateMachineComponent managing the AI.
     */
    void Exit(AIStateMachineComponent* aiComponent) override {
        if (!aiComponent) return;

        // Log exit
#ifdef _LOGGING
		ImGuiConsole::Cout("Exiting Flee State.");
#endif // _LOGGING
    }

private:
    /**
     * @brief Moves the AI away from the player.
     *
     * This method calculates the direction from the player to the AI, then moves the AI
     * away from the player by updating the AI's position.
     *
     * @param aiComponent A pointer to the AIStateMachineComponent managing the AI.
     * @param playerObj A pointer to the player GameObject.
     */
    void MoveAwayFromPlayer(AIStateMachineComponent* aiComponent, GameObject* playerObj) {
        if (!aiComponent || !playerObj) return;

        const auto& playerPosition = playerObj->GetComponent<TransformComponent>(TypeOfComponent::TRANSFORM)->GetLocalPosition();
        GameObject* aiObject = aiComponent->GetParentGameObject();
        if (!aiObject) return;

        TransformComponent* aiTransform = aiObject->GetComponent<TransformComponent>(TypeOfComponent::TRANSFORM);
        if (!aiTransform) return;

        // Calculate the direction vector from the player to the AI.
        Vector2 direction = aiTransform->GetLocalPosition() - playerPosition;

        // Normalize the direction and update the AI's position to move away.
        if (direction.Length() > 0) {
            direction = direction.Normalize();
            // Vector2 newPosition = aiTransform->GetLocalPosition() + direction * aiComponent->GetMoveSpeed() * static_cast<float>(InputManager::deltaTime);
			Vector2 newPosition = aiTransform->GetLocalPosition() + direction * aiComponent->GetMoveSpeed() * static_cast<float>(Engine::GetInstance().fixedDT * (long long)Engine::GetInstance().currentNumberOfSteps);
            aiTransform->SetLocalPosition(newPosition);
        }
    }

    float fleeTimer = 0.0f; ///< Tracks how long the AI has been fleeing
};