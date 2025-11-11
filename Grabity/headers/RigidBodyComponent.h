/*!****************************************************************
\file: RigidBodyComponent.h
\author: Lee Yu Jie Brandon , l.yujiebrandon , 2301232
\co-author: Johny Yong Jun Siang, j.yong, 2301301
\brief: Declares the RigidBodyComponent class for simulating basic physics properties such as mass, velocity, and acceleration.
        It integrates with the TransformComponent to update the position of game objects based on physics calculations.
        Supports serialization, deserialization, and debugging.

        Brandon created the file and the functions needed for this class. (30%)
        Johny provided the Component base class hence the virtual functions and setter/getters. (70%)

Copyright (C) 2024 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents
without the prior written consent of DigiPen Institute of
Technology is prohibited.
*******************************************************************!*/
#pragma once
#include "Component.h"
#include "Vector2.h" 
#include "TransformComponent.h"
#include "LuaConfig.h"

/**
 * @class RigidBodyComponent
 * @brief Represents the physical properties and behavior of a rigid body in a 2D space.
 *
 * This component manages the mass, velocity, and acceleration of an object,
 * and updates its position through the associated TransformComponent.
 */
class RigidBodyComponent : public Component {
private:
    float mass;         ///< The mass of the rigid body
    Vector2 velocity;   ///< The current velocity of the rigid body
    Vector2 acceleration; ///< The current acceleration of the rigid body
    float dragCoefficient;

public:

    bool isInKnockback = false;  // Flag to check if the object is in knockback
    float knockbackDuration = 0.0f;  // Duration of the knockback effect
    float knockbackTimeLeft = 0.0f;  // Time left before knockback ends

    /**
     * @brief Default constructor.
     */
    RigidBodyComponent(GameObject* parent);
    RigidBodyComponent();

    ~RigidBodyComponent();
    /**
     * @brief Parameterized constructor.
     * @param mass The mass of the rigid body.
     * @param velocity The initial velocity of the rigid body.
     * @param acceleration The initial acceleration of the rigid body.
     * @param transform Pointer to the associated TransformComponent.
     */
    RigidBodyComponent(GameObject* parent, float mass, Vector2 velocity, Vector2 acceleration);

    /**
     * @brief Updates the rigid body's state.
     *
     * This method should be called each frame to update the velocity and position
     * based on the current acceleration and velocity.
     */
    void Update() override;

    /**
     * @brief Serializes the component's data to a Lua file.
     * @param luaFilePath The path to the Lua file.
     * @param tableName The name of the table in the Lua file.
     */
    void Serialize(const std::string& luaFilePath, const std::string& tableName) override;

    /**
     * @brief Deserializes the component's data from a Lua file.
     * @param luaFilePath The path to the Lua file.
     * @param tableName The name of the table in the Lua file.
     */
    void Deserialize(const std::string& luaFilePath, const std::string& tableName) override;

    /**
     * @brief Provides debug information about the component.
     * @return A string containing debug information.
     */
    std::string DebugInfo() const override;

    /**
     * @brief Gets the mass of the rigid body.
     * @return The mass value.
     */
    const float GetMass() { return mass; }

    /**
     * @brief Sets the mass of the rigid body.
     * @param newMass The new mass value.
     */
    void SetMass(float newMass) { mass = newMass; }

    /**
     * @brief Gets the current velocity of the rigid body.
     * @return The velocity vector.
     */
    const Vector2 GetVelocity() { return velocity; }

    /**
     * @brief Sets the velocity of the rigid body.
     * @param newVelocity The new velocity vector.
     */
    void SetVelocity(const Vector2& newVelocity) { velocity = newVelocity; }

    /**
     * @brief Gets the current acceleration of the rigid body.
     * @return The acceleration vector.
     */
    const Vector2 GetAcceleration() { return acceleration; }

    /**
     * @brief Sets the acceleration of the rigid body.
     * @param newAccel The new acceleration vector.
     */
    void SetAcceleration(const Vector2& newAccel) { acceleration = newAccel; }

    //By Johny, force application
    float GetDrag() { return dragCoefficient; }
    void SetDrag(float newVal) { dragCoefficient = newVal; }
    void ApplyForce(const Vector2& force);
};