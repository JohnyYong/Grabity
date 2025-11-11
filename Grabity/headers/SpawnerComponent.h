/*!****************************************************************
\file: SpawnerComponent.h
\author: Jeremy Lim Ting Jie, jeremytingjie.lim, 2301370
\brief: Header file for the spawner component with getters and setters

Copyright (C) 2024 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents
without the prior written consent of DigiPen Institute of
Technology is prohibited.
*******************************************************************!*/

#pragma once
#include "Component.h"
#include <random>
#include <chrono>
#include "Maths.h"

/**
 * @class SpawnerComponent
 * @brief A component responsible for spawning enemies at random intervals and positions.
 *
 * The SpawnerComponent manages the spawning of enemies in a game. It allows for configuration
 * of spawn intervals, enemy types, and spawn points. The component also handles the logic
 * to spawn enemies at randomized intervals and specific spawn locations.
 */
class SpawnerComponent : public Component {
public:
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
    SpawnerComponent(GameObject* parent, float minInterval = 5.0f, float maxInterval = 7.0f);

    // Configuration methods
    void SetSpawnInterval(float newMinInterval, float newMaxInterval);
    void SetEnemyTypes(const std::vector<std::string>& types);
    void SetSpawnPoints(const std::vector<Vector2>& points); // Set predefined spawn points

    // Getters defined inline
    float GetMinInterval() const { return minInterval; } ///< Get the minimum spawn interval
    float GetMaxInterval() const { return maxInterval; } ///< Get the maximum spawn interval
    float GetNextSpawnTime() const { return nextSpawnTime; } ///< Get the next spawn time
    std::vector<std::string>& GetEnemyTypes() {
        return enemyTypes;
    }

    std::vector<std::string> GetAllAvailableEnemyTypes() const {
        return { "Light_Enemy", "Heavy_Enemy" , "Bomb_Enemy" };
    }

    std::vector<Vector2>& GetSpawnPoints() { return spawnPoints; } ///< Get the spawn points

    // Overridden methods from Component
    void Update() override;
    void Serialize(const std::string& luaFilePath, const std::string& tableName) override;
    void Deserialize(const std::string& luaFilePath, const std::string& tableName) override;
    std::string DebugInfo() const override;

private:
    void SpawnEnemy(); // Method to spawn an enemy

    std::vector<int> weights;
    std::vector<std::string> enemyTypes; // List of enemy types (e.g., "Light_Enemy", "Heavy_Enemy")
    float minInterval;                   // Minimum spawn interval
    float maxInterval;                   // Maximum spawn interval
    float spawnTimer = 0;                    // Tracks time since the last spawn
    float nextSpawnTime = 0;                 // Randomly determined time for the next spawn
    bool firstSpawn = false;
    // Random number generation
    std::default_random_engine rng;
    std::uniform_real_distribution<float> intervalDist;
    std::discrete_distribution<int> enemyTypeDist;
    std::vector<Vector2> spawnPoints; // Predefined spawn points
};
