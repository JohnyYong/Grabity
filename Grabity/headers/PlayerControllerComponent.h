/*!****************************************************************
\file: PlayerControllerComponent.h
\author: Goh Jun Jie, g.junjie, 2301293
\co-author: Johny Yong Jun Siang, j.yong, 2301301
\brief: Contains declarions for:
        Serialize and Deserialize functions
        Movement and Knockback functions
        Controls of the playerHand object such as:
          - Transformations
          - Grabbing/Dragging enemy objects
          - Direction following mouse position
        Contains data of held object/enemy which transformation updates based on playerHand's transformation as well.


        Jun Jie contributed (30%) of the code with player movements and serialization/deserialization.
        Johny contributed (70%) of the code with playerHand controls/transformations and shooting and grabbing of interactable objects

Copyright (C) 2024 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents
without the prior written consent of DigiPen Institute of
Technology is prohibited.
*******************************************************************!*/

#pragma once
#include "Component.h"
#include "Vector2.h"
#include "pch.h"
#include "ParticleSystem.h"
#include "RigidBodyComponent.h"
#include "SpriteComponent.h"

class PlayerControllerComponent : public Component {
public:

    //Constructors
    PlayerControllerComponent();
    PlayerControllerComponent(GameObject* parent);
    PlayerControllerComponent(GameObject* parent, float moveSpeed, RigidBodyComponent* rigidBody, SpriteComponent* sprite);

    ~PlayerControllerComponent();
    //Inherited functions from component.h
    void Serialize(const std::string& luaFilePath, const std::string& tableName) override;
    void Deserialize(const std::string& luaFilePath, const std::string& tableName) override;
    void Update() override;
    std::string DebugInfo() const override;

    //All below are getter setters
    const float& GetMoveSpd() const { return moveSpeed; }
    void SetMoveSpd(float newSpeed) { moveSpeed = newSpeed; }

    //Added in M3 for player hand controls
    void SetHandOffset(float newOffset) { handOffset = newOffset; }
    const float& GetHandOffset() { return handOffset; }
    //Added in M3 for player hand controls
    void SetHeldOBJ(GameObject* obj) { heldObject = obj; }
    GameObject* GetHeldOBJ() { return heldObject; }
    //Added in M3 for player hand controls
    void SetDraggingOBJ(GameObject* dragOBJ) { draggingObject = dragOBJ; }
    GameObject* GetDraggingOBJ() { return draggingObject; }

    void SetPrevDraggingOBJ(GameObject* obj) { prevDraggingObject = obj; }
    GameObject* GetPrevDraggingOBJ() { return prevDraggingObject; }

    void SetCurrenlyColliding(GameObject* obj) { getCurrentlyColliding = obj; }
    GameObject* GetCurrentlyColliding() { return getCurrentlyColliding; }

    void SetSuctionTime(float time) { suctionTime = time; }
    float GetSuctionTime() { return suctionTime; }
    bool playBGM = false;
    bool changeBGM = false;


private:
    float playerLifetime = 0.0f;

    float moveSpeed;
    RigidBodyComponent* playerRigidBody = nullptr;
    SpriteComponent* playerSprite = nullptr;

    GameObject* suctionVFX; // Particle system for suction effect
    bool particleSystemInitialized;   // Flag to track if we've initialized the particles
    std::vector<GameObject*> trailingVFX; // Particle system for suction effect

    // // Knockback-related attributes
    // bool inKnockback = false;
    // float knockbackDuration = 0.2f; // Duration of knockback in seconds
    // float knockbackTimer = 0.0f;
    // Vector2 knockbackVelocity = { 0.0f, 0.0f }; // Velocity applied during knockback
    //Added in M3 for player hand controls
    GameObject* playerHand = nullptr;
    GameObject* draggingObject = nullptr;
    GameObject* prevDraggingObject = nullptr;
    GameObject* heldObject = nullptr;
    GameObject* getCurrentlyColliding = nullptr;

    bool setHand = false;
    float handOffset = { 300.f};
    float heldOBJOffset = { 200.f };
    float suctionTime = 0.0f;
    int randomFootstepID = 20;

};