/*!****************************************************************
\file: VertexBuffer.cpp
\author: Teng Shi Heng, shiheng.teng, 2301269
\brief:
    The `VertexBuffer` class function definition where it handles the
    storage of the VBO ID and manipulation of vertex data for rendering.

Copyright (C) 2024 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents
without the prior written consent of DigiPen Institute of
Technology is prohibited.
*******************************************************************!*/

#include "VertexBuffer.h"
#include "glhelper.h"

// default constructor to get the ID
VertexBuffer::VertexBuffer() : mRendererID(0)
{
    glGenBuffers(1, &mRendererID);
}

// initializes the vertex buffer with the specified data.
void VertexBuffer::SetVertexBuffer(const void* data, unsigned int size, GLenum drawType)
{
    glBindBuffer(GL_ARRAY_BUFFER, mRendererID);
    glBufferData(GL_ARRAY_BUFFER, size, data, drawType);
}

// cleans up the vertex buffer resources.
VertexBuffer::~VertexBuffer()
{
    glDeleteBuffers(1, &mRendererID);
}

// bind the VBO to the renderer
void VertexBuffer::Bind() const
{
    glBindBuffer(GL_ARRAY_BUFFER, mRendererID);
}

// unbind the VBO from the renderer
void VertexBuffer::Unbind() const
{
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}