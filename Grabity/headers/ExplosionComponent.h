/*!****************************************************************
\file: ExplosionComponent.h
\author: Jeremy Lim Ting Jie, jeremytingjie.lim, 2301370
\brief: Header file for Explosion Component with declarations
 setters and getters

Copyright (C) 2024 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents
without the prior written consent of DigiPen Institute of
Technology is prohibited.
*******************************************************************/
#pragma once
#include "Component.h"
#include <string>
#include "Vector2.h"
class AIStateMachineComponent;
/**
 * @brief Component responsible for handling explosion mechanics.
 *
 * Manages explosion timing, damage, visual effects, and interaction with AI components.
 */
class ExplosionComponent : public Component {
private:
    float countdownTime;   // Time left before explosion
    float damageRadius;    // Radius of explosion effect
    float damageAmount;    // Damage dealt by the explosion
    bool hasExploded;      // To track if the explosion has already occurred
	float blinkTimer;      // Timer for blinking effect
	bool blinkState;	   // State for blinking effect
    bool explodingSFX;     ///< Indicates if explosion sound effects should play.
    AIStateMachineComponent* aiComponent = nullptr; // Pointer to the AIStateMachineComponent
    GameObject* explosiveSFX = nullptr; ///< Pointer to explosion sound effect object.


public:
    // Constructor
    ExplosionComponent(GameObject* parent, float countdown, float radius, float damage, bool explodingSFX);

    ExplosionComponent(GameObject* parent = nullptr);

    // Destructor
    ~ExplosionComponent() override = default;

    // Update method for ticking down and triggering explosion
    void Update() override;

    void SetAIStateMachine(AIStateMachineComponent* aiComponentParam) {
        this->aiComponent = aiComponentParam;
    }

    // Serialize and deserialize to/from Lua files
    void Serialize(const std::string& luaFilePath, const std::string& tableName) override;
    void Deserialize(const std::string& luaFilePath, const std::string& tableName) override;

    // Debug info for the component
    std::string DebugInfo() const override;

    // Trigger the explosion manually
    void TriggerExplosion();

    // Getter methods
    bool GetBlinkState() const { return blinkState;  }
    float GetCountdownTime() const { return countdownTime; }
    float GetDamageRadius() const { return damageRadius; }
    float GetDamageAmount() const { return damageAmount; }
    bool HasExploded() const { return hasExploded; }
    void DisplayBlastRadius(const Vector2& center, float radius);

    // Setters for modifying explosion parameters
    void SetCountdownTime(float time) { countdownTime = time; }
    void SetDamageRadius(float radius) { damageRadius = radius; }
    void SetDamageAmount(float amount) { damageAmount = amount; }

    bool GetExplodingSFXState() { return explodingSFX; }
    bool wasShotOut = false; // Track if the object was shot out

};
