/*!****************************************************************
\file: GameObjectFactory.cpp
\author: Johny Yong Jun Siang, j.yong, 2301301
\co-author: Goh Jun Jie, g.junjie, 2301293
\brief: Defines the functions declared in GameObjectFactory.h, a singleton that manages the creation, serialization, and lifecycle of
        GameObjects within the game. It provides functions to spawn game objects from templates or from de-serialization, manage
        object pools, and handle their updates and destruction. The factory also supports ID-based management and
        recycling of GameObject instances.

        Johny created the file and defined all the functions outside of CreateFromLua (90%)
        Jun Jie defined the CreateFromLua function (10%)

Copyright (C) 2024 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents
without the prior written consent of DigiPen Institute of
Technology is prohibited.
*******************************************************************!*/

#include "GameObjectFactory.h"
#include "LuaConfig.h"
#include "ButtonComponent.h"
#include "UIComponent.h"
#include "ExplosionComponent.h"

/**
 * @brief Retrieves the singleton instance of the GameObjectFactory.
 * @return A reference to the GameObjectFactory instance.
 */
GameObjectFactory& GameObjectFactory::GetInstance() {
    static GameObjectFactory instance;
    return instance;
}
GameObjectFactory::GameObjectFactory() : gameObjectPool(3000) { isSpatial = true; useForce = false; }

/**
 * @brief Creates a new GameObject with the specified name and adds it to the internal storage.
 *        Provides an empty canvas for customization.
 * @param name The name of the GameObject to create.
 * @return A pointer to the newly created GameObject.
 */
GameObject* GameObjectFactory::Create(const std::string& name) {
    GameObject* object = gameObjectPool.Create();
    
    int assignedID = !freedIDs.empty() ? freedIDs.back() : nextID++; //Reuse old IDs or next ID for GOs

    if (!freedIDs.empty())
    {
        freedIDs.pop_back(); //Remove the ID in the vector
    }

    object->SetId(assignedID);
    object->SetName(name);
    object->SetTag("Untagged"); //Default set as untagged
    object->SetLayer("Default");
    object->ClearComponents();
    gameObjectMaps[assignedID] = object; //Add to map for ID look up

    return object;
}

//With tag
GameObject* GameObjectFactory::Create(const std::string& name, const std::string& tagName) {
    GameObject* object = gameObjectPool.Create();

    int assignedID = !freedIDs.empty() ? freedIDs.back() : nextID++; //Reuse old IDs or next ID for GOs

    if (!freedIDs.empty())
    {
        freedIDs.pop_back(); //Remove the ID in the vector
    }

    object->SetId(assignedID);
    object->SetName(name);
    object->SetTag(tagName);
    object->SetLayer("Default");
    object->ClearComponents();
    gameObjectMaps[assignedID] = object; //Add to map for ID look up

    return object;
}

GameObject* GameObjectFactory::Create(const std::string& name, const std::string& tagName, const std::string& layerName)
{
    GameObject* object = gameObjectPool.Create();

    int assignedID = !freedIDs.empty() ? freedIDs.back() : nextID++; //Reuse old IDs or next ID for GOs

    if (!freedIDs.empty())
    {
        freedIDs.pop_back(); //Remove the ID in the vector
    }

    object->SetId(assignedID);
    object->SetName(name);
    object->SetTag(tagName);
    object->SetLayer(layerName); 
    object->ClearComponents();
    gameObjectMaps[assignedID] = object; //Add to map for ID look up

    return object;
}


/**
 * @brief Creates a new GameObject using data from a Lua file.
 * @param luaFilePath The file path to the Lua file.
 * @param tableName The table name in the Lua file containing the GameObject's data.
 * @return A pointer to the newly created GameObject.
 */
GameObject* GameObjectFactory::CreateFromLua(const std::string& luaFilePath, const std::string& tableName) {
    LuaManager luaManager(luaFilePath);

    // Read the name and create the GameObject
    GameObject* object = Create(luaManager.LuaReadFromName<std::string>(tableName, "name"), luaManager.LuaReadFromName<std::string>(tableName, "tag"), luaManager.LuaReadFromName<std::string>(tableName, "layer"));


    // Check and add TransformComponent if it exists
    if (luaManager.TableExists(tableName, "Transform")) {
        std::unique_ptr<TransformComponent> transformComponent = std::make_unique<TransformComponent>(object);
        transformComponent->Deserialize(luaFilePath, tableName);
        object->AddComponent<TransformComponent>(TypeOfComponent::TRANSFORM, std::move(transformComponent));
    }

    // Check and add SpriteComponent if it exists
    if (luaManager.TableExists(tableName, "Sprite")) {
        std::unique_ptr<SpriteComponent> spriteComponent = std::make_unique<SpriteComponent>();
        spriteComponent->Deserialize(luaFilePath, tableName);
        object->AddComponent<SpriteComponent>(TypeOfComponent::SPRITE, std::move(spriteComponent));
    }

    // Check and add RigidBodyComponent if it exists
    if (luaManager.TableExists(tableName, "RigidBody")) {
        std::unique_ptr<RigidBodyComponent> rigidBodyComponent = std::make_unique<RigidBodyComponent>();
        rigidBodyComponent->Deserialize(luaFilePath, tableName);
        object->AddComponent<RigidBodyComponent>(TypeOfComponent::RIGIDBODY, std::move(rigidBodyComponent));
    }

    // Check and add RectColliderComponent if it exists
    if (luaManager.TableExists(tableName, "Collider")) {
        std::unique_ptr<RectColliderComponent> rectColliderComponent = std::make_unique<RectColliderComponent>(object, object->GetComponent<RigidBodyComponent>(TypeOfComponent::RIGIDBODY));
        rectColliderComponent->Deserialize(luaFilePath, tableName);
        object->AddComponent<RectColliderComponent>(TypeOfComponent::RECTCOLLIDER, std::move(rectColliderComponent));
    }

    // Check and add AudioComponent if it exists
    if (luaManager.TableExists(tableName, "Audio")) {
        std::unique_ptr<AudioComponent> audioComponent = std::make_unique<AudioComponent>();
        audioComponent->Deserialize(luaFilePath, tableName);
        object->AddComponent<AudioComponent>(TypeOfComponent::AUDIO, std::move(audioComponent));
    }

    if (luaManager.TableExists(tableName, "PlayerController")) {
        std::unique_ptr<PlayerControllerComponent> playerControllerComponent = std::make_unique<PlayerControllerComponent>();
        playerControllerComponent->Deserialize(luaFilePath, tableName);
        
        object->AddComponent<PlayerControllerComponent>(
            TypeOfComponent::PLAYER,
            playerControllerComponent->GetMoveSpd(),                
            object->GetComponent<RigidBodyComponent>(TypeOfComponent::RIGIDBODY),
            object->GetComponent<SpriteComponent>(TypeOfComponent::SPRITE)
        );
    }

    // Check and add AIStateMachineComponent if it exists
    if (luaManager.TableExists(tableName, "AIState")) {
        std::unique_ptr<AIStateMachineComponent> aiStateComponent = std::make_unique<AIStateMachineComponent>(object);
        aiStateComponent->Deserialize(luaFilePath, tableName);
        object->AddComponent<AIStateMachineComponent>(TypeOfComponent::AISTATE, std::move(aiStateComponent));
    }

    // Check and add TextComponent if it exists
    if (luaManager.TableExists(tableName, "Text")) {
        std::unique_ptr<TextComponent> textComponent = std::make_unique<TextComponent>();
        textComponent->Deserialize(luaFilePath, tableName);
        object->AddComponent<TextComponent>(TypeOfComponent::TEXT, std::move(textComponent));
    }

    if (luaManager.TableExists(tableName, "TextUI")) {
        std::unique_ptr<UITextComponent> textComponent = std::make_unique<UITextComponent>();
        textComponent->Deserialize(luaFilePath, tableName);
        object->AddComponent<UITextComponent>(TypeOfComponent::TEXT_UI, std::move(textComponent));
    }

    if (luaManager.TableExists(tableName, "Spawner")) {
        std::unique_ptr<SpawnerComponent> spawnerComponent = std::make_unique<SpawnerComponent>(object);
        spawnerComponent->Deserialize(luaFilePath, tableName);
        object->AddComponent<SpawnerComponent>(TypeOfComponent::SPAWNER, std::move(spawnerComponent));

    }
    // Check and add CanvasComponent if it exists
    if (luaManager.TableExists(tableName, "Canvas")) {
        std::unique_ptr<CanvasComponent> canvasComponent = std::make_unique<CanvasComponent>();
        canvasComponent->Deserialize(luaFilePath, tableName);
        object->AddComponent<CanvasComponent>(TypeOfComponent::CANVAS_UI, std::move(canvasComponent));
    }

    // Check and add SpriteUIComponent if it exists
    if (luaManager.TableExists(tableName, "SpriteUI")) {
        std::unique_ptr<UISpriteComponent> spriteUIComponent = std::make_unique<UISpriteComponent>();
        spriteUIComponent->Deserialize(luaFilePath, tableName);
        object->AddComponent<UISpriteComponent>(TypeOfComponent::SPRITE_UI, std::move(spriteUIComponent));
    }

	// Check and add ButtonComponent if it exists
	if (luaManager.TableExists(tableName, "ButtonComponent")) {
		std::unique_ptr<ButtonComponent> buttonComponent = std::make_unique<ButtonComponent>();
		buttonComponent->Deserialize(luaFilePath, tableName);
		object->AddComponent<ButtonComponent>(TypeOfComponent::BUTTON, std::move(buttonComponent));
	}

	// Check and add UIComponent if it exists
	if (luaManager.TableExists(tableName, "UIComponent")) {
		std::unique_ptr<UIComponent> uiComponent = std::make_unique<UIComponent>();
		uiComponent->Deserialize(luaFilePath, tableName);
		object->AddComponent<UIComponent>(TypeOfComponent::UI, std::move(uiComponent));
	}
    if (luaManager.TableExists(tableName, "Health")) {
        std::unique_ptr<HealthComponent> healthComponent = std::make_unique<HealthComponent>(object);
        healthComponent->Deserialize(luaFilePath, tableName);
        object->AddComponent<HealthComponent>(TypeOfComponent::HEALTH, std::move(healthComponent));
    }

    if (luaManager.TableExists(tableName, "Explosion")) {
        std::unique_ptr<ExplosionComponent> explosionComponent = std::make_unique<ExplosionComponent>(object);
        explosionComponent->Deserialize(luaFilePath, tableName);
        object->AddComponent<ExplosionComponent>(TypeOfComponent::EXPLOSION, std::move(explosionComponent));
    }

    if (luaManager.TableExists(tableName, "PauseMenuButton")) {
        std::unique_ptr<PauseMenuButton> PauseMenuButtonComponent = std::make_unique<PauseMenuButton>(object);
        PauseMenuButtonComponent->Deserialize(luaFilePath, tableName);
        object->AddComponent<PauseMenuButton>(TypeOfComponent::PAUSEMENUBUTTON, std::move(PauseMenuButtonComponent));
    }

    if (luaManager.TableExists(tableName, "Animator")) {
        std::unique_ptr<AnimatorComponent> animComponent = std::make_unique<AnimatorComponent>(object);
        animComponent->Deserialize(luaFilePath, tableName);
        object->AddComponent<AnimatorComponent>(TypeOfComponent::ANIMATOR, std::move(animComponent));
    }
    // Check and add SliderComponent if it exists
    if (luaManager.TableExists(tableName, "SliderComponent")) {
        std::unique_ptr<SliderComponent> slider = std::make_unique<SliderComponent>(object);
        slider->Deserialize(luaFilePath, tableName);
        object->AddComponent<SliderComponent>(TypeOfComponent::SLIDER, std::move(slider));
    }
    // Check and add SliderComponent if it exists
    if (luaManager.TableExists(tableName, "ParticleSystem")) {
        std::unique_ptr<ParticleSystem> slider = std::make_unique<ParticleSystem>(object);
        slider->Deserialize(luaFilePath, tableName);
        object->AddComponent<ParticleSystem>(TypeOfComponent::PARTICLE, std::move(slider));
    }

    if (luaManager.TableExists(tableName, "Splitting")) {
        std::unique_ptr<SplittingComponent> splitting = std::make_unique<SplittingComponent>(object);
        splitting->Deserialize(luaFilePath, tableName);
        object->AddComponent<SplittingComponent>(TypeOfComponent::SPLITTING, std::move(splitting));

    }

    if (luaManager.TableExists(tableName, "Video"))
    {
        std::unique_ptr<VideoComponent> videoCom = std::make_unique<VideoComponent>(object);
        videoCom->Deserialize(luaFilePath, tableName);
        object->AddComponent<VideoComponent>(TypeOfComponent::VIDEO, std::move(videoCom));
    }

    if (luaManager.TableExists(tableName, "VfxFollowComponent"))
    {
        std::unique_ptr<VfxFollowComponent> vfxFollowScript = std::make_unique<VfxFollowComponent>(object);
        vfxFollowScript->Deserialize(luaFilePath, tableName);
        object->AddComponent<VfxFollowComponent>(TypeOfComponent::VFX_FOLLOW, std::move(vfxFollowScript));
    }
    return object;
}



int GameObjectFactory::ResetObjectFromLua(const std::string& luaFilePath, const std::string& tableName, GameObject* gObject) {
    // Reset the GameObject with data from the Lua file
    // For now only updates the transform and health components
    LuaManager luaManager(luaFilePath);
    try
    {
        // Check and add TransformComponent if it exists
        if (luaManager.TableExists(tableName, "Transform")) {
            std::unique_ptr<TransformComponent> transformComponent = std::make_unique<TransformComponent>(gObject);
            transformComponent->Deserialize(luaFilePath, tableName);
            gObject->GetComponent<TransformComponent>(TypeOfComponent::TRANSFORM)->SetPosition(transformComponent->GetPosition());
            gObject->GetComponent <TransformComponent>(TypeOfComponent::TRANSFORM)->SetScale(transformComponent->GetScale());
            gObject->GetComponent <TransformComponent>(TypeOfComponent::TRANSFORM)->SetRotation(transformComponent->GetRotation());
            gObject->GetComponent<TransformComponent>(TypeOfComponent::TRANSFORM)->SetLocalPosition(transformComponent->GetLocalPosition());
            gObject->GetComponent <TransformComponent>(TypeOfComponent::TRANSFORM)->SetLocalScale(transformComponent->GetLocalScale());
            gObject->GetComponent <TransformComponent>(TypeOfComponent::TRANSFORM)->SetLocalRotation(transformComponent->GetLocalRotation());
        }
        if (luaManager.TableExists(tableName, "Health")) {
            std::unique_ptr<HealthComponent> healthComponent = std::make_unique<HealthComponent>();
            healthComponent->Deserialize(luaFilePath, tableName);
            gObject->GetComponent<HealthComponent>(TypeOfComponent::HEALTH)->SetHealth(healthComponent->GetHealth());
        }
        //if (luaManager.TableExists(tableName, "PauseMenuButton")) {
        //    gObject->GetComponent<PauseMenuButton>(TypeOfComponent::PAUSEMENUBUTTON)->RestartGame();
        //}
    }
    catch (const std::exception& e)
    {
        ImGuiConsole::Cout(e.what());
        return 1;
    }
    return 0;
}

/**
 * @brief Updates all GameObjects managed by the factory.
 */
void GameObjectFactory::UpdateAllGameObjects() {
    for (std::unordered_map<int, GameObject*>::iterator it = gameObjectMaps.begin(); it != gameObjectMaps.end(); ++it) {
        GameObject* object = it->second;
        object->Update();

    }
    YSortLayers();
    ProcessDespawnQueue(); //Added in M3
}

/**
 * @brief Despawns a specified GameObject, effectively removing it from the game.
 * @param object The GameObject to despawn.
 */
void GameObjectFactory::Despawn(GameObject* object) {

    // Recursively despawn children
    const std::vector<GameObject*>& children = object->GetChildren();
    for (GameObject* child : children) {
        if (child) {
            Despawn(child); // Recursively despawn each child
        }
    }

    // Remove this object from its parent's children vector
    GameObject* parent = object->GetParent();
    if (parent) {
        std::vector<GameObject*>& parentChildren = parent->GetChildren();
        parentChildren.erase(std::remove(parentChildren.begin(), parentChildren.end(), object), parentChildren.end());
    }

    // Proceed with despawning the object
    int objectId = object->GetId();
    object->ClearComponents();
    gameObjectMaps.erase(objectId); //Kill off the object via ID
    gameObjectPool.Remove(object); //Remove the GO from object pool
    freedIDs.push_back(objectId); //New ID to be reused

  //  ImGuiConsole::Cout("GameObject despawned successfully: " << objectId);

#ifdef _IMGUI
    Engine::GetInstance().SetSelectedObject(nullptr);
#endif // _IMGUI
}

/**
 * @brief Processes all queued GameObjects for despawning.
 *        This method is called after all component updates to prevent nullptr access.
 */
void GameObjectFactory::ProcessDespawnQueue() {
    for (GameObject* object : despawnQueue) {
        Despawn(object);
    }
    despawnQueue.clear();
}
/**
 * @brief Adds a GameObject to the queue for despawning.
 *        This ensures safe removal after all necessary updates are complete.
 * @param object The GameObject to queue for despawning.
 */
void GameObjectFactory::QueueDespawn(GameObject* object) {
    if (std::find(despawnQueue.begin(), despawnQueue.end(), object) == despawnQueue.end()) {
        despawnQueue.push_back(object);
    }
}
/**
 * @brief Serializes all GameObjects to a specified file.
 * @param newFileName The name of the file to serialize to.
 */
void GameObjectFactory::SerializeAllGameObjects(const std::string& newFileName)
{        
    //Create new luafile
    LuaManager luaManager(newFileName);
    luaManager.SaveNewScene(newFileName, "-- Lua Level file");
    //luaManager.ClearLuaFile();

    //loop all gameobjects and call each serialize
    for (std::unordered_map<int, GameObject*>::iterator it = gameObjectMaps.begin(); it != gameObjectMaps.end(); ++it) {
        GameObject* object = it->second;
        if (object) {

            //Unique name to stop oversaving
            std::string uniqueObjectName = object->GetName() + "_" + std::to_string(it->first);

            //Hard coded to save "Name" table
            LuaManager luaName(newFileName);

            // Serialize the parent relationship
            GameObject* parentGameObject = object->GetParent(); // Access parent
            int parentID = parentGameObject ? parentGameObject->GetId() : -1;

            std::vector<std::string> keys = { "name", "parentID", "tag", "layer"};
            LuaManager::LuaValueContainer values = { object->GetName(), parentID, object->GetTag(), object->GetLayer()};
            luaName.LuaWrite(uniqueObjectName, values, keys, "Name");


            for (auto& pair : object->GetComponents()) {
                // Serialize each component of the game object
                pair.second->Serialize(newFileName, uniqueObjectName);

            }
        }
    }
}
/**
 * @brief Serializes a GameObject hierarchy, including the parent object and its children, to a specified file.
 * @param newFileName The name of the file to serialize to.
 * @param parentObject The parent GameObject in the hierarchy.
 * @param children A vector of child GameObjects to serialize.
 */
void GameObjectFactory::SerializeGameObjectHierarchy(const std::string& newFileName, GameObject* parentObject, const std::vector<GameObject*>& children) {
    if (!parentObject) {
        ImGuiConsole::Cout("Error: Parent GameObject is null.");
        return;
    }

    (void)children;

    // Create or clear the Lua file
    LuaManager luaManager(newFileName);
    luaManager.SaveNewScene(newFileName, "-- Lua Level file");

    int currentID = 0;

    // Helper lambda function for recursive serialization
    std::function<void(GameObject*, int&)> serializeHierarchy = [&](GameObject* object, int& idCounter) {
        if (!object) return;

        // Generate a unique name for the GameObject
        std::string uniqueObjectName = object->GetName() + "_" + std::to_string(idCounter);

        // Serialize the parent relationship
        GameObject* parentGameObject = object->GetParent();
        int parentID = parentGameObject ? idCounter - 1 : -1;

        std::vector<std::string> keys = { "name", "parentID", "tag", "layer"};
        LuaManager::LuaValueContainer values = { object->GetName(), parentID, object->GetTag(), object->GetLayer()};

        LuaManager luaObject(newFileName);
        luaObject.LuaWrite(uniqueObjectName, values, keys, "Name");

        // Serialize components of the GameObject
        for (auto& pair : object->GetComponents()) {
            pair.second->Serialize(newFileName, uniqueObjectName);
        }

        // Increment the ID counter
        idCounter++;

        // Serialize all children recursively
        for (GameObject* child : object->GetChildren()) {
            serializeHierarchy(child, idCounter);
        }
        };

    // Start serialization with the parent object
    serializeHierarchy(parentObject, currentID);
}

/**
 * @brief Clears all GameObject pools, effectively removing all managed GameObjects.
 */
void GameObjectFactory::Clear() {

    // Collect all GameObjects in a separate container
    std::vector<GameObject*> objectsToDespawn;
    for (const auto& it : gameObjectMaps) {
        objectsToDespawn.push_back(it.second);
    }

    // Despawn each GameObject safely
    for (GameObject* object : objectsToDespawn) {
        Despawn(object);
    }
    gameObjectPool.Clear();
    gameObjectMaps.clear();
    freedIDs.clear();
    nextID = 0;
}
/**
 * @brief Retrieves the current count of active GameObjects.
 * @return The number of active GameObjects.
 */
size_t GameObjectFactory::GetNumObjects() const {
    return gameObjectMaps.size(); // Return the current count of active game objects
}

GameObjectFactory::~GameObjectFactory()
{
    Clear();
}
/**
 * @brief Helper function to retrieve the player GameObject in a single call.
 * @return A pointer to the player GameObject, or nullptr if not found.
 */
GameObject* GameObjectFactory::GetPlayerObject() {
    for (auto& [id, obj] : GetAllGameObjects()) {
        if (obj->GetComponent<PlayerControllerComponent>(TypeOfComponent::PLAYER) != nullptr) {
            return obj;  // Return the player object when found
        }
    }
    return nullptr; // Return nullptr if no player object is found
}

//Done by Johny
/**
* @brief Sorts GameObjects based on their Y-axis position.
*        Lower Y-axis positions are given higher layer priority.
*/
void GameObjectFactory::YSortLayers() {

        //Check for top and lower borders
    GameObject* topBorder = nullptr;
    GameObject* lowerBorder = nullptr;

    for (auto& [id, object] : gameObjectMaps) {
        if (object->GetTag() == "TopBorder") {
            topBorder = object;
        }
        else if (object->GetTag() == "LowerBorder") {
            lowerBorder = object;
        }
        if (topBorder && lowerBorder) break;
    }
    
    if (!topBorder || !lowerBorder) {
        return;
    }

    //Getting transforms of borders
    TransformComponent* topTransform = topBorder->GetComponent<TransformComponent>(TypeOfComponent::TRANSFORM);
    TransformComponent* lowerTransform = lowerBorder->GetComponent<TransformComponent>(TypeOfComponent::TRANSFORM);

    if (!topTransform || !lowerTransform) {
        std::cerr << "Error: Border objects missing TransformComponent!" << std::endl;
        return;
    }

    const float minY = lowerTransform->GetLocalPosition().y;
    const float maxY = topTransform->GetLocalPosition().y;
    const float rangeY = maxY - minY;

    for (auto& [id, object] : gameObjectMaps) {
        // Skip objects without a SpriteComponent
        SpriteComponent* sprite = object->GetComponent<SpriteComponent>(TypeOfComponent::SPRITE);
        if (!sprite) continue;

        if (object->GetName().find("PlayerHand") == 0)
        {
            sprite->SetLayer(99);
            continue;
        }
        else if (object->GetLayer() == "Background" || object->GetLayer() == "EnvironmentalAsset" && object->GetLayer() == "UI")
        {
            continue;
        }

        //Determine Y-axis based on presence of RectColliderComponent
        float yPosition = 0.0f;
        RectColliderComponent* collider = object->GetComponent<RectColliderComponent>(TypeOfComponent::RECTCOLLIDER);
        TransformComponent* transform = object->GetComponent<TransformComponent>(TypeOfComponent::TRANSFORM);

        if (collider) {
            if (transform && !collider->GetColliderData().empty()) {
                //Calculate the true center of the first collider
                yPosition = transform->GetLocalPosition().y + collider->GetColliderData()[0].second.y;
            }
        }
        else {
            if (transform) {
                yPosition = transform->GetLocalPosition().y;
            }
        }

        if (object->GetName() == "DeadTree") {
            bool hasSomethingInFront = false;

            RectColliderComponent* treeCollider = object->GetComponent<RectColliderComponent>(TypeOfComponent::RECTCOLLIDER);
            TransformComponent* treeTransform = object->GetComponent<TransformComponent>(TypeOfComponent::TRANSFORM);
            if (!treeCollider || !treeTransform || treeCollider->GetColliderData().empty()) return;

            float treeY = yPosition;
            float treeX = treeTransform->GetLocalPosition().x;
            float treeHalfWidth = treeCollider->GetColliderData()[0].first.x * 0.5f;

            float treeMinX = treeX - treeHalfWidth;
            float treeMaxX = treeX + treeHalfWidth;

            for (auto& [otherId, otherObject] : gameObjectMaps) {
                std::string tag = otherObject->GetTag();

                if (tag != "Enemy" && 
                    tag != "HeavyEnemy" &&
                    tag != "BombEnemy" &&
                    tag != "Player") continue;

                RectColliderComponent* otherCollider = otherObject->GetComponent<RectColliderComponent>(TypeOfComponent::RECTCOLLIDER);
                TransformComponent* otherTransform = otherObject->GetComponent<TransformComponent>(TypeOfComponent::TRANSFORM);
                if (!otherCollider || !otherTransform || otherCollider->GetColliderData().empty()) continue;

                float otherY = otherTransform->GetLocalPosition().y + otherCollider->GetColliderData()[0].second.y;
                float otherX = otherTransform->GetLocalPosition().x;
                float otherHalfWidth = otherCollider->GetColliderData()[0].first.x * 0.5f;

                float otherMinX = otherX - otherHalfWidth;
                float otherMaxX = otherX + otherHalfWidth;

                // Check for X-overlap
                bool overlapsHorizontally = !(treeMaxX < otherMinX || treeMinX > otherMaxX);

                const float yProximityMin = 10.0f;   
                const float yProximityMax = 250.f;

                if ((otherY > treeY + yProximityMin && otherY < treeY + yProximityMax) && overlapsHorizontally) {
                    hasSomethingInFront = true;
                    break;
                }

            }

            if (hasSomethingInFront) {
                sprite->SetRGB(Vector4(1.f, 1.f, 1.f, 0.5f)); // Translucent
            }
            else {
                sprite->SetRGB(Vector4(1.f, 1.f, 1.f, 1.f)); // Opaque
            }
        }

        //This one is where we change the layer based on the sensitivity we gave it (49 for now)
        int layer = static_cast<int>(100 - ((yPosition - minY) / rangeY) * 49);
        layer = std::clamp(layer, 2, 100);

        sprite->SetLayer(layer);
    }
}
/**
 * @brief Checks if game object exist
 * 
 * @param object The game object to check
 */
bool GameObjectFactory::IsGameObjectValid(const GameObject* object) const {
    if (!object) {
        return false; // Null objects are invalid
    }

    int objectId = object->GetId(); // Assuming GetId() is available
    auto it = gameObjectMaps.find(objectId);
    return it != gameObjectMaps.end() && it->second == object; // Ensure object is present and matches
}



std::vector<GameObject*> GameObjectFactory::FindGameObjectsByTag(const std::string& tag) {
    std::vector<GameObject*> taggedObjects; 

    for (auto& [id, object] : gameObjectMaps) {
        if (object && object->GetTag() == tag) {
            taggedObjects.push_back(object);
        }
    }

    if (taggedObjects.empty()) {
        std::cerr << "Warning: No game objects found with tag '" << tag << "'" << std::endl;
    }

    return taggedObjects;
}