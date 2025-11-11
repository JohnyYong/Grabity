/*!****************************************************************
\file:      ButtonComponent.h
\author:    Ridhwan Mohamed Afandi, mohamedridhwan.b, 2301367
\brief:     Button component for handling button functionality.
\details:   Contains the definition for ButtonComponent struct which 
            is inherited from Component Class. The component stores
            a functiontype for the button for a variety of functions 
            such as Loadnextscene and Exit.  These functions are 
            found in Engine class which is then called from updateUI()

Copyright (C) 2024 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents
without the prior written consent of DigiPen Institute of
Technology is prohibited. 
*******************************************************************!*/
#pragma once

#include "Component.h"
#include "GLHelper.h"
#include "LuaConfig.h"
#include "Engine.h"

enum class ButtonFunctionType
{
    LOAD_NEXT_SCENE = 0,
    EXIT_GAME = 1,
    PLACEHOLDER = 2,
    RESTART_LEVEL = 3,
    RESUME_GAME = 4,
    ACHIEVEMENT = 5,
    ACHIEVEMENTBACK = 6,
    OPTIONS = 7,
    SLIDER = 8,
    HOWTOPLAY = 9,
    SOUND_POPUP = 10,
    CHANGEIMAGEFORWARD = 11,
    CHANGEIMAGEBACKWARD = 12,
};

struct ButtonComponent : public Component
{
    ButtonComponent() : Component(nullptr), m_functionType(ButtonFunctionType::LOAD_NEXT_SCENE) {}
    ButtonComponent(GameObject* parent) : Component(parent), m_functionType(ButtonFunctionType::LOAD_NEXT_SCENE) {}


	ButtonFunctionType m_functionType;
	std::string pathNextScene;

    // Override virtual functions from Component
    void Update() override {
    }

/*!****************************************************************
\func Serialize
\brief Serializes the button component to a Lua file.
\param luaFilePath The path to the Lua file.
\param tableName The name of the table in the Lua file.
*******************************************************************!*/

    void Serialize(const std::string& luaFilePath, const std::string& tableName) {
        LuaManager luaManager(luaFilePath);

        // Define keys and values for serialization
        std::vector<std::string> keys = {
            "FunctionType",
            "PathNextScene"
        };
        LuaManager::LuaValueContainer values = {
            static_cast<int>(m_functionType),
            pathNextScene
        };

        // Call the LuaWrite function with the keys and values
        luaManager.LuaWrite(tableName, values, keys, "ButtonComponent");
    }

/*!****************************************************************
\func Deserialize
\brief Deserializes the button component from a Lua file.
\param luaFilePath The path to the Lua file.
\param tableName The name of the table in the Lua file.
*******************************************************************!*/

    void Deserialize(const std::string& luaFilePath, const std::string& tableName) {
        LuaManager luaManager(luaFilePath);

        // Read the function type and path next scene from Lua
        m_functionType = static_cast<ButtonFunctionType>(luaManager.LuaRead<int>(tableName, { "ButtonComponent", "FunctionType" }));
        pathNextScene = luaManager.LuaRead<std::string>(tableName, { "ButtonComponent", "PathNextScene" });
    }

    std::string DebugInfo() const override {
        return "ButtonComponent Debug Info";
    }

    bool sfxPlay = false;
};
