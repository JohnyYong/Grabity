/*!****************************************************************
\file: UITextComponent.cpp
\author: Teng Shi Heng, shiheng.teng, 2301269
\co-author: Goh Jun Jie, g.junjie, 2301293
\brief:
    The UITextComponent class function definition

    Shi Heng created the file and functions needed for this class. (50%)
    Jun Jie provided the serialize and deserialize codes. (50%)

Copyright (C) 2024 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents
without the prior written consent of DigiPen Institute of
Technology is prohibited.
*******************************************************************!*/
#include "UITextComponent.h"
#include "TransformComponent.h"
#include "LuaConfig.h"
#include "GameObject.h"
//  Default constructor for UITextComponent.
UITextComponent::UITextComponent()
    : textContent(""), font(""), position({}), fontSize(0.f), rgb({}), alpha(0.f)
{
}

//  Constructor for UITextComponent with parent GameObject.
UITextComponent::UITextComponent(GameObject* parent)
    : Component(parent), textContent(""), font(""), position({}), fontSize(0.f), rgb({}), alpha(0.f)
{
}

// Constructor for UITextComponent with detailed initialization.
UITextComponent::UITextComponent(GameObject* parent, const std::string& text, const std::string& fontType, const Vector2& pos, float textSize, const Vector3& RGB, float newAlpha)
    : Component(parent), textContent(text), font(fontType), position(pos), fontSize(textSize), rgb(RGB), alpha(newAlpha) {
}

//  Destructor for UITextComponent.
UITextComponent::~UITextComponent()
{
}

//  Updates the text component state.
void UITextComponent::Update()
{
    auto* parent = GetParentGameObject();
    if (parent) {
        auto* transfrom = parent->GetComponent<TransformComponent>(TypeOfComponent::TRANSFORM);
        SetPosition(transfrom->GetLocalPosition());
    }
}

// Serializes the text component data to a Lua file.
void UITextComponent::Serialize(const std::string& luaFilePath, const std::string& tableName)
{
    (void)tableName;
    (void)luaFilePath;

    LuaManager luaManager(luaFilePath);

    // Define the keys that correspond to the values you're saving
    std::vector<std::string> keys = { "textContent", "font", "positionX", "positionY", "alpha", "RGB_R", "RGB_G", "RGB_B", "fontSize" };
    LuaManager::LuaValueContainer values = { textContent, font, position.x, position.y, alpha, rgb.x, rgb.y, rgb.z, fontSize };

    luaManager.LuaWrite(tableName, values, keys, "TextUI");
}

// Deserializes the text component data from a Lua file.
void UITextComponent::Deserialize(const std::string& luaFilePath, const std::string& tableName)
{
    (void)tableName;
    (void)luaFilePath;
    LuaManager luaManager(luaFilePath);

    textContent = luaManager.LuaReadFromTextUI<std::string>(tableName, "textContent");
    font = luaManager.LuaReadFromTextUI<std::string>(tableName, "font");

    position.x = luaManager.LuaReadFromTextUI<float>(tableName, "positionX");
    position.y = luaManager.LuaReadFromTextUI<float>(tableName, "positionY");

    alpha = luaManager.LuaReadFromTextUI<float>(tableName, "alpha");
    rgb.x = luaManager.LuaReadFromTextUI<float>(tableName, "RGB_R");
    rgb.y = luaManager.LuaReadFromTextUI<float>(tableName, "RGB_G");
    rgb.z = luaManager.LuaReadFromTextUI<float>(tableName, "RGB_B");

    fontSize = luaManager.LuaReadFromTextUI<float>(tableName, "fontSize");
}

//  Returns debug information for the text component.
std::string UITextComponent::DebugInfo() const
{
    return "Text Component - Position: (" + std::to_string(position.x) + ", " + std::to_string(position.y) +
        "), Font type: (" + font + "), Size: " + std::to_string(fontSize) +
        ")" + " with content as: " + textContent;
}
