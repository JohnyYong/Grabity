/*!****************************************************************
\file: CanvasComponent.h
\author: Teng Shi Heng, shiheng.teng, 2301269
\brief:
    The CanvasComponent class manages the canvas functionality
    within a game object. It provides methods for updating the
    canvas state, as well as serialization and deserialization
    for saving and loading canvas component data.

Copyright (C) 2024 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents
without the prior written consent of DigiPen Institute of
Technology is prohibited.
*******************************************************************!*/

#pragma once

#include "Component.h"

/*!****************************************************************
\class CanvasComponent
\brief
    Manages the canvas functionality for a game object, including
    update, serialization, deserialization, and debug information.
*******************************************************************!*/
class CanvasComponent : public Component
{
public:
    /*!****************************************************************
    \brief
        Default constructor for CanvasComponent.
    *******************************************************************!*/
    CanvasComponent();

    /*!****************************************************************
    \brief
        Constructor for CanvasComponent with parent GameObject.
    \param parent
        Pointer to the parent GameObject.
    *******************************************************************!*/
    CanvasComponent(GameObject* parent);

    /*!****************************************************************
    \brief
        Destructor for CanvasComponent.
    *******************************************************************!*/
    ~CanvasComponent();

    /*!****************************************************************
    \brief
        Updates the canvas component state.
    *******************************************************************!*/
    void Update() override;

    /*!****************************************************************
    \brief
        Serializes the canvas component data to a Lua file.
    \param luaFilePath
        Path to the Lua file.
    \param tableName
        Name of the table in the Lua file.
    *******************************************************************!*/
    void Serialize(const std::string& luaFilePath, const std::string& tableName) override;

    /*!****************************************************************
    \brief
        Deserializes the canvas component data from a Lua file.
    \param luaFilePath
        Path to the Lua file.
    \param tableName
        Name of the table in the Lua file.
    *******************************************************************!*/
    void Deserialize(const std::string& luaFilePath, const std::string& tableName) override;

    /*!****************************************************************
    \brief
        Returns debug information for the canvas component.
    \return
        Debug information string.
    *******************************************************************!*/
    std::string DebugInfo() const override;

private:
    bool isCanvas; // Indicates if the component is a canvas.
};
