/*!****************************************************************
\file: Geometry.cpp
\author: Teng Shi Heng, shiheng.teng, 2301269
\brief:
    The `Geometry` class member function definition to create various
    geometric shapes

Copyright (C) 2024 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents
without the prior written consent of DigiPen Institute of
Technology is prohibited.
*******************************************************************!*/

#include "Geometry.h"
#include "VertexBufferLayout.h"

// Function to create a font textured quad (2 triangles) with specified transformation and texture coordinates
Vertex* CreateFontQuad(Vertex* target, Vector2 position, float layerID, float texSlot, float textureCoordinateX, float textureCoordinateY, Vector3 color)
{
    // Bottom-left
    target->position = { position.x, position.y, layerID };
    target->color = { color.x, color.y, color.z, 1.0f };
    target->texCoords = { 0.0f, 1.0f };
    target->texSlot = texSlot;
    target++;

    // Bottom-right
    target->position = { position.x + textureCoordinateX, position.y, layerID };
    target->color = { color.x, color.y, color.z, 1.0f };
    target->texCoords = { 1.0f, 1.0f };
    target->texSlot = texSlot;
    target++;

    // Top-right
    target->position = { position.x + textureCoordinateX, position.y + textureCoordinateY, layerID };
    target->color = { color.x, color.y, color.z, 1.0f };
    target->texCoords = { 1.0f, 0.0f };
    target->texSlot = texSlot;
    target++;

    // Top-left
    target->position = { position.x, position.y + textureCoordinateY, layerID };
    target->color = { color.x, color.y, color.z, 1.0f }; // Ensure alpha is 1.0 for no transparency; adjust as needed
    target->texCoords = { 0.0f,  0.0f };
    target->texSlot = texSlot;
    target++;

    return target;
}

// Function to create a textured quad (2 triangles) with specified transformation and texture coordinates
// RIDHWAN: Added color parameter to allow for custom color
Vertex* CreateTextureQuad(Vertex* target, Matrix4x4 mtx, float layerID, float texSlot, float textureCoordinateX, float textureCoordinateY, float frameX, float frameY, Vector4 color)
{
    Vector2 offset(1.f / frameX, 1.f / frameY);
    Vector2 animationPos(offset.x * textureCoordinateX, offset.y * textureCoordinateY);

    // Bottom-left
    target->position = { -0.5f, -0.5f, layerID };
    target->position = mtx * target->position;
    target->color = color;
    target->texCoords = { 0.0f * offset.x + animationPos.x, 0.0f * offset.y + animationPos.y };
    target->texSlot = texSlot;
    target++;

    // Bottom-right
    target->position = { 0.5f, -0.5f, layerID };
    target->position = mtx * target->position;
    target->color = color;
    target->texCoords = { 1.0f * offset.x + animationPos.x, 0.0f * offset.y + animationPos.y };
    target->texSlot = texSlot;
    target++;

    // Top-right
    target->position = { 0.5f, 0.5f, layerID };
    target->position = mtx * target->position;
    target->color = color;
    target->texCoords = { 1.0f * offset.x + animationPos.x, 1.0f * offset.y + animationPos.y };
    target->texSlot = texSlot;
    target++;

    // Top-left
    target->position = { -0.5f, 0.5f, layerID };
    target->position = mtx * target->position;
    target->color = color;
    target->texCoords = { 0.0f * offset.x + animationPos.x, 1.0f * offset.y + animationPos.y };
    target->texSlot = texSlot;
    target++;

    return target;
}

// To handle individual start and end points to render a line
Vertex* CreateLine(Vertex* target, const Vector3& start, const Vector3& end, float layerID)
{
    target->position = { start.x, start.y, layerID };
    target->color = { 1.0f, 0.0f, 0.0f, 1.0f };
    target->texCoords = { 0.0f , 0.0f };
    target->texSlot = 0;
    target++;

    target->position = { end.x, end.y, layerID };
    target->color = { 1.0f, 0.0f, 0.0f, 1.0f };
    target->texCoords = { 0.0f , 0.0f };
    target->texSlot = 0;
    target++;

    return target;
}

void Geometry::SetGeometryData(int g_Type)
{
    // Create the vertex buffer layout that defines the format for vertex data
    VertexBufferLayout layout;

    // All vertices will be stored in this format:
    // x, y, z for geometry shapes like points, lines, and triangles.
    // u, v for texture mapping (textures and font) typically used for quads.

    // Static draw means the vertex data cannot be modified after being set.
    // Dynamic draw means the vertex data can be updated using glSubData.

    switch (g_Type)
    {
    case GeometryType::G_POINT:
    {
        // Setup for point rendering
        va.Bind();
        vb.Bind();

        // Allocate buffer for vertex data. MaxLineVertexCount will define the size.
        glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * MaxLineVertexCount, nullptr, GL_DYNAMIC_DRAW);

        // Set up the vertex attributes for position (3D coordinates), color (RGBA), texture coordinates (UV), and texture slot ID
        glEnableVertexAttribArray(0); // Position attribute
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, position));

        glEnableVertexAttribArray(1); // Color attribute
        glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const void*)offsetof(Vertex, color));

        glEnableVertexAttribArray(2); // Texture coordinate attribute
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const void*)offsetof(Vertex, texCoords));

        glEnableVertexAttribArray(3); // Texture slot attribute
        glVertexAttribPointer(3, 1, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const void*)offsetof(Vertex, texSlot));

        va.Unbind();
        vb.Unbind();
        break;
    }
    case GeometryType::G_LINE_DYNAMIC:
    {
        // Setup for dynamic line rendering (lines that can be updated)
        va.Bind();
        vb.Bind();

        // Allocate buffer for vertex data. MaxLineVertexCount will define the size.
        glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * MaxLineVertexCount, nullptr, GL_DYNAMIC_DRAW);

        // Set up the vertex attributes for position (3D coordinates), color (RGBA), texture coordinates (UV), and texture slot ID
        glEnableVertexAttribArray(0); // Position attribute
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, position));

        glEnableVertexAttribArray(1); // Color attribute
        glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const void*)offsetof(Vertex, color));

        glEnableVertexAttribArray(2); // Texture coordinate attribute
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const void*)offsetof(Vertex, texCoords));

        glEnableVertexAttribArray(3); // Texture slot attribute
        glVertexAttribPointer(3, 1, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const void*)offsetof(Vertex, texSlot));

        // Prepare the index data for lines (pairs of indices)
        uint32_t* indices_Line = new uint32_t[MaxLineIndexCount];
        uint32_t offset = 0;
        for (size_t index = 0; index < MaxLineIndexCount; index += 2)
        {
            indices_Line[index + 0] = 0 + offset;
            indices_Line[index + 1] = 1 + offset;

            offset += 2;
        }

        // Bind the index buffer and allocate data for the line indices
        ib.Bind();
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, MaxLineIndexCount * sizeof(uint32_t), indices_Line, GL_STATIC_DRAW);

        va.Unbind();
        vb.Unbind();
        ib.Unbind();

        // Clean up dynamic memory allocation for indices
        delete[] indices_Line;
        break;
    }
    case GeometryType::G_TRIANGLE:
    {
        // Setup for rendering a triangle (static, non-dynamic)
        // Define triangle vertices for a simple 2D triangle
        float triangle_vertices[] = {
            -0.5f, -0.5f, 0.0f, // Bottom-left vertex
             0.5f, -0.5f, 0.0f, // Bottom-right vertex
             0.5f,  0.5f, 0.0f  // Top-right vertex
        };

        // Setup vertex buffer for the triangle vertices
        vb.SetVertexBuffer(triangle_vertices, 3 * 3 * sizeof(float), GL_STATIC_DRAW);
        layout.Push<float>(3); // Define 3 components per vertex (x, y, z)

        va.Bind();
        va.AddBuffer(vb, layout); // Add the vertex buffer to the vertex array

        va.Unbind();
        vb.Unbind();
        break;
    }
    case GeometryType::G_QUAD:
    {
        // Setup for rendering quads (rectangles)
        va.Bind();
        vb.Bind();

        // Allocate buffer for vertex data. MaxVertexCount will define the size.
        glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * MaxVertexCount, nullptr, GL_DYNAMIC_DRAW);

        // Set up the vertex attributes for position (3D coordinates), color (RGBA), texture coordinates (UV), and texture slot ID
        glEnableVertexAttribArray(0); // Position attribute
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, position));

        glEnableVertexAttribArray(1); // Color attribute
        glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const void*)offsetof(Vertex, color));

        glEnableVertexAttribArray(2); // Texture coordinate attribute
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const void*)offsetof(Vertex, texCoords));

        glEnableVertexAttribArray(3); // Texture slot attribute
        glVertexAttribPointer(3, 1, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const void*)offsetof(Vertex, texSlot));

        // Prepare the index data for quads (2 triangles per quad)
        uint32_t* indices = new uint32_t[MaxIndexCount];
        uint32_t offset = 0;
        for (size_t index = 0; index < MaxIndexCount; index += 6)
        {
            // Two triangles per quad (indices define the vertices)
            indices[index + 0] = 0 + offset;
            indices[index + 1] = 1 + offset;
            indices[index + 2] = 2 + offset;

            indices[index + 3] = 2 + offset;
            indices[index + 4] = 3 + offset;
            indices[index + 5] = 0 + offset;

            offset += 4;
        }

        // Bind the index buffer and allocate data for the quad indices
        ib.Bind();
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, MaxIndexCount * sizeof(uint32_t), indices, GL_STATIC_DRAW);

        va.Unbind();
        vb.Unbind();
        ib.Unbind();

        // Clean up dynamic memory allocation for indices
        delete[] indices;
        break;
    }
    case GeometryType::G_FONT:
    {
        // Setup for rendering text (font quads)
        va.Bind();
        vb.Bind();

        // Allocate buffer for vertex data. MaxVertexCount will define the size.
        glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * MaxVertexCount, nullptr, GL_DYNAMIC_DRAW);

        // Set up the vertex attributes for position (3D coordinates), color (RGBA), texture coordinates (UV), and texture slot ID
        glEnableVertexAttribArray(0); // Position attribute
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, position));

        glEnableVertexAttribArray(1); // Color attribute
        glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const void*)offsetof(Vertex, color));

        glEnableVertexAttribArray(2); // Texture coordinate attribute
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const void*)offsetof(Vertex, texCoords));

        glEnableVertexAttribArray(3); // Texture slot attribute
        glVertexAttribPointer(3, 1, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const void*)offsetof(Vertex, texSlot));

        // Prepare the index data for font rendering (2 triangles per quad)
        uint32_t* indices_Font = new uint32_t[MaxIndexCount];
        uint32_t offset = 0;
        for (size_t index = 0; index < MaxIndexCount; index += 6)
        {
            // Two triangles per quad for each character
            indices_Font[index + 0] = 0 + offset;
            indices_Font[index + 1] = 1 + offset;
            indices_Font[index + 2] = 2 + offset;

            indices_Font[index + 3] = 2 + offset;
            indices_Font[index + 4] = 3 + offset;
            indices_Font[index + 5] = 0 + offset;

            offset += 4;
        }

        // Bind the index buffer and allocate data for the font indices
        ib.Bind();
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, MaxIndexCount * sizeof(uint32_t), indices_Font, GL_STATIC_DRAW);

        va.Unbind();
        vb.Unbind();
        ib.Unbind();

        // Clean up dynamic memory allocation for indices
        delete[] indices_Font;
        break;
    }
    case GeometryType::G_CIRCLE:
    {
        // Setup for rendering circles (currently not implemented)
        break;
    }
    default:
    {
        // Handle any invalid or unsupported geometry types
        break;
    }
    }
}
