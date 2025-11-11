/*!****************************************************************
\file: AIStateMachineComponent.cpp
\author: Jeremy Lim Ting Jie, jeremytingjie.lim, 2301370
\co-author: Johny Yong Jun Siang, j.yong, 2301301
\brief: Implements the `AIStateMachineComponent` class, which 
        manages the AI behavior of game objects through a state 
        machine system. 

        Key Features:
        - Supports dynamic states (`Idle`, `Chase`, `Flee`) with transitions.
        - Allows custom state registration and condition-based transitions.
        - Handles serialization and deserialization of state data.
        - Integrates with game objects and components for AI-driven behaviors.
        - Includes debug utilities for monitoring AI state changes.
        - Includes checks for if AI entity is currently held or is an projectile such as:
            - Not collidable when it is held as an object
            - Changing of sprite when it is held
            - Changing of sprite when it is a projectile

        Jeremy contributed (80%) of the code by created the file and included the functionalities of the AI states
        Johny contributed (20%) of the code by making checks for whether it is held or is a projectile.

Copyright (C) 2024 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents
without the prior written consent of DigiPen Institute of
Technology is prohibited.
*******************************************************************!*/
#include "AIStateMachineComponent.h"

#ifdef _IMGUI
#include <iostream>
#endif // _IMGUI
#include "IdleState.h"
#include "ChaseState.h"
#include "FleeState.h"

/**
 * @brief Constructor for the AIStateMachineComponent.
 *
 * Initializes the AI state machine and registers default states. The default state is set to IDLE.
 *
 * @param parent The parent GameObject that this component is attached to.
 */
AIStateMachineComponent::AIStateMachineComponent(GameObject* parent)
    : Component(parent) {
    InitializeStates();
}

/**
 * @brief Destructor for the AIStateMachineComponent.
 *
 * Cleans up and deletes all state instances that were registered for this state machine.
 */
AIStateMachineComponent::~AIStateMachineComponent() {
    for (auto& statePair : stateInstances) {
        delete statePair.second;
    }
    stateInstances.clear();
}

/**
 * @brief Initializes the default states for the state machine.
 *
 * Registers the default AI states such as "IDLE", "CHASE", and "FLEE" and sets the initial state to IDLE.
 */
void AIStateMachineComponent::InitializeStates() {
    RegisterState("IDLE", new IdleState());
    RegisterState("CHASE", new ChaseState());
    RegisterState("FLEE", new FleeState());

    // Default state is IDLE
    SetState("IDLE");
}

/**
 * @brief Updates the current state of the AI.
 *
 * The update function checks the player's actions (such as holding or dragging the object) and updates the
 * AI's state accordingly. It handles state transitions, projectile behavior, and changes in the sprite state.
 * It also processes interactions between the AI and the player (e.g., when the AI is being held by the player).
 */
void AIStateMachineComponent::Update() {
    GameObject* player = GameObjectFactory::GetInstance().GetPlayerObject();

    auto* playerComponent = player->GetComponent<PlayerControllerComponent>(TypeOfComponent::PLAYER);

    TransformComponent* transform = GetParentGameObject()->GetComponent<TransformComponent>(TypeOfComponent::TRANSFORM);
    if (!transform) return;  // Safety check

    Vector2 enemyPosition = transform->GetPosition();

    if (currentStateInstance) {
        if (player)
        {
            //If player is not holding or dragging onto this object, update as per normal. Does not update if this is a projectile as well
            if (playerComponent->GetHeldOBJ() != GetParentGameObject() &&
                playerComponent->GetDraggingOBJ() != GetParentGameObject() && !isProjectile)
            {
                currentStateInstance->Update(this);
            }
        }
    }
    // Handle the case where this object is being held by the player
    if (playerComponent->GetHeldOBJ() != nullptr)
    {
        //This is the AI that has been held
        if (GetParentGameObject() == playerComponent->GetHeldOBJ())
        {
            //Set as trigger to not resolve knockback collisions
            GetParentGameObject()->GetComponent<RectColliderComponent>(TypeOfComponent::RECTCOLLIDER)->SetTrigger(true);
            auto* sprite = GetParentGameObject()->GetComponent<SpriteComponent>(TypeOfComponent::SPRITE);
            auto* anim = GetParentGameObject()->GetComponent<AnimatorComponent>(TypeOfComponent::ANIMATOR);

            if (sprite && anim)
            {
                // sprite->ChangeState(1);
                anim->animationController->SetParameter("HeldEnemy", true);
                anim->animationController->SetParameter("ThrowEnemy", false);
            }
        }
    }
    // threshold for detecting minimal movement
    const float epsilon = 10.f;
    if (isProjectile)
    {
        projectileTimer += (float)InputManager::GetDeltaTime(); // Update the timer

        auto* rb = GetParentGameObject()->GetComponent<RigidBodyComponent>(RIGIDBODY);
        if (rb)
        {
            auto* sprite = GetParentGameObject()->GetComponent<SpriteComponent>(TypeOfComponent::SPRITE);
            auto* anim = GetParentGameObject()->GetComponent<AnimatorComponent>(TypeOfComponent::ANIMATOR);

            if (sprite && anim)
            {
                anim->animationController->SetParameter("ThrowEnemy", true);
                anim->animationController->SetParameter("HeldEnemy", false);
            }

            if (GetParentGameObject()->GetTag() == "BombEnemy")
            {
                auto* trans = GetParentGameObject()->GetComponent<TransformComponent>(TypeOfComponent::TRANSFORM);
                trans->SetLocalScale({ 300, 300 });
            }

            // If velocity is low OR timer exceeded, reset projectile state
            if ((std::fabs(rb->GetVelocity().x) < epsilon && std::fabs(rb->GetVelocity().y) < epsilon) ||
                projectileTimer >= maxProjectileDuration)
            {
                isProjectile = false;
                projectileTimer = 0.0f;

                auto* trans = GetParentGameObject()->GetComponent<TransformComponent>(TypeOfComponent::TRANSFORM);
                if (sprite)
                {
                    trans->SetLocalRotation(0.f);  // Reset rotation
                    if (anim)
                    {
                        anim->animationController->SetParameter("HeldEnemy", false);
                        anim->animationController->SetParameter("ThrowEnemy", false);
                    }
                }

                // Reset collider
                auto* collider = GetParentGameObject()->GetComponent<RectColliderComponent>(TypeOfComponent::RECTCOLLIDER);
                if (collider) {
                    auto* parentObject = GetParentGameObject();
                    if (parentObject) {
                        if (parentObject->GetTag() == "HeavyEnemy") {
                            collider->RestoreOriginalColliderBox("HeavyEnemy");
                        }
                        if (parentObject->GetTag() == "BombEnemy") {
                            collider->RestoreOriginalColliderBox("BombEnemy");
                        }
                    }
                }
            }
        }
    }

    // Check transitions dynamically
    for (const auto& [transitionName, condition] : transitions) {
        if (condition() && stateInstances.count(transitionName)) {
            SetState(transitionName);
            break;
        }
    }

#pragma region OutOfBounds
    const float despawnOffset = 700.f;  // Buffer zone before despawning

    if (playerComponent) {
        GameObject* heldObject = playerComponent->GetHeldOBJ();
        if (heldObject == GetParentGameObject()) {
            return;  // Skip despawn if this enemy is currently held
        }
    }

    float minX = -FLT_MAX, maxX = FLT_MAX;
    float minY = -FLT_MAX, maxY = FLT_MAX;

    auto TopBorder = GameObjectFactory::GetInstance().FindGameObjectsByTag("TopBorder");
    if (!TopBorder.empty()) {
        TransformComponent* position = TopBorder[0]->GetComponent<TransformComponent>(TypeOfComponent::TRANSFORM);
        RectColliderComponent* collider = TopBorder[0]->GetComponent<RectColliderComponent>(TypeOfComponent::RECTCOLLIDER);
        if (collider ) {
            auto& colliderData = collider->GetColliderData();
            maxY = position->GetLocalPosition().y + colliderData[0].second.y + colliderData[0].first.y + despawnOffset;  // Added Offset
        }
    }

    auto LowerBorder = GameObjectFactory::GetInstance().FindGameObjectsByTag("LowerBorder");
    if (!LowerBorder.empty()) {
        TransformComponent* position = LowerBorder[0]->GetComponent<TransformComponent>(TypeOfComponent::TRANSFORM);
        RectColliderComponent* collider = LowerBorder[0]->GetComponent<RectColliderComponent>(TypeOfComponent::RECTCOLLIDER);
        if (collider) {
            auto& colliderData = collider->GetColliderData();
            minY = position->GetLocalPosition().y + colliderData[0].second.y - colliderData[0].first.y - despawnOffset;  // Added Offset
        }
    }

    auto LeftBorder = GameObjectFactory::GetInstance().FindGameObjectsByTag("LeftBorder");
    if (!LeftBorder.empty()) {
        TransformComponent* position = LeftBorder[0]->GetComponent<TransformComponent>(TypeOfComponent::TRANSFORM);
        RectColliderComponent* collider = LeftBorder[0]->GetComponent<RectColliderComponent>(TypeOfComponent::RECTCOLLIDER);
        if (collider) {
            auto& colliderData = collider->GetColliderData();
            minX = position->GetLocalPosition().x + colliderData[0].second.x - colliderData[0].first.x - despawnOffset;  // Added Offset
        }
    }

    auto RightBorder = GameObjectFactory::GetInstance().FindGameObjectsByTag("RightBorder");
    if (!RightBorder.empty()) {
        TransformComponent* position = RightBorder[0]->GetComponent<TransformComponent>(TypeOfComponent::TRANSFORM);
        RectColliderComponent* collider = RightBorder[0]->GetComponent<RectColliderComponent>(TypeOfComponent::RECTCOLLIDER);
        if (collider) {
            auto& colliderData = collider->GetColliderData();
            maxX = position->GetLocalPosition().x + colliderData[0].second.x + colliderData[0].first.x + despawnOffset * 3;  // Added Offset
        }
    }

    if (enemyPosition.x < minX || enemyPosition.x > maxX ||
        enemyPosition.y < minY || enemyPosition.y > maxY) {
        ImGuiConsole::Cout("Enemy out of bounds! Despawning...");
        GameObjectFactory::GetInstance().QueueDespawn(GetParentGameObject());
        return;  // Stop updating
    }

#pragma endregion


}

/**
 * @brief Registers a new state in the state machine.
 *
 * This method adds a new state to the state machine. If a state with the same name already exists, it will not be added.
 *
 * @param stateName The name of the state to register.
 * @param state A pointer to the state instance to be registered.
 */
void AIStateMachineComponent::RegisterState(const std::string& stateName, AIStateBase* state) {
    if (stateInstances.count(stateName) == 0) {
        stateInstances[stateName] = state;
    }
}

/**
 * @brief Sets the current state of the state machine by name.
 *
 * This method sets the current state of the state machine to the specified state name. It first exits the current state,
 * then updates to the new state, and enters the new state.
 *
 * @param stateName The name of the state to set as the current state.
 *
 * @note If the state being set is the same as the current state, the method does nothing.
 */
void AIStateMachineComponent::SetState(const std::string& stateName) {
    if (currentStateName == stateName) return;

    // Exit current state
    if (currentStateInstance) {
        currentStateInstance->Exit(this);
    }

    // Update state
    currentStateName = stateName;
    currentStateInstance = stateInstances[stateName];

    // Enter new state
    if (currentStateInstance) {
        currentStateInstance->Enter(this);
    }
}

/**
 * @brief Sets a state conditionally with a guard function.
 *
 * This method sets the current state to the specified state name only if the provided guard function returns true.
 *
 * @param stateName The name of the state to set as the current state.
 * @param guard A function that returns a boolean indicating whether the state change is allowed.
 */
void AIStateMachineComponent::SetStateWithGuard(const std::string& stateName, std::function<bool()> guard) {
    if (guard()) {
        SetState(stateName);
    }
}

/**
 * @brief Adds a transition to the state machine.
 *
 * This method adds a transition condition to the state machine. The transition will be checked each update cycle.
 *
 * @param transitionName The name of the transition.
 * @param condition A function that returns a boolean value, indicating if the transition should occur.
 */
void AIStateMachineComponent::AddTransition(const std::string& transitionName, std::function<bool()> condition) {
    transitions[transitionName] = condition;
}

/**
 * @brief Checks the condition of a specific transition.
 *
 * This method checks the condition of a specific transition. If the transition exists and the condition evaluates to true,
 * the transition is considered valid.
 *
 * @param transitionName The name of the transition to check.
 * @return True if the transition condition is met, false otherwise.
 */
bool AIStateMachineComponent::CheckTransition(const std::string& transitionName) const {
    if (transitions.count(transitionName)) {
        return transitions.at(transitionName)();
    }
    return false;
}

/**
 * @brief Serializes the state machine to a Lua file.
 *
 * This method serializes the state machine's current state, timing, movement variables, and target GameObject ID to a Lua
 * file for persistence.
 *
 * @param luaFilePath The path to the Lua file where the state machine data will be saved.
 * @param tableName The name of the Lua table under which the state machine data will be written.
 */
void AIStateMachineComponent::Serialize(const std::string& luaFilePath, const std::string& tableName) {
    LuaManager luaManager(luaFilePath);

    std::vector<std::string> keys;
    LuaManager::LuaValueContainer values;

    // Serialize the current state as a string
    keys.push_back("currentStateName");
    values.push_back(currentStateName);

    // Serialize timing and movement variables
    keys.push_back("waitTime");
    values.push_back(waitTime);

    keys.push_back("moveSpeed");
    values.push_back(moveSpeed);

    // Serialize target GameObject ID, if it exists
    keys.push_back("target");
    values.push_back(target ? target->GetId() : -1);

    // Write serialized data into the Lua file
    luaManager.LuaWrite(tableName, values, keys, "AIState");
}

/**
 * @brief Deserializes the state machine from a Lua file.
 *
 * This method reads the state machine's data from a Lua file and restores the state machine to its previous state,
 * including the current state, timing, movement variables, and target GameObject ID.
 *
 * @param luaFilePath The path to the Lua file from which the state machine data will be loaded.
 * @param tableName The name of the Lua table from which the state machine data will be read.
 */
void AIStateMachineComponent::Deserialize(const std::string& luaFilePath, const std::string& tableName) {
    LuaManager luaManager(luaFilePath);

    // Deserialize the current state name
    std::string stateName = luaManager.LuaRead<std::string>(tableName, { "AIState", "currentStateName" });

    // Ensure the state is registered before setting it
    if (stateInstances.count(stateName)) {
        SetState(stateName);
    }
    else {
		ImGuiConsole::Cout("Warning: State '%s' not found during deserialization. Defaulting to 'IDLE'.\n", stateName.c_str());
        SetState("IDLE"); // Default to "IDLE" if state is not found
    }

    // Deserialize timing and movement variables
    waitTime = luaManager.LuaRead<float>(tableName, { "AIState", "waitTime" });
    moveSpeed = luaManager.LuaRead<float>(tableName, { "AIState", "moveSpeed" });

    // Deserialize target GameObject ID
    int targetID = luaManager.LuaRead<int>(tableName, { "AIState", "target" });
    if (targetID != -1) {
        // Retrieve the target GameObject using the ID
        target = GameObjectFactory::GetInstance().GetAllGameObjects()[targetID];
    }
    else {
        target = nullptr;  // No target is set
    }
}


/**
 * @brief Provides debugging information about the current state machine.
 *
 * This method returns a string containing the current state of the state machine, useful for debugging purposes.
 *
 * @return A string containing debugging information about the current state machine.
 */
std::string AIStateMachineComponent::DebugInfo() const {
    return "AIStateMachineComponent - Current State: " + currentStateName;
}