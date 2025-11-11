/*!****************************************************************
\file: HealthComponent.cpp
\author: Jeremy Lim Ting Jie, jeremytingjie.lim, 2301370
\co-author: Johny Yong Jun Siang, j.yong, 2301301
            Teng Shi Heng, shiheng.teng,  2301269
\brief: This file implements the `HealthComponent` class, which 
        manages the health of a game object in the game engine. 

        Johny completed (10%) of the code: Adding color turning check
        Shi heng completed (10%) of the code: Adding of vfx when player or enemy get hits
        Jeremy completed (80%) the rest of the features

        Features include:
        - Health management (damage, max health, and current health).
        - Death callbacks for custom behavior on reaching zero health.
        - Integration with other components (e.g., sprite color feedback).
        - Serialization/Deserialization for saving and loading states.
        - Debugging utilities for health information display.
        - Turning the damaged object to red color for a certain time and returning back to original color after.
        - Spawning of blood splatter vfx when player gets damaged
        - Spawning of hit vfx when enemy gets damaged

Copyright (C) 2024 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents
without the prior written consent of DigiPen Institute of
Technology is prohibited.
*******************************************************************!*/
#include "HealthComponent.h"
#include "EventSystem.h"
#include "DespawnManager.h"
#include "FloatUpComponent.h"

#ifdef _IMGUI
#include <iostream>
#include "ImGuiConsole.h"
#endif // _IMGUI

/*!
\brief Constructs a `HealthComponent` with initial and maximum health values.
\param parent Pointer to the parent GameObject.
\param initialHealth The starting health value.
\param maxHealthValue The maximum health the object can have.
*/
HealthComponent::HealthComponent(GameObject* parent, int initialHealth, int maxHealthValue)
    : Component(parent), // This should initialize parent if the Component constructor is available
    health(initialHealth),
    maxHealth(maxHealthValue),
    onDeathCallback(nullptr) {}


/*!
\brief Default constructor for `HealthComponent`. Initializes members to default values.
*/
HealthComponent::HealthComponent()
    : health(50),    
    maxHealth(50), 
    onDeathCallback(nullptr) 
{}

/*!
\brief Sets a callback to execute when health reaches zero.
\param callback A function to call upon death of the object.
*/
void HealthComponent::SetOnDeathCallback(const std::function<void()>& callback) {
    onDeathCallback = [this, callback]() {
        if (GameObjectFactory::GetInstance().IsGameObjectValid(this->GetParentGameObject())) {
            callback();
        }
        else {
			ImGuiConsole::Cout("Error: Attempted to execute callback on an invalid GameObject.");
        }
        };
}


/*!
\brief Reduces health by the specified damage amount. Handles visual feedback.
\param damage The amount of damage to apply.
*/
void HealthComponent::TakeDamage(int damage) {
    if (health <= 0) return; // Already "dead"
    if (damageCooldownTimer > 0.0f) return; // Still on cooldown, can't take damage yet

    if (Engine::GetInstance().isGodMode)
    {
        health -= damage * 2;
    }
    else
    {
        health -= damage;
    }
    // Start cooldown
    damageCooldownTimer = damageCooldownDuration;

    colorTimer = 1.f;
    turnRed = true;
    GameObject* parent = GetParentGameObject();
    
    //Damaged SFX
    if (parent)
    {
        int damageSFX = -1;
        float audioControl = 0.3f;
        //Slimes
        if (parent->GetName() == "BabyEnemy" || parent->GetName() == "Light_Enemy")
        {
            damageSFX = 32;
        }
        else if (parent->GetName() == "Heavy_Enemy")
        {
            damageSFX = 34 + (std::rand() % 3); // Pick a random damage SFX for heavy Enemy between audio id 34 - 36
        }
        else if (parent->GetName() == "Bomb_Enemy")
        {
            damageSFX = 62 + (std::rand() % 6); // Pick a random damage SFX for heavy Enemy between audio id 62 - 67
            audioControl = 0.5f;
        }

        if (damageSFX != -1)
        {
            AudioManager::GetInstance().PlayAudio(damageSFX);
            AudioManager::GetInstance().SetChannelVolume(damageSFX, audioControl);
        }
    }

    //spawning of vfx upon collision
    if (GetParentGameObject()->GetTag() == "Player")
    {
        GameObject* damageVfx = GameObjectFactory::GetInstance().CreateFromLua("Assets/Lua/Prefabs/Blood_Vfx.lua", "Blood_Vfx_0");
        auto* vfxTransform = damageVfx->GetComponent<TransformComponent>(TypeOfComponent::TRANSFORM);
        if (vfxTransform) {
            TransformComponent* playerTrans = GetParentGameObject()->GetComponent<TransformComponent>(TypeOfComponent::TRANSFORM);
            vfxTransform->SetLocalPosition(playerTrans->GetPosition());
        }
        auto* vfxSprite = damageVfx->GetComponent<SpriteComponent>(TypeOfComponent::SPRITE);
        if (vfxSprite) {
            vfxSprite->SetFlipX(GetParentGameObject()->GetComponent<SpriteComponent>(TypeOfComponent::SPRITE)->GetFlipX());
        }
        auto* vfxRigidBody = damageVfx->GetComponent<RigidBodyComponent>(TypeOfComponent::RIGIDBODY);
        if (vfxSprite) {
            bool playerFacingLeft = GetParentGameObject()->GetComponent<SpriteComponent>(TypeOfComponent::SPRITE)->GetFlipX();
            vfxRigidBody->SetVelocity({ playerFacingLeft ? -vfxRigidBody->GetVelocity().x : vfxRigidBody->GetVelocity().x,0.f });
        }

        DespawnManager::ScheduleDespawn(damageVfx, 1.f);
        int randomAudioID = 11 + (std::rand() % 2); // Generate either 11 or 12

        auto* playerAudio = GetParentGameObject()->GetComponent<AudioComponent>(TypeOfComponent::AUDIO);
        if (playerAudio) {
            playerAudio->PlayAudio(randomAudioID);
        }
        else {
            ImGuiConsole::Cout("Error: AudioComponent is null!");
        }
    }

    if (Engine::GetInstance().isGodMode)
    {
        SpawnDamagePopup(damage * 2);
    }
    else
    {
        SpawnDamagePopup(damage);
    }
#ifdef _LOGGING
	ImGuiConsole::Cout("Health: %d / %d", health, maxHealth);
#endif // _LOGGING
}

// Get the current health
int HealthComponent::GetHealth() const {
    return health;
}

void HealthComponent::SpawnDamagePopup(int damage)
{
    GameObject* parent = GetParentGameObject();
    if (!parent) return;
    if (damage <= 0) return;

    AudioManager::GetInstance().PlayAudio(4); //EnemyHit
    AudioManager::GetInstance().SetChannelVolume(4, 0.5f);

    Vector2 collisionPoint = parent->GetComponent<TransformComponent>(TypeOfComponent::TRANSFORM)->GetLocalPosition();
    GameObjectFactory& factory = GameObjectFactory::GetInstance();

    GameObject* damageText = factory.CreateFromLua("Assets/Lua/Prefabs/GameDmgIndicatorText.lua", "GameDmgIndicatorText_0");

    auto* textTransform = damageText->GetComponent<TransformComponent>(TypeOfComponent::TRANSFORM);
    if (textTransform) {
        textTransform->SetLocalPosition(collisionPoint);
    }

    auto* textComponent = damageText->GetComponent<TextComponent>(TypeOfComponent::TEXT);
    if (textComponent) {
        textComponent->SetText(std::to_string(damage));
        textComponent->SetFont("sleepySans", 0.75f);
        textComponent->SetRGB(Vector3(1.0f, 0.2f, 0.2f));
        textComponent->SetAlpha(1.0f);
        textComponent->SetPosition(collisionPoint);
    }

    damageText->AddComponent<FloatUpComponent>(TypeOfComponent::FLOATUP);
    auto* floatComp = damageText->GetComponent<FloatUpComponent>(TypeOfComponent::FLOATUP);
    if (floatComp) {
        floatComp->SetSpeed(50.0f);
        floatComp->SetLifetime(1.5f);
    }

    DespawnManager::ScheduleDespawn(damageText, 1.5f);

    // VFX
    GameObject* damageVfx = factory.CreateFromLua("Assets/Lua/Prefabs/Hit_Vfx.lua", "Hit_Vfx_0");
    auto* vfxTransform = damageVfx->GetComponent<TransformComponent>(TypeOfComponent::TRANSFORM);
    if (vfxTransform) {
        vfxTransform->SetLocalPosition(collisionPoint);
    }
    auto* vfxParticle = damageVfx->GetComponent<ParticleSystem>(TypeOfComponent::PARTICLE);
    DespawnManager::ScheduleDespawn(damageVfx, vfxParticle->GetLifetime());
}


/*!
\brief Checks if the object is alive.
\return True if health is greater than 0, false otherwise.
*/
bool HealthComponent::IsAlive() const {
    return health > 0;
}

/*!
\brief Updates the component each frame. Handles health state, death callbacks,
       and visual feedback for damage.
*/
void HealthComponent::Update() {
    if (damageCooldownTimer > 0.0f) {
        damageCooldownTimer -= (float)InputManager::deltaTime;
    }

    if (!GetActive())
        return;

    if (health <= 0 && !isDespawned) {
        isDespawned = true;  // Ensure despawn happens only once
		EventSystem::GetInstance().AddDeathEvent(*GetParentGameObject());

        if (onDeathCallback) {
            onDeathCallback();  // Trigger death callback first
        }
        TriggerDespawn();  // Call despawn last to prevent accessing invalid memory
    }

    //For SFX only, temporarily solution to despawn the VFX fast
    if (GetParentGameObject()->GetLayer() == "VFX")
    {
        if (!isHit)
        {
            isHit = true;
        }
        health -= 10;
    }

    if (turnRed)
    {
        GameObject* parent = GetParentGameObject();
        if (parent)
        {
            auto* sprite = parent->GetComponent<SpriteComponent>(TypeOfComponent::SPRITE);
            auto* anim = GetParentGameObject()->GetComponent<AnimatorComponent>(TypeOfComponent::ANIMATOR);

            if (sprite)
            {
                if (anim)
                {
                    anim->animationController->SetParameter("IsDamaged", true);
                }

                sprite->SetRGB(Vector4(1.f, 0.f, 0.f, 1.f));

            }

            if (colorTimer > 0.f)
            {
                colorTimer -= (float)InputManager::GetDeltaTime();
            }
            else
            {
                turnRed = false;
                sprite->SetRGB(Vector4(1.f, 1.f, 1.f, 1.f)); //Return to original color
                if (anim)
                {
                    anim->animationController->SetParameter("IsDamaged", false);
                }

            }
        }

    }
}

/*!
\brief Queues the parent GameObject for despawning.
*/
void HealthComponent::TriggerDespawn() {
    GameObject* parent = GetParentGameObject();

    if (parent) {
        //ImGuiConsole::Cout("Despawning GO = " << parent->GetName());
        GameObjectFactory::GetInstance().QueueDespawn(parent);

        //Death SFX
        int deathSFX = -1;
        float sfxControl = 0.1f;
        //Slime
        if (parent->GetName() == "BabyEnemy" || parent->GetName() == "Light_Enemy")
        {
            deathSFX = 33;

        }
        else if (parent->GetName() == "Heavy_Enemy")
        {
            deathSFX = 37 + (std::rand() % 2); // Death SFX 37 & 38
        }
        else if (parent->GetName() == "Bomb_Enemy")
        {
            deathSFX = 39 + (std::rand() % 2); // Death SFX 39 & 40
            sfxControl = 0.4f;
        }

        if (deathSFX != -1)
        {
            AudioManager::GetInstance().PlayAudio(deathSFX);
            AudioManager::GetInstance().SetChannelVolume(deathSFX, sfxControl);
        }
    }
}

/*!
\brief Serializes health data to a Lua file.
\param luaFilePath The path to the Lua file.
\param tableName The name of the table to write data into.
*/
void HealthComponent::Serialize(const std::string& luaFilePath, const std::string& tableName) {
    LuaManager luaManager(luaFilePath);  // Create a LuaManager instance for managing the Lua file.

    std::vector<std::string> keys;
    LuaManager::LuaValueContainer values;

    // Add health and maxHealth values to the container.
    keys.push_back("health");
    values.push_back(health);

    keys.push_back("maxHealth");
    values.push_back(maxHealth);

    // Write these values into the Lua file under the provided table name.
    luaManager.LuaWrite(tableName, values, keys, "Health");
}



/*!
\brief Deserializes health data from a Lua file.
\param luaFilePath The path to the Lua file.
\param tableName The name of the table to read data from.
*/
void HealthComponent::Deserialize(const std::string& luaFilePath, const std::string& tableName) {
    LuaManager luaManager(luaFilePath);  // Create a LuaManager instance for managing the Lua file.

    // Read the health and maxHealth values from the Lua file.
    health = luaManager.LuaRead<int>(tableName, { "Health", "health" });
    maxHealth = luaManager.LuaRead<int>(tableName, { "Health", "maxHealth" });

    // You could add additional checks or logic here if needed.
}

/*!
\brief Generates a string for debugging health information.
\return A string containing current and maximum health.
*/
std::string HealthComponent::DebugInfo() const {
    return "Health: " + std::to_string(health) + "/" + std::to_string(maxHealth);
}

/*!
\brief Sets the maximum health value and adjusts current health if necessary.
\param newMaxHealth The new maximum health value.
*/
void HealthComponent::SetMaxHealth(int newMaxHealth) {
    maxHealth = newMaxHealth;
    if (health > maxHealth) {
        health = maxHealth; // Adjust health if it exceeds max health
    }
}

/*!
\brief Retrieves the maximum health value.
\return The maximum health.
*/
int HealthComponent::GetMaxHealth() const {
    return maxHealth;
}