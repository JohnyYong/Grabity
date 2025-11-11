/*!****************************************************************
\file: TransformComponent.cpp
\author: Johny Yong Jun Siang, j.yong, 2301301
\co-author: Goh Jun Jie, g.junjie, 2301293
\brief: Contains definitions of functions declared in TransformComponent.h. Serialization and De-Serialization function is provided.

        Johny provided the base functions and the DebugInfo() content. (20%)
        Jun Jie provided the lua writing/reading within the Serialize and Deserialize content. (80%)

Copyright (C) 2024 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents
without the prior written consent of DigiPen Institute of
Technology is prohibited.
*******************************************************************!*/
#include "TransformComponent.h"
#include "GameObject.h"



TransformComponent::TransformComponent()
{
}

TransformComponent::TransformComponent(GameObject* parent) : Component(parent), position({}), scale({1.f, 1.f}), rotation(0.f)
, localPosition({}), localScale({ 1.f, 1.f }), localRotation(0.f)
{
}

TransformComponent::TransformComponent(GameObject* parent, const Vector2& pos, const Vector2& scl, float rot)
    : Component(parent), position(pos), scale(scl), rotation(rot), localPosition(pos), localScale(scl), localRotation(rot)
{
}


void TransformComponent::SetLocalPosition(const Vector2& newPos) { localPosition = newPos; GetParentGameObject()->UpdateWorldTransform(); }


void TransformComponent::SetLocalScale(const Vector2& newScale) { localScale = newScale; GetParentGameObject()->UpdateWorldTransform(); }


void TransformComponent::SetLocalRotation(const float& newRot) {
    localRotation = newRot; GetParentGameObject()->UpdateWorldTransform();
}

void TransformComponent::Update() {
    
    //No such thing game object must have transform component

    /*if (!GetActive())
    {
        return;
    }*/
}

// Serializes the position data to a Lua file
void TransformComponent::Serialize(const std::string& luaFilePath, const std::string& tableName) {
    LuaManager luaManager(luaFilePath);

    // Define the keys that correspond to the values you're saving
    std::vector<std::string> keys = { "positionX", "positionY", "scaleX", "scaleY", "rotation" };

    LuaManager::LuaValueContainer values = { localPosition.x, localPosition.y, localScale.x, localScale.y, rotation };


    luaManager.LuaWrite(tableName, values, keys, "Transform");
}

//Loading
void TransformComponent::Deserialize(const std::string& luaFilePath, const std::string& tableName)
{
    LuaManager luaManager(luaFilePath);


    // Read position
    localPosition.x = luaManager.LuaReadFromTransform<float>(tableName, "positionX");
    localPosition.y = luaManager.LuaReadFromTransform<float>(tableName, "positionY");

    // Read scale
    localScale.x = luaManager.LuaReadFromTransform<float>(tableName, "scaleX");
    localScale.y = luaManager.LuaReadFromTransform<float>(tableName, "scaleY");

    // Read rotation
    localRotation = luaManager.LuaReadFromTransform<float>(tableName, "rotation");

}

std::string TransformComponent::DebugInfo() const
{
    return "TransformComponent - Position: (" + std::to_string(position.x) + ", " + std::to_string(position.y) +
        "), Scale: (" + std::to_string(scale.x) + ", " + std::to_string(scale.y) + 
        "), Rotation: " + std::to_string(rotation);
}