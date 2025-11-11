/*!****************************************************************
\file: Font.cpp
\author: Teng Shi Heng, shiheng.teng, 2301269
\brief:
    The `Font` class member function definition where it handles the
    loading and rendering of text.

Copyright (C) 2024 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents
without the prior written consent of DigiPen Institute of
Technology is prohibited.
*******************************************************************!*/

#include "Font.h"
#include <glhelper.h>
#include <ImGuiConsole.h>

#ifdef _IMGUI
#include <iostream>
#endif // _IMGUI

//loading of font based on the path to the path and the pixel to be extracted
void Font::LoadFont(std::string fontPath, FT_UInt pixelSize)
{
    FT_Library ft;

    // All functions return a value different than 0 whenever an error occurred
    if (FT_Init_FreeType(&ft))
    {
#ifdef _LOGGING
        ImGuiConsole::Cout("ERROR::FREETYPE: Could not init FreeType Library");
#endif // _LOGGING
    }

    // load font as face
    FT_Face face;
    if (FT_New_Face(ft, fontPath.c_str(), 0, &face)) 
    {
#ifdef _LOGGING
        ImGuiConsole::Cout("ERROR::FREETYPE: Failed to load font");
#endif // _LOGGING
    }
    else 
    {
        // set size to load glyphs as
        FT_Set_Pixel_Sizes(face, 0, pixelSize);

        // OpenGL has a restriction requiring textures to adhere to a 4-byte alignment, therefore
        // disable byte-alignment restriction as the texture width might not be in a multiple of 4
        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

        // load first 128 characters of ASCII set
        for (unsigned char c = 0; c < 128; c++)
        {
            // Load character glyph 
            if (FT_Load_Char(face, c, FT_LOAD_RENDER))
            {
#ifdef _LOGGING
                ImGuiConsole::Cout("ERROR::FREETYTPE: Failed to load Glyph");
#endif // _LOGGING
                continue;
            }

            // generate texture
            unsigned int texture;
            glGenTextures(1, &texture);
            glBindTexture(GL_TEXTURE_2D, texture);

            // The glyph's bitmap is a grayscale 8-bit image, with each pixel represented by a single byte.
            // Therefore, we use GL_RED for both the internal format and format to stored data in the red channel
            // to allows interpretation of the single-byte values as grayscale.
            glTexImage2D(GL_TEXTURE_2D,                 // target
                         0,                             // base image level
                         GL_RED,                        // the number of color components in the texture
                         face->glyph->bitmap.width,     // width
                         face->glyph->bitmap.rows,      // height
                         0,                             // border (must be 0)
                         GL_RED,                        // the format of the pixel data (for RGB setting)
                         GL_UNSIGNED_BYTE,              // the format of the pixel data
                         face->glyph->bitmap.buffer     // pointer to the image data
            );

            // set texture options
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            
            // store character for later use
            Character character = { texture,
                Vector2(static_cast<float>(face->glyph->bitmap.width), static_cast<float>(face->glyph->bitmap.rows)),
                Vector2(static_cast<float>(face->glyph->bitmap_left), static_cast<float>(face->glyph->bitmap_top)),
                static_cast<unsigned int>(face->glyph->advance.x)
            };
            characters.insert(std::pair<char, Character>(c, character));
        }

        // unbind it upon finished modifying
        glBindTexture(GL_TEXTURE_2D, 0);
    }

    // destroy FreeType once finished
    FT_Done_Face(face);
    FT_Done_FreeType(ft);
}

// Render of the text based on its given string, coordinates, color, VAO and VBO IDs
void Font::RenderText(Shader& shader, std::string text, float x, float y, float scale, Vector3 color, VertexArray& va, VertexBuffer& vb)
{
	shader.SetUniform3f("textColor", color.x, color.y, color.z);
    glActiveTexture(GL_TEXTURE0);
    glBindVertexArray(va.GetRenderID());

    // iterate through all characters
    std::string::const_iterator c;
    for (c = text.begin(); c != text.end(); c++)
    {
        Character ch = characters[*c];

        float xpos = x + ch.bearing.x * scale;
        float ypos = y - (ch.size.y - ch.bearing.y) * scale;

        float w = ch.size.x * scale;
        float h = ch.size.y * scale;

        // update VBO for each character
        float vertices[6][5] = {
            { xpos,     ypos + h, 0.0f,  0.0f, 0.0f },
            { xpos,     ypos,     0.0f,  0.0f, 1.0f },
            { xpos + w, ypos,     0.0f,  1.0f, 1.0f },

            { xpos,     ypos + h, 0.0f,  0.0f, 0.0f },
            { xpos + w, ypos,     0.0f,  1.0f, 1.0f },
            { xpos + w, ypos + h, 0.0f,  1.0f, 0.0f }
        };

        // render glyph texture over quad
        glBindTexture(GL_TEXTURE_2D, ch.textureID);

        // update content of VBO memory
        glBindBuffer(GL_ARRAY_BUFFER, vb.GetRenderID());
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices); // to redefine the data in the vertices
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        // render quad
        glDrawArrays(GL_TRIANGLES, 0, 6);

        // now advance cursors for next glyph (note that advance is number of 1/64 pixels)
        x += (ch.advance / 64) * scale;
    }

    // unbind the VAO and the texture after finished rendering 
    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);
}

