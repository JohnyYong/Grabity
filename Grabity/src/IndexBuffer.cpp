/*!****************************************************************
\file: IndexBuffer.cpp
\author: Teng Shi Heng, shiheng.teng, 2301269
\brief:
    The `IndexBuffer` class member function definition where it handles the
    storage of the IBO ID and index data for rendering geometry in OpenGL.

Copyright (C) 2024 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents
without the prior written consent of DigiPen Institute of
Technology is prohibited.
*******************************************************************!*/

#include "IndexBuffer.h"
#include "glhelper.h"

// default constructor to get the ID
IndexBuffer::IndexBuffer() : mRendererID(0), mCount(0)
{
   glGenBuffers(1, &mRendererID);
}

// initializes the index buffer with the specified data.
void IndexBuffer::IndexBufferSetData(const unsigned int* data, unsigned int count)
{
    mCount = count;
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mRendererID);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, count * sizeof(unsigned int), data, GL_STATIC_DRAW);
}

// cleans up the index buffer resources.
IndexBuffer::~IndexBuffer() 
{
    glDeleteBuffers(1, &mRendererID);
}

// Bind the IBO to the renderer
void IndexBuffer::Bind() const
{
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mRendererID);

}

// Unbind the IBO to the renderer
void IndexBuffer::Unbind() const
{
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

}