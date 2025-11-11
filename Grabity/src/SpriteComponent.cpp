/*!****************************************************************
\file: SpriteComponent.cpp
\author: Johny Yong Jun Siang, j.yong, 2301301
\co-author: Teng Shi Heng, shiheng.teng, 2301269
\co-author: Goh Jun Jie, g.junjie, 2301293
\brief: Declares the SpriteComponent class for managing sprite rendering and animations. It supports multiple animation
        states, texture flipping, and layering. The class integrates with the TransformComponent for positioning and provides
        serialization, deserialization, and debugging.

        Johny first provided initial functions of the file (10%)
        Shi Heng updates the functions to support sprite animations (70%)
        Jun Jie filled up the Serialize and De-Serialize datas (20%)

Copyright (C) 2024 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents
without the prior written consent of DigiPen Institute of
Technology is prohibited.
*******************************************************************!*/
#include "SpriteComponent.h"
#include "assetmanager.h"
#include "Engine.h"

// Default constructor for the SpriteComponent.
SpriteComponent::SpriteComponent()
    : texturePath(""), layer(0), flipX(0), flipY(0), accumulatedTime(0)
{
}

// Constructs the SpriteComponent with a parent GameObject.
SpriteComponent::SpriteComponent(GameObject* parent) : Component(parent), texturePath(""), layer(0), flipX(0), flipY(0), accumulatedTime(0)
{
}

// Retrieves the currently active sprite animation.
SpriteAnimation* SpriteComponent::GetCurrentSprite()
{
    return spriteAnimations.get();
}

// Constructs the SpriteComponent with specified parameters.
SpriteComponent::SpriteComponent(GameObject* parent, std::unique_ptr<SpriteAnimation> animation, int lyr, bool fx, bool fy)
    : Component(parent), layer(lyr), flipX(fx), flipY(fy)
{

    // using std move to create the new sprite animation transfer it into the
    // variable stored in this class
    spriteAnimations = std::move(animation);
}

// change the current sprite
void SpriteComponent::ChangeSprite(std::unique_ptr<SpriteAnimation> animation)
{
    // using std move to create the new sprite animation transfer it into the
    // variable stored in this class
    spriteAnimations = std::move(animation);
}

// Destructor for the SpriteComponent.
SpriteComponent::~SpriteComponent()
{
}

// Updates the state of the SpriteComponent.
void SpriteComponent::Update() {
    if (!Engine::GetInstance().isPaused && spriteAnimations) {
        // Normal animation update with speed multiplier
        accumulatedTime += Engine::GetInstance().fixedDT * animationSpeedMultiplier;
        spriteAnimations->UpdateSprite(accumulatedTime);
    }
}


// Serializes the SpriteComponent data to a Lua file.
void SpriteComponent::Serialize(const std::string& luaFilePath, const std::string& tableName)
{
    // Serialization testing, save into lua
    LuaManager luaManager(luaFilePath);


    //int totalID = maxID;
    std::vector<std::string> keys;
    LuaManager::LuaValueContainer values;

    // Define keys for each collider using indexed keys
    keys.push_back("SpriteAnimationFrame_" + std::to_string(0));
    values.push_back(spriteAnimations->GetSpriteTexture()->GetFramePs());

    keys.push_back("SpriteAnimationTotalFrame_" + std::to_string(0));
    values.push_back(spriteAnimations->GetSpriteTexture()->GetTotalFrames());

    //currently hard coded values in asset manager
    keys.push_back("SpriteAnimationFrameX_" + std::to_string(0));
    values.push_back(spriteAnimations->GetSpriteTexture()->GetNxFrames());

    keys.push_back("SpriteAnimationFrameY_" + std::to_string(0));
    values.push_back(spriteAnimations->GetSpriteTexture()->GetNyFrames());

    keys.push_back("SpritePathName_" + std::to_string(0));
    values.push_back(spriteAnimations->GetSpriteTexture()->codeName);

    keys.push_back("Spritelayer");
    values.push_back(layer);

    keys.push_back("SpriteFlipX");
    values.push_back(flipX);

    keys.push_back("SpriteFlipY");
    values.push_back(flipY);

    // Call the LuaWrite function with the keys and values
    luaManager.LuaWrite(tableName, values, keys, "Sprite");
}

// Deserializes the SpriteComponent data from a Lua file.
void SpriteComponent::Deserialize(const std::string& luaFilePath, const std::string& tableName)
{
    //Load from lua
    LuaManager luaManager(luaFilePath);

    texturePath = luaManager.LuaReadFromSprite<std::string>(tableName, "SpritePathName", 0);
    spriteAnimations = std::make_unique<SpriteAnimation>(AssetManager::GetInstance().GetSprite(texturePath));
    GetCurrentSprite()->GetSpriteTexture()->SetFramePs(luaManager.LuaRead<double>(tableName, { "Sprite", "SpriteAnimationFrame_0" }));
    GetCurrentSprite()->GetSpriteTexture()->SetTotalFrames((int)luaManager.LuaRead<float>(tableName, { "Sprite", "SpriteAnimationTotalFrame_0" }));
    GetCurrentSprite()->GetSpriteTexture()->SetNxFrames((int)luaManager.LuaRead<float>(tableName, { "Sprite", "SpriteAnimationFrameX_0" }));
    GetCurrentSprite()->GetSpriteTexture()->SetNyFrames((int)luaManager.LuaRead<float>(tableName, { "Sprite", "SpriteAnimationFrameY_0" }));

    layer = luaManager.LuaRead<int>(tableName, { "Sprite", "Spritelayer" });
    flipX = luaManager.LuaRead<bool>(tableName, { "Sprite", "SpriteFlipX" });
    flipY = luaManager.LuaRead<bool>(tableName, { "Sprite", "SpriteFlipY" });
}

// Returns a string containing debug information for the component.
std::string SpriteComponent::DebugInfo() const
{
    return "SpriteComponent - TexturePath: " + texturePath +
        ", Layer: " + std::to_string(layer) +
        ", FlipX: " + (flipX ? "true" : "false") +
        ", FlipY: " + (flipY ? "true" : "false");
}

/**
 * @brief Removes a sprite animation from the component by its name
 * @param spriteName The name of the sprite to remove
 * @details Searches through all sprite animations and removes any matching the given name.
 *          If the current sprite is removed:
 *          - Attempts to switch to another sprite if available
 *          - Selects the next sprite in sequence, or wraps to beginning
 *          - Updates the currentID to reflect the new active sprite
 */
void SpriteComponent::RemoveSpriteByName(const std::string& spriteName) {
    if (HasSprite(spriteName))
        spriteAnimations.release();
}

/**
 * @brief Checks if this component has a specific sprite animation
 * @param spriteName The name of the sprite to check for
 * @return true if the sprite exists in this component, false otherwise
 * @details Iterates through all sprite animations in the component
 *          and checks if any match the provided name
 */
bool SpriteComponent::HasSprite(const std::string& spriteName) {
    return (spriteAnimations->GetSpriteTexture()->codeName == spriteName) ? true : false;
}

/*!****************************************************************
\func SetSpeedBoostEffect
\brief Enables or disables the speed boost visual effect on the sprite.

\param enabled A boolean indicating whether to enable or disable the effect.
\param boostColor The color to apply when the speed boost is enabled.
*******************************************************************/
void SpriteComponent::SetSpeedBoostEffect(bool enabled, Vector3 boostColor) {
    (void)boostColor;
    // Store original color if we're just enabling the effect
    if (enabled && !isSpeedBoosted) {
        originalColor = { GetRGB().x,GetRGB().y,GetRGB().z };
    }

    isSpeedBoosted = enabled;
    pulsationTime = 0.0f;

    if (!enabled) {
        // Reset to original color when disabling
        SetRGB({ originalColor.x,originalColor.y,originalColor.z,1.f });
    }
}

/*!****************************************************************
\func SetAnimationSpeedMultiplier
\brief Sets the speed multiplier for sprite animation.

\param multiplier The factor by which animation speed should be scaled.
*******************************************************************/
void SpriteComponent::SetAnimationSpeedMultiplier(float multiplier) {
    animationSpeedMultiplier = multiplier;
}
