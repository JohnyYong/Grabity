/*!****************************************************************
\file: VertexArray.cpp
\author: Teng Shi Heng, shiheng.teng, 2301269
\brief:
	The `VertexArray` class member function definition that manages 
	vertex array objects in OpenGL.

Copyright (C) 2024 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents
without the prior written consent of DigiPen Institute of
Technology is prohibited.
*******************************************************************!*/

#include "VertexArray.h"
#include "VertexBufferLayout.h"

// default constructor to get the ID
VertexArray::VertexArray()
{
	glGenVertexArrays(1, &mRendererID);
}

// cleans up the vertex array resources.
VertexArray::~VertexArray()
{
	glDeleteVertexArrays(1, &mRendererID);
}

// initializes the vertex array with the specified data from vertex buffer and the vertex buffer layout
void VertexArray::AddBuffer(const VertexBuffer& vb, const VertexBufferLayout& layout)
{
	Bind();
	vb.Bind();
	const auto& elements = layout.GetElements();
	unsigned int offset = 0;

	// restructuring the vertex buffer to store in the vertex array in a specified layout in the shader program
	for (unsigned int i = 0; i < elements.size(); i++)
	{
		const auto& element = elements[i];
		glVertexAttribPointer(i, element.count, element.type, element.normalized, layout.GetStride(), reinterpret_cast<const void*>(static_cast<std::uintptr_t>(offset)));
		offset += element.count * VertexBufferElement::GetSizeOfType(element.type);
		glEnableVertexAttribArray(i);
	}
}

// bind the VAO to the renderer
void VertexArray::Bind() const
{
	glBindVertexArray(mRendererID);
}

// unbind the VAO to the renderer
void VertexArray::Unbind() const 
{
	glBindVertexArray(0);
}