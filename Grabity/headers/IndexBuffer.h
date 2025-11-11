/*!****************************************************************
\file: IndexBuffer.h
\author: Teng Shi Heng, shiheng.teng, 2301269
\brief:
    The `IndexBuffer` class manages the storage of the IBO ID and 
    handling of index data for rendering geometry in OpenGL.

Copyright (C) 2024 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents
without the prior written consent of DigiPen Institute of
Technology is prohibited.
*******************************************************************!*/

#pragma once

class IndexBuffer
{
public:

    /*!****************************************************************
    \brief
        Default constructor that generates an OpenGL ID for the
        created `IndexBuffer` object.
    *******************************************************************!*/
    IndexBuffer();

    /*!****************************************************************
    \brief
        Destructor that frees the resources allocated for the index
        buffer, ensuring proper cleanup.
    *******************************************************************!*/
    ~IndexBuffer();

    /*!****************************************************************
    \brief
        Sets the index data for the buffer.

    \param data
        A pointer to an array of unsigned integers representing
        the index data.

    \param count
        The number of indices to be stored in the buffer.
    *******************************************************************!*/
    void IndexBufferSetData(const unsigned int* data, unsigned int count);

    /*!****************************************************************
    \brief
        Retrieves the OpenGL renderer ID for the index buffer.

    \return
        A const reference to the OpenGL ID associated with this index buffer.
    *******************************************************************!*/
    inline const unsigned int& GetRenderID() { return mRendererID; }

    /*!****************************************************************
    \brief
        Binds the index buffer for rendering, making it active
        in the OpenGL context.
    *******************************************************************!*/
    void Bind() const;

    /*!****************************************************************
    \brief
        Unbinds the index buffer, making it inactive in the
        OpenGL context.
    *******************************************************************!*/
    void Unbind() const;

private:
    unsigned int mRendererID; // OpenGL ID for the index buffer
    unsigned int mCount;      // Number of indices in the buffer
};
