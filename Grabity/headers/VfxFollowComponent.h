/*!****************************************************************
\file: VfxFollowComponent.h
\author: Teng Shi Heng, shiheng.teng, 2301269
\brief  Header file for the VfxFollowComponent class that makes 
        a component follow a target GameObject.

Copyright (C) 2025 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents
without the prior written consent of DigiPen Institute of
Technology is prohibited.
*******************************************************************/
#pragma once
#include "Component.h"
#include "GameObject.h"
#include "LuaConfig.h"

/*!****************************************************************
\class VfxFollowComponent
\brief A component that allows a visual effect (VFX) to follow a target GameObject.
*******************************************************************/
class VfxFollowComponent : public Component
{
    GameObject* target = nullptr;  /*!< Pointer to the target GameObject that the VFX follows */

public:
    /*!****************************************************************
    \brief Default constructor.
    \param parent The parent GameObject to which this component will be attached. Defaults to nullptr.
    *******************************************************************!*/
    VfxFollowComponent() : Component(nullptr) {}

    /*!****************************************************************
    \brief Constructor that initializes the component with a parent GameObject.
    \param parent The parent GameObject to which this component will be attached.
    *******************************************************************!*/
    VfxFollowComponent(GameObject* parent) : Component(parent) {}

    /*!****************************************************************
    \brief Sets the target GameObject for the visual effect to follow.
    \param followTarget The GameObject that the VFX will follow.
    *******************************************************************!*/
    void SetTarget(GameObject* followTarget) { target = followTarget; }

    /*!****************************************************************
    \brief Updates the VfxFollowComponent.
    *******************************************************************!*/
    void Update() override;

    /*!****************************************************************
    \brief Serializes the VfxFollowComponent's properties to a Lua file.
    \param luaFilePath The file path to the Lua file where the data will be saved.
    \param tableName The name of the table in the Lua file where the data will be written.
    *******************************************************************!*/
    void Serialize(const std::string& luaFilePath, const std::string& tableName) override {
        LuaManager luaManager(luaFilePath);

        // Define keys and values for serialization
        std::vector<std::string> keys = {
            "Type",
        };
        LuaManager::LuaValueContainer values = {
            //static_cast<int>(type),
        };

        // Call the LuaWrite function with the keys and values
        luaManager.LuaWrite(tableName, values, keys, "VfxFollowComponent");
    }

    /*!****************************************************************
    \func  Deserialize
    \brief Deserializes the VfxFollowComponent's properties from a Lua file.
    \param luaFilePath The file path to the Lua file where the data will be loaded from.
    \param tableName The name of the table in the Lua file where the data will be read from.
    *******************************************************************!*/
    void Deserialize(const std::string& luaFilePath, const std::string& tableName) override {
        //LuaManager luaManager(luaFilePath);
        (void)luaFilePath;
        (void)tableName;
    }

    /*!****************************************************************
    \brief Returns a string with debug information about the VfxFollowComponent.
    \return A string containing debug information.
    *******************************************************************!*/
    std::string DebugInfo() const override {
        return "VfxFollowComponent Debug Info";
    }
};
