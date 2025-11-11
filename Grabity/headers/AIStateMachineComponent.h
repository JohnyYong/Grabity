/*!****************************************************************
\file: AIStateMachineComponent.h
\author: Jeremy Lim Ting Jie, jeremytingjie.lim, 2301370
\co-author: Johny Yong Jun Siang, j.yong, 2301301
\brief: Defines the AIStateMachineComponent class responsible
        for managing AI states and behaviors in a game object.

        Jeremy contributed (98%) of the code by creating header file and most of the function declarations
        Johny contributed (2%) of the code with the addition of isProjectile check

Copyright (C) 2024 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents
without the prior written consent of DigiPen Institute of
Technology is prohibited.
*******************************************************************!*/
#pragma once
#include "Component.h"
#include <string>
#include <map>
#include <functional>
#include "AIStateBase.h"

class GameObject;

/*!
 * \class AIStateMachineComponent
 * \brief Manages AI states and behaviors for a game object.
 *
 * The `AIStateMachineComponent` class handles state registration,
 * state transitions, and AI-specific actions such as chasing a target.
 * It provides dynamic state management and supports guarded transitions
 * based on customizable conditions.
 */
class AIStateMachineComponent : public Component {
public:
    float projectileTimer = 0.0f;
    const float maxProjectileDuration = 2.0f; // seconds

    /*!
     * \brief Constructs an AIStateMachineComponent.
     * \param parent The parent `GameObject` to which this component belongs.
     */
    AIStateMachineComponent(GameObject* parent);

    /*!
     * \brief Destructor for the AIStateMachineComponent.
     */
    ~AIStateMachineComponent();

    void InitializeStates(); ///< Initialize state objects
    void Update() override;  ///< Update the current state

    // Serialization/ Deserialization
    void Serialize(const std::string& luaFilePath, const std::string& tableName) override;
    void Deserialize(const std::string& luaFilePath, const std::string& tableName) override;

    std::string DebugInfo() const override;  // Debug information

    // State registration and dynamic management
    void RegisterState(const std::string& stateName, AIStateBase* state);
    void SetState(const std::string& stateName);
    void SetStateWithGuard(const std::string& stateName, std::function<bool()> guard);
    void SetMoveSpeed(float newSpeed) { moveSpeed = newSpeed; }
    void SetChaseTarget(GameObject* newTarget) { target = newTarget; }

    //Getters
    const std::string& GetCurrentStateName() const { return currentStateName; }
    AIStateBase* GetCurrentStateInstance() const { return currentStateInstance; }
    float GetMoveSpeed() const { return moveSpeed; }
    GameObject* GetChaseTarget() const { return target; }
    // Method to retrieve all available states
    std::vector<std::string> GetAllStates() const {
        std::vector<std::string> stateNames;
        for (const auto& statePair : stateInstances) {
            stateNames.push_back(statePair.first); // Collect state names
        }
        return stateNames;
    }
    // Transition conditions
    void AddTransition(const std::string& transitionName, std::function<bool()> condition);
    bool CheckTransition(const std::string& transitionName) const;


    bool isProjectile = false; ///< Indicates if the component is associated with a projectile.

    float walkSFXTimer = 0.0f;
    const float walkSFXCooldown = 0.3f; // Play a walking sound every 0.3 seconds

private:
    std::string currentStateName;                           ///< Name of the current state
    AIStateBase* currentStateInstance = nullptr;            ///< Pointer to the current state instance
    std::map<std::string, AIStateBase*> stateInstances;     ///< Map of state names to instances
    std::map<std::string, std::function<bool()>> transitions; ///< Transition conditions

    GameObject* target = nullptr; ///< The object the AI will chase
    float moveSpeed;       ///< Speed at which the AI moves towards/away from the target
    float waitTime = 0.1f;        ///< Time delay before the AI starts chasing or fleeing


};

