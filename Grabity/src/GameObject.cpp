/*!****************************************************************
\file: GameObject.cpp
\author: Johny Yong Jun Siang, j.yong, 2301301
\brief: Define functions from GameObject.h, handles component lifecycle, updates, and state management through serialization
        and de-serialization. Provides robust component manipulation and ensures clean resource handling.

Copyright (C) 2024 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents
without the prior written consent of DigiPen Institute of
Technology is prohibited.
*******************************************************************!*/
#include "GameObject.h"
#include "GameObjectFactory.h"
#include "engine.h"

//For templated functions, definitions is at the header

//Zero initialized Ctor
GameObject::GameObject() : id(0) {}

//Destructor
GameObject::~GameObject() {
    ClearComponents();
}

// Updates all components of the GameObject
void GameObject::Update() {
#ifdef _IMGUI
    if (Engine::GetInstance().isInGameScene && !Engine::GetInstance().isPaused)
    {
        //if (Engine::GetInstance().isInGameScene && !Engine::GetInstance().isPaused)
        //{
            //HealthComponent* health = GetComponent<HealthComponent>(TypeOfComponent::HEALTH);
            //if (health && !health->IsAlive())//Checks component first then isAlive
            //{
            //    GameObjectFactory::GetInstance().QueueDespawn(this);
            //    return; 
            //}

            // Iterate through each component and call its update method
        for (std::pair<const TypeOfComponent, std::unique_ptr<Component>>& pair : components) {
            if (!GameObjectFactory::GetInstance().IsGameObjectValid(this)) {
                return; // Prevent update if the game object was deleted
            }

            if (this != NULL)
            {
                if (pair.first != TypeOfComponent::PAUSEMENUBUTTON)
                {
                    pair.second->Update();  // No need to manually manage raw pointers
                }

            }
        }

        for (GameObject* child : children) {
            //child->Update(); //Update the transform of the children after parent has changed their transformation
            child->UpdateWorldTransform();
        }

    }
#else
    if (!Engine::GetInstance().isPaused)
    {
        //if (Engine::GetInstance().isInGameScene && !Engine::GetInstance().isPaused)
        //{
            //HealthComponent* health = GetComponent<HealthComponent>(TypeOfComponent::HEALTH);
            //if (health && !health->IsAlive())//Checks component first then isAlive
            //{
            //    GameObjectFactory::GetInstance().QueueDespawn(this);
            //    return; 
            //}

            // Iterate through each component and call its update method
        for (std::pair<const TypeOfComponent, std::unique_ptr<Component>>& pair : components) {

            if (this != NULL)
            {
                if (pair.first != TypeOfComponent::PAUSEMENUBUTTON)
                    pair.second->Update();  // No need to manually manage raw pointers
            }
        }

        for (GameObject* child : children) {
            //child->Update(); //Update the transform of the children after parent has changed their transformation
            child->UpdateWorldTransform();
        }

    }
#endif // _IMGUI
    for (std::pair<const TypeOfComponent, std::unique_ptr<Component>>& pair : components) {

        if (this != NULL)
        {
            if (pair.first == TypeOfComponent::PAUSEMENUBUTTON)
            {
                pair.second->Update();
            }
            UpdateWorldTransform();
        }
    }
}

#ifdef _IMGUI
// Serializes all components of the GameObject
void GameObject::Serialize(const std::string& luaFilePath, const std::string& tableName) {
    std::ifstream fileCheck(luaFilePath);
    if (!fileCheck) {
        // If the file doesn't exist, create an empty file
        std::ofstream fileCreate(luaFilePath);
        if (!fileCreate) {
			ImGuiConsole::Cout("Error: Could not create file %s", luaFilePath.c_str());
            return;
        }
        fileCreate << "-- Lua file\n";
        fileCreate.close();
    }
	ImGuiConsole::Cout("%s", luaFilePath.c_str());
    
    LuaManager luaManager(luaFilePath);

    //Serialize the GameObject Name
    std::vector<std::string> keys = { "name", "tag"};
    LuaManager::LuaValueContainer values = { name, tag };
    luaManager.LuaWrite(tableName, values, keys, "Name");

    // Serialize components
    // Iterate through each component and call its serialize method
    for (std::pair<const TypeOfComponent, std::unique_ptr<Component>>& pair : components) {
        pair.second->Serialize(luaFilePath, tableName);  // No need to manually manage raw pointers
    }
}
#endif

const std::string& GameObject::GetName() const
{
    return name;
}

void GameObject::Deserialize(const std::string& luaFilePath, const std::string& tableName)
{
    LuaManager luaManager(luaFilePath);

    name = luaManager.LuaReadFromName<std::string>(tableName, "name");
    tag = luaManager.LuaReadFromName<std::string>(tableName, "tag");

    if (!TagManager::GetInstance().IsTagValid(tag))
    {
		ImGuiConsole::Cout("Tag read not valid! Adding as new tag!");
        TagManager::GetInstance().AddTag(tag);
    }

    for (std::pair<const TypeOfComponent, std::unique_ptr<Component>>& pair : components) {
        pair.second->Deserialize(luaFilePath, tableName);  // No need to manually manage raw pointers
    }
}

int GameObject::GetId() const
{
    return id;
}

void GameObject::SetId(int newID)
{
    id = newID;
}

void GameObject::RemoveComponent(const TypeOfComponent& componentType)
{
    auto it = components.find(componentType);
    
    if (componentType == TypeOfComponent::RIGIDBODY) {
        RemoveComponent(TypeOfComponent::RECTCOLLIDER);
    }

    if (it != components.end())
        components.erase(it);
}

//Returns all components attached to the Game Object
const std::unordered_map<TypeOfComponent, std::unique_ptr<Component>>& GameObject::GetComponents() const {
    return components;
}

void GameObject::ClearComponents()
{
    components.clear();
}

//M2

void GameObject::SetParent(GameObject* parent) {
    if (parentGameObject != parent) {
        if (parentGameObject) {
            parentGameObject->RemoveChild(this);
        }

        parentGameObject = parent;
        if (parent) {
            parent->AddChild(this);

            // Adjust the child's local position relative to the new parent’s position
            TransformComponent* childTransform = GetComponent<TransformComponent>(TypeOfComponent::TRANSFORM);
            TransformComponent* parentTransform = parent->GetComponent<TransformComponent>(TypeOfComponent::TRANSFORM);

            if (childTransform && parentTransform && !isDeserializing) {
                // Set localPosition to ensure world position remains the same
                childTransform->SetLocalPosition(childTransform->GetPosition() - parentTransform->GetPosition());
                childTransform->SetLocalScale({
                    childTransform->GetLocalScale().x / parentTransform->GetLocalScale().x,
                    childTransform->GetLocalScale().y / parentTransform->GetLocalScale().y });
            }

            GameObjectFactory& factory = GameObjectFactory::GetInstance();
            factory.UpdateAllGameObjects();
        }
    }
}


void GameObject::UnsetParent()
{
    if (parentGameObject)
    {
        TransformComponent* transform = GetComponent<TransformComponent>(TypeOfComponent::TRANSFORM);
        if (transform)
        {
            transform->SetLocalPosition(transform->GetPosition());
            transform->SetLocalScale(transform->GetScale());
            transform->SetLocalRotation(transform->GetRotation());
        }
        parentGameObject->RemoveChild(this); //Remove self
        parentGameObject = nullptr;
    }
}

void GameObject::AddChild(GameObject* child)
{
    if (std::find(children.begin(), children.end(), child) == children.end()) {
        children.push_back(child);
    }
}

void GameObject::RemoveChild(GameObject* child) {
    auto it = std::find(children.begin(), children.end(), child);
    if (it != children.end()) {
        children.erase(it);
    }
}

void GameObject::UpdateWorldTransform() {
    if (isDeserializing)
    {
        return;
    }

    TransformComponent* transform = GetComponent<TransformComponent>(TypeOfComponent::TRANSFORM);
    if (!transform) {
        return;
    }

    GameObject* parent = this->GetParent();

    // Only recalculate world position if it hasn't been set by physics
    if (parent) {
        parent->UpdateWorldTransform();

        TransformComponent* parentTransform = parent->GetComponent<TransformComponent>(TypeOfComponent::TRANSFORM);
        if (parentTransform) {
            //ImGuiConsole::Cout("Local Pos of child: " << transform->GetLocalPosition().x << ", " << transform->GetLocalPosition().y);
            Vector2 newPos = parentTransform->GetPosition() + transform->GetLocalPosition();
            transform->SetPosition(newPos);

            Vector2 worldScale = {
                parentTransform->GetScale().x * transform->GetLocalScale().x,
                parentTransform->GetScale().y * transform->GetLocalScale().y
            };
            transform->SetScale(worldScale);

            transform->SetRotation(parentTransform->GetRotation() + transform->GetLocalRotation());
        }


    }
    else {
        transform->SetPosition(transform->GetLocalPosition());
        transform->SetScale(transform->GetLocalScale());
        transform->SetRotation(transform->GetLocalRotation());
    }
}

void GameObject::SetTag(const std::string& newTag)
{
    //Need to check if tag is valid
    if (TagManager::GetInstance().IsTagValid(newTag))
    {
        tag = newTag;
    }
    else
    {
		ImGuiConsole::Cout("Failed to add tag. Tag does not exist in the registry.");
    }
}

const std::string& GameObject::GetTag() const {
    return tag;
}

