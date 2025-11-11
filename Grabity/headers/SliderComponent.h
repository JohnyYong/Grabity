/*!****************************************************************
\file: SliderComponent.h
\author: Goh Jun Jie, 2301293
\co-author:
\brief: Declares the functions defined in SliderComponent.cpp. 

Copyright (C) 2024 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents
without the prior written consent of DigiPen Institute of
Technology is prohibited.
*******************************************************************!*/

#pragma once

#include "Component.h"
#include "LuaConfig.h"

enum VolumeType {
    MASTER,
    BGM, 
    SFX
};

/*!
 * \class SliderComponent
 * \brief A component that represents a UI slider.
 *
 * This component allows a GameObject to have a slider with a defined range and value.
 * It supports serialization to and deserialization from Lua files.
 */
class SliderComponent : public Component
{
public:
    /*!****************************************************************
    * \brief Constructor
    *********************************************************************/
    SliderComponent(GameObject* parent);
    /*!****************************************************************
    * \brief Constructor
    *********************************************************************/
    SliderComponent();


    float currentValue;
    float handleOffset = 0.0f;
    float sliderWidth = 200.0f;
    bool isDragging = false;
    float minPosX;
    float maxPosX;
    float minPosXOffset;
    GameObject* parent;
    VolumeType volumeType;

    /*!****************************************************************
    * \brief Updates the slider's state each frame.
    *
    * Handles user interaction and updates the slider's value accordingly.
    *********************************************************************/
    void Update() override;

    /*!****************************************************************
    \func Serialize
    \brief Serializes the slider component to a Lua file.
    *******************************************************************!*/
    void Serialize(const std::string& luaFilePath, const std::string& tableName);

    /*!****************************************************************
    \func Deserialize
    \brief Deserializes the slider component from a Lua file.
    *******************************************************************!*/
    void Deserialize(const std::string& luaFilePath, const std::string& tableName);

    std::string DebugInfo() const override {
        return "SliderComponent Debug Info";
    }

private:
};