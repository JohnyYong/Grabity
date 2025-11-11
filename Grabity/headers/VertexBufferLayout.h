/*!****************************************************************
\file: VertexBufferLayout.h
\author: Teng Shi Heng, shiheng.teng, 2301269
\brief:
    The `VertexBufferLayout` class defines the layout of vertex
    buffer data, including types and counts of vertex attributes.

    A struct 'VertexBufferElement' is being defined here to represents
    an element in the vertex buffer layout, defining the data type, 
    number of components, and normalization.

Copyright (C) 2024 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents
without the prior written consent of DigiPen Institute of
Technology is prohibited.
*******************************************************************!*/

#pragma once

#include <vector>
#include <glad.h>

/*!****************************************************************
\brief
    Represents an element in the vertex buffer layout, defining
    the data type, number of components, and normalization.

    A static member function is being declared here for other
    user to access and get the size of type in bytes.
*******************************************************************!*/
struct VertexBufferElement
{
    unsigned int type;          // The data type of the element
    unsigned int count;         // The number of components for this element.
    unsigned char normalized;   // Whether the data should be normalized (1 for true, 0 for false).

    /*!****************************************************************
    \brief
        Retrieves the size in bytes of a specified type as 64 bit
        and 32 bit for the variable types might varies.

    \param type
        The OpenGL type identifier

    \return
        The size in bytes of the specified type.
    *******************************************************************!*/
    static unsigned GetSizeOfType(unsigned int type);
};

/*!****************************************************************
\brief
    The `VertexBufferLayout` class defines the layout of a vertex
    buffer, storing elements and calculating stride.
*******************************************************************!*/
class VertexBufferLayout
{
public:
    /*!****************************************************************
    \brief
        Default constructor for the `VertexBufferLayout` class.
    *******************************************************************!*/
    VertexBufferLayout();

    /*!****************************************************************
    \brief
        Pushes a new element of the specified type to the layout.

    \param count
        The number of components for this element.

    \tparam T
        The type of the element to push.
    *******************************************************************!*/
    template<typename T>
    void Push(unsigned int count);

    /*!****************************************************************
    \brief
        Specialization of Push for float types.
    *******************************************************************!*/
    template<>
    void Push<float>(unsigned int count);

    /*!****************************************************************
    \brief
        Specialization of Push for unsigned int types.
    *******************************************************************!*/
    template<>
    void Push<unsigned int>(unsigned int count);

    /*!****************************************************************
    \brief
        Specialization of Push for unsigned char types.
    *******************************************************************!*/
    template<>
    void Push<unsigned char>(unsigned int count);

    /*!****************************************************************
    \brief
        Retrieves the elements defined in the layout.

    \return
        A const reference to the vector of vertex buffer elements.
    *******************************************************************!*/
    inline const std::vector<VertexBufferElement>& GetElements() const { return numElements; }

    /*!****************************************************************
    \brief
        Retrieves the total stride of the vertex buffer layout.

    \return
        The total size of the layout in bytes.
    *******************************************************************!*/
    inline unsigned int GetStride() const { return mStride; }

private:
    std::vector<VertexBufferElement> numElements;   // List of elements in the layout
    unsigned int mStride;                           // Total size of the layout in bytes
};
