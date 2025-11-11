/*!****************************************************************
\file: SpriteComponent.h
\author: Johny Yong Jun Siang, j.yong, 2301301
\co-author: Teng Shi Heng, shiheng.teng, 2301269
\brief: Declares the SpriteComponent class for managing sprite rendering and animations. It supports multiple animation
        states, texture flipping, and layering. The class integrates with the TransformComponent for positioning and provides
        serialization, deserialization, and debugging.

        Johny first provided initial functions of the file, as well as the get setters (20%)
        Shi Heng updates the functions to support sprite animations (70%)
        Brandon added functions that removes a sprite animation from the component by its name, and checks if this component has a specific sprite animation (10%)

Copyright (C) 2024 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents
without the prior written consent of DigiPen Institute of
Technology is prohibited.
*******************************************************************!*/
#pragma once
#include "Component.h"
#include "TransformComponent.h"
#include "ObjectPool.h"
#include "LuaConfig.h"
#include "pch.h"
#include "SpriteAnimation.h"
#include "Geometry.h"

#pragma once

#include "Component.h"
#include "TransformComponent.h"
#include "ObjectPool.h"
#include "LuaConfig.h"
#include "pch.h"
#include "SpriteAnimation.h"
#include "Geometry.h"

class SpriteComponent : public Component {
public:
    // Constructors & Destructor
    SpriteComponent();
    SpriteComponent(GameObject* parent);
    SpriteComponent(GameObject* parent, std::unique_ptr<SpriteAnimation> animation, int lyr = 0, bool fx = false, bool fy = false);
    ~SpriteComponent();

    // Animation Management

    /*!****************************************************************
    \func GetCurrentSprite
    \brief Retrieves the current sprite animation being played.

    \return A pointer to the current SpriteAnimation object.
    *******************************************************************!*/
    SpriteAnimation* GetCurrentSprite();

    /*!****************************************************************
    \func ChangeSprite
    \brief Changes the current sprite animation to the given animation.

    \param animation A unique pointer to the new SpriteAnimation object that will replace the current one.
    *******************************************************************!*/
    void ChangeSprite(std::unique_ptr<SpriteAnimation> animation);

    /**
     * @brief Removes a sprite animation from the component by its name
     * @param spriteName The name of the sprite to remove
     * @details Searches through all sprite animations and removes any matching the given name.
     *          If the current sprite is removed:
     *          - Attempts to switch to another sprite if available
     *          - Selects the next sprite in sequence, or wraps to beginning
     *          - Updates the currentID to reflect the new active sprite
     */
    void RemoveSpriteByName(const std::string& spriteName);

    /**
     * @brief Checks if this component has a specific sprite animation
     * @param spriteName The name of the sprite to check for
     * @return true if the sprite exists in this component, false otherwise
     * @details Iterates through all sprite animations in the component
     *          and checks if any match the provided name
     */
    bool HasSprite(const std::string& spriteName);

    // Update & Serialization
    void Update() override;
    void Serialize(const std::string& luaFilePath, const std::string& tableName) override;
    void Deserialize(const std::string& luaFilePath, const std::string& tableName) override;

    // Getters & Setters
    const std::string& GetTexturePath() const { return texturePath; }
    void SetTexturePath(const std::string& newText) { texturePath = newText; }
    int GetLayer() const { return layer; }
    void SetLayer(int newLayer) { layer = newLayer; }
    bool GetFlipX() const { return flipX; }
    void SetFlipX(bool newFlipX) { flipX = newFlipX; }
    bool GetFlipY() const { return flipY; }
    void SetFlipY(bool newFlipY) { flipY = newFlipY; }
    Vector4 GetRGB() { return RGB; }
    void SetRGB(Vector4 newRGB) { RGB = newRGB; }

    // Debug
    std::string DebugInfo() const override;

    //jeremys stuff for explosive sprite

    /*!****************************************************************
    \func SetAnimationSpeedMultiplier
    \brief Sets the speed multiplier for sprite animation.

    \param multiplier The factor by which animation speed should be scaled.
    *******************************************************************/
    void SetAnimationSpeedMultiplier(float multiplier);

    /*!****************************************************************
    \func SetSpeedBoostEffect
    \brief Enables or disables the speed boost visual effect on the sprite.

    \param enabled A boolean indicating whether to enable or disable the effect.
    \param boostColor The color to apply when the speed boost is enabled.
    *******************************************************************/
    void SetSpeedBoostEffect(bool enabled, Vector3 boostColor = Vector3(1.0f, 0.5f, 0.0f));
    
    /*!****************************************************************
    \func IsSpeedBoosted
    \brief Checks if the speed boost effect is currently enabled.

    \return True if the speed boost effect is enabled, false otherwise.
    *******************************************************************!*/
    bool IsSpeedBoosted() const { return isSpeedBoosted; }
private:

    std::unique_ptr<SpriteAnimation> spriteAnimations;
    std::string texturePath;
    double accumulatedTime;
    Vector4 RGB = { 1.f, 1.f, 1.f, 1.f };
    int layer;
    bool flipX;
    bool flipY;
    int currentID = 0;
    int maxID = 0;

    //jeremys stuff for explosive sprite
    float animationSpeedMultiplier = 1.0f;
    bool isSpeedBoosted = false;
    float pulsationTime = 0.0f;
    Vector3 originalColor = Vector3(1.0f, 1.0f, 1.0f);


};

