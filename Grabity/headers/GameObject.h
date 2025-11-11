/*!****************************************************************
\file: GameObject.h
\author: Johny Yong Jun Siang, j.yong, 2301301
\brief: Defines the GameObject class, manages components via unique pointers throughout each GOs lifecycle. Supports dynamic 
        component management such as addition of components, retrieval/get of components, move semantics, serialization and deserialization. 
        Copy operations are disabled to ensure resource uniqueness.

Copyright (C) 2024 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents
without the prior written consent of DigiPen Institute of
Technology is prohibited.
*******************************************************************!*/
#pragma once
#include <unordered_map>
#include <string>
#include <memory>
#include "Component.h"
#include "ObjectPool.h"
#include "LuaConfig.h"
#include "Engine.h"
#include "ComponentHeaders.h"
#include "TagManager.h"
#include "LayerManager.h"

//https://en.cppreference.com/w/cpp/memory/enable_shared_from_this
// GameObject class
class GameObject {
public:
    GameObject();  // Default constructor
    ~GameObject(); //Default destructor
    //Disable Copying
    GameObject(const GameObject&) = delete;
    GameObject& operator=(const GameObject&) = delete;

    //Getter and setter for the names
    void SetName(const std::string& objName) { this->name = objName; }
    const std::string& GetName() const;
    int GetId() const;
    void SetId(int id);

    // Allow default move semantics
    GameObject(GameObject&&) = default;
    GameObject& operator=(GameObject&&) = default;

#pragma region Components
    /**
     * @brief Adds a component to the GameObject by its type and arguments.
     *        Supports variadic arguments to pass initialization parameters to the component's constructor.
     * @tparam ComponentType The type of the component to add.
     * @tparam Args Variadic template arguments for the component constructor.
     * @param componentType The type identifier of the component being added (e.g., TypeOfComponent::TRANSFORM).
     * @param args Arguments to forward to the component's constructor.
     */
    template<typename ComponentType, typename... Args>
    void AddComponent(const TypeOfComponent& componentType, Args&&... args);

    /**
     * @brief Helper function to add a component to the GameObject with specified arguments.
     *        Ensures proper construction and initialization of the component.
     * @tparam ComponentType The type of the component to add.
     * @tparam Args Variadic template arguments for the component constructor.
     * @param componentType The type identifier of the component being added.
     * @param args Arguments to forward to the component's constructor.
     */
    template<typename ComponentType, typename... Args>
    void AddComponentHelper(const TypeOfComponent& componentType, Args&&... args);

    /**
     * @brief Adds a component to the GameObject by moving a unique pointer to the component.
     * @tparam ComponentType The type of the component being added.
     * @param componentName The type identifier of the component being added.
     * @param component A unique pointer to the component.
     * @return A raw pointer to the added component.
     */
    template<typename ComponentType>
    ComponentType* AddComponent(const TypeOfComponent& componentName, std::unique_ptr<ComponentType> component);

    /**
     * @brief Removes a component from the GameObject by its type identifier.
     * @param componentType The type identifier of the component to remove.
     */
    void RemoveComponent(const TypeOfComponent& componentType);

    /**
     * @brief Retrieves a component from the GameObject by its type.
     * @tparam T The type of the component to retrieve.
     * @param name The type identifier of the component.
     * @return A pointer to the component if found, or nullptr otherwise.
     */
    template<typename T>
    T* GetComponent(const TypeOfComponent& name);

    /**
     * @brief Retrieves all components currently attached to the GameObject.
     * @return A constant reference to the unordered map of components.
     */
    const std::unordered_map<TypeOfComponent, std::unique_ptr<Component>>& GetComponents() const;

    /**
     * @brief Removes all components from the GameObject, clearing the component list.
     */
    void ClearComponents();
#pragma endregion 

    /**
     * @brief Updates all components of the GameObject.
     *        Calls the update method of each component attached to the GameObject.
     */
    void Update();

    /**
     * @brief Serializes the GameObject's components and data to a Lua file.
     * @param luaFilePath The file path to the Lua file.
     * @param tableName The name of the table in the Lua file to serialize to.
     */
    void Serialize(const std::string& luaFilePath, const std::string& tableName);

    /**
     * @brief Deserializes the GameObject's components and data from a Lua file.
     * @param luaFilePath The file path to the Lua file.
     * @param tableName The name of the table in the Lua file to deserialize from.
     */
    void Deserialize(const std::string& luaFilePath, const std::string& tableName);

#pragma region ParentChild
    /**
     * @brief Sets the parent of the GameObject, establishing a hierarchical relationship.
     * @param parent A pointer to the parent GameObject.
     */
    void SetParent(GameObject* parent);

    /**
     * @brief Unsets the parent of the GameObject, breaking the hierarchical relationship.
     */
    void UnsetParent();

    /**
     * @brief Retrieves the parent GameObject.
     * @return A pointer to the parent GameObject, or nullptr if none is set.
     */
    GameObject* GetParent() const { return parentGameObject; };

    /**
     * @brief Adds a child GameObject to the current GameObject.
     *        Establishes a parent-child relationship.
     * @param child A pointer to the child GameObject to add.
     */
    void AddChild(GameObject* child);

    /**
     * @brief Removes a child GameObject from the current GameObject.
     *        Breaks the parent-child relationship.
     * @param child A pointer to the child GameObject to remove.
     */
    void RemoveChild(GameObject* child);

    /**
     * @brief Retrieves the list of child GameObjects.
     * @return A reference to the vector of child GameObjects.
     */
    std::vector<GameObject*>& GetChildren() { return children; };
#pragma endregion 

    /**
     * @brief Updates the world transform of the GameObject.
     *        Recalculates the global position, scale, and rotation based on the parent-child hierarchy.
     */
    void UpdateWorldTransform();

    bool isDeserializing = false;

    //Tagging system
    /**
     * @brief Set the gameobject's tag
     */
    void SetTag(const std::string& newTag);
    //Tagging system
    /**
     * @brief Get the gameobject's tag
     */
    const std::string& GetTag() const;

    //Layering System
    void SetLayer(const std::string& newLayer) {
        if (LayerManager::GetInstance().IsLayerValid(newLayer)) {
            layer = newLayer;
            LayerManager::GetInstance().SetLayerForGameObject(this, newLayer);

            //Set the layer to all child game objects
            /*for (GameObject* child : children) {
                child->SetLayer(layer);
            }*/
        }
        else {
            std::cout << "Layer not valid: " << layer << std::endl;
        }
    }

    const std::string& GetLayer() const {return layer; }

   

private:
    // A map to store components by name, allowing quick lookup (e.g., TypeOfComponent::TRANSFORM, TypeOfComponent::SPRITE)
    std::unordered_map<TypeOfComponent, std::unique_ptr<Component>> components;
    std::string name;  // The name of the game object
    int id; //Unique ID per Game Object

    GameObject* parentGameObject = nullptr; 
    std::vector<GameObject*> children;

    std::string tag = "Untagged"; //Default tag for any game object would be untagged
    std::string layer = "Default";
};

//Templates section

// Template function to get a component of the requested type
template<typename T>
T* GameObject::GetComponent(const TypeOfComponent& name) {
    auto it = components.find(name);
    if (it != components.end()) {
        // Use dynamic_cast on the raw pointer from unique_ptr
        return dynamic_cast<T*>(it->second.get());  // Extract raw pointer using get() and perform the cast
    }
    return nullptr;
}

template<typename ComponentType, typename ...Args>
inline void GameObject::AddComponent(const TypeOfComponent& theComponent, Args && ...args)
{
    // Check if we are adding a RectColliderComponent
    if (theComponent == TypeOfComponent::RECTCOLLIDER) {
        // Ensure the game object has a RigidBodyComponent
        if (!GetComponent<RigidBodyComponent>(TypeOfComponent::RIGIDBODY)) {
            AddComponent<RigidBodyComponent>(TypeOfComponent::RIGIDBODY);
        }
    }

    AddComponentHelper<ComponentType>(theComponent, std::forward<Args>(args)...);
}

//Passing in component
template<typename ComponentType>
inline ComponentType* GameObject::AddComponent(const TypeOfComponent& componentName, std::unique_ptr<ComponentType> component) {
    ComponentType* componentPtr = component.get();
    components[componentName] = std::move(component);
    return componentPtr;
}

template<typename ComponentType, typename ...Args>
inline void GameObject::AddComponentHelper(const TypeOfComponent& theComponent, Args && ...args)
{
    std::unique_ptr<ComponentType> component = std::make_unique<ComponentType>(this, std::forward<Args>(args)...);
    components[theComponent] = std::move(component);
}