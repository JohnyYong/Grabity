/*!****************************************************************
\file: PauseMenuButton.h
\author: Teng Shi Heng, shiheng.teng, 2301269

\brief
    This header file defines the PauseMenuButton class, which is a
    component responsible for handling button interactions in the
    pause menu, including restarting the game.

Copyright (C) 2025 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents
without the prior written consent of DigiPen Institute of
Technology is prohibited.
*******************************************************************/
#pragma once

#include "Component.h"
#include "glhelper.h"

/*!****************************************************************
\brief
    The PauseMenuButton class handles button interactions in the
    pause menu, allowing for actions such as restarting the game.
*******************************************************************/
class PauseMenuButton : public Component
{
public:
    /*!****************************************************************
    \brief
        Default constructor for PauseMenuButton.
    *******************************************************************/
    PauseMenuButton();

    /*!****************************************************************
    \brief
        Constructor that assigns the button to a parent GameObject.
    \param parent
        Pointer to the parent GameObject.
    *******************************************************************/
    PauseMenuButton(GameObject* parent);

    /*!****************************************************************
    \brief
        Updates the button's state each frame.
    *******************************************************************/
    void Update() override;

    /*!****************************************************************
    \brief
        Serializes the button's data to a Lua file.
    \param luaFilePath
        Path to the Lua script file.
    \param tableName
        Name of the Lua table where the data will be stored.
    *******************************************************************/
    void Serialize(const std::string& luaFilePath, const std::string& tableName) override;

    /*!****************************************************************
    \brief
        Deserializes the button's data from a Lua file.
    \param luaFilePath
        Path to the Lua script file.
    \param tableName
        Name of the Lua table from which the data will be loaded.
    *******************************************************************/
    void Deserialize(const std::string& luaFilePath, const std::string& tableName) override;

    /*!****************************************************************
    \brief
        Restarts the game when the button is activated.
    *******************************************************************/
    void RestartGame();

    /*!****************************************************************
    \brief
        Provides debugging information for the button.
    \return
        A string containing debug information.
    *******************************************************************/
    std::string DebugInfo() const override;
};
