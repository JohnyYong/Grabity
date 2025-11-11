/*!****************************************************************
\file: FloatUpComponent.h
\author: Lee Yu Jie Brandon, l.yujiebrandon, 2301232
\brief: Component that makes game objects move upward at a configurable
        speed, with optional fade-out functionality. Commonly used for
        damage indicators, floating text, or visual effects that rise
        and disappear over their lifetime.
Copyright (C) 2025 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents
without the prior written consent of DigiPen Institute of
Technology is prohibited.
*******************************************************************!*/
#pragma once
#include "Component.h"
#include "GameObject.h"
#include "Vector2.h"
#include "TransformComponent.h"
#include "Engine.h"
#include "TextComponent.h"
#include "SpriteComponent.h"

/*!****************************************************************
\class FloatUpComponent
\brief Handles floating and fading effects for game objects.

The FloatUpComponent is responsible for displaying the hit damage
counter for this scenario.
*******************************************************************/
class FloatUpComponent : public Component {
private:
    float speed;
    float fadeStartTime;
    float lifetime;
    float elapsedTime;
    SpriteComponent* spriteComponent; // For sprite fading
    TextComponent* textComponent;     // For text fading
    float initialAlpha;               // Store initial alpha for fading calculations

public:
    TypeOfComponent componentType = TypeOfComponent::FLOATUP;

    FloatUpComponent() :
        speed(100.0f),
        fadeStartTime(1.0f),
        lifetime(2.0f),
        elapsedTime(0.0f),
        spriteComponent(nullptr),
        textComponent(nullptr),
        initialAlpha(1.0f) {}

    FloatUpComponent(GameObject* parent) :
        Component(parent),
        speed(100.0f),
        fadeStartTime(1.0f),
        lifetime(2.0f),
        elapsedTime(0.0f),
        spriteComponent(nullptr),
        textComponent(nullptr),
        initialAlpha(1.0f) {

        // Try to get the sprite component for fading
        spriteComponent = GetParentGameObject()->GetComponent<SpriteComponent>(TypeOfComponent::SPRITE);

        // Try to get the text component for text fading
        textComponent = GetParentGameObject()->GetComponent<TextComponent>(TypeOfComponent::TEXT);

        // Store initial alpha if we have a text component
        if (textComponent) {
            initialAlpha = textComponent->GetAlpha();
        }
    }

    ~FloatUpComponent() override = default;

    void SetSpeed(float newSpeed) {
        speed = newSpeed;
    }

    void SetLifetime(float newLifetime) {
        lifetime = newLifetime;
    }

    void SetFadeStartTime(float newFadeStartTime) {
        fadeStartTime = newFadeStartTime;
    }

    void Update() override {
        if (!GetActive()) return;
        float deltaTime = static_cast<float>(Engine::GetInstance().fixedDT);
        elapsedTime += deltaTime;
        // Move upward
        auto* transform = GetParentGameObject()->GetComponent<TransformComponent>(TypeOfComponent::TRANSFORM);
        Vector2 newPos(0,0);
        if (transform) {
            Vector2 currentPos = transform->GetLocalPosition();
            newPos = { currentPos.x, currentPos.y + speed * deltaTime }; // Move upward (negative Y)
            transform->SetLocalPosition(newPos);

 
        }

        // Update text position to match transform (regardless of fading)
        if (textComponent) {
            textComponent->SetPosition(newPos);
        }

        // Handle fading 
        if (elapsedTime > fadeStartTime) {
            float fadeProgress = (elapsedTime - fadeStartTime) / (lifetime - fadeStartTime);
            float alpha = initialAlpha * (1.0f - fadeProgress);
            if (alpha < 0.0f) alpha = 0.0f;

            // Apply fade to sprite if we have one
            if (spriteComponent) {
                // Assuming SpriteComponent has alpha setting capability
                // spriteComponent->SetAlpha(alpha);
            }

            // Apply fade to text if we have one
            if (textComponent) {
                textComponent->SetAlpha(alpha);
            }
     
        }

        // Optional: Destroy the object when lifetime is up
        if (elapsedTime >= lifetime) {
            // Despawn is handled by DespawnManager, so we don't need to do it here
        }
    }

    // Implement required virtual methods
    void Serialize(const std::string& luaFilePath, const std::string& tableName) override {
        // Implement serialization if needed
        (void)luaFilePath;
        (void)tableName;
    }

    void Deserialize(const std::string& luaFilePath, const std::string& tableName) override {
        // Implement deserialization if needed
        (void)luaFilePath;
        (void)tableName;
    }

    std::string DebugInfo() const override {
        return "FloatUp Component | Speed: " + std::to_string(speed) +
            " | Elapsed Time: " + std::to_string(elapsedTime) +
            " | Lifetime: " + std::to_string(lifetime);
    }
};