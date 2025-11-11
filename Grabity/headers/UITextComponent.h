/*!****************************************************************
\file: UITextComponent.h
\author: Teng Shi Heng, shiheng.teng, 2301269
\brief:
    The UITextComponent class provides functionality to manage
    and display text within a game object. It supports text
    customization including font type, size, color, and position.
    Additionally, it provides serialization capabilities for
    saving and loading text component data.

Copyright (C) 2024 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents
without the prior written consent of DigiPen Institute of
Technology is prohibited.
*******************************************************************!*/

#pragma once
#include "Component.h"

#ifdef _IMGUI
#include <iostream>
#endif // _IMGUI
#include "Vector2.h"
#include "Vector3.h"

/*!****************************************************************
\class UITextComponent
\brief
    Manages text display for a game object, including text content,
    font type, position, size, color, and opacity. Supports
    serialization for saving and loading state.
*******************************************************************!*/
class UITextComponent : public Component
{
public:
    /*!****************************************************************
    \brief
        Default constructor for UITextComponent.
    *******************************************************************!*/
    UITextComponent();

    /*!****************************************************************
    \brief
        Constructor for UITextComponent with parent GameObject.
    \param parent
        Pointer to the parent GameObject.
    *******************************************************************!*/
    UITextComponent(GameObject* parent);

    /*!****************************************************************
    \brief
        Constructor for UITextComponent with detailed initialization.
    \param parent
        Pointer to the parent GameObject.
    \param text
        Text content to display.
    \param fontType
        Type of font used for the text.
    \param pos
        Position of the text.
    \param textSize
        Size of the text.
    \param RGB
        Color of the text.
    \param newAlpha
        Opacity of the text.
    *******************************************************************!*/
    UITextComponent(GameObject* parent, const std::string& text, const std::string& fontType, const Vector2& pos, float textSize, const Vector3& RGB, float newAlpha);

    /*!****************************************************************
    \brief
        Destructor for UITextComponent.
    *******************************************************************!*/
    ~UITextComponent();

    /*!****************************************************************
    \brief
        Updates the text component state.
    *******************************************************************!*/
    void Update() override;

    /*!****************************************************************
    \brief
        Serializes the text component data to a Lua file.
    \param luaFilePath
        Path to the Lua file.
    \param tableName
        Name of the table in the Lua file.
    *******************************************************************!*/
    void Serialize(const std::string& luaFilePath, const std::string& tableName) override;

    /*!****************************************************************
    \brief
        Deserializes the text component data from a Lua file.
    \param luaFilePath
        Path to the Lua file.
    \param tableName
        Name of the table in the Lua file.
    *******************************************************************!*/
    void Deserialize(const std::string& luaFilePath, const std::string& tableName) override;

    /*!****************************************************************
    \brief
        Returns debug information for the text component.
    \return
        Debug information string.
    *******************************************************************!*/
    std::string DebugInfo() const override;

    /*!****************************************************************
    \brief
        Gets the text content.
    \return
        Reference to the text content.
    *******************************************************************!*/
    const std::string& GetText() const { return textContent; }

    /*!****************************************************************
    \brief
        Sets the text content.
    \param newText
        New text content.
    *******************************************************************!*/
    void SetText(const std::string& newText) { textContent = newText; }

    /*!****************************************************************
    \brief
        Gets the position of the text.
    \return
        Reference to the text position.
    *******************************************************************!*/
    const Vector2& GetPosition() const { return position; }

    /*!****************************************************************
    \brief
        Sets the position of the text.
    \param newPos
        New position for the text.
    *******************************************************************!*/
    void SetPosition(const Vector2& newPos) { position = newPos; }

    /*!****************************************************************
    \brief
        Gets the font type of the text.
    \return
        Reference to the font type.
    *******************************************************************!*/
    const std::string& GetFontType() const { return font; }

    /*!****************************************************************
    \brief
        Gets the font size of the text.
    \return
        Font size.
    *******************************************************************!*/
    float GetFontSize() const { return fontSize; }

    /*!****************************************************************
    \brief
        Sets the font type and size for the text.
    \param newFont
        New font type.
    \param newSize
        New font size.
    *******************************************************************!*/
    void SetFont(const std::string& newFont, float newSize) { font = newFont; fontSize = newSize; }

    /*!****************************************************************
    \brief
        Gets the color of the text.
    \return
        Reference to the RGB color.
    *******************************************************************!*/
    const Vector3& GetRGB() { return rgb; }

    /*!****************************************************************
    \brief
        Sets the color of the text.
    \param newRGB
        New RGB color.
    *******************************************************************!*/
    void SetRGB(const Vector3& newRGB) { rgb = newRGB; }

    /*!****************************************************************
    \brief
        Gets the opacity of the text.
    \return
        Reference to the text opacity.
    *******************************************************************!*/
    const float& GetAlpha() { return alpha; }

    /*!****************************************************************
    \brief
        Sets the opacity of the text.
    \param newAlpha
        New opacity value.
    *******************************************************************!*/
    void SetAlpha(const float& newAlpha) { alpha = newAlpha; }

    //Getter Setter
    bool GetRenderable() { return renderable; }
    void SetRenderable(bool renderableNew) { renderable = renderableNew; }

private:
    std::string textContent;   // Content of the text.
    std::string font;          // Font type of the text.
    Vector2 position;          // Position of the text.
    float alpha;               // Opacity of the text.
    Vector3 rgb;               // Color of the text.
    float fontSize;            // Font size of the text.
    bool renderable = true;
};
