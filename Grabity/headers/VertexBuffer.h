/*!****************************************************************
\file: VertexBuffer.h
\author: Teng Shi Heng, shiheng.teng, 2301269
\brief:
    The `VertexBuffer` class manages OpenGL vertex buffer objects,
    allowing for the storage of the VBO ID and manipulation of vertex
    data for rendering.

Copyright (C) 2024 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents
without the prior written consent of DigiPen Institute of
Technology is prohibited.
*******************************************************************!*/

#pragma once

#include <glad.h>

/*!****************************************************************
\brief
    The `VertexBuffer` class encapsulates the functionality of
    OpenGL Vertex Buffer Objects (VBOs), which are used to store
    vertex data.
*******************************************************************!*/
class VertexBuffer
{
public:
    /*!****************************************************************
    \brief
        Default constructor for the `VertexBuffer` class.
    *******************************************************************!*/
    VertexBuffer();

    /*!****************************************************************
    \brief
        Destructor that cleans up the vertex buffer resources.
    *******************************************************************!*/
    ~VertexBuffer();

    /*!****************************************************************
    \brief
        Initializes the vertex buffer with the specified data.

    \param data
        A pointer to the vertex data to be stored in the buffer.

    \param size
        The size of the vertex data in bytes.

    \param drawType
        The draw type for the buffer (e.g., GL_STATIC_DRAW).
    *******************************************************************!*/
    void SetVertexBuffer(const void* data, unsigned int size, GLenum drawType);

    /*!****************************************************************
    \brief
        Retrieves the renderer ID of the vertex buffer.

    \return
      The OpenGL ID of the vertex buffer.
    *******************************************************************!*/
    inline unsigned int GetRenderID() { return mRendererID; }

    /*!****************************************************************
    \brief
        Binds the vertex buffer for use in rendering.
    *******************************************************************!*/
    void Bind() const;

    /*!****************************************************************
    \brief
        Unbinds the currently bound vertex buffer.
    *******************************************************************!*/
    void Unbind() const;

private:
    unsigned int mRendererID; // OpenGL ID for the vertex buffer
};
