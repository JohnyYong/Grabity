/*!****************************************************************
\file: EventSystem.cpp
\author: Ridhwan Afandi (mohamedridhwan.b)
\brief  Contains the implementation of the EventSystem class, which
        manages game events via GameEventComponent on a separate thread.
        The worker thread starts automatically upon instantiation and
        continuously processes events in the background. This file also
        provides functions to add, save, and load events.

Copyright (C) 2024 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents
without the prior written consent of DigiPen Institute of
Technology is prohibited.
*******************************************************************!*/

#include "EventSystem.h"
#include <fstream>
#include <filesystem>
#include "UISystem.h"

EventSystem& EventSystem::GetInstance() {
    if (!evtSystem_Instance)
        evtSystem_Instance = new EventSystem();
    return *evtSystem_Instance;
}


void EventSystem::ShutDown() {
    delete evtSystem_Instance;
    evtSystem_Instance = nullptr;
}

EventSystem::EventSystem() {
    workerThread = std::thread([this] {
        ProcessEvents();
        });
}

void EventSystem::AddEvent(const GameEventComponent& event) {
    {
        std::lock_guard<std::mutex> lock(queueMutex);
        eventQueue.push(event);
    }
    cv.notify_all(); // wake up the worker thread if it is waiting
}

void EventSystem::SaveEvents(const std::string& filename) {
    std::lock_guard<std::mutex> lock(queueMutex);
    std::ofstream ofs(filename, std::ios::binary);
    if (!ofs) {
#ifdef _LOGGING
        ImGuiConsole::Cout("Failed to open file %s for writing.", filename.c_str());
#endif
        return;
    }
	int size = static_cast<int>(eventQueue.size()) + static_cast<int>(missionVector.size());
    ofs.write(reinterpret_cast<char*>(&size), sizeof(int));

    // Copy the queue to iterate without modifying the original
    auto copy = eventQueue;
    while (!copy.empty()) {
        GameEventComponent event = copy.top();
        copy.pop();
        int typeInt = static_cast<int>(event.eventType);
        int enemyTypeInt = static_cast<int>(event.enemyType);
        ofs.write(reinterpret_cast<char*>(&typeInt), sizeof(int));
		ofs.write(reinterpret_cast<char*>(&enemyTypeInt), sizeof(int));
        ofs.write(reinterpret_cast<char*>(&event.numEnemies), sizeof(int));
        ofs.write(reinterpret_cast<char*>(&event.startTime), sizeof(float));
    }
    for (auto& mission : missionVector) {
        int typeInt = static_cast<int>(mission.eventType);
        int enemyTypeInt = static_cast<int>(mission.enemyType);
        ofs.write(reinterpret_cast<char*>(&typeInt), sizeof(int));
		ofs.write(reinterpret_cast<char*>(&enemyTypeInt), sizeof(int));
        ofs.write(reinterpret_cast<char*>(&mission.numEnemies), sizeof(int));
    }
}

void EventSystem::LoadEvents(const std::string& filename) {
    std::lock_guard<std::mutex> lock(queueMutex);

	WinCondition = false;

    if (!std::filesystem::exists(filename) || std::filesystem::is_empty(filename)) {
#ifdef _LOGGING
        ImGuiConsole::Cout("File %s does not exist or is empty.", filename.c_str());
#endif
        return;
    }

    std::ifstream ifs(filename, std::ios::binary);
    if (!ifs) {
#ifdef _LOGGING
        ImGuiConsole::Cout("Failed to open file %s for reading.", filename.c_str());
#endif
        return;
    }
    int size = 0;
    ifs.read(reinterpret_cast<char*>(&size), sizeof(int));

    // Clear current events before loading new ones
    while (!eventQueue.empty()) {
        eventQueue.pop();
    }
    missionVector.clear();


    for (int i = 0; i < size; ++i) {
        int typeInt = 0;
        ifs.read(reinterpret_cast<char*>(&typeInt), sizeof(int));
		if (typeInt == static_cast<int>(EventType::Mission)) {
			int enemyTypeInt = 0;
            int numEnemies = 0;
			ifs.read(reinterpret_cast<char*>(&enemyTypeInt), sizeof(int));
			ifs.read(reinterpret_cast<char*>(&numEnemies), sizeof(int));
			GameMissionComponent mission(static_cast<EnemyType>(enemyTypeInt), numEnemies);
			missionVector.push_back(mission);
			continue;
		}
        else if (typeInt == static_cast<int>(EventType::SpawnEnemies)) {
            int enemyTypeInt = 0;
            int numEnemies = 0;
            float startTime = 0.0f;
            ifs.read(reinterpret_cast<char*>(&enemyTypeInt), sizeof(int));
            ifs.read(reinterpret_cast<char*>(&numEnemies), sizeof(int));
            ifs.read(reinterpret_cast<char*>(&startTime), sizeof(float));
            GameEventComponent event(static_cast<EnemyType>(enemyTypeInt), numEnemies, startTime);
            eventQueue.push(event);
        }
    }
    cv.notify_all(); // wake up the processing thread if necessary
}

void EventSystem::ClearEvents() {
    while (!eventQueue.empty())
    {
        eventQueue.pop();
    }
}

void EventSystem::ClearMissions() {
	missionVector.clear();
}


void EventSystem::AddMission(const GameMissionComponent& mission) {
	{
		std::lock_guard<std::mutex> lock(queueMutex);
		missionVector.push_back(mission);
	}
	cv.notify_all(); // wake up the worker thread if it is waiting
}

void EventSystem::AddDeathEvent(const GameObject& GO) {
    if (GO.GetName() == "Heavy_Enemy") {
        for (auto& mission : missionVector) {
            if (mission.enemyType == EnemyType::Heavy) {
                --mission.numEnemies;
                TextChange(mission.totalEnemies - mission.numEnemies, "TextChangeSkeleton", mission.totalEnemies);
                if (mission.numEnemies <= 0) {
                    mission.isCompleted = true;
                    // popup message
                    SetGreen(EnemyType::Heavy);
                }
            }
        }
    }
    else if (GO.GetName() == "Light_Enemy") {
        for (auto& mission : missionVector) {
            if (mission.enemyType == EnemyType::Light) {
                --mission.numEnemies;
                TextChange( mission.totalEnemies - mission.numEnemies, "TextChangeSlime", mission.totalEnemies);
                if (mission.numEnemies <= 0) {
                    mission.isCompleted = true;
                    // popup message
                    SetGreen(EnemyType::Light);
                }
            }
        }
    }
    else if (GO.GetName().starts_with("BabyEnemy"))
    {
        for (auto& mission : missionVector) {
            if (mission.enemyType == EnemyType::Light) {
                --mission.numEnemies;
                TextChange(mission.totalEnemies - mission.numEnemies, "TextChangeSlime", mission.totalEnemies);
                if (mission.numEnemies <= 0) {
                    mission.isCompleted = true;
                    // popup message
                    SetGreen(EnemyType::Light);
                }
            }
        }
    }
    else if (GO.GetName() == "Bomb_Enemy" || GO.GetName() == "ExplosionVFX") {
        for (auto& mission : missionVector) {
            if (mission.enemyType == EnemyType::Bomb) {
                --mission.numEnemies;
                TextChange(mission.totalEnemies - mission.numEnemies, "TextChangeBomb", mission.totalEnemies);
                if (mission.numEnemies <= 0) {
                    mission.isCompleted = true;
                    // popup message
                    SetGreen(EnemyType::Bomb);
                }
            }
        }
    }

    for (auto& mission : missionVector) {
        if (!mission.isCompleted)
            return;
    }
	// If all missions are completed
	WinCondition = true;
}


const std::priority_queue<GameEventComponent, std::vector<GameEventComponent>, EventCompare> EventSystem::GetEventQueue() {
    std::lock_guard<std::mutex> lock(queueMutex);
    return eventQueue;
}

const std::vector<GameMissionComponent>& EventSystem::GetMissionVector() {
	std::lock_guard<std::mutex> lock(queueMutex);
	return missionVector;
}

EventSystem::~EventSystem() {
    {
        std::lock_guard<std::mutex> lock(queueMutex);
        running = false;
        cv.notify_all();
    }
    if (workerThread.joinable())
        workerThread.join();
}

void EventSystem::ProcessEvents() {
    std::unique_lock<std::mutex> lock(queueMutex);
    while (running) {

        // If engine is not in the game scene or is paused, wait a while.
        if (eventQueue.empty() || !Engine::GetInstance().isInGameScene || Engine::GetInstance().isPaused) {
			cv.wait_for(lock, std::chrono::seconds(1));
            continue; // Recheck conditions after the wait
        }

        // Get the event at the top (earliest scheduled)
        const GameEventComponent& nextEvent = eventQueue.top();
        float currTime = Engine::GetInstance().time;


        // If the event is not due yet, wait abit
        if (currTime > nextEvent.startTime) {
			float waitTime = currTime - nextEvent.startTime;
			cv.wait_for(lock, std::chrono::seconds(static_cast<int>(waitTime)));
            continue; // Recheck conditions after the wait
        }
        else {
            //TriggerUpDown();
            GameEventComponent e = nextEvent;
            eventQueue.pop();
            lock.unlock();
#ifdef _LOGGING
            ImGuiConsole::Cout("Processing event: %d enemies to spawn", e.numEnemies);
#endif
            GameObjectFactory& factory = GameObjectFactory::GetInstance();
            auto gameObjects = factory.GetAllGameObjects();
            GameObject* player = factory.GetPlayerObject();
			Vector2 playerPos = player->GetComponent<TransformComponent>(TRANSFORM)->GetLocalPosition();

			// Check if player is on the right or left side of the screen
            float rightBorder = 1997.0f;
            float leftBorder = -848.f;
            float distRight = rightBorder - playerPos.x;
			float distLeft = playerPos.x - leftBorder;

			// Spawn enemies on the side opposite to the player
			Vector2 spawnPosition;
			if (abs(distRight) < abs(distLeft)) {
				spawnPosition = Vector2{ leftBorder , 0.f };
			}
			else {
				spawnPosition = Vector2{ rightBorder , 0.f };
			}

            // check type here
			if (e.enemyType == EnemyType::Light) {
				for (int i = 0; i < e.numEnemies; i++)
				{
					GameObject* GO = factory.CreateFromLua("Assets/Lua/Prefabs/Light_Enemy.lua", "Light_Enemy_0");
					GO->GetComponent<TransformComponent>(TRANSFORM)->SetLocalPosition(spawnPosition);
                    GO->GetComponent<AIStateMachineComponent>(AISTATE)->SetState("CHASE");
				}
			}
            else if (e.enemyType == EnemyType::Heavy) {
                for (int i = 0; i < e.numEnemies; i++)
                {
                    GameObject* GO = factory.CreateFromLua("Assets/Lua/Prefabs/Heavy_Enemy.lua", "Heavy_Enemy_0");
                    // Get the player object to set as the chase target
                    GameObject* playerOBJ = factory.GetPlayerObject();

                    // Configure AI behavior
                    if (playerOBJ && GO) {
                        AIStateMachineComponent* aiComponent = GO->GetComponent<AIStateMachineComponent>(TypeOfComponent::AISTATE);
                        GO->GetComponent<TransformComponent>(TRANSFORM)->SetLocalPosition(spawnPosition);
                        if (aiComponent) {
                            aiComponent->SetState("CHASE");
                            aiComponent->SetChaseTarget(playerOBJ);
                            aiComponent->SetMoveSpeed(100.0f);
                        }
                    }
                }
			}
			else if (e.enemyType == EnemyType::Bomb) {
				for (int i = 0; i < e.numEnemies; i++)
				{
					GameObject* GO = factory.CreateFromLua("Assets/Lua/Prefabs/Bomb_Enemy.lua", "Bomb_Enemy_0");
                    // Get the player object to set as the chase target
                    GameObject* playerOBJ = factory.GetPlayerObject();

                    // Configure AI behavior
                    if (playerOBJ && GO) {
                        AIStateMachineComponent* aiComponent = GO->GetComponent<AIStateMachineComponent>(TypeOfComponent::AISTATE);
                        GO->GetComponent<TransformComponent>(TRANSFORM)->SetLocalPosition(spawnPosition);
                        if (aiComponent) {
                            aiComponent->SetState("CHASE");
                            aiComponent->SetChaseTarget(playerOBJ);
                        }
                    }
				}
			}


            lock.lock();
        }

    }
}
