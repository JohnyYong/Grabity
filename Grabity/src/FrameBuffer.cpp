/*!****************************************************************
\file:      FrameBuffer.cpp
\author:    Ridhwan Afandi, mohamedridhwan.b, 2301367

\brief:     This file implements the FrameBuffer class, which is 
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

#include "FrameBuffer.h"


FrameBuffer::FrameBuffer()
	: fbo(0), texture(0), rbo(0)
{
}

FrameBuffer::~FrameBuffer()
{
	Cleanup();
}

void FrameBuffer::Init(GLuint width, GLuint height)
{
	// Create framebuffer
	glGenFramebuffers(1, &fbo);
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);

	// Create texture
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture, 0);

	// Create renderbuffer
	glGenRenderbuffers(1, &rbo);
	glBindRenderbuffer(GL_RENDERBUFFER, rbo);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);

	// Check if framebuffer is complete
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		std::cout << "Framebuffer is incomplete!" << std::endl;

	// Unbind
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void FrameBuffer::Bind()
{
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);
}

void FrameBuffer::Unbind()
{
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}


void FrameBuffer::Cleanup()
{
	glDeleteFramebuffers(1, &fbo);
	glDeleteTextures(1, &texture);
	glDeleteRenderbuffers(1, &rbo);
}

#endif // _IMGUI