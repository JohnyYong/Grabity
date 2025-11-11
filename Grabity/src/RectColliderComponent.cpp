/*!****************************************************************
\file: RectColliderComponent.cpp
\author: Johny Yong Jun Siang, j.yong, 2301301
\co-author:  Goh Jun Jie, g.junjie, 2301293
\brief: Contain defiinitions for the RectColliderComponent class, managing AABBs. Supports 
        collision detection, trigger handling, serialization, de-serialization and their properties.

        Johny created the file and every of its functions (80%)
        Jun Jie provided the AABB data ans serialization/deserialization (20%)

Copyright (C) 2024 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents
without the prior written consent of DigiPen Institute of
Technology is prohibited.
*******************************************************************!*/

#include "RectColliderComponent.h"
#include "Collision.h"
#include "GameObject.h"
#include "GLhelper.h"

#ifdef _IMGUI
#include <iostream>
#endif // _IMGUI
/**
 * @brief Constructor that initializes the RectColliderComponent with a parent game object.
 *        Automatically adds a collider based on the parent's TransformComponent scale.
 * @param parent The parent game object associated with this collider.
 */
RectColliderComponent::RectColliderComponent(GameObject* parent) : Component(parent), rigidbody(nullptr), isTrigger(false) {

    TransformComponent* transform = parent->GetComponent<TransformComponent>(TypeOfComponent::TRANSFORM);
    rigidbody = parent->GetComponent<RigidBodyComponent>(TypeOfComponent::RIGIDBODY);
    AddCollider(transform->GetScale(), Vector2({}));



}
/**
 * @brief Constructor that initializes the RectColliderComponent with a parent and RigidBodyComponent.
 *        Automatically adds a collider based on the parent's TransformComponent scale.
 * @param parent The parent game object associated with this collider.
 * @param rb The RigidBodyComponent associated with this collider.
 */
RectColliderComponent::RectColliderComponent(GameObject* parent, RigidBodyComponent* rb) : Component(parent), rigidbody(rb), isTrigger(false) {
    TransformComponent* transform = parent->GetComponent<TransformComponent>(TypeOfComponent::TRANSFORM);
    AddCollider(transform->GetScale(), Vector2({}));
}
/**
 * @brief Adds a new collider to the RectColliderComponent.
 * @param size The size of the collider (width and height).
 * @param center The center position of the collider relative to the game object.
 */
void RectColliderComponent::AddCollider(const Vector2& size, const Vector2& center)
{
    colliderData.emplace_back(size, center); //Add the collider data to the back
    colliders.push_back(AABB()); //Add the collider
}
/**
 * @brief Gets the total number of colliders managed by this component.
 * @return The number of colliders.
 */
int RectColliderComponent::GetColliderCount() const {
    return static_cast<int>(colliders.size());
}
/**
 * @brief Retrieves the axis-aligned bounding box (AABB) for a specified collider.
 * @param index The index of the collider.
 * @return The AABB of the collider at the specified index. Returns a default AABB if the index is invalid.
 */
AABB RectColliderComponent::GetAABB(int index) const {
    if (index >= 0 && index < colliders.size()) {
        return colliders[index];
    }
    return AABB();  // Return a default AABB if index is out of range
}
/**
 * @brief Updates the colliders dynamically. This ensures collider bounds are updated if size or position changes at runtime.
 */
//This is for constant update of collider if the size is altered while in run time
//collideData[i].second is the center
void RectColliderComponent::Update() {

    GameObject* parent = GetParentGameObject();
    TransformComponent* transform = parent->GetComponent<TransformComponent>(TypeOfComponent::TRANSFORM);
    
    for (size_t i = 0; i < colliders.size(); ++i) {
        Vector2 halfSize = { colliderData[i].first.x * 0.5f, colliderData[i].first.y * 0.5f};

        colliders[i].min = { transform->GetPosition().x + colliderData[i].second.x - halfSize.x,
                             transform->GetPosition().y + colliderData[i].second.y - halfSize.y };
        colliders[i].max = { transform->GetPosition().x + colliderData[i].second.x + halfSize.x,
                             transform->GetPosition().y + colliderData[i].second.y + halfSize.y };
    }
}
/**
 * @brief Updates the size of a collider at a specified index.
 * @param index The index of the collider to update.
 * @param newSize The new size of the collider.
 */
void RectColliderComponent::UpdateColliderSize(int index, const Vector2& newSize) {
    if (index >= 0 && index < colliderData.size()) {
        colliderData[index].first = newSize;  // Update size
    }
}
/**
 * @brief Updates the center position of a collider at a specified index.
 * @param index The index of the collider to update.
 * @param newCenter The new center position of the collider.
 */
// Updates the center of the collider at a specific index
void RectColliderComponent::UpdateColliderCenter(int index, const Vector2& newCenter) {
    if (index >= 0 && index < colliderData.size()) {
        colliderData[index].second = newCenter;  // Update center
    }
}
/**
 * @brief Trigger event handler called when this collider enters a trigger collision with another collider.
 * @param other The other RectColliderComponent involved in the trigger collision.
 */
void RectColliderComponent::OnTriggerEnter(const RectColliderComponent& other) {
    (void)other;

    GameObject* parentObject = GetParentGameObject();
    if (!parentObject) {
        return;
    }

    //Get the parent of the parent object (assumes the player's hand is a child of the player)
    GameObject* playerObject = parentObject->GetParent();
    if (!playerObject) {
        return;
    }

    //Check if the player object has a PlayerControllerComponent
    auto* player = playerObject->GetComponent<PlayerControllerComponent>(TypeOfComponent::PLAYER);
    if (!player) {
        return;
    }


    // If there is no dragging object, update the current collided object
    if (!player->GetDraggingOBJ()) {
        GameObject* currentCollidedObject = other.GetParentGameObject();
        if (currentCollidedObject) {
            SetCurrentlyColliding(currentCollidedObject);
        }
    }
}
/**
 * @brief Checks for collisions between this collider and another RectColliderComponent.
 * @param other The other RectColliderComponent to check against.
 * @return True if a collision is detected, false otherwise.
 */
bool RectColliderComponent::CheckCollision(const RectColliderComponent& other) {

    Vector2 otherVelocity = other.rigidbody->GetVelocity();

    // Iterate over all this object's colliders
    for (const AABB& thisAABB : colliders) {
        // Iterate over all the other object's colliders
        for (int i = 0; i < other.GetColliderCount(); ++i) {
            AABB otherAABB = other.GetAABB(i);

            if (isTrigger || other.isTrigger) {
                auto* sprite = other.GetParentGameObject()->GetComponent<SpriteComponent>(TypeOfComponent::SPRITE);

                if (TestAABBAABB(thisAABB, otherAABB)) {

                    GameObject* parent = GetParentGameObject()->GetParent();
                    if (parent)
                    {
                        auto* playerComp = parent->GetComponent<PlayerControllerComponent>(TypeOfComponent::PLAYER);

                        if (playerComp)
                        {
                            if (sprite)
                            {
                                //Player is not dragging anything
                                if (!playerComp->GetDraggingOBJ())
                                {
                                    auto* IsEnemy = other.GetParentGameObject()->GetComponent<AIStateMachineComponent>(TypeOfComponent::AISTATE);
                                    if (IsEnemy)
                                    {
                                        sprite->SetRGB(Vector4(0.f, 1.f, 0.f, 1.f));
                                    }
                                }

                            }
                        }

                    }

                    if (InputManager::IsMouseButtonPressed(GLFW_MOUSE_BUTTON_LEFT)) //Imagine taking 3 hours to realise this basically fixes the problem of grabbing enemy - Johny (Tho not optimal)
                    {
                        OnTriggerEnter(other);
                    }
                    return true;
                }
                else //Hand is not touching this object
                {
                    GameObject* parent = GetParentGameObject()->GetParent();
                    if (parent)
                    {
                        auto* playerComp = parent->GetComponent<PlayerControllerComponent>(TypeOfComponent::PLAYER);

                        if (playerComp)
                        {
                            if (sprite)
                            {
                                auto* otherOBJ = other.GetParentGameObject();
                                //Player is not dragging this specifically gameobject
                                if (playerComp->GetDraggingOBJ() != otherOBJ)
                                {
                                    auto* health = otherOBJ->GetComponent<HealthComponent>(TypeOfComponent::HEALTH);
									auto* explode = otherOBJ->GetComponent<ExplosionComponent>(TypeOfComponent::EXPLOSION);
                                    if (explode) {
                                        isBlink = explode->GetBlinkState();
                                    }

                                    if (health && !explode)
                                    {
                                        //It is not red color for damaged
                                        if (!health->turnRed)
                                        {
                                            sprite->SetRGB(Vector4(1.f, 1.f, 1.f, 1.f));
                                        }
                                    }
									if (health && explode)
									{
										if (!health->turnRed && !isBlink)
										{
											sprite->SetRGB(Vector4(1.f, 1.f, 1.f, 1.f));
										}
									}
                
                                }
                            }
                        }

                    }
                }
            }

            float firstTimeOfCollision = 0.f;

            if (Collision_RectRect(thisAABB, rigidbody->GetVelocity(), otherAABB, otherVelocity, firstTimeOfCollision))
            {
                return true;
            }
        }
    }

    return false; // No collision detected
}

/**
 * @brief Serializes the RectColliderComponent's data to a Lua file.
 * @param luaFilePath The file path to the Lua file.
 * @param tableName The table name under which data will be stored.
 */
void RectColliderComponent::Serialize(const std::string& luaFilePath, const std::string& tableName) {
    LuaManager luaManager(luaFilePath);

    // Define the number of colliders
    int colliderCount = static_cast<int>(colliderData.size());

    // Prepare the keys and values for the Collider table
    std::vector<std::string> keys;
    LuaManager::LuaValueContainer values;

    // Set ColliderCount and isTrigger
    keys.push_back("ColliderCount");
    values.push_back(colliderCount);

    keys.push_back("isTrigger");
    values.push_back(isTrigger);

    // Serialize each collider's size and center
    for (int i = 0; i < colliderCount; ++i) {
        // Define keys for each collider using indexed keys
        keys.push_back("CollisionSizeX_" + std::to_string(i));
        values.push_back(colliderData[i].first.x);

        keys.push_back("CollisionSizeY_" + std::to_string(i));
        values.push_back(colliderData[i].first.y);

        keys.push_back("CollisionCenterX_" + std::to_string(i));
        values.push_back(colliderData[i].second.x);

        keys.push_back("CollisionCenterY_" + std::to_string(i));
        values.push_back(colliderData[i].second.y);
    }

    // Call LuaWrite once for all properties in the Collider table
    luaManager.LuaWrite(tableName, values, keys, "Collider");
}

/**
 * @brief Deserializes the RectColliderComponent's data from a Lua file.
 * @param luaFilePath The file path to the Lua file.
 * @param tableName The table name from which data will be loaded.
 */
void RectColliderComponent::Deserialize(const std::string& luaFilePath, const std::string& tableName) {
    LuaManager luaManager(luaFilePath);

    // Read the number of colliders
    int colliderCount = luaManager.LuaRead<int>(tableName, { "Collider", "ColliderCount" });

    // Clear existing collider data
    colliderData.clear();
    colliders.clear();

    // Deserialize each collider's size, center, and add it to the component
    for (int i = 0; i < colliderCount; ++i) {
        Vector2 size;
        Vector2 center;

        // Use the helper function to read collider properties
        size.x = luaManager.LuaReadFromCollider<float>(tableName, "CollisionSizeX", i);
        size.y = luaManager.LuaReadFromCollider<float>(tableName, "CollisionSizeY", i);
        center.x = luaManager.LuaReadFromCollider<float>(tableName, "CollisionCenterX", i);
        center.y = luaManager.LuaReadFromCollider<float>(tableName, "CollisionCenterY", i);

        // Add the collider back to the component
        AddCollider(size, center);
    }

    // Deserialize isTrigger (if all colliders share this property)
    isTrigger = luaManager.LuaRead<bool>(tableName, { "Collider", "isTrigger" });
}

/**
 * @brief Provides a debug string with details about the RectColliderComponent's state.
 * @return A string containing debug information.
 */
std::string RectColliderComponent::DebugInfo() const {
    std::string debugInfo = "RectColliderComponent - Number of Colliders: " + std::to_string(GetColliderCount()) + "\n";

    for (int i = 0; i < GetColliderCount(); ++i) {
        Vector2 size = colliderData[i].first;
        Vector2 center = colliderData[i].second;
        AABB aabb = GetAABB(i);

        debugInfo += "Collider " + std::to_string(i) + ":\n";
        debugInfo += "  Size: (" + std::to_string(size.x) + ", " + std::to_string(size.y) + ")\n";
        debugInfo += "  Center: (" + std::to_string(center.x) + ", " + std::to_string(center.y) + ")\n";
        debugInfo += "  AABB Min: (" + std::to_string(aabb.min.x) + ", " + std::to_string(aabb.min.y) + ")\n";
        debugInfo += "  AABB Max: (" + std::to_string(aabb.max.x) + ", " + std::to_string(aabb.max.y) + ")\n";
    }

    debugInfo += "Is Trigger: " + std::string(isTrigger ? "True" : "False") + "\n";

    return debugInfo;
}




/**
 * @brief Restores the original collider box for a given game object based on its tag.
 * @param tagName The tag name of the game object.
 */
void RectColliderComponent::RestoreOriginalColliderBox(const std::string& tagName) {
    if (tagName == "HeavyEnemy") {
        Vector2 size = { 160.f, 64.f };
        Vector2 center = { 0.f, -70.f };
        UpdateColliderSize(0, size);
        UpdateColliderCenter(0, center);
    }
    if (tagName == "BombEnemy")
    {
        Vector2 center = { -2.f, -96.f };
        UpdateColliderCenter(0, center);
        ImGuiConsole::Cout(GetParentGameObject()->GetName().c_str());
    }
}
/**
 * @brief Modifies the collider box when the game object is in bullet form.
 * @param tagName The tag name of the game object.
 */
void RectColliderComponent::SetBulletColliderBox(const std::string& tagName) {
    if (tagName == "HeavyEnemy") {
        Vector2 size = { 100.f, 100.f };
        Vector2 center = { 0.f, 0.f };
        UpdateColliderSize(0, size);
        UpdateColliderCenter(0, center);
    }

    if (tagName == "BombEnemy")
    {
        Vector2 center = { 0.f, 0.f };
        UpdateColliderCenter(0, center);
    }

}