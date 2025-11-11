/*!****************************************************************
\file: SplittingComponent.cpp
\author: Jeremy Lim Ting Jie, jeremytingjie.lim, 2301370
\brief: Implementation of the SplittingComponent class which allows
        enemies to split into multiple smaller
        versions when destroyed.

Copyright (C) 2024 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents
without the prior written consent of DigiPen Institute of
Technology is prohibited.
*******************************************************************/
#include "SplittingComponent.h"
#include "ImGuiConsole.h"
#include "MathUtils.h"

/**
 * @brief Default constructor for SplittingComponent.
 */
SplittingComponent::SplittingComponent()
    : Component(),
    numSplits(2),
    healthMultiplier(0.5f),
    speedMultiplier(1.2f),
    sizeMultiplier(0.5f),
    hasSplit(false),
    listenersAttached(false),
    splitPrefabPath("Assets/Lua/Prefabs/Light_Enemy.lua"),
    splitPrefabName("Baby_Enemy_0")
{
}

/**
 * @brief Constructor with parent game object.
 * @param parent The parent GameObject.
 */
SplittingComponent::SplittingComponent(GameObject* parent)
    : Component(parent),
    numSplits(2),
    healthMultiplier(0.5f),
    speedMultiplier(1.2f),
    sizeMultiplier(0.5f),
    hasSplit(false),
    listenersAttached(false),
    splitPrefabPath("Assets/Lua/Prefabs/Light_Enemy.lua"),
    splitPrefabName("Light_Enemy_0")
{
}

/**
 * @brief Constructor with customizable parameters.
 * @param parent The parent GameObject.
 * @param numSplits Number of entities to split into.
 * @param healthMultiplier Multiplier for health of split entities.
 * @param speedMultiplier Multiplier for speed of split entities.
 * @param sizeMultiplier Multiplier for size of split entities.
 */
SplittingComponent::SplittingComponent(GameObject* parent, int numSplits, float healthMultiplier, float speedMultiplier, float sizeMultiplier)
    : Component(parent),
    numSplits(numSplits),
    healthMultiplier(healthMultiplier),
    speedMultiplier(speedMultiplier),
    sizeMultiplier(sizeMultiplier),
    hasSplit(false),
    listenersAttached(false),
    splitPrefabPath("Assets/Lua/Prefabs/Light_Enemy.lua"),
    splitPrefabName("Light_Enemy_0")
{
}

/**
 * @brief Updates the component each frame. Ensures health listeners are attached.
 */
void SplittingComponent::Update() {
    if (!GetActive() || hasSplit)
        return;

    // Attach health listener if not already attached
    if (!listenersAttached) {
        AttachHealthListener();
        listenersAttached = true;
    }
}

/**
 * @brief Attaches a listener to the parent object's health component to trigger splitting.
 */
void SplittingComponent::AttachHealthListener() {
    GameObject* parent = GetParentGameObject();
    if (!parent) return;

    HealthComponent* health = parent->GetComponent<HealthComponent>(TypeOfComponent::HEALTH);
    if (!health) {
        ImGuiConsole::Cout("SplittingComponent: No HealthComponent found on parent");
        return;
    }

    health->SetOnDeathCallback([this]() {
        if (!hasSplit) {
            SplitIntoSmallerEnemies();
        }
        });

    ImGuiConsole::Cout("SplittingComponent: Health listener attached");
}

/**
 * @brief Splits the parent object into multiple smaller entities.
 */
void SplittingComponent::SplitIntoSmallerEnemies() {
    GameObject* parent = GetParentGameObject();
    if (!parent || hasSplit) return;

    // Mark as split to prevent recursion
    hasSplit = true;
    ImGuiConsole::Cout("SplittingComponent: Splitting enemy into %d parts", numSplits);

    // Get required components from parent
    TransformComponent* origTransform = parent->GetComponent<TransformComponent>(TypeOfComponent::TRANSFORM);
    HealthComponent* origHealth = parent->GetComponent<HealthComponent>(TypeOfComponent::HEALTH);
    AIStateMachineComponent* origAI = parent->GetComponent<AIStateMachineComponent>(TypeOfComponent::AISTATE);

    if (!origTransform || !origHealth || !origAI) {
        ImGuiConsole::Cout("SplittingComponent: Missing required components for splitting");
        return;
    }

    // Get original attributes
    Vector2 originalPos = origTransform->GetLocalPosition();
    float smallerHealth = static_cast<float>(origHealth->GetMaxHealth()) * healthMultiplier;

    // Create smaller enemies
    for (int i = 0; i < numSplits; ++i) {
        // Create a new enemy from the specified prefab
        GameObject* smallerEnemy = GameObjectFactory::GetInstance().CreateFromLua(splitPrefabPath, splitPrefabName);
        if (!smallerEnemy) {
            ImGuiConsole::Cout("SplittingComponent: Failed to create smaller enemy %d", i);
            continue;
        }

        std::string smallerEnemyName = "BabyEnemy";
        smallerEnemy->SetName(smallerEnemyName);

        // Calculate position offset for this split
        Vector2 offset = CalculateOffsetPosition(i);

        // Set position and scale
        TransformComponent* transform = smallerEnemy->GetComponent<TransformComponent>(TypeOfComponent::TRANSFORM);
        if (transform) {
            transform->SetLocalPosition(originalPos + offset);
            transform->SetLocalScale(origTransform->GetLocalScale() * sizeMultiplier);
        }

        // Set health
        HealthComponent* health = smallerEnemy->GetComponent<HealthComponent>(TypeOfComponent::HEALTH);
        if (health) {
            int newHealth = static_cast<int>(smallerHealth);
            health->SetMaxHealth(newHealth);
            health->TakeDamage(0); // Reset health to max

            // Check if SplittingComponent already exists
            SplittingComponent* splitComp = smallerEnemy->GetComponent<SplittingComponent>(TypeOfComponent::SPLITTING);
            if (!splitComp) {
                // If no splitting component exists, add one
                smallerEnemy->AddComponent<SplittingComponent>(TypeOfComponent::SPLITTING, numSplits, healthMultiplier, speedMultiplier, sizeMultiplier);
                // Now get the component we just added
                splitComp = smallerEnemy->GetComponent<SplittingComponent>(TypeOfComponent::SPLITTING);
            }
            // Mark as already split to prevent infinite splitting
            splitComp->SetHasSplit(true);
        }

        // Increase speed
        AIStateMachineComponent* ai = smallerEnemy->GetComponent<AIStateMachineComponent>(TypeOfComponent::AISTATE);
        if (ai) {
            ai->SetMoveSpeed(origAI->GetMoveSpeed() * speedMultiplier);
            // Set the chase target to be the same as the original
            ai->SetChaseTarget(origAI->GetChaseTarget());
        }

        // Update sprite to indicate it's a split enemy
        SpriteComponent* sprite = smallerEnemy->GetComponent<SpriteComponent>(TypeOfComponent::SPRITE);
        if (sprite) {
            // Slightly reddish tint to indicate it's a split entity
            sprite->SetRGB(Vector4(1.0f, 0.8f, 0.8f, 1.f));
        }

        ImGuiConsole::Cout("SplittingComponent: Created smaller enemy %d at position (%.2f, %.2f)",
            i, originalPos.x + offset.x, originalPos.y + offset.y);
    }
}

/**
 * @brief Calculates offset positions for split entities in a radial pattern.
 * @param index The index of the split entity.
 * @return Vector2 position offset.
 */
Vector2 SplittingComponent::CalculateOffsetPosition(int index) {
    // For 3 splits, use a triangle formation
    if (numSplits == 2) {
        switch (index) {
        case 0: return Vector2(-20.0f, -20.0f); // Left bottom
        case 1: return Vector2(20.0f, -20.0f);  // Right bottom
        default: return Vector2(0.0f, 0.0f);
        }
    }

    // For any other number, calculate radial positions
    float angle = (2.0f * static_cast<float>(PI) * static_cast<float>(index)) / static_cast<float>(numSplits);
    float distance = 25.0f; // Base distance from center

    return Vector2(
        distance * cos(angle),
        distance * sin(angle)
    );
}

/**
 * @brief Serializes the component data to a Lua file.
 * @param luaFilePath The file path for the Lua file.
 * @param tableName The table name in the Lua file.
 */
void SplittingComponent::Serialize(const std::string& luaFilePath, const std::string& tableName) {
    LuaManager luaManager(luaFilePath);
    LuaManager::LuaValueContainer values;
    std::vector<std::string> keys;

    keys.push_back("numSplits");
    values.push_back(numSplits);

    keys.push_back("healthMultiplier");
    values.push_back(healthMultiplier);

    keys.push_back("speedMultiplier");
    values.push_back(speedMultiplier);

    keys.push_back("sizeMultiplier");
    values.push_back(sizeMultiplier);

    keys.push_back("hasSplit");
    values.push_back(hasSplit);

    keys.push_back("splitPrefabPath");
    values.push_back(splitPrefabPath);

    keys.push_back("splitPrefabName");
    values.push_back(splitPrefabName);

    luaManager.LuaWrite(tableName, values, keys, "Splitting");
}

/**
 * @brief Deserializes the component data from a Lua file.
 * @param luaFilePath The file path for the Lua file.
 * @param tableName The table name in the Lua file.
 */
void SplittingComponent::Deserialize(const std::string& luaFilePath, const std::string& tableName) {
    LuaManager luaManager(luaFilePath);

    numSplits = luaManager.LuaRead<int>(tableName, { "Splitting", "numSplits" });
    healthMultiplier = luaManager.LuaRead<float>(tableName, { "Splitting", "healthMultiplier" });
    speedMultiplier = luaManager.LuaRead<float>(tableName, { "Splitting", "speedMultiplier" });
    sizeMultiplier = luaManager.LuaRead<float>(tableName, { "Splitting", "sizeMultiplier" });
    hasSplit = luaManager.LuaRead<bool>(tableName, { "Splitting", "hasSplit" });
    splitPrefabPath = luaManager.LuaRead<std::string>(tableName, { "Splitting", "splitPrefabPath" });
    splitPrefabName = luaManager.LuaRead<std::string>(tableName, { "Splitting", "splitPrefabName" });
}

/**
 * @brief Provides debugging information for the component.
 * @return A string containing component debug information.
 */
std::string SplittingComponent::DebugInfo() const {
    return "SplittingComponent: { numSplits: " + std::to_string(numSplits) +
        ", healthMultiplier: " + std::to_string(healthMultiplier) +
        ", speedMultiplier: " + std::to_string(speedMultiplier) +
        ", sizeMultiplier: " + std::to_string(sizeMultiplier) +
        ", hasSplit: " + (hasSplit ? "true" : "false") + " }";
}