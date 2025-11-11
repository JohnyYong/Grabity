/*!****************************************************************
\file: SpawnerComponent.cpp
\author: Jeremy Lim Ting Jie, jeremytingjie.lim, 2301370
\brief: The SpawnerComponent class handles the spawning of enemies at 
        random intervals, spawn intervals, and enemy types

Copyright (C) 2024 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents
without the prior written consent of DigiPen Institute of
Technology is prohibited.
*******************************************************************!*/
#include "SpawnerComponent.h"
#include "GameObjectFactory.h"
#include "AIStateMachineComponent.h"

#ifdef _IMGUI
#include <iostream>
#endif // _IMGUI

/**
 * @brief Constructs a SpawnerComponent with default spawn intervals and enemy types.
 *
 * Initializes the spawn intervals and the list of enemy types. The random engine is also
 * set up to handle spawn timings and enemy selection.
 *
 * @param parent The GameObject this component is attached to.
 * @param minInterval The minimum time interval between spawns (in seconds).
 * @param maxInterval The maximum time interval between spawns (in seconds).
 */
SpawnerComponent::SpawnerComponent(GameObject* parent, float minInterval, float maxInterval)
    : Component(parent), minInterval(minInterval), maxInterval(maxInterval), spawnTimer(0.0f), nextSpawnTime(0.0f), firstSpawn(true) {

    // Seed the random number generator
    rng.seed(static_cast<unsigned>(std::chrono::system_clock::now().time_since_epoch().count()));
    intervalDist = std::uniform_real_distribution<float>(minInterval, maxInterval);

    SetEnemyTypes({ "Light_Enemy", "Heavy_Enemy", "Bomb_Enemy" });
}

/**
 * @brief Sets the spawn interval range for enemy spawns.
 *
 * @param minInterval The minimum time interval between spawns (in seconds).
 * @param maxInterval The maximum time interval between spawns (in seconds).
 */
void SpawnerComponent::SetSpawnInterval(float newMinInterval, float newMaxInterval) {
    minInterval = newMinInterval;
    maxInterval = newMaxInterval;

    // Update the random distribution with the new range
    intervalDist = std::uniform_real_distribution<float>(minInterval, maxInterval);
    if (!firstSpawn) {
        nextSpawnTime = intervalDist(rng);
    }
}

/**
 * @brief Sets the types of enemies to be spawned, along with their weights for distribution.
 *
 * This method takes a list of enemy types and assigns weights to each type.
 * The weight is higher for "Light_Enemy" types, which increases the chance of spawning them.
 *
 * @param types A vector of strings representing the enemy types that the spawner will handle.
 */
void SpawnerComponent::SetEnemyTypes(const std::vector<std::string>& types) {
    enemyTypes.reserve(types.size());
    enemyTypes = types;

    weights.clear();
    weights.resize(types.size(), 1);
    enemyTypeDist = std::discrete_distribution<int>(weights.begin(), weights.end());
}

/**
 * @brief Updates the spawner to manage enemy spawning over time.
 *
 * This method is called each frame to handle the spawn timer. When the spawn timer exceeds the
 * designated interval, an enemy is spawned, and the timer is reset. The next spawn time is chosen
 * randomly based on a distribution.
 */
void SpawnerComponent::Update() {
    //spawnTimer += static_cast<float>(InputManager::deltaTime);
	spawnTimer += static_cast<float>(Engine::GetInstance().fixedDT);

    if (spawnTimer >= nextSpawnTime) {
        SpawnEnemy(); // Spawn an enemy
        spawnTimer = 0.0f; // Reset timer
        if (firstSpawn) {
            firstSpawn = false;
            nextSpawnTime = intervalDist(rng); // Set the next spawn time using random distribution
        }
        else {
            nextSpawnTime = intervalDist(rng); // Set the next spawn time
        }
    }
}

/**
 * @brief Spawns an enemy at a random or predefined location within the spawn area.
 *
 * This function retrieves the spawn area boundaries from the top and lower border
 * objects, selects an enemy type using weighted distribution, and determines a spawn
 * position. If predefined spawn points exist, it selects one randomly; otherwise,
 * it generates a random position while avoiding the player's vicinity. The enemy is
 * instantiated from a Lua prefab file, and necessary components such as health and AI
 * are configured.
 */
void SpawnerComponent::SpawnEnemy() {
    // Check if there are any enemy types to spawn
    if (enemyTypes.empty()) return;

    // Get the player object and its transform component
    GameObject* player = GameObjectFactory::GetInstance().GetPlayerObject();
    if (!player) return;

    TransformComponent* playerTransform = player->GetComponent<TransformComponent>(TRANSFORM);
    if (!playerTransform) return;

    // Get the player's position
    Vector2 playerPos = playerTransform->GetLocalPosition();

    // Find the border objects to determine spawn sides
    GameObject* rightBorder = GameObjectFactory::GetInstance().FindGameObjectsByTag("RightBorder").front();
    GameObject* leftBorder = GameObjectFactory::GetInstance().FindGameObjectsByTag("LeftBorder").front();

    // Check if border objects are found
    if (!rightBorder || !leftBorder) {
        std::cerr << "Error: Border objects not found. Cannot spawn enemies." << std::endl;
        return;
    }

    // Get the transform components of the border objects
    TransformComponent* rightBorderTransform = rightBorder->GetComponent<TransformComponent>(TRANSFORM);
    TransformComponent* leftBorderTransform = leftBorder->GetComponent<TransformComponent>(TRANSFORM);

    if (!rightBorderTransform || !leftBorderTransform) return;

    // Get the positions of the border objects
    float rightBorderX = rightBorderTransform->GetLocalPosition().x;
    float leftBorderX = leftBorderTransform->GetLocalPosition().x;

    // Determine the spawn position on the opposite side of the player
    Vector2 spawnPosition = (abs(rightBorderX - playerPos.x) < abs(playerPos.x - leftBorderX)) ?
        Vector2{ leftBorderX * 0.5f, 0 } :
        Vector2{ rightBorderX * 0.5f, 0 };

    // Randomly select an enemy type using weighted distribution
    std::string enemyType = enemyTypes[enemyTypeDist(rng)];
    std::string prefabPath = "Assets/Lua/Prefabs/" + enemyType + ".lua";
    enemyType += "_0";

    // Create a new GameObject using the prefab path and enemy type
    GameObject* newGO = GameObjectFactory::GetInstance().CreateFromLua(prefabPath, enemyType);
    if (!newGO) return;

    // Set the position of the new GameObject
    TransformComponent* newGOTransform = newGO->GetComponent<TransformComponent>(TRANSFORM);
    if (newGOTransform) {
        newGOTransform->SetLocalPosition(spawnPosition);
    }

    // Determine health values based on enemy type
    int initialHealth = 0;
    int maxHealth = 0;

    if (enemyType.find("Light_Enemy") != std::string::npos) {
        initialHealth = 20;
        maxHealth = 20;
    }
    else if (enemyType.find("Heavy_Enemy") != std::string::npos) {
        initialHealth = 50;
        maxHealth = 50;
    }
    else if (enemyType.find("Bomb_Enemy") != std::string::npos) {
        initialHealth = 20;
        maxHealth = 20;
    }

    // Add the HealthComponent to the enemy GameObject with the correct values
    newGO->AddComponent<HealthComponent>(TypeOfComponent::HEALTH, initialHealth, maxHealth);

    // Configure AI behavior
    AIStateMachineComponent* aiComponent = newGO->GetComponent<AIStateMachineComponent>(TypeOfComponent::AISTATE);
    if (aiComponent) {
        aiComponent->SetState("CHASE");
        aiComponent->SetChaseTarget(player);
        aiComponent->SetMoveSpeed(100.0f);
    }
}

/**
 * @brief Sets predefined spawn points for enemy spawning.
 *
 * @param points A vector of `Vector2` positions defining potential spawn locations.
 * If set, enemies will spawn only at these locations instead of a random position.
 */
void SpawnerComponent::SetSpawnPoints(const std::vector<Vector2>& points) {
    spawnPoints = points;
}


/**
 * @brief Serializes the spawner component's data to a Lua file.
 *
 * Stores properties such as spawn intervals and the next spawn time in a Lua table.
 *
 * @param luaFilePath The file path of the Lua script to write data into.
 * @param tableName The name of the Lua table in which to store the component data.
 */
void SpawnerComponent::Serialize(const std::string& luaFilePath, const std::string& tableName) {
    LuaManager luaManager(luaFilePath);

    LuaManager::LuaValueContainer values;
    std::vector<std::string> keys;

    keys.push_back("minInterval");
    values.push_back(minInterval);

    keys.push_back("maxInterval");
    values.push_back(maxInterval);

    keys.push_back("nextSpawnTime");
    values.push_back(nextSpawnTime);
    keys.push_back("firstSpawn");
    values.push_back(firstSpawn);
    luaManager.LuaWrite(tableName, values, keys, "Spawner");
}

/**
 * @brief Deserializes spawner properties from a Lua file.
 *
 * Reads spawn intervals and timing values from a Lua script to restore the spawner's state.
 * Updates the random interval distribution to match the loaded values.
 *
 * @param luaFilePath The file path of the Lua script to read from.
 * @param tableName The name of the Lua table containing the component data.
 */
void SpawnerComponent::Deserialize(const std::string& luaFilePath, const std::string& tableName) {
    LuaManager luaManager(luaFilePath);

    minInterval = luaManager.LuaRead<float>(tableName, { "Spawner", "minInterval" });
    maxInterval = luaManager.LuaRead<float>(tableName, { "Spawner", "maxInterval" });
    nextSpawnTime = luaManager.LuaRead<float>(tableName, { "Spawner", "nextSpawnTime" });
    firstSpawn = luaManager.LuaRead<float>(tableName, { "Spawner", "firstSpawn" });
    // Update the random interval distribution
    intervalDist = std::uniform_real_distribution<float>(minInterval, maxInterval);
}

/**
 * @brief Generates a debug string containing the spawner's current state.
 *
 * This includes spawn interval ranges, the next scheduled spawn time, and the list
 * of enemy types available for spawning.
 *
 * @return A formatted string with the spawner's debug information.
 */
std::string SpawnerComponent::DebugInfo() const {
    std::string debugInfo = "SpawnerComponent Info:\n";
    debugInfo += "Spawn Interval: " + std::to_string(minInterval) + " - " + std::to_string(maxInterval) + "\n";
    debugInfo += "Next Spawn Time: " + std::to_string(nextSpawnTime) + "\n";
    debugInfo += "Enemy Types: ";
    for (const auto& type : enemyTypes) {
        debugInfo += type + " ";
    }
    debugInfo += "\n";
    return debugInfo;
}
