/*!****************************************************************
\file:      PhysicsSystem.h
\author:    Lee Yu Jie Brandon , l.yujiebrandon , 2301232

\brief:     This header file defines the PhysicsSystem class, which is responsible 
            for handling physics calculations and updates for game objects. The 
            system applies gravitational forces between objects and updates their 
            positions based on velocity and acceleration. It includes methods for 
            applying gravitational forces and capping acceleration to prevent 
            excessive speeds, forming a core part of the game's physics engine.

Copyright (C) 2024 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents
without the prior written consent of DigiPen Institute of
Technology is prohibited.
*******************************************************************!*/
#pragma once
#include "System.h"
#include "GameObjectFactory.h"
#include "RigidBodyComponent.h"
#include "TransformComponent.h"
#include <cmath>

#ifdef _IMGUI
#include <iostream>
#endif // _IMGUI // Include for debug output
#include <GLHelper.h>
/**
 * @class PhysicsSystem
 * @brief Handles physics calculations and updates for game objects.
 *
 * This system applies gravitational forces between objects and updates
 * their positions based on velocity.
 */
class PhysicsSystem : public System {
public:
    void Update() override {
        GameObjectFactory& factory = GameObjectFactory::GetInstance();
        auto gameObjects = factory.GetAllGameObjects();

        //// Apply gravitational forces between objects
        //for (auto& [id1, obj1] : gameObjects) {
        //    RigidBodyComponent* rb1 = obj1->GetComponent<RigidBodyComponent>(TypeOfComponent::RIGIDBODY);
        //    GravityComponent* grav1 = obj1->GetComponent<GravityComponent>(TypeOfComponent::GRAVITY);
        //    TransformComponent* trans1 = obj1->GetComponent<TransformComponent>(TypeOfComponent::TRANSFORM);

        //    if (rb1 && grav1 && trans1) {
        //        for (auto& [id2, obj2] : gameObjects) {
        //            if (id1 == id2) continue; // Skip self-interaction
        //            RigidBodyComponent* rb2 = obj2->GetComponent<RigidBodyComponent>(TypeOfComponent::RIGIDBODY);
        //            TransformComponent* trans2 = obj2->GetComponent<TransformComponent>(TypeOfComponent::TRANSFORM);

        //            if (rb2 && trans2) {
        //                // Apply gravitational force directly to rb2
        //                ApplyGravitationalForce(trans1, rb2, trans2);
        //            }
        //        }
        //    }
        //}
        Engine& engine = Engine::GetInstance();
        std::vector<std::string> allActiveLayer = LayerManager::GetInstance().GetActiveLayers();
        for (int step = 0; step < engine.currentNumberOfSteps; ++step) {
            // Update each game object's RigidBodyComponent and TransformComponent
            for (std::string str : allActiveLayer)
            {
                std::vector<GameObject*> layer = LayerManager::GetInstance().GetSpecifiedLayer(str);
                for (GameObject* obj : layer)
                {
                    RigidBodyComponent* rb = obj->GetComponent<RigidBodyComponent>(TypeOfComponent::RIGIDBODY);
                    TransformComponent* trans = obj->GetComponent<TransformComponent>(TypeOfComponent::TRANSFORM);
                    if (rb && trans) {
                        rb->Update();
                        rb->SetVelocity(rb->GetVelocity() + rb->GetAcceleration() * static_cast<float>(engine.fixedDT) * static_cast<float>(engine.currentNumberOfSteps));
                        Vector2 newWorldPosition = trans->GetPosition() + rb->GetVelocity() * static_cast<float>(engine.fixedDT) * static_cast<float>(engine.currentNumberOfSteps);

                        // Recalculate the local position relative to the parent
                        GameObject* parent = trans->GetParentGameObject()->GetParent();
                        if (!parent) {
                            trans->SetLocalPosition(newWorldPosition);
                        }

                        // Reset acceleration for next frame
                        rb->SetAcceleration({ 0, 0 });
                    }
                }
            }
        }
    }

private:
    ///**
    // * @brief Applies gravitational force between two objects.
    // *
    // * @param trans1 TransformComponent of the first object (source).
    // * @param rb2 RigidBodyComponent of the second object (target).
    // * @param trans2 TransformComponent of the second object.
    // */
    //void ApplyGravitationalForce(TransformComponent* trans1, RigidBodyComponent* rb2, TransformComponent* trans2) {
    //    const float pullStrength = 1000.f; // Gravitational pull strength
    //    Vector2 direction = trans1->GetPosition() - trans2->GetPosition();
    //    Vector2 gravitationalForce = direction.Normalize() * pullStrength;

    //    // Apply gravitational force directly to the RigidBodyComponent
    //    rb2->ApplyForce(gravitationalForce);
    //}

    ///**
    // * @brief Caps the acceleration of an object to prevent excessive speeds.
    // *
    // * @param rb The RigidBodyComponent of the object to cap.
    // */
    //void CapAcceleration(RigidBodyComponent* rb) {
    //    const float maxAcceleration = 100.f; // Maximum acceleration in units per second squared
    //    if (rb->GetAcceleration().Length() > maxAcceleration) {
    //        rb->SetAcceleration(rb->GetAcceleration().Normalize() * maxAcceleration);
    //    }
    //}
};