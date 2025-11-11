/*!****************************************************************
\file: TransformComponent.h
\author: Johny Yong Jun Siang, j.yong, 2301301
\brief: Declares the TransformComponent which includes the transformation data per game object. Provides methods for serialization/de-serialization and
        debugging.

Copyright (C) 2024 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents
without the prior written consent of DigiPen Institute of
Technology is prohibited.
*******************************************************************!*/
#pragma once
#include "Component.h"
#include "ObjectPool.h"
#include "LuaConfig.h"
#include "pch.h"

// TransformComponent: Represents the position and movement of a GameObject.
class TransformComponent : public Component {

    //These are world transforms, decided not to change name so that other places don't change as well
    Vector2 position;  // Position of the GameObject
    Vector2 scale;  // Size of the GameObject
    float rotation;  // Rotation of the GameObject

    Vector2 localPosition;
    Vector2 localScale;
    float localRotation;
    
public:
    // Constructor with position, scale, and rotation
    TransformComponent();
    TransformComponent(GameObject* parent);
    TransformComponent(GameObject* parent, const Vector2& pos, const Vector2& scl, float rot);

    void Update() override;    // Update method to adjust the position based on the velocity
    void Serialize(const std::string& luaFilePath, const std::string& tableName) override;    // Serialize method to save position data to Lua
    void Deserialize(const std::string& luaFilePath, const std::string& tableName) override;
    std::string DebugInfo() const override;

    //Getter Setters
    const Vector2& GetPosition() const { return position; }
    bool isWorldPositionSetByPhysics = false;

    // Setters
    void SetPosition(const Vector2& newPosition) {
        position = newPosition;
        isWorldPositionSetByPhysics = true;
    }

    //getter setters
    const Vector2& GetScale() const { return scale; }
    void SetScale(const Vector2& newScale) { scale = newScale; }
    const float& GetRotation() const { return rotation; }
    void SetRotation(const float& newRotation) { rotation = newRotation; }

    const Vector2& GetLocalPosition() const { return localPosition;  }
    void SetLocalPosition(const Vector2& newPos);
    const Vector2& GetLocalScale() const { return localScale; }
    void SetLocalScale(const Vector2& newScale);
    const float& GetLocalRotation() const { return localRotation; }
    void SetLocalRotation(const float& newRotation);



};
