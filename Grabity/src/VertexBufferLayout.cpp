/*!****************************************************************
\file: VertexBufferLayout.h
\author: Teng Shi Heng, shiheng.teng, 2301269
\brief:
    The `VertexBufferLayout` class and struct 'VertexBufferElement'
    member functions definitions here where the 'VertexBufferElement'
    member function is a static function to get the size of the
    specified type.

    The `VertexBufferLayout` class mainly defines the template
    specialization for the supported layout types and return an assert
    error when the type is not supported

Copyright (C) 2024 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents
without the prior written consent of DigiPen Institute of
Technology is prohibited.
*******************************************************************!*/


#include "VertexBufferLayout.h"

// retrieves the size in bytes of a specified type as 64 bit and 32 bit 
// for the variable types might varies.
unsigned VertexBufferElement::GetSizeOfType(unsigned int type)
{
    switch (type)
    {
    case GL_FLOAT:          return 4;
    case GL_UNSIGNED_INT:   return 4;
    case GL_UNSIGNED_BYTE:  return 1;
    }

    return 0; // Type not supported
}


// default constructor
VertexBufferLayout::VertexBufferLayout() : mStride(0)
{

}

// template for pushes a new element of the specified type to the layout
// will only come here when the type is not supported
template<typename T>
void VertexBufferLayout::Push(unsigned int count)
{
    static_assert(sizeof(T) == 0, "Unsupported type");
}

// specialization of Push for float types
template<>
void VertexBufferLayout::Push<float>(unsigned int count)
{
    numElements.push_back({ GL_FLOAT, count, GL_FALSE });
    mStride += VertexBufferElement::GetSizeOfType(GL_FLOAT) * count;
}

// specialization of Push for unsigned int types
template<>
void VertexBufferLayout::Push<unsigned int>(unsigned int count)
{
    numElements.push_back({ GL_UNSIGNED_INT, count, GL_FALSE });
    mStride += VertexBufferElement::GetSizeOfType(GL_UNSIGNED_INT) * count;
}

// specialization of Push for unsigned char types
template<>
void VertexBufferLayout::Push<unsigned char>(unsigned int count)
{
    numElements.push_back({ GL_UNSIGNED_BYTE, count, GL_TRUE });
    mStride += VertexBufferElement::GetSizeOfType(GL_UNSIGNED_BYTE) * count;
}
