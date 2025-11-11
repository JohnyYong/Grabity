/*!****************************************************************
\file:      UIComponent.h
\author:    Ridhwan Mohamed Afandi, mohamedridhwan.b, 2301367
\co-author: Teng Shi Heng, shiheng.teng,  2301269
\brief:     UIComponent for keeping track of UI elements in the game world.
\details:   Implements the UIComponent struct to keep track of UI elements
			such as health, bar, status, timer, and FPS counter.

            Shi heng completed (5%) of the code: Health bar display
            Ridhwan completed (95%) of the code: All rest of the features

Copyright (C) 2024 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents
without the prior written consent of DigiPen Institute of
Technology is prohibited.
*******************************************************************!*/



#pragma once

#include "Component.h"
#include "LuaConfig.h"

enum class UIComponentType
{
    Health = 0,
    Bar = 1,
    Status = 2,
    Timer = 3,
    FPS = 4,
    PopUp = 5,
    PopUpLeftRight = 6,
};

struct UIComponent : public Component
{
    UIComponent() : Component(nullptr), type(UIComponentType::Health) {}
    UIComponent(GameObject* parent) : Component(parent), type(UIComponentType::Health) {}
    UIComponent(GameObject* parent, UIComponentType type) : Component(parent), type(type) {}

    UIComponentType type;
    Vector2 originalSize{ 0.f,0.f };

    // Override virtual functions from Component
    void Update() override {
    }
/*!****************************************************************
\func  Serialize
\brief Serializes the UIComponent to a Lua file.
\param luaFilePath The file path to the Lua file.
\param tableName The name of the table in the Lua file.
*******************************************************************!*/
    void Serialize(const std::string& luaFilePath, const std::string& tableName) override {
        LuaManager luaManager(luaFilePath);

        // Define keys and values for serialization
        std::vector<std::string> keys = {
            "Type",
        };
        LuaManager::LuaValueContainer values = {
            static_cast<int>(type),
        };

        // Call the LuaWrite function with the keys and values
        luaManager.LuaWrite(tableName, values, keys, "UIComponent");
    }
/*!****************************************************************
\func  Deserialize
\brief Deserializes the UIComponent from a Lua file.
\param luaFilePath The file path to the Lua file.
\param tableName The name of the table in the Lua file.
*******************************************************************!*/
    void Deserialize(const std::string& luaFilePath, const std::string& tableName) override {
        LuaManager luaManager(luaFilePath);

        // Read the type and additional info from Lua
        type = static_cast<UIComponentType>(luaManager.LuaRead<int>(tableName, { "UIComponent", "Type"}));
    }

/*!****************************************************************
\func  DebugInfo
\brief Returns a string with debug information about the UIComponent.
\return A string with debug information.
*******************************************************************!*/
    std::string DebugInfo() const override {
        return "UIComponent Debug Info";
    }
};

