/*!****************************************************************
\file: HealthComponent.h
\author: Jeremy Lim Ting Jie, jeremytingjie.lim, 2301370
\co-author: Johny Yong Jun Siang, j.yong, 2301301
\brief: Defines the HealthComponent class for managing health-related functionality in a game object.
        Johny completed (20%) of the code: Adding color turning check
        Jeremy completed (80%) the rest of the features

Copyright (C) 2024 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents
without the prior written consent of DigiPen Institute of
Technology is prohibited.
*******************************************************************!*/
#ifndef HEALTH_COMPONENT_H
#define HEALTH_COMPONENT_H

#include "Component.h"
#include <functional>
#include "LuaConfig.h"
#include "GameObjectFactory.h"

/*!
\class HealthComponent
\brief A component for managing the health of a game object.

This component tracks the current and maximum health of a game object,
handles damage, triggers death callbacks, and integrates with other
components for visual feedback and game logic.
*/
class HealthComponent : public Component {
private:
    int health;                //!< Current health of the game object.
    int maxHealth;             //!< Maximum allowable health of the game object.
    std::function<void()> onDeathCallback; //!< Callback function triggered when health reaches zero.

    float damageCooldownTimer = 0.0f; // Time left before we can take damage again
    float damageCooldownDuration = 2.f; // Half a second cooldown (adjust as needed)


public:
    /*!
    \brief Constructs a `HealthComponent` with specified initial and maximum health.
    \param parent Pointer to the parent game object.
    \param initialHealth The starting health value (default: 50).
    \param maxHealthValue The maximum health value (default: 50).
    */
    HealthComponent(GameObject* parent, int initialHealth = 50, int maxHealthValue = 50);

    /*!
    \brief Default constructor.
    */
    HealthComponent();

    /*!
    \brief Attaches a callback function to be triggered when health reaches zero.
    \param callback The function to execute on death.
    */
    void SetOnDeathCallback(const std::function<void()>& callback);

    /*!
    \brief Reduces the health of the game object by a specified amount.
    \param damage The amount of damage to apply.
    */
    void TakeDamage(int damage);

    /*!
    \brief Gets the current health value.
    \return The current health of the game object.
    */
    int GetHealth() const;

    void SpawnDamagePopup(int damage);

    /*!
    \brief Checks if the game object is "alive" (i.e., health > 0).
    \return True if the object is alive, false otherwise.
    */
    bool IsAlive() const;

    /*!
    \brief Updates the health component, including damage color feedback and death handling.
    */
    void Update() override;

    /*!
    \brief Saves the health component's state to a Lua configuration file.
    \param luaFilePath The path to the Lua file.
    \param tableName The table name under which data will be stored.
    */
    void Serialize(const std::string& luaFilePath, const std::string& tableName) override;

    /*!
    \brief Loads the health component's state from a Lua configuration file.
    \param luaFilePath The path to the Lua file.
    \param tableName The table name from which data will be retrieved.
    */
    void Deserialize(const std::string& luaFilePath, const std::string& tableName) override;

    /*!
    \brief Provides debugging information about the health component.
    \return A string representing the current health status.
    */
    std::string DebugInfo() const override;

    /*!
    \brief Sets the maximum health value and adjusts current health if necessary.
    \param newMaxHealth The new maximum health value.
    */
    void SetMaxHealth(int newMaxHealth);

    /*!
    \brief Gets the maximum health value.
    \return The maximum health of the game object.
    */
    int GetMaxHealth() const;

    /*!
    \brief Directly sets the current health value.
    \param newHealth The new health value.
    */
    void SetHealth(int newHealth) { health = newHealth; }

    /*!
    \brief Triggers the despawning of the game object.
    */
    void TriggerDespawn();

    bool isDespawned = false;  //!< Indicates if the game object has been despawned.
    bool turnRed = false;      //!< Indicates if the object should display red color feedback for damage.
    float colorTimer = 0.f;    //!< Timer to control the duration of damage feedback.
    bool isHit = false;
};

#endif // HEALTH_COMPONENT_H