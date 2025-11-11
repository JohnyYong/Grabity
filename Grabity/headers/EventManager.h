#include <queue>
#include <functional>
#include <chrono>
#include <thread>
#include <mutex>
#include "Engine.h"
#include "ImGuiConsole.h"

enum class EventType {
    SpawnEnemies
};

struct GameEvent {
    EventType type;
    int numEnemies;
    float startTime;
    bool cleared;
    std::function<void()> callback;

    // Constructor for convenience
    GameEvent(EventType t, int num, float start, std::function<void()> cb)
        : type(t), numEnemies(num), startTime(start), cleared(false) {
    }
};

// Comparator for priority queue (earliest event has the highest priority)
struct EventCompare {
    bool operator()(const GameEvent& a, const GameEvent& b) const {
        return a.startTime > b.startTime;
    }
};

class EventManager {
public:
    EventManager() {
        workerThread = std::thread([this] {
            while (running) {
                ProcessEvents();
                // Sleep briefly; adjust as necessary
                std::this_thread::sleep_for(std::chrono::milliseconds(1));
            }
            });
    }

    ~EventManager() {
        running = false;
        if (workerThread.joinable())
            workerThread.join();
    }

    // Add an event to the queue
    void AddEvent(const GameEvent& event) {
        std::lock_guard<std::mutex> lock(queueMutex);
        eventQueue.push(event);
    }

private:
    std::priority_queue<GameEvent, std::vector<GameEvent>, EventCompare> eventQueue;
    std::mutex queueMutex;
    bool running = true;
    std::thread workerThread;

    void ProcessEvents() {
        std::lock_guard<std::mutex> lock(queueMutex);
        if (!eventQueue.empty()) {
            float currTime = static_cast<float>(glfwGetTime());
            GameEvent e = eventQueue.top();
            if (currTime >= e.startTime && !e.cleared) {
                eventQueue.pop();
				ImGuiConsole::Cout("Processing event: %d enemies to spawn", e.numEnemies);
            }
        }
    }
};
