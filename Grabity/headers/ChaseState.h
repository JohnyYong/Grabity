/*!****************************************************************
\file: ChaseState.h
\author: Jeremy Lim Ting Jie, jeremytingjie.lim, 2301370
\brief: AI Script file for chase state behavior.
        This state handles AI movement towards a specified target.

Copyright (C) 2024 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents
without the prior written consent of DigiPen Institute of
Technology is prohibited.
*******************************************************************/
#pragma once
#include "AIStateBase.h"
#include "AIStateMachineComponent.h"
#include "GameObject.h"
#include "TransformComponent.h"
#include "collision.h"
#ifdef _IMGUI
#include <iostream>
#endif // _IMGUI
#include "GameObjectFactory.h"

/**
 * @class ChaseState
 * @brief A state in the AI state machine that makes the AI chase a target.
 *
 * The ChaseState is a concrete implementation of the `AIStateBase` class,
 * where the AI pursues a target GameObject. The state includes entering,
 * updating, and exiting behaviors as the AI moves towards the target.
 */
class ChaseState : public AIStateBase {
public:
    /**
     * @brief Called when entering the chase state.
     *
     * This method is responsible for logging or performing any setup
     * needed when transitioning to the chase state.
     *
     * @param aiComponent A pointer to the AIStateMachineComponent managing the AI.
     */
    void Enter(AIStateMachineComponent* aiComponent) override {
        if (!aiComponent) return;
        // Log state transition

#ifdef _LOGGING
		ImGuiConsole::Cout("Entering Chase State");
#endif // _LOGGING
    }
     /**
     * @brief Updates the chase state, moving the AI towards its target.
     *
     * This method contains the logic to move the AI towards its current target.
     * The AI continuously updates its position to close the gap with the target.
     *
     * @param aiComponent A pointer to the AIStateMachineComponent managing the AI.
     */
    void Update(AIStateMachineComponent* aiComponent) override {
        if (!aiComponent) return;

        GameObject* target = aiComponent->GetChaseTarget();
        if (!target) {
#ifdef _LOGGING
			ImGuiConsole::Cout("No target set for AI to chase!");
#endif // _LOGGING
            return;
        }

        // Update cooldown timer
        if (isOnCooldown) {
            leapCooldownTimer -= static_cast<float>(Engine().GetInstance().currentNumberOfSteps * Engine().GetInstance().fixedDT);
            if (leapCooldownTimer <= 0.0f) {
                isOnCooldown = false; // Cooldown finished, allow leaping again
            }
        }


        MoveTowardsTarget(aiComponent, target);
    }
    /**
     * @brief Called when exiting the chase state.
     *
     * This method is responsible for logging or performing any cleanup
     * needed when transitioning out of the chase state.
     *
     * @param aiComponent A pointer to the AIStateMachineComponent managing the AI.
     */
    void Exit(AIStateMachineComponent* aiComponent) override {
        if (!aiComponent) return;

#ifdef _LOGGING
		ImGuiConsole::Cout("Exiting Chase State");
#endif // _LOGGING
    }

private:
    //declaration of variables
    Vector2 initialTargetPosition;
    bool isCharging = false;
    float chargeUpTimer = 0.0f;
	const float chargeUpDuration = 1.0f;
    bool blinkState = false;
	float blinkTimer = 0.0f;
    float blinkInterval = 0.06f;
    bool isLeaping = false;
    float leapSpeed = 700.0f;
    bool isOnCooldown = false;
    float leapCooldownTimer = 0.0f;
    const float leapCooldownDuration = 5.0f;

     /**
      * @brief Moves the AI towards the target GameObject.
      *
      * This method calculates the direction from the AI to the target and moves
      * the AI towards it. It also adjusts the AI's position based on its movement speed.
      *
      * @param aiComponent A pointer to the AIStateMachineComponent managing the AI.
      * @param targetObj A pointer to the target GameObject to chase.
      */
    void MoveTowardsTarget(AIStateMachineComponent* aiComponent, GameObject* targetObj) {
        if (!aiComponent || !targetObj) return;

        auto* health = targetObj->GetComponent<HealthComponent>(HEALTH);
        if (!health) return;

        const auto& targetPosition = targetObj->GetComponent<TransformComponent>(TypeOfComponent::TRANSFORM)->GetLocalPosition();

        GameObject* aiObject = aiComponent->GetParentGameObject();
        if (!aiObject) return;

        TransformComponent* aiTransform = aiObject->GetComponent<TransformComponent>(TypeOfComponent::TRANSFORM);
        if (!aiTransform) return;

        SpriteComponent* sprite = aiObject->GetComponent<SpriteComponent>(TypeOfComponent::SPRITE);
        AnimatorComponent* anim = aiObject->GetComponent<AnimatorComponent>(TypeOfComponent::ANIMATOR);
        Vector2 direction = targetPosition - aiTransform->GetLocalPosition();
        float angle = std::atan2(direction.y, direction.x);
        angle = angle * (180.0f / 3.14159f) - 90.0f; // Adjust to match rotation convention
        float distanceToPlayer = direction.Length();

        const float leapRadius = 500.0f;
        const float leapDistance = 1000.0f;

        // BombEnemy speed boost behavior
        const float speedBoostRadius = 500.0f; // Radius within which BombEnemy speeds up
        const float speedBoostMultiplier = 2.5f; // How much faster the BombEnemy moves when in range
        float currentMoveSpeed = aiComponent->GetMoveSpeed();

        if (aiObject->GetTag() == "BombEnemy" && distanceToPlayer <= speedBoostRadius) {
            // Apply speed boost
            currentMoveSpeed *= speedBoostMultiplier;

            // Also adjust sprite animation speed to match movement speed
            if (sprite) {
                sprite->SetAnimationSpeedMultiplier(speedBoostMultiplier);
        }
    }
        else if (aiObject->GetTag() == "BombEnemy") {


            // Reset sprite animation speed
            if (sprite) {
                sprite->SetAnimationSpeedMultiplier(1.0f);
            }
        }


        // If the enemy is close enough and not on cooldown, it can start charging up
        if (distanceToPlayer <= leapRadius && aiObject->GetTag() == "HeavyEnemy" && !isLeaping && !isOnCooldown && !isCharging) {
            initialTargetPosition = targetPosition;
            isCharging = true;
            chargeUpTimer = chargeUpDuration;
            blinkState = false;

            // Change sprite to charging state
            if (sprite) {
                aiTransform->SetLocalRotation(angle);  // Set rotation towards target
                if (anim)
                {
                    anim->animationController->SetParameter("HeldEnemy", true);
                    anim->animationController->SetParameter("ThrowEnemy", false);
                }
            }
        }

        if (isCharging) {
            ChargeUp(aiTransform, sprite, anim);
        }
        else if (isLeaping) {
            JumpLeapTowardsPlayer(aiTransform, initialTargetPosition, leapDistance, sprite, anim);
        }
        else {
            if (direction.Length() > 0) {
                direction = direction.Normalize();
                // Use the potentially boosted move speed
                Vector2 newPosition = aiTransform->GetLocalPosition() + direction * currentMoveSpeed * static_cast<float>(Engine().GetInstance().currentNumberOfSteps * Engine().GetInstance().fixedDT);
                aiTransform->SetLocalPosition(newPosition);
                aiComponent->walkSFXTimer += (float)Engine().GetInstance().fixedDT;

                if (aiComponent->walkSFXTimer >= aiComponent->walkSFXCooldown)
                {
                    int walkingSFX = -1;
                    float sfxControl = 0.05f;

                    if (aiObject->GetName() == "BabyEnemy" || aiObject->GetName() == "Light_Enemy")
                    {
                        walkingSFX = 41 + (std::rand() % 8); //41 - 48
                        sfxControl = 0.02f;
                    }
                    else if (aiObject->GetName() == "Heavy_Enemy")
                    {
                        walkingSFX = 49 + (std::rand() % 7); //49 - 55
                        sfxControl = 0.07f;
                    }
                    else if (aiObject->GetName() == "Bomb_Enemy")
                    {
                        walkingSFX = 56 + (std::rand() % 6); //56 - 61
                        sfxControl = 0.1f;
                    }

                    if (walkingSFX != -1)
                    {
                        AudioManager::GetInstance().PlayAudio(walkingSFX);
                        AudioManager::GetInstance().SetChannelVolume(walkingSFX, sfxControl);
                        aiComponent->walkSFXTimer = 0.0f; // Reset timer
                    }
                }
            }
        }


    }

    /**
     * @brief Initiates and manages the charge-up phase for a heavy enemy, including blink effects
     *        and transition into a leap state when the charge is complete.

     * This function handles the logic for charging up the heavy enemy's power, managing a charge-up timer,
     * controlling the blink effect (a toggle between black and white sprites), and transitioning into a leap
     * when the charge is completed.
     *
     * @param aiTransform The transform component of the AI character. Currently unused, but passed for potential future logic.
     * @param sprite The sprite component of the AI character, used to change the sprite color based on blink state.
     * @param anim The animator component of the AI character, used to control the animations during the leap phase.
     */
     /**
      * @brief Initiates and manages the charge-up phase for a heavy enemy, including blink effects
      *        and transition into a leap state when the charge is complete.
      *
      * This function handles the logic for charging up the heavy enemy's power, managing a charge-up timer,
      * controlling the blink effect (a toggle between black and white sprites), and transitioning into a leap
      * when the charge is completed.
      *
      * @param aiTransform The transform component of the AI character. Currently unused, but passed for potential future logic.
      * @param sprite The sprite component of the AI character, used to change the sprite color based on blink state.
      * @param anim The animator component of the AI character, used to control the animations during the leap phase.
      */
    void ChargeUp(TransformComponent* aiTransform, SpriteComponent* sprite, AnimatorComponent* anim) {
        (void)aiTransform; // Avoid unused parameter warning

        // Get reference to engine
        Engine& engine = Engine::GetInstance();
        float fixedDeltaTime = static_cast<float>(engine.currentNumberOfSteps * engine.fixedDT);

        // Decrease charge up timer
        chargeUpTimer -= fixedDeltaTime;

        // Update blink timer
        blinkTimer += fixedDeltaTime;

        // Ensure blinkInterval is dynamically adjusted
        float dynamicBlinkInterval = std::max(0.1f, chargeUpTimer / 10.0f);

        // Toggle the blink state if the blink timer exceeds the blink interval
        if (blinkTimer >= dynamicBlinkInterval) {
            blinkState = !blinkState;
            blinkTimer -= dynamicBlinkInterval;
        }

        // Set the sprite color based on the blink state
        if (sprite) {
            sprite->SetRGB(blinkState ? Vector4(0.0f, 0.0f, 0.0f, 1.f) : Vector4(1.0f, 1.0f, 1.0f, 1.f)); // Toggle black/white
        }

        // If charge-up is complete
        if (chargeUpTimer <= 0.0f) {
            // Reset sprite color to original
            if (sprite) {
                sprite->SetRGB(Vector4(1.0f, 1.0f, 1.0f, 1.f));
            }

            isCharging = false;
            isLeaping = true;
            leapSpeed = 700.0f; // Reset leap speed at the start of the leap

            // Change sprite to leaping state
            if (sprite && anim) {
                anim->animationController->SetParameter("ThrowEnemy", true);
                anim->animationController->SetParameter("HeldEnemy", false);
            }
        }
    }



    /**
     * @brief Makes the AI perform a jump leap towards the initial target position.
     *
     * This method moves the AI towards the initial target position with a leap.
     * The leap is performed incrementally over time to create a smooth movement.
     *
     * @param aiTransform A pointer to the AI's TransformComponent.
     * @param targetPosition The initial target position of the player.
     * @param leapDistance The distance of the leap.
     * @param sprite A pointer to the AI's SpriteComponent to change its state.
     */
    void JumpLeapTowardsPlayer(TransformComponent* aiTransform, const Vector2& targetPosition, float leapDistance, SpriteComponent* sprite, AnimatorComponent* anim) {
        // Calculate the direction vector from the AI to the target position
        Vector2 direction = targetPosition - aiTransform->GetLocalPosition();
        float distanceToTarget = direction.Length();

        // Check if there is any distance to cover
        if (distanceToTarget > 0) {
            // Normalize the direction vector to get the unit direction
            direction = direction.Normalize();
            // Calculate the new position by moving the AI in the direction of the target
            Vector2 newPosition = aiTransform->GetLocalPosition() + direction * leapSpeed * static_cast<float>(Engine().GetInstance().currentNumberOfSteps * Engine().GetInstance().fixedDT);

            // Check for collision
            RectColliderComponent* aiCollider = aiTransform->GetParentGameObject()->GetComponent<RectColliderComponent>(TypeOfComponent::RECTCOLLIDER);
            if (aiCollider) {
                for (auto& [id, target] : GameObjectFactory::GetInstance().GetAllGameObjects()) {
                    if (!target || target == aiTransform->GetParentGameObject()) continue; // Skip null objects or self

                    RectColliderComponent* targetCollider = target->GetComponent<RectColliderComponent>(TypeOfComponent::RECTCOLLIDER);
                    if (targetCollider && aiCollider->CheckCollision(*targetCollider)) {
                        // Collision detected, stop leaping and start cooldown
                        isLeaping = false;
                        isOnCooldown = true;
                        leapCooldownTimer = leapCooldownDuration;

                        // Reset sprite back to normal chase state
                        if (sprite) {
                            aiTransform->SetLocalRotation(0.f);  // Reset rotation to default
                            if (anim)
                            {
                                //sprite->ChangeState(0);
                                anim->animationController->SetParameter("HeldEnemy", false);
                                anim->animationController->SetParameter("ThrowEnemy", false);
                            }
                        }
                        return;
                    }
                }
            }

            // Update the AI's position
            aiTransform->SetLocalPosition(newPosition);

            // Check if leap is complete
            if (distanceToTarget <= leapDistance * 0.1f) { // Slight buffer to prevent overshooting
                isLeaping = false; // Stop leaping
                isOnCooldown = true; // Start cooldown
                leapCooldownTimer = leapCooldownDuration;

                // Reset sprite back to normal chase state
                if (sprite) {
                    aiTransform->SetLocalRotation(0.f);  // Reset rotation to default
                    if (anim)
                    {
                        //sprite->ChangeState(0);
                        anim->animationController->SetParameter("HeldEnemy", false);
                        anim->animationController->SetParameter("ThrowEnemy", false);
                    }
                }
            }
        }
        else {
            // If the AI has reached the target position, stop leaping and start cooldown
            isLeaping = false;
            isOnCooldown = true;
            leapCooldownTimer = leapCooldownDuration;

            // Reset sprite back to normal chase state
            if (sprite) {
                aiTransform->SetLocalRotation(0.f);  // Reset rotation to default
                if (anim)
                {
                    //sprite->ChangeState(0);
                    anim->animationController->SetParameter("HeldEnemy", false);
                    anim->animationController->SetParameter("ThrowEnemy", false);
                }
            }
        }
    }
};
