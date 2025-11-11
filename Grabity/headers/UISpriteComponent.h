/*!****************************************************************
\file: UISpriteComponent.h
\author: Teng Shi Heng, shiheng.teng, 2301269
\brief:
    The UISpriteComponent class manages sprite animations within
    a game object, including rendering properties like layer,
    flipping, and color. It supports serialization for saving and
    loading sprite component data.

Copyright (C) 2024 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents
without the prior written consent of DigiPen Institute of
Technology is prohibited.
*******************************************************************!*/

#pragma once

#include "Component.h"
#include "SpriteAnimation.h"
#include "Geometry.h"
#include <memory>

/*!****************************************************************
\class UISpriteComponent
\brief
    Manages sprite animations for a game object, including rendering
    properties and serialization. Provides functions to update,
    serialize, deserialize, and debug the component.
*******************************************************************!*/
class UISpriteComponent : public Component
{
public:
    /*!****************************************************************
    \brief
        Default constructor for UISpriteComponent.
    *******************************************************************!*/
    UISpriteComponent();

    /*!****************************************************************
    \brief
        Constructor for UISpriteComponent with parent GameObject.
    \param parent
        Pointer to the parent GameObject.
    *******************************************************************!*/
    UISpriteComponent(GameObject* parent);

    /*!****************************************************************
    \brief
        Destructor for UISpriteComponent.
    *******************************************************************!*/
    ~UISpriteComponent();

    /*!****************************************************************
    \brief
        Updates the sprite component state.
    *******************************************************************!*/
    void Update() override;

    /*!****************************************************************
    \brief
        Serializes the sprite component data to a Lua file.
    \param luaFilePath
        Path to the Lua file.
    \param tableName
        Name of the table in the Lua file.
    *******************************************************************!*/
    void Serialize(const std::string& luaFilePath, const std::string& tableName) override;

    /*!****************************************************************
    \brief
        Deserializes the sprite component data from a Lua file.
    \param luaFilePath
        Path to the Lua file.
    \param tableName
        Name of the table in the Lua file.
    *******************************************************************!*/
    void Deserialize(const std::string& luaFilePath, const std::string& tableName) override;

    /*!****************************************************************
    \brief
        Returns debug information for the sprite component.
    \return
        Debug information string.
    *******************************************************************!*/
    std::string DebugInfo() const override;

    /*!****************************************************************
    \brief
        Gets the current sprite animation.
    \return
        Shared pointer to the current sprite animation.
    *******************************************************************!*/
    SpriteAnimation* GetCurrentSprite() { return sprite.get(); }

    /*!****************************************************************
    \brief
        Gets the rendering layer of the sprite.
    \return
        Rendering layer.
    *******************************************************************!*/
    int GetLayer() const { return layer; }

    /*!****************************************************************
    \brief
        Sets the rendering layer of the sprite.
    \param newLayer
        New rendering layer.
    *******************************************************************!*/
    void SetLayer(int newLayer) { layer = newLayer; }

    /*!****************************************************************
    \brief
        Gets the horizontal flipping state of the sprite.
    \return
        True if the sprite is flipped horizontally; otherwise, false.
    *******************************************************************!*/
    bool GetFlipX() const { return flipX; }

    /*!****************************************************************
    \brief
        Sets the horizontal flipping state of the sprite.
    \param newFlipX
        New horizontal flipping state.
    *******************************************************************!*/
    void SetFlipX(bool newFlipX) { flipX = newFlipX; }

    /*!****************************************************************
    \brief
        Gets the vertical flipping state of the sprite.
    \return
        True if the sprite is flipped vertically; otherwise, false.
    *******************************************************************!*/
    bool GetFlipY() const { return flipY; }

    /*!****************************************************************
    \brief
        Sets the vertical flipping state of the sprite.
    \param newFlipY
        New vertical flipping state.
    *******************************************************************!*/
    void SetFlipY(bool newFlipY) { flipY = newFlipY; }

    /*!****************************************************************
    \brief
        Gets the renderable state of the UI sprite.
    \return
        True if the UI sprite is renderable; otherwise, false.
    *******************************************************************!*/
    bool const& GetIsRenderable() const { return renderableUI; }

    /*!****************************************************************
    \brief
        Sets the renderable state of the UI sprite.
    \param isRenderable
        New renderable state.
    *******************************************************************!*/
    void SetIsRenderable(bool const& isRenderable) { renderableUI = isRenderable; }

    /*!****************************************************************
    \brief
        Gets the color of the UI sprite.
    \return
        Reference to the color of the UI sprite.
    *******************************************************************!*/
    Vector4 const& GetColor() const { return colorUI; }

    /*!****************************************************************
    \brief
        Sets the color of the UI sprite.
    \param color
        New color.
    *******************************************************************!*/
    void SetColor(Vector4 const& color) { colorUI = color; }

    // change the current sprite
    void ChangeSprite(std::unique_ptr<SpriteAnimation> animation)
    {
        // using std move to create the new sprite animation transfer it into the
        // variable stored in this class
        sprite = std::move(animation);
    }

private:
    std::unique_ptr<SpriteAnimation> sprite;   // Pointer to the sprite animation.
    Vector4 colorUI;                           // Color of the UI sprite.
    int layer;                                 // Rendering layer of the sprite.
    bool flipX;                                // Horizontal flipping state.
    bool flipY;                                // Vertical flipping state.
    bool renderableUI;                         // Renderable state of the UI sprite.
    double accumulatedTime;
};
