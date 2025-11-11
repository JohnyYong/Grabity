/*!****************************************************************
\file: TextComponent.cpp
\author: Johny Yong Jun Siang, j.yong, 2301301
\co-author: Goh Jun Jie, g.junjie, 2301293
\brief: Contains definitions of functions declared in TextComponent.h. 

        Johny provided the base functions and the DebugInfo() content. (20%)
        Jun Jie provided the lua writing/reading within the Serialize and Deserialize content. (80%)

Copyright (C) 2024 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents
without the prior written consent of DigiPen Institute of
Technology is prohibited.
*******************************************************************!*/
#include "TextComponent.h"

// Default constructor for the TextComponent.
TextComponent::TextComponent() {

}

// Constructs the TextComponent with a parent GameObject.
TextComponent::TextComponent(GameObject* parent)
    : Component(parent), textContent(""), font(""), position({}), fontSize(0.f), rgb({}), alpha(0.f)
{
}

// Constructs the TextComponent with specified parameters.
TextComponent::TextComponent(GameObject* parent, const std::string& text, const std::string& fontType, const Vector2& pos, float textSize, const Vector3& RGB, float newAlpha)
    : Component(parent), textContent(text), font(fontType), position(pos), fontSize(textSize), rgb(RGB), alpha(newAlpha) {
}

// Updates the state of the TextComponent.
void TextComponent::Update()
{
}

// Serializes the TextComponent data to a Lua file.
void TextComponent::Serialize(const std::string& luaFilePath, const std::string& tableName)
{
    (void)tableName;
    (void)luaFilePath;

    LuaManager luaManager(luaFilePath);

    // Define the keys that correspond to the values you're saving
    std::vector<std::string> keys = { "textContent", "font", "positionX", "positionY", "alpha", "RGB_R", "RGB_G", "RGB_B", "fontSize"};
    LuaManager::LuaValueContainer values = { textContent, font, position.x, position.y, alpha, rgb.x, rgb.y, rgb.z, fontSize };

    luaManager.LuaWrite(tableName, values, keys, "Text");
}

// Deserializes the TextComponent data from a Lua file.
void TextComponent::Deserialize(const std::string& luaFilePath, const std::string& tableName)
{
    (void)tableName;
    (void)luaFilePath;
    LuaManager luaManager(luaFilePath);

    textContent = luaManager.LuaReadFromText<std::string>(tableName, "textContent");
    font = luaManager.LuaReadFromText<std::string>(tableName, "font");

    position.x = luaManager.LuaReadFromText<float>(tableName, "positionX");
    position.y = luaManager.LuaReadFromText<float>(tableName, "positionY");

    alpha = luaManager.LuaReadFromText<float>(tableName, "alpha");
    rgb.x = luaManager.LuaReadFromText<float>(tableName, "RGB_R");
    rgb.y = luaManager.LuaReadFromText<float>(tableName, "RGB_G");
    rgb.z = luaManager.LuaReadFromText<float>(tableName, "RGB_B");

    fontSize = luaManager.LuaReadFromText<float>(tableName, "fontSize");
}

// Returns a string containing debug information for the TextComponent.
std::string TextComponent::DebugInfo() const
{
    return "Text Component - Position: (" + std::to_string(position.x) + ", " + std::to_string(position.y) +
        "), Font type: (" + font + "), Size: " + std::to_string(fontSize) +
        ")" + " with content as: " + textContent;
}
