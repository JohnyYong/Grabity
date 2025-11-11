/*!****************************************************************
\file: Font.h
\author: Teng Shi Heng, shiheng.teng, 2301269
\brief:
    The `Font` class facilitates loading and rendering text using
    FreeType, enabling applications to display text with various
    fonts and styles.

Copyright (C) 2024 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents
without the prior written consent of DigiPen Institute of
Technology is prohibited.
*******************************************************************!*/

#pragma once

#include <unordered_map>
#include <ft2build.h>
#include FT_FREETYPE_H 

#include "Shader.h"
#include "VertexArray.h"
#include "VertexBuffer.h"
#include "Vector3.h"
#include "Vector2.h"

/*!****************************************************************
\brief
    Represents the glyph information for each character in the font.
*******************************************************************!*/
struct Character
{
    unsigned int textureID = 0;  // ID handle of the glyph texture
    Vector2 size;                // Size of the glyph
    Vector2 bearing;             // Offset from baseline to left/top of glyph
    unsigned int advance = 0;        // Offset to advance to the next glyph
};

/*!****************************************************************
\brief
    The `Font` class is responsible for loading font data and
    rendering text using FreeType.
    It contains a mapping of characters to their corresponding
    glyph information for rendering.
*******************************************************************!*/
class Font
{
public:
    /*!****************************************************************
    \brief
        Loads a font from the specified file path at a given pixel size.

    \param fontPath
        The file path to the font file.

    \param pixelSize
        The size of the font in pixels.
    *******************************************************************!*/
    void LoadFont(std::string fontPath, FT_UInt pixelSize);

    /*!****************************************************************
    \brief
        Renders a string of text using the specified shader, position,
        scale, and color.

    \param s
        The shader program used for rendering.

    \param text
        The text string to render.

    \param x
        The x-coordinate of the text's starting position.

    \param y
        The y-coordinate of the text's starting position.

    \param scale
        The scaling factor for the text size.

    \param color
        The color of the text.

    \param va
        The vertex array used for rendering.

    \param vb
        The vertex buffer used for rendering.
    *******************************************************************!*/
    void RenderText(Shader& s, std::string text, float x, float y, float scale, Vector3 color, VertexArray& va, VertexBuffer& vb);

    std::unordered_map<char, Character> const& GetCharacterDictionary() { return characters; };
private:
    std::unordered_map<char, Character> characters; // Map of characters and their glyph information
};
