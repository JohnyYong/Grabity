/*!****************************************************************
\file: Geometry.h
\author: Teng Shi Heng, shiheng.teng, 2301269
\brief:
    The `Geometry` class defines various geometric shapes and
    manages their associated vertex and index buffers for rendering.

Copyright (C) 2024 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents
without the prior written consent of DigiPen Institute of
Technology is prohibited.
*******************************************************************!*/

#pragma once


#ifdef _IMGUI
#include <iostream>
#endif // _IMGUI

#include "VertexArray.h"
#include "IndexBuffer.h"
#include "VertexBuffer.h"
#include "Matrix4x4.h"
#include "Vector3.h"
#include "Vector2.h"

/*!****************************************************************
\brief
    Enumeration of different geometry types supported by the
    `Geometry` class.
*******************************************************************!*/
enum GeometryType {
    G_POINT,
    G_LINE_DYNAMIC,
    G_TRIANGLE,         // to be updated
    G_QUAD,
    G_FONT,
    G_CIRCLE,           // to be implemented

    G_TOTAL
};

/*!****************************************************************
\brief
    Struct to represent a 4D vector, typically used to represent a color with RGBA components
    x = Red, y = Green, z = Blue, w = Alpha (opacity)
*******************************************************************!*/
struct Vector4 {
    float x, y, z, w;  // Color components (Red, Green, Blue, Alpha)
};

/*!****************************************************************
\brief
    Struct to represent a vertex in 3D space
    Each vertex holds its position in 3D space (position), color (color), texture coordinates (texCoords),
    and the texture slot ID (texSlot) for binding the correct texture.
*******************************************************************!*/
struct Vertex {
    Vector3 position;  // 3D position of the vertex (x, y, z)
    Vector4 color;     // RGBA color of the vertex
    Vector2 texCoords; // Texture coordinates for the vertex (u, v) to map a texture
    float texSlot;     // The texture slot ID used to bind the texture to the vertex
};

const uint32_t MaxBatchSize = 1000;  // Max vertices per batch

const size_t MaxQuadCount = 1000;    // Max quads (rectangles) to be drawn
const size_t MaxVertexCount = MaxQuadCount * 4;  // Max vertices per batch
const size_t MaxIndexCount = MaxQuadCount * 6;   // Max indices per batch

const size_t MaxLineCount = MaxQuadCount * 4;   // Max lines to be drawn
const size_t MaxLineVertexCount = MaxLineCount;  // Max vertices for lines
const size_t MaxLineIndexCount = MaxLineCount;   // Max indices for lines

/*!****************************************************************
 \brief
     Creates a quad for font rendering (a rectangle with texture mapping for each character).

 \param target
     A pointer to the first vertex where the quad data will be stored.
 \param position
     The 2D position (x, y) for the quad's bottom-left corner.
 \param layerID
     The layer ID for the quad, used to determine the rendering order in layers.
 \param texSlot
     The texture slot ID for binding the correct texture.
 \param textureCoordinateX
     The X-coordinate of the texture to be mapped.
 \param textureCoordinateY
     The Y-coordinate of the texture to be mapped.
 \param color
     The RGB color of the quad, applied to each vertex of the quad.

 \return
     A pointer to the next available vertex after the quad has been created.
 *******************************************************************!*/
Vertex* CreateFontQuad(Vertex* target, Vector2 position, float layerID, float texSlot, float textureCoordinateX, float textureCoordinateY, Vector3 color);


/*!****************************************************************
 \brief
     Creates a textured quad (rectangle) for rendering with a transformation matrix.

 \param target
     A pointer to the first vertex where the quad data will be stored.
 \param mtx
     The transformation matrix applied to the quad's position.
 \param layerID
     The layer ID for the quad, used to determine the rendering order in layers.
 \param texSlot
     The texture slot ID for binding the correct texture.
 \param textureCoordinateX
     The X-coordinate of the texture to be mapped.
 \param textureCoordinateY
     The Y-coordinate of the texture to be mapped.
 \param frameX
     The number of horizontal frames in the texture atlas.
 \param frameY
     The number of vertical frames in the texture atlas.

 \return
     A pointer to the next available vertex after the quad has been created.
 *******************************************************************!*/
Vertex* CreateTextureQuad(Vertex* target, Matrix4x4 mtx, float layerID, float texSlot, float textureCoordinateX, float textureCoordinateY, float frameX, float frameY, Vector4 color);


/*!****************************************************************
 \brief
     Creates a line between two 3D points.

 \param target
     A pointer to the first vertex where the line data will be stored.
 \param start
     The 3D position of the start point of the line.
 \param end
     The 3D position of the end point of the line.
 \param layerID
     The layer ID for the line, used to determine the rendering order in layers.

 \return
     A pointer to the next available vertex after the line has been created.
 *******************************************************************!*/
Vertex* CreateLine(Vertex* target, const Vector3& start, const Vector3& end, float layerID);


/*!****************************************************************
\brief
    The `Geometry` class handles the setup and management of
    vertex and index data for various geometric shapes.
*******************************************************************!*/
class Geometry
{
public:
    /*!****************************************************************
    \brief
        Sets the geometry data based on the specified geometry type.

    \param g_Type
        The type of geometry to set up, represented by the
        `GeometryType` enumeration.
    *******************************************************************!*/
    void SetGeometryData(int g_Type);

    /*!****************************************************************
    \brief
        Retrieves the vertex array associated with the geometry.

    \return
        The reference vertex array used for rendering.
    *******************************************************************!*/
    inline VertexArray& GetVertexArray() { return va; }

    /*!****************************************************************
    \brief
        Retrieves the vertex buffer associated with the geometry.

    \return
        The reference  vertex buffer used for managing vertex attributes.
    *******************************************************************!*/
    inline VertexBuffer& GetVertexBuffer() { return vb; }

    /*!****************************************************************
    \brief
        Retrieves the index buffer associated with the geometry.

    \return
        The reference index buffer used for storing indices of vertices.
    *******************************************************************!*/
    inline IndexBuffer& GetIndexBuffer() { return ib; }

private:
    VertexArray va;  // Vertex array for storing vertex data
    VertexBuffer vb; // Vertex buffer for managing vertex attributes
    IndexBuffer ib;  // Index buffer for storing indices of vertices
};
