/*!****************************************************************
\file: RectColliderComponent.h
\author: Johny Yong Jun Siang, j.yong, 2301301
\co-author: Goh Jun Jie, g.junjie, 2301293
\brief: Declares the RectColliderComponent class, managing rectangular colliders (AABBs) for game objects. Supports multiple colliders,
        collision detection, trigger events, and integrates with RigidBodyComponent. Provides serialization, deserialization,
        and debugging.

        Johny created the file and every of its functions (80%)
        Jun Jie provided the AABB data and serialization/deserialization (20%)

Copyright (C) 2024 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents
without the prior written consent of DigiPen Institute of
Technology is prohibited.
*******************************************************************!*/
#pragma once
#include "Component.h"
#include "RigidBodyComponent.h"
#include "Collision.h"
#include "pch.h"
#include <functional>
#include "glhelper.h"

class RectColliderComponent : public Component {
public:
    /**
     * @brief Default constructor for RectColliderComponent. Initializes an empty collider component.
     */
    RectColliderComponent();

    /**
     * @brief Constructor for RectColliderComponent with a parent GameObject.
     * @param parent The parent GameObject associated with this collider.
     */
    RectColliderComponent(GameObject* parent);

    /**
     * @brief Constructor for RectColliderComponent with a parent GameObject and a RigidBodyComponent.
     * @param parent The parent GameObject associated with this collider.
     * @param rb The RigidBodyComponent associated with this collider.
     */
    RectColliderComponent(GameObject* parent, RigidBodyComponent* rb);

    /**
     * @brief Adds a new collider with the specified size and center.
     * @param size The size of the collider (width and height).
     * @param center The center position of the collider relative to the GameObject.
     */
    void AddCollider(const Vector2& size, const Vector2& center);

    /**
     * @brief Gets the number of colliders managed by this component.
     * @return The count of colliders.
     */
    int GetColliderCount() const;

    /**
     * @brief Retrieves the axis-aligned bounding box (AABB) for a specific collider.
     * @param index The index of the collider.
     * @return The AABB of the collider at the specified index.
     */
    AABB GetAABB(int index) const;

    /**
     * @brief Checks if the collider is set as a trigger.
     * @return True if the collider is a trigger, false otherwise.
     */
    bool const GetTrigger() const { return isTrigger; }

    /**
     * @brief Provides access to the collider data (size and center).
     * @return A reference to the vector of collider data pairs.
     */
    std::vector<std::pair<Vector2, Vector2>>& GetColliderData() { return colliderData; }

    /**
     * @brief Sets the collider as a trigger or non-trigger.
     * @param trigger True to set the collider as a trigger, false otherwise.
     */
    void SetTrigger(bool trigger) { isTrigger = trigger; };

    /**
     * @brief Trigger event handler called when this collider enters a trigger collision with another collider.
     * @param other The other RectColliderComponent involved in the trigger event.
     */
    void OnTriggerEnter(const RectColliderComponent& other);

    /**
     * @brief Updates the collider's bounding boxes based on the GameObject's position and size.
     */
    void Update() override;

    /**
     * @brief Updates the size of a specific collider.
     * @param index The index of the collider to update.
     * @param newSize The new size for the collider.
     */
    void UpdateColliderSize(int index, const Vector2& newSize);

    /**
     * @brief Updates the center position of a specific collider.
     * @param index The index of the collider to update.
     * @param newCenter The new center position for the collider.
     */
    void UpdateColliderCenter(int index, const Vector2& newCenter);

    /**
     * @brief Serializes the RectColliderComponent's data to a Lua file.
     * @param luaFilePath The file path to the Lua file.
     * @param tableName The table name under which data will be stored.
     */
    void Serialize(const std::string& luaFilePath, const std::string& tableName) override;

    /**
     * @brief Deserializes the RectColliderComponent's data from a Lua file.
     * @param luaFilePath The file path to the Lua file.
     * @param tableName The table name from which data will be loaded.
     */
    void Deserialize(const std::string& luaFilePath, const std::string& tableName) override;

    /**
     * @brief Provides a debug string with details about the RectColliderComponent's state.
     * @return A string containing debug information.
     */
    std::string DebugInfo() const override;

    /**
     * @brief Checks for collisions between this collider and another RectColliderComponent.
     * @param other The other RectColliderComponent to check against.
     * @return True if a collision is detected, false otherwise.
     */
    bool CheckCollision(const RectColliderComponent& other);

    /**
     * @brief Retrieves the RigidBodyComponent associated with this collider.
     * @return A pointer to the RigidBodyComponent.
     */
    RigidBodyComponent* GetRigidbody() const { return rigidbody; }

    /**
     * @brief Sets the RigidBodyComponent associated with this collider.
     * @param rb A pointer to the RigidBodyComponent.
     */
    void SetRigidbody(RigidBodyComponent* rb) { rigidbody = rb; }

    /**
     * @brief Retrieves the GameObject currently colliding with this collider.
     * @return A pointer to the currently colliding GameObject.
     */
    GameObject* GetCurrentlyColliding() const { return currentCollidingOBJ; }

    /**
     * @brief Sets the currently colliding GameObject. Includes a delay to prevent immediate collision reset.
     * @param object A pointer to the currently colliding GameObject, or nullptr to clear the collision.
     */
    void SetCurrentlyColliding(GameObject* object) {
        static float clearDelay = 0.1f; // Short delay (100ms) to prevent immediate collision reset
        static float timeSinceClear = 0.0f;

        if (object == nullptr) {
            // Start the delay timer
            timeSinceClear = 0.0f;
        }
        else if (timeSinceClear < clearDelay) {
            // Ignore updates to currentlyCollidingObject during the delay
            timeSinceClear += (float)InputManager::GetDeltaTime();
            return;
        }

        currentCollidingOBJ = object;
    }

    bool isTrigger; // Indicates if the collider is a trigger.

    /**
     * @brief Modifies the collider box when the game object is in bullet form.
     * @param tagName The tag name of the game object.
     */
    void RestoreOriginalColliderBox(const std::string& tagName);
    /**
     * @brief Modifies the collider box when the game object is in bullet form.
     * @param tagName The tag name of the game object.
     */
    void SetBulletColliderBox(const std::string& tagName);

    bool isBlink;
private:
    RigidBodyComponent* rigidbody; // Pointer to the RigidBodyComponent associated with this collider.
    std::vector<AABB> colliders; // List of axis-aligned bounding boxes (AABBs) for this component.
    std::vector<std::pair<Vector2, Vector2>> colliderData; // Pairs of size and center for each collider.
    GameObject* currentCollidingOBJ = nullptr; // Pointer to the currently colliding GameObject.
};
