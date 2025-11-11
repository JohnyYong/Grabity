/*!****************************************************************
\file: EventSystem.h
\author: Ridhwan Afandi (mohamedridhwan.b)
\brief  Contains the declaration of the EventSystem class, which
        manages game events via GameEventComponent on a separate thread.
        The worker thread starts automatically upon instantiation and
        continuously processes events in the background. This file also
        provides functions to add, save, and load events.

Copyright (C) 2024 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents
without the prior written consent of DigiPen Institute of
Technology is prohibited.
*******************************************************************!*/
#pragma once
#ifndef _EVENT_SYSTEM_H_
#include <queue>
#include <chrono>
#include <thread>
#include <mutex>
#include <condition_variable>
#include "Engine.h"
#include "ImGuiConsole.h"
#include "GameObjectFactory.h"
#include "GameEventComponent.h"

// Comparator for priority queue (earliest event has the highest priority)
struct EventCompare {
    bool operator()(const GameEventComponent& a, const GameEventComponent& b) const {
        return a.startTime < b.startTime;
    }
};

/**
 * @class EventSystem
 * @brief Manages game events and missions, handling their storage, retrieval, and processing.
 */
class EventSystem {
public:
    EventSystem(const EventSystem&) = delete;
    EventSystem& operator=(const EventSystem&) = delete;

    static EventSystem& GetInstance();

    /**
     * @brief Shuts down the event system and cleans up resources.
     */
    static void ShutDown();

    /**
     * @brief Adds a game event to the event queue.
     *
     * @param event The GameEventComponent to be added to the event queue.
     */
    void AddEvent(const GameEventComponent& event);

    /**
     * @brief Adds a mission to the mission list.
     *
     * @param mission The GameMissionComponent to be added.
     */
    void AddMission(const GameMissionComponent& mission);

    /**
     * @brief Saves the current events and missions to a file.
     *
     * @param filename The file path where events should be saved. Default is "Assets/Lua/events.dat".
     */
    void SaveEvents(const std::string& filename = "Assets/Lua/events.dat");

    /**
     * @brief Loads game events and missions from a file.
     *
     * @param filename The file path from which to load events. Default is "Assets/Lua/events.dat".
     */
    void LoadEvents(const std::string& filename = "Assets/Lua/events.dat");

    /**
     * @brief Clears game events from the current event queue
     */
    void ClearEvents();


     /**
     * @brief Clears game missions from the current event queue
     */
    void ClearMissions();

    /**
     * @brief Adds a death event for a specified GameObject.
     *
     * @param GO The GameObject that has died.
     */
    void AddDeathEvent(const GameObject& GO);

    /**
     * @brief Retrieves the win condition state.
     *
     * @return True if the win condition is met, false otherwise.
     */
    bool GetWinCondition() { return WinCondition; }

    void SetWinCondition(bool state) { WinCondition = state; }
    /**
     * @brief Retrieves the event queue.
     *
     * @return A priority queue of GameEventComponent objects.
     */
    const std::priority_queue<GameEventComponent, std::vector<GameEventComponent>, EventCompare> GetEventQueue();

    /**
     * @brief Retrieves the mission vector.
     *
     * @return A reference to the vector containing all active missions.
     */
    const std::vector<GameMissionComponent>& GetMissionVector();


private:
    EventSystem();
    ~EventSystem();
private:
    static inline EventSystem* evtSystem_Instance = nullptr;
    std::priority_queue<GameEventComponent, std::vector<GameEventComponent>, EventCompare> eventQueue;
	std::vector <GameMissionComponent> missionVector;
    std::mutex queueMutex;
    std::condition_variable cv;
	bool WinCondition = false;
    bool running = true;
    std::thread workerThread;

    void ProcessEvents();
};
#endif // _EVENT_SYSTEM_H_