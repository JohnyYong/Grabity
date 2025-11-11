/*!****************************************************************
\file: VertexArray.h
\author: Teng Shi Heng, shiheng.teng, 2301269
\brief:
	The `VertexArray` class manages vertex array objects in OpenGL,
	allowing for the configuration and binding of vertex buffers.

Copyright (C) 2024 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents
without the prior written consent of DigiPen Institute of
Technology is prohibited.
*******************************************************************!*/

#pragma once

#include "VertexBuffer.h"

class VertexBufferLayout; //class forward declaration to prevent files from cross referencing

/*!****************************************************************
\brief
	The `VertexArray` class encapsulates the functionality of
	OpenGL's Vertex Array Objects (VAOs), which are used to
	store vertex attribute configurations.
*******************************************************************!*/
class VertexArray 
{
public:

	/*!****************************************************************
	\brief
		Default constructor for the `VertexArray` class.
	*******************************************************************!*/
	VertexArray();

	/*!****************************************************************
	\brief
		Destructor that cleans up the vertex array resources.
	*******************************************************************!*/
	~VertexArray();

	/*!****************************************************************
	\brief
		Adds a vertex buffer to the vertex array with the specified
		layout.

	\param vb
		The const reference to the vertex buffer to add to the vertex array.

	\param layout
		The const reference to the layout of the vertex buffer attributes.
	*******************************************************************!*/
	void AddBuffer(const VertexBuffer& vb, const VertexBufferLayout& layout);

	/*!****************************************************************
	\brief
		Retrieves the renderer ID of the vertex array.

	\return
		The OpenGL ID of the vertex array.
	*******************************************************************!*/
	inline unsigned int GetRenderID() { return mRendererID; }

	/*!****************************************************************
	\brief
		Binds the vertex array for use in rendering.
	*******************************************************************!*/
	void Bind() const;

	/*!****************************************************************
	\brief
		Unbinds the currently bound vertex array.
	*******************************************************************!*/
	void Unbind() const;

private:
	unsigned int mRendererID;  // OpenGL ID for the vertex array
};