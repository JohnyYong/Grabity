/*!****************************************************************
\file: PauseMenuButton.cpp
\author: Teng Shi Heng, shiheng.teng, 2301269

\brief
    This source file implements the PauseMenuButton class, which handles
    button interactions in the pause menu, including restarting the game
    and other related functionalities.

    Shi Heng created and defined all the functions and logic in the update for the Escape button condition(90%)
    Johny added another condition in the update loop for the text ui component (10%)

Copyright (C) 2025 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents
without the prior written consent of DigiPen Institute of
Technology is prohibited.
*******************************************************************/
#include "PauseMenuButton.h"
#include "UISpriteComponent.h"
#include "GameObject.h"

/*!****************************************************************
\brief
    Default constructor for PauseMenuButton.
*******************************************************************/
PauseMenuButton::PauseMenuButton()
{

}

/*!****************************************************************
\brief
    Constructor that assigns the button to a parent GameObject.
\param parent
    Pointer to the parent GameObject.
*******************************************************************/
PauseMenuButton::PauseMenuButton(GameObject* parent) : Component(parent)
{

}

/*!****************************************************************
\brief
    Updates the button's state each frame.
*******************************************************************/
void PauseMenuButton::Update() 
{
    if (InputManager::IsKeyPressed(GLFW_KEY_ESCAPE))
    {
        GameObject* parent = GetParentGameObject();
        UISpriteComponent* trans = parent->GetComponent<UISpriteComponent>(TypeOfComponent::SPRITE_UI);
        if (trans)
        {
            trans->SetIsRenderable(!trans->GetIsRenderable());
        }
    }
    static bool visible = false;
    GameObject* parent = GetParentGameObject();
    
    if (parent)
    {
        if (parent->GetTag() == "FPS") {
            UITextComponent* txt = parent->GetComponent<UITextComponent>(TypeOfComponent::TEXT_UI);

            if (txt)
            {
                if (!visible)
                {
                    txt->SetFont(txt->GetFontType(), 0);
                }
                else
                {
                    txt->SetFont(txt->GetFontType(), 0.5);
                }
            }

            if (InputManager::IsKeyReleased(GLFW_KEY_L))
            {
                if (visible == true)
                {
                    visible = false;
                    ImGuiConsole::Cout("DONT SHOW FPS");
                }
                else
                {
                    visible = true;
                    ImGuiConsole::Cout("SHOW FPS");
                }
            }
        }
    }

    
}

/*!****************************************************************
\brief
    Restarts the game when the button is activated.
*******************************************************************/
void PauseMenuButton::RestartGame()
{
    GameObject* parent = GetParentGameObject();
    UISpriteComponent* trans = parent->GetComponent<UISpriteComponent>(TypeOfComponent::SPRITE_UI);
    if (trans)
        trans->SetIsRenderable(false);
}

/*!****************************************************************
\brief
    Serializes the button's data to a Lua file.
\param luaFilePath
    Path to the Lua script file.
\param tableName
    Name of the Lua table where the data will be stored.
*******************************************************************/
void PauseMenuButton::Serialize(const std::string& luaFilePath, const std::string& tableName) 
{
    LuaManager luaManager(luaFilePath);

    // Define the keys that correspond to the values you're saving
    std::vector<std::string> keys = { "IsScript" };

    LuaManager::LuaValueContainer values = { true };


    luaManager.LuaWrite(tableName, values, keys, "PauseMenuButton");
}

/*!****************************************************************
\brief
    Deserializes the button's data from a Lua file.
\param luaFilePath
    Path to the Lua script file.
\param tableName
    Name of the Lua table from which the data will be loaded.
*******************************************************************/
void PauseMenuButton::Deserialize(const std::string& luaFilePath, const std::string& tableName) 
{
    (void)tableName;
    LuaManager luaManager(luaFilePath);
}

/*!****************************************************************
\brief
    Provides debugging information for the button.
\return
    A string containing debug information.
*******************************************************************/
std::string PauseMenuButton::DebugInfo() const
{
    return std::string();
}