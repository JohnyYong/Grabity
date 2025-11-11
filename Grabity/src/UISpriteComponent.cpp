/*!****************************************************************
\file: UISpriteComponent.cpp
\author: Teng Shi Heng, shiheng.teng, 2301269
\co-author: Goh Jun Jie, g.junjie, 2301293
\brief:
    The UISpriteComponent class function definition

    Shi Heng created the file and functions needed for this class. (50%)
    Jun Jie provided the serialize and deserialize codes. (50%)

Copyright (C) 2024 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents
without the prior written consent of DigiPen Institute of
Technology is prohibited.
*******************************************************************!*/
#include "UISpriteComponent.h"
#include "assetmanager.h"
#include "Engine.h"

// Default constructor for UISpriteComponent.
UISpriteComponent::UISpriteComponent()
    : flipX(false), flipY(false), layer(0),
    colorUI({ 1.0f, 1.0f, 1.0f, 1.0f }), renderableUI(true)
{
    sprite = std::make_unique<SpriteAnimation>(AssetManager::GetInstance().GetSprite("Animation_Ame"));
}

// Constructor for UISpriteComponent with parent GameObject.
UISpriteComponent::UISpriteComponent(GameObject* parent)
    : Component(parent), flipX(false), flipY(false), layer(0),
    colorUI({ 1.0f, 1.0f, 1.0f, 1.0f }), renderableUI(true)
{
    sprite = std::make_unique<SpriteAnimation>(AssetManager::GetInstance().GetSprite("Animation_Ame"));


}

//  Destructor for UISpriteComponent.
UISpriteComponent::~UISpriteComponent()
{
}

// Updates the sprite component state.
void UISpriteComponent::Update()
{
    if (!Engine::GetInstance().isPaused)
    {
        accumulatedTime += Engine::GetInstance().fixedDT;
        sprite.get()->UpdateSprite(accumulatedTime);
    }
}

// Serializes the sprite component data to a Lua file.
void UISpriteComponent::Serialize(const std::string& luaFilePath, const std::string& tableName)
{
    // Serialization testing, save into lua
    LuaManager luaManager(luaFilePath);

    std::vector<std::string> keys;
    LuaManager::LuaValueContainer values;

    keys.push_back("SpritePathName");
    values.push_back(sprite->GetSpriteTexture()->codeName);

    keys.push_back("SpriteNxFrame");
    values.push_back(sprite->GetSpriteTexture()->GetNxFrames());

    keys.push_back("SpriteNyFrame");
    values.push_back(sprite->GetSpriteTexture()->GetNyFrames());

    keys.push_back("FramesPerSecond");
    values.push_back(sprite->GetSpriteTexture()->GetFramePs());

    keys.push_back("TotalFrame");
    values.push_back(sprite->GetSpriteTexture()->GetTotalFrames());

    keys.push_back("Spritelayer");
    values.push_back(layer);

    keys.push_back("SpriteFlipX");
    values.push_back(flipX);

    keys.push_back("SpriteFlipY");
    values.push_back(flipY);

    keys.push_back("Renderable");
    values.push_back(renderableUI);


    luaManager.LuaWrite(tableName, values, keys, "SpriteUI");
}

// Deserializes the sprite component data from a Lua file.
void UISpriteComponent::Deserialize(const std::string& luaFilePath, const std::string& tableName)
{
    //Load from lua
    LuaManager luaManager(luaFilePath);

    std::string texturePath = luaManager.LuaReadFromSpriteUI<std::string>(tableName, "SpritePathName");
    sprite = std::make_unique<SpriteAnimation>(AssetManager::GetInstance().GetSprite(texturePath));

    layer = luaManager.LuaRead<int>(tableName, { "SpriteUI", "Spritelayer" });
    flipX = luaManager.LuaRead<bool>(tableName, { "SpriteUI", "SpriteFlipX" });
    flipY = luaManager.LuaRead<bool>(tableName, { "SpriteUI", "SpriteFlipY" });
    sprite->GetSpriteTexture()->SetNxFrames((int)luaManager.LuaRead<float>(tableName, { "SpriteUI", "SpriteNxFrame" }));
    sprite->GetSpriteTexture()->SetNyFrames((int)luaManager.LuaRead<float>(tableName, { "SpriteUI", "SpriteNyFrame" }));
    sprite->GetSpriteTexture()->SetFramePs(luaManager.LuaRead<double>(tableName, { "SpriteUI", "FramesPerSecond" }));
    sprite->GetSpriteTexture()->SetTotalFrames((int)luaManager.LuaRead<float>(tableName, { "SpriteUI", "TotalFrame" }));

    renderableUI = luaManager.LuaRead<bool>(tableName, { "SpriteUI", "Renderable" });

}

//  Returns debug information for the sprite component.
std::string UISpriteComponent::DebugInfo() const
{
    return std::string();
}