/*!****************************************************************
\file: RigidBodyComponent.cpp
\author: Lee Yu Jie Brandon , l.yujiebrandon , 2301232
\co-author: Johny Yong Jun Siang, j.yong, 2301301 / Goh Jun Jie, g.junjie, 2301293
\brief: Defines the RigidBodyComponent functions, managing mass, velocity, acceleration, and integration with the
        TransformComponent for updating game object positions under Update().

        Brandon provided the constructors. (60%)
        Johny provided the update. (20%)
        Jun Jie provided Serialize and Deserialize functions (20%)

Copyright (C) 2024 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents
without the prior written consent of DigiPen Institute of
Technology is prohibited.
*******************************************************************!*/

#include "RigidBodyComponent.h"
#include "pch.h"
#include "GameObject.h"
#include "LuaConfig.h"

/**
 * @brief Default constructor for RigidBodyComponent.
 *
 * Initializes the component with default values:
 * - mass: 1.0f
 * - velocity: (0.0f, 0.0f)
 * - acceleration: (0.0f, 0.0f)
 * - parentTransform: nullptr
 */
RigidBodyComponent::RigidBodyComponent(GameObject* parent)
    : Component(parent), mass(1.0f), velocity(Vector2(0.0f, 0.0f)), acceleration(Vector2(0.0f, 0.0f))
{
}

/**
 * @brief constructor for RigidBodyComponent.
 */
RigidBodyComponent::RigidBodyComponent()
{
}

/**
 * @brief destructor for RigidBodyComponent.
 */
RigidBodyComponent::~RigidBodyComponent()
{
}

/**
 * @brief Parameterized constructor for RigidBodyComponent.
 *
 * @param mass The mass of the rigid body.
 * @param velocity The initial velocity of the rigid body.
 * @param acceleration The initial acceleration of the rigid body.
 * @param transform Pointer to the associated TransformComponent.
 */
RigidBodyComponent::RigidBodyComponent(GameObject* parent, float mass, Vector2 velocity, Vector2 acceleration)
    : Component(parent), mass(mass), velocity(velocity), acceleration(acceleration)
{
}

/**
 * @brief Serializes the component's data to a Lua file.
 *
 * This method saves the RigidBodyComponent's state (mass, velocity, acceleration)
 * to a Lua file in a table with the specified name using the LuaManager.
 *
 * @param luaFilePath The path to the Lua file.
 * @param tableName The name of the table in the Lua file.
 */
void RigidBodyComponent::Serialize(const std::string& luaFilePath, const std::string& tableName)
{
    LuaManager luaManager(luaFilePath);

    // Define the keys that correspond to the values you're saving
    std::vector<std::string> keys = { "mass", "velocityX", "velocityY", "drag", "accelerationX", "accelerationY" };
    LuaManager::LuaValueContainer values = { mass, velocity.x, velocity.y, dragCoefficient, acceleration.x, acceleration.y };

    // Call LuaWrite once for all properties in the RigidBodyComponent
    luaManager.LuaWrite(tableName, values, keys, "RigidBody");
}

/**
 * @brief Deserializes the component's data from a Lua file.
 *
 * This method loads the RigidBodyComponent's state (mass, velocity, acceleration)
 * from a Lua file, from a table with the specified name using the LuaManager.
 *
 * @param luaFilePath The path to the Lua file.
 * @param tableName The name of the table in the Lua file.
 */
void RigidBodyComponent::Deserialize(const std::string& luaFilePath, const std::string& tableName)
{
    LuaManager luaManager(luaFilePath);

     // Read each component of the RigidBodyComponent
     mass = luaManager.LuaReadFromRigidBody<float>(tableName, "mass");
     velocity.x = luaManager.LuaReadFromRigidBody<float>(tableName, "velocityX");
     velocity.y = luaManager.LuaReadFromRigidBody<float>(tableName, "velocityY");
     acceleration.x = luaManager.LuaReadFromRigidBody<float>(tableName, "accelerationX");
     acceleration.y = luaManager.LuaReadFromRigidBody<float>(tableName, "accelerationY");
     dragCoefficient = luaManager.LuaReadFromRigidBody<float>(tableName, "drag");
	//Clearing warnings...
}

/**
 * @brief Provides debug information about the component.
 *
 * @return A string containing the mass, velocity, and acceleration of the rigid body.
 */
std::string RigidBodyComponent::DebugInfo() const
{
    return "RigidBodyComponent - Mass: " + std::to_string(mass) +
        ", Velocity: (" + std::to_string(velocity.x) + ", " + std::to_string(velocity.y) +
        "), Acceleration: (" + std::to_string(acceleration.x) + ", " + std::to_string(acceleration.y) + ")";
}

/**
 * @brief Updates the rigid body's position based on its velocity.
 *
 */
void RigidBodyComponent::Update()
{

	Engine& engine = Engine::GetInstance();
    // If the object is in knockback, reduce the time left
    if (isInKnockback) {
        //knockbackTimeLeft -= static_cast<float>(InputManager::deltaTime);
        knockbackTimeLeft -= static_cast<float>(engine.fixedDT);
        if (knockbackTimeLeft <= 0.0f) {
            isInKnockback = false;  // End knockback
        }
    }
    Vector2 oppVelocity{ -velocity.x, -velocity.y };
    Vector2 drag = oppVelocity * dragCoefficient;
    ApplyForce(drag);

    //velocity = velocity + (acceleration * (float)InputManager::deltaTime);
    velocity = velocity + (acceleration * static_cast<float>(engine.fixedDT));

    //Reset acceleration for the next frame (forces should be re-applied each update cycle)
    //acceleration = { 0, 0 }; //Already done in PhysisSystem.h
}

/**
 * @brief Applies a force to the rigid body.
 *
 * This method applies a given force to the RigidBodyComponent, updating its
 * acceleration based on the object's mass. The force is divided by mass and
 * added to the current acceleration.
 *
 * @param force The force vector applied to the rigid body.
 */
void RigidBodyComponent::ApplyForce(const Vector2& force)
{
    if (mass > 0) {
        acceleration = acceleration + (force / mass);
    }
}