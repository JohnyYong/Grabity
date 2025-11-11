/*!****************************************************************
\file: TextComponent.h
\author: Johny Yong Jun Siang, j.yong, 2301301
\brief: Declares the TextComponent for text rendering. Manages text attributes and provides methods for serialization and 
        debugging.

Copyright (C) 2024 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents
without the prior written consent of DigiPen Institute of
Technology is prohibited.
*******************************************************************!*/
#pragma once
#include "Component.h"
#include "ObjectPool.h"
#include "LuaConfig.h"
#include <string>
#include "pch.h"

/*!****************************************************************
\class TextComponent
\brief
    Manages text properties and behavior for a GameObject.
*******************************************************************/
class TextComponent : public Component {
    std::string textContent;
    std::string font;
    Vector2 position;
    float alpha;
    Vector3 rgb;
    float fontSize;
public:
    /*!****************************************************************
    \brief
        Default constructor for the TextComponent.
    *******************************************************************/
    TextComponent();

    /*!****************************************************************
    \brief
        Constructs the TextComponent with a parent GameObject.
    \param parent
        Pointer to the parent GameObject.
    *******************************************************************/
    TextComponent(GameObject* parent);

    /*!****************************************************************
    \brief
        Constructs the TextComponent with specified parameters.
    \param parent
        Pointer to the parent GameObject.
    \param text
        The content of the text.
    \param fontType
        The type of font to be used for the text.
    \param pos
        The position of the text in 2D space.
    \param textSize
        The size of the text.
    \param RGB
        The color of the text as a Vector3.
    \param newAlpha
        The transparency of the text (0.0 to 1.0).
    *******************************************************************/
    TextComponent(GameObject* parent, const std::string& text, const std::string& fontType, const Vector2& pos, float textSize, const Vector3& RGB, float newAlpha);

    /*!****************************************************************
    \brief
        Updates the state of the TextComponent.
    *******************************************************************/
    void Update() override;

    /*!****************************************************************
    \brief
        Serializes the TextComponent data to a Lua file.
    \param luaFilePath
        Path to the Lua file.
    \param tableName
        Name of the Lua table to write data into.
    *******************************************************************/
    void Serialize(const std::string& luaFilePath, const std::string& tableName) override;

    /*!****************************************************************
    \brief
        Deserializes the TextComponent data from a Lua file.
    \param luaFilePath
        Path to the Lua file.
    \param tableName
        Name of the Lua table to read data from.
    *******************************************************************/
    void Deserialize(const std::string& luaFilePath, const std::string& tableName) override;

    /*!****************************************************************
    \brief
        Returns a string containing debug information for the TextComponent.
    \return
        Debug information as a string.
    *******************************************************************/
    std::string DebugInfo() const override;

    /*!****************************************************************
    \brief
        Retrieves the current text content.
    \return
        The current text as a string.
    *******************************************************************/
    const std::string& GetText() const { return textContent; }

    /*!****************************************************************
    \brief
        Sets the text content.
    \param newText
        The new text content.
    *******************************************************************/
    void SetText(const std::string& newText) { textContent = newText; }

    /*!****************************************************************
    \brief
        Retrieves the current position of the text.
    \return
        The position as a Vector2.
    *******************************************************************/
    const Vector2& GetPosition() const { return position; }

    /*!****************************************************************
    \brief
        Sets the position of the text.
    \param newPos
        The new position as a Vector2.
    *******************************************************************/
    void SetPosition(const Vector2& newPos) { position = newPos; }

    /*!****************************************************************
    \brief
        Retrieves the font type of the text.
    \return
        The font type as a string.
    *******************************************************************/
    const std::string& GetFontType() const { return font; }

    /*!****************************************************************
    \brief
        Retrieves the font size of the text.
    \return
        The font size as a float.
    *******************************************************************/
    float GetFontSize() const { return fontSize; }

    /*!****************************************************************
    \brief
        Sets the font type and size.
    \param newFont
        The new font type.
    \param newSize
        The new font size.
    *******************************************************************/
    void SetFont(const std::string& newFont, float newSize) { font = newFont; fontSize = newSize;}
    
    /*!****************************************************************
    \brief
        Retrieves the RGB color of the text.
    \return
        The RGB color as a Vector3.
    *******************************************************************/
    const Vector3& GetRGB() { return rgb; }

    /*!****************************************************************
    \brief
        Sets the RGB color of the text.
    \param newRGB
        The new RGB color as a Vector3.
    *******************************************************************/
    void SetRGB(const Vector3& newRGB) { rgb = newRGB; }

    /*!****************************************************************
    \brief
        Retrieves the alpha (transparency) of the text.
    \return
        The alpha value as a float.
    *******************************************************************/
    const float& GetAlpha() { return alpha; }

    /*!****************************************************************
    \brief
        Sets the alpha (transparency) of the text.
    \param newAlpha
        The new alpha value.
    *******************************************************************/
    void SetAlpha(const float& newAlpha) { alpha = newAlpha; }
};
