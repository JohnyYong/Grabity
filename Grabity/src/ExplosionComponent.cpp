/*!****************************************************************
\file: ExplosionComponent.cpp
\author: Jeremy Lim Ting Jie, jeremytingjie.lim, 2301370
\brief: Handles explosion behavior, including countdown, visual effects, 
        damage application, and serialization

Copyright (C) 2024 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents
without the prior written consent of DigiPen Institute of
Technology is prohibited.
*******************************************************************/
#include "ExplosionComponent.h"
#include "GameObject.h"
#include "HealthComponent.h"
#include "GameObjectFactory.h"
#include "GLHelper.h"
#include "SpriteComponent.h"
#include "assetmanager.h"
#include <iostream>
#include "ImGuiConsole.h"

/**
 * @brief Constructs an ExplosionComponent with default values.
 *
 * Initializes explosion properties such as countdown time, damage radius,
 * and explosion state.
 */
ExplosionComponent::ExplosionComponent(GameObject* parent)
    : Component(parent), countdownTime(2.0f), damageRadius(25.0f), damageAmount(40.0f), hasExploded(false), blinkState(false), blinkTimer(0.0f) {
    explodingSFX = false;
}


/**
 * @brief Constructs an ExplosionComponent with custom parameters.
 *
 * Allows customization of countdown time, radius, damage, and explosion sound effect.
 */
ExplosionComponent::ExplosionComponent(GameObject* parent, float countdown, float radius, float damage, bool sfx)
    : Component(parent), countdownTime(countdown), damageRadius(radius), damageAmount(damage), hasExploded(false), blinkState(false), blinkTimer(0.0f), explodingSFX(sfx) {
}

/**
 * @brief Updates the explosion timer and triggers visual effects.
 *
 * Manages countdown timing, blinking visual cue, and eventually triggers an explosion.
 */
void ExplosionComponent::Update() {
    Engine& engine = Engine::GetInstance();
    if (hasExploded) return;
    if (aiComponent && !aiComponent->isProjectile) return;
    if (!wasShotOut) return;

    // Use fixed time step as required
    float fixedDeltaTime = static_cast<float>(engine.fixedDT * (long long)engine.currentNumberOfSteps);

    // Decrease countdown time
    countdownTime -= fixedDeltaTime;

    // Calculate dynamic blink interval
    float blinkInterval = std::max(0.1f, countdownTime / 10.0f);

    // Update blink timer
    blinkTimer += fixedDeltaTime;

    if (blinkTimer >= blinkInterval) {
        blinkState = !blinkState;
        blinkTimer -= blinkInterval;  // Maintain accuracy by subtracting, not resetting

        if (!explodingSFX) {
            AudioManager::GetInstance().PlayAudio(14);
            AudioManager::GetInstance().SetChannelVolume(14, 0.5f);  // Make this audio softer
            explodingSFX = true;

        }
    }

    // Update sprite color based on blink state
    GameObject* parent = GetParentGameObject();
    auto* sprite = parent->GetComponent<SpriteComponent>(TypeOfComponent::SPRITE);
    if (sprite) {
        sprite->SetRGB(blinkState ? Vector4(0.0f, 0.0f, 0.0f,1.f) : Vector4(1.0f, 1.0f, 1.0f, 1.f)); // Toggle between black/white
    }
    else {
        ImGuiConsole::Cout("Sprite component not found");
    }

    // Trigger explosion when countdown ends
    if (countdownTime <= 0.0f) {
        TriggerExplosion();
    }
}

/**
 * @brief Triggers the explosion, applying damage and removing the object.
 *
 * Detects objects in the explosion radius and applies damage where necessary.
 */
void ExplosionComponent::TriggerExplosion() {
    if (hasExploded) return;  // Ensure it doesn't explode multiple times
    hasExploded = true;       // Mark as exploded

    GameObject* explosionObject = GetParentGameObject();
    if (!explosionObject) return; // Ensure the explosion object is valid

    // Get the transform of the explosion center
    TransformComponent* explosionTransform = explosionObject->GetComponent<TransformComponent>(TypeOfComponent::TRANSFORM);
    if (!explosionTransform) return;

    Vector2 explosionCenter = explosionTransform->GetLocalPosition();

    // Define explosion radius (for simplicity, it's a square in this case)
    float explosionRadius = GetDamageRadius();

    // Create the explosion's AABB
    AABB explosionAABB(
        explosionCenter - Vector2(explosionRadius, explosionRadius),
        explosionCenter + Vector2(explosionRadius, explosionRadius)
    );

    // Display the blast radius
    DisplayBlastRadius(explosionCenter, explosionRadius * 2);

    // Loop through all game objects in the factory to check for collision
    for (auto& [id, target] : GameObjectFactory::GetInstance().GetAllGameObjects()) {
        if (!target || target == explosionObject) continue; // Skip null objects or the explosion source

        auto* targetCollider = target->GetComponent<RectColliderComponent>(TypeOfComponent::RECTCOLLIDER);
        if (!targetCollider) continue; // Skip objects without colliders

        // Get the position and size of the target collider
        Vector2 targetPos = targetCollider->GetColliderData()[0].first;  // Assuming first collider
        Vector2 targetSize = targetCollider->GetColliderData()[0].second;

        // Calculate the target's AABB
        AABB targetAABB(
            targetPos,
            targetPos + targetSize
        );

        // Check for overlap between the explosion AABB and the target's AABB
        if (explosionAABB.CheckCollision(targetAABB)) {
            // If a collision is detected, apply damage if the target has a health component
            HealthComponent* health = target->GetComponent<HealthComponent>(TypeOfComponent::HEALTH);
            if (health) {
                ImGuiConsole::Cout("Collision detected with target %d!", target->GetId());
            }
        }
    }

    // Queue the explosion object for despawning 
    HealthComponent* health = explosionObject->GetComponent<HealthComponent>(TypeOfComponent::HEALTH);
    if (health) {
        health->TriggerDespawn(); // Trigger despawn via health component
    }
}

/**
 * @brief Displays the explosion's blast radius effect.
 *
 * Creates a visual effect indicating the explosion area.
 */
void ExplosionComponent::DisplayBlastRadius(const Vector2& center, float radius) {
    // Load the explosion VFX prefab from Lua
    const std::string prefabPath = "Assets/Lua/Prefabs/ExplosionVFX.lua";
    const std::string prefabName = "ExplosionVFX_0";

    GameObject* blastRadiusObject = GameObjectFactory::GetInstance().CreateFromLua(prefabPath, prefabName);
    if (!blastRadiusObject) {
        std::cerr << "Failed to create blast radius object from prefab: " << prefabPath << std::endl;
        return;
    }
    ImGuiConsole::Cout("Blast radius object created from prefab\n");

    // Set the position using the TransformComponent
    TransformComponent* transformComponent = blastRadiusObject->GetComponent<TransformComponent>(TypeOfComponent::TRANSFORM);
    if (transformComponent) {
        transformComponent->SetLocalPosition(center);

        // Retrieve the texture width from the SpriteComponent
        SpriteComponent* spriteComponent = blastRadiusObject->GetComponent<SpriteComponent>(TypeOfComponent::SPRITE);
        if (spriteComponent) {
            SpriteAnimation* spriteAnimation = spriteComponent->GetCurrentSprite();
            if (spriteAnimation) {
                float textureWidth = spriteAnimation->GetTextureWidth();
                float scale = 300 * (radius / (textureWidth / 2.0f));
                transformComponent->SetLocalScale(Vector2(scale, scale));
                ImGuiConsole::Cout( "Blast radius object scaled to: %f",scale);
                AudioManager::GetInstance().PlayAudio(13);
                AudioManager::GetInstance().SetChannelVolume(13, 1.f);  // Make this audio louder

            }
            else {
                std::cerr << "Failed to get sprite animation from component" << std::endl;
            }
        }
        else {
            std::cerr << "Sprite component not found" << std::endl;
        }
    }
    else {
        std::cerr << "Transform component not found" << std::endl;
    }
}

/**
 * @brief Serializes the explosion component data to a Lua file.
 */
void ExplosionComponent::Serialize(const std::string& luaFilePath, const std::string& tableName) {
    LuaManager luaManager(luaFilePath);
    LuaManager::LuaValueContainer values;
    std::vector<std::string> keys;

    keys.push_back("countdownTime");
    values.push_back(countdownTime);

    keys.push_back("damageRadius");
    values.push_back(damageRadius);

    keys.push_back("damageAmount");
    values.push_back(damageAmount);

    keys.push_back("hasExploded");
    values.push_back(hasExploded);

    luaManager.LuaWrite(tableName, values, keys, "Explosion");
}

/**
 * @brief Deserializes the explosion component data from a Lua file.
 */
void ExplosionComponent::Deserialize(const std::string& luaFilePath, const std::string& tableName) {
    LuaManager luaManager(luaFilePath);

    countdownTime = luaManager.LuaRead<float>(tableName, { "Explosion", "countdownTime" });
    damageRadius = luaManager.LuaRead<float>(tableName, { "Explosion", "damageRadius" });
    damageAmount = luaManager.LuaRead<float>(tableName, { "Explosion", "damageAmount" });
    hasExploded = luaManager.LuaRead<float>(tableName, { "Explosion", "hasExploded" });
}

/**
 * @brief Provides debugging information
 *
 *
 * @return A string containing debugging information.
 */
std::string ExplosionComponent::DebugInfo() const {
    return "ExplosionComponent: { countdownTime: " + std::to_string(countdownTime) +
        ", damageRadius: " + std::to_string(damageRadius) +
        ", damageAmount: " + std::to_string(damageAmount) +
        ", hasExploded: " + (hasExploded ? "true" : "false") + " }";
}