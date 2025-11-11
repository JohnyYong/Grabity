/*!****************************************************************
\file:      FrameBuffer.h
\author:    Ridhwan Afandi, mohamedridhwan.b, 2301367

\brief:     This file defines the FrameBuffer class, which is
			responsible for creating and managing a custom framebuffer.
			It provides functionality for creating a framebuffer,
			attaching a texture to it, and binding/unbinding the
			framebuffer. The FrameBuffer class is used to render
			to a texture, which can then be used as a texture in
			ImGui windows or other rendering operations.

Copyright (C) 2024 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents
without the prior written consent of DigiPen Institute of
Technology is prohibited.
*******************************************************************!*/
#ifdef _IMGUI
#pragma once
#ifndef FRAMEBUFFER_H
#define FRAMEBUFFER_H

#include <pch.h>

class FrameBuffer {
public:
	FrameBuffer();
	~FrameBuffer();

	void Init(GLuint width, GLuint height);
	void Bind();
	void Unbind();
	void Cleanup();

	inline GLuint GetTextureID() const { return texture; }
	inline GLuint GetFBOID() const { return fbo; }
	inline GLuint GetRBOID() const { return rbo; }

private:
	GLuint fbo;
	GLuint texture;
	GLuint rbo;
};

#endif // FRAMEBUFFER_H

#endif // _IMGUI