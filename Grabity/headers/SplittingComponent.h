/*!****************************************************************
\file: SplittingComponent.h
\author: Jeremy Lim Ting Jie, jeremytingjie.lim, 2301370
\brief: Handles splitting of enemies into multiple smaller entities
        when destroyed. Configurable for number of splits, size reduction,
        and attribute modifications of the resulting entities.

Copyright (C) 2024 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents
without the prior written consent of DigiPen Institute of
Technology is prohibited.
*******************************************************************/
#pragma once
#include "Component.h"
#include "GameObject.h"
#include "TransformComponent.h"
#include "HealthComponent.h"
#include "AIStateMachineComponent.h"
#include "SpriteComponent.h"
#include "GameObjectFactory.h"
#include <functional>

class SplittingComponent : public Component {
public:
    // Constructors
    SplittingComponent();
    SplittingComponent(GameObject* parent);
    SplittingComponent(GameObject* parent, int numSplits, float healthMultiplier, float speedMultiplier, float sizeMultiplier);

    // Virtual functions from Component base class
    void Update() override;
    void Serialize(const std::string& luaFilePath, const std::string& tableName) override;
    void Deserialize(const std::string& luaFilePath, const std::string& tableName) override;
    std::string DebugInfo() const override;

    // Getter and setter methods
    int GetNumSplits() const { return numSplits; }
    void SetNumSplits(int numberSplits) { this->numSplits = numberSplits; }

    float GetHealthMultiplier() const { return healthMultiplier; }
    void SetHealthMultiplier(float tmphealthMultiplier) { this->healthMultiplier = tmphealthMultiplier; }

    float GetSpeedMultiplier() const { return speedMultiplier; }
    void SetSpeedMultiplier(float speedofMultiplier) { this->speedMultiplier = speedofMultiplier; }

    float GetSizeMultiplier() const { return sizeMultiplier; }
    void SetSizeMultiplier(float sizeOfMultiplier) { this->sizeMultiplier = sizeOfMultiplier; }

    bool GetHasSplit() const { return hasSplit; }
    void SetHasSplit(bool hasSplited) { this->hasSplit = hasSplited; }

    void SetSplitPrefabPath(const std::string& path) { splitPrefabPath = path; }
    void SetSplitPrefabName(const std::string& name) { splitPrefabName = name; }

    // Main functionality
    void SplitIntoSmallerEnemies();

private:
    int numSplits;               // Number of smaller entities to create
    float healthMultiplier;      // Multiplier for health of smaller entities
    float speedMultiplier;       // Multiplier for speed of smaller entities  
    float sizeMultiplier;        // Multiplier for size of smaller entities
    bool hasSplit;               // Flag to prevent infinite splitting
    bool listenersAttached;      // Flag to track if listeners are attached

    std::string splitPrefabPath; // Path to the Lua prefab file for split entities
    std::string splitPrefabName; // Name of the Lua table for split entities

    // Helper method to calculate offset positions for split entities
    Vector2 CalculateOffsetPosition(int index);

    // Helper method to attach health listener for split triggering
    void AttachHealthListener();
};