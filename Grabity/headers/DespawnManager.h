/*!****************************************************************
\file: DespawnManager.h
\author: Lee Yu Jie Brandon, l.yujiebrandon, 2301232
\brief: Singleton manager for scheduling game objects to be automatically 
        despawned after a specified time delay. Handles tracking and 
        cleanup of objects marked for delayed removal from the game.

Copyright (C) 2025 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents
without the prior written consent of DigiPen Institute of
Technology is prohibited.
*******************************************************************!*/


#pragma once
#include <vector>
#include <utility>
#include "GameObject.h"
#include "Engine.h"

class DespawnManager {
private:
    struct DespawnEntry {
        GameObject* object;
        float timeRemaining;
    };

    std::vector<DespawnEntry> pendingDespawns;

    // Private constructor for singleton
    DespawnManager() = default;
    // Delete copy constructor and assignment operator
    DespawnManager(const DespawnManager&) = delete;
    DespawnManager& operator=(const DespawnManager&) = delete;

public:
    // Singleton access
    static DespawnManager& GetInstance() {
        static DespawnManager instance;
        return instance;
    }

    // Schedule a game object to be despawned after the specified delay
    static void ScheduleDespawn(GameObject* obj, float delay) {
        DespawnEntry entry;
        entry.object = obj;
        entry.timeRemaining = delay;
        GetInstance().pendingDespawns.push_back(entry);
    }

    // Update method to be called each frame
    void Update() {
        float deltaTime = static_cast<float>(Engine::GetInstance().fixedDT);

        // Update all timers and despawn as needed
        auto it = pendingDespawns.begin();
        while (it != pendingDespawns.end()) {
            it->timeRemaining -= deltaTime;

            if (it->timeRemaining <= 0.0f) {
                GameObjectFactory::GetInstance().Despawn(it->object);
                it = pendingDespawns.erase(it);
            }
            else {
                ++it;
            }
        }
    }
};