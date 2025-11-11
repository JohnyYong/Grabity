/*!****************************************************************
\file: GameObjectFactory.h
\author: Johny Yong Jun Siang, j.yong, 2301301
\co-author: Goh Jun Jie, g.junjie, 2301293
\brief: Declares the GameObjectFactory class, a singleton that manages the creation, serialization, and lifecycle of 
        GameObjects within the game. It provides functions to spawn game objects from templates or from de-serialization, manage 
        object pools, and handle their updates and destruction. The factory also supports ID-based management and 
        recycling of GameObject instances.

        Johny created the file and all the functions outside of CreateFromLua (98%)
        Jun Jie declared the CreateFromLua function (20%)

Copyright (C) 2024 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents
without the prior written consent of DigiPen Institute of
Technology is prohibited.
*******************************************************************!*/
#pragma once
#include <vector>
#include "GameObject.h"
#include "ObjectPool.h"
#include "PlayerControllerComponent.h"

class GameObjectFactory {
public:
    /**
     * @brief Retrieves the singleton instance of the GameObjectFactory.
     * @return A reference to the GameObjectFactory instance.
     */
    static GameObjectFactory& GetInstance();

    /**
     * @brief Deleted copy constructor to prevent copying of the GameObjectFactory.
     */
    GameObjectFactory(const GameObjectFactory&) = delete;

    /**
     * @brief Deleted copy assignment operator to prevent copying of the GameObjectFactory.
     */
    GameObjectFactory& operator=(const GameObjectFactory&) = delete;

    /**
     * @brief Creates a new GameObject with the specified name and adds it to the internal storage.
     *        Provides an empty canvas for customization.
     * @param name The name of the GameObject to create.
     * @return A pointer to the newly created GameObject.
     */
    GameObject* Create(const std::string& name);

    //With tag
    GameObject* Create(const std::string& name, const std::string& tagName);
    
    //With tag & layer
    GameObject* Create(const std::string& name, const std::string& tagName, const std::string& layerName);

    /**
     * @brief Creates a new GameObject using data from a Lua file.
     * @param luaFilePath The file path to the Lua file.
     * @param tableName The table name in the Lua file containing the GameObject's data.
     * @return A pointer to the newly created GameObject.
     */
    GameObject* CreateFromLua(const std::string& luaFilePath, const std::string& tableName);


    /**
	 * @brief Resets a GameObject using data from a Lua file.
	 * @param luaFilePath The file path to the Lua file.
	 * @param tableName The table name in the Lua file containing the GameObject's data.
     * @param gObject The GameObject to reset.
	 * @return Error code. 0 if successful, -1 if failed.
	 */
    int ResetObjectFromLua(const std::string& luaFilePath, const std::string& tableName, GameObject* gObject);


    /**
     * @brief Despawns a specified GameObject, effectively removing it from the game.
     * @param object The GameObject to despawn.
     */
    void Despawn(GameObject* object);

    /**
     * @brief Adds a GameObject to the queue for despawning.
     *        This ensures safe removal after all necessary updates are complete.
     * @param object The GameObject to queue for despawning.
     */
    void QueueDespawn(GameObject* object);

    /**
     * @brief Processes all queued GameObjects for despawning.
     *        This method is called after all component updates to prevent nullptr access.
     */
    void ProcessDespawnQueue();

    /**
     * @brief Updates all GameObjects managed by the factory.
     */
    void UpdateAllGameObjects();

    /**
     * @brief Retrieves all GameObjects currently managed by the factory.
     * @return An unordered map of GameObject IDs and their associated GameObjects.
     */
    std::unordered_map<int, GameObject*> GetAllGameObjects() { return gameObjectMaps; }

    /**
     * @brief Serializes all GameObjects to a specified file.
     * @param newFileName The name of the file to serialize to.
     */
    void SerializeAllGameObjects(const std::string& newFileName);

    /**
     * @brief Serializes a GameObject hierarchy, including the parent object and its children, to a specified file.
     * @param newFileName The name of the file to serialize to.
     * @param parentObject The parent GameObject in the hierarchy.
     * @param children A vector of child GameObjects to serialize.
     */
    void SerializeGameObjectHierarchy(const std::string& newFileName, GameObject* parentObject, const std::vector<GameObject*>& children);

    /**
     * @brief Retrieves a GameObject by its unique ID.
     * @param id The ID of the GameObject to retrieve.
     * @return A pointer to the GameObject if found, or nullptr otherwise.
     */
    GameObject* GetObjectByID(int id) {
        auto it = gameObjectMaps.find(id);
        if (it != gameObjectMaps.end()) {
            return it->second; // Return the GameObject associated with the ID
        }
        return nullptr; // Return nullptr if not found
    }

    /**
     * @brief Clears all GameObject pools, effectively removing all managed GameObjects.
     */
    void Clear();

    /**
     * @brief Retrieves the current count of active GameObjects.
     * @return The number of active GameObjects.
     */
    size_t GetNumObjects() const;

    /**
     * @brief Destructor for the GameObjectFactory. Cleans up resources.
     */
    ~GameObjectFactory();

    /**
     * @brief Helper function to retrieve the player GameObject in a single call.
     * @return A pointer to the player GameObject, or nullptr if not found.
     */
    GameObject* GetPlayerObject();

    /**
     * @brief Sorts GameObjects based on their Y-axis position.
     *        Lower Y-axis positions are given higher layer priority.
     */
    void YSortLayers();


    bool isSpatial; //Temp
    bool useForce;

    /**
     * @brief Checks if game object exist
     *
     * @param object The game object to check
     */
    bool IsGameObjectValid(const GameObject* object) const;

    /**
     * @brief Adds a new tag to the current set of tags.
     * @param tagName The name of the tag to be added.
     */
    void AddTag(const std::string& tagName) { tags.insert(tagName); } //New type of tag
    /**
     * @brief Removes an existing tag from the current set of tags.
     * @param tagName The name of the tag to be removed.
     */
    void RemoveTag(const std::string& tagName) { tags.erase(tagName); }
    /**
     * @brief Checks if the specified tag exists in the current set of tags.
     * @param tagName The name of the tag to check.
     * @return True if the tag exists, false otherwise.
     */
    bool HasTag(const std::string& tagName) const { return tags.count(tagName) > 0; }
    /**
     * @brief Retrieves all tags currently associated with the object.
     * @return A constant reference to the set of all tags.
     */
    const std::unordered_set<std::string>& GetTags() const { return tags; } //Find all tags
    /**
     * @brief Finds and retrieves all game objects associated with a specific tag.
     * @param tag The tag used to search for game objects.
     * @return A vector containing pointers to the matching game objects.
     */
    std::vector<GameObject*> FindGameObjectsByTag(const std::string& tag);

private:
    GameObjectFactory();  //Private constructor for singleton
    std::unordered_map<int, GameObject*> gameObjectMaps; //ID based Map
    // Object pools for game objects and components
    ObjectPool<GameObject> gameObjectPool;
    int nextID = 0;
    std::vector<int> freedIDs; //Reuse of despawned IDs
    std::vector<GameObject*> despawnQueue;
    std::unordered_set<std::string> tags;

};
