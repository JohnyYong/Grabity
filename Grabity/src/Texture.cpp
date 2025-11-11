/*!****************************************************************
\file: Texture.cpp
\author: Teng Shi Heng, shiheng.teng, 2301269
\brief:
	The `Texture` classmember function definition that manages the
	loading, binding, and cleanup of texture resources for use in
	graphical applications.

Copyright (C) 2024 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents
without the prior written consent of DigiPen Institute of
Technology is prohibited.
*******************************************************************!*/

#include "Texture.h"
#include "glhelper.h"
#include "External Lib/stb_image.h"

#ifdef _IMGUI
#include <iostream>
#endif // _IMGUI

unsigned int Texture::totalTexType = 0;

// default constructor to get the ID
Texture::Texture() : mRendererID(0),
mFilePath(""), mLocalBuffer(nullptr),
mWidth(0), mHeight(0), mBPP(0), texSlot(0),
nxFrames(1), nyFrames(1), totalFrames(1), framePS(1),
isAnimtation(false), codeName("")
{
	glGenTextures(1, &mRendererID);
}


// Copy constructor
Texture::Texture(const Texture& texture) :
	mRendererID(texture.mRendererID),  // Initialize to zero, then generate a new texture ID
	mFilePath(texture.mFilePath),
	mLocalBuffer(texture.mLocalBuffer),
	mWidth(texture.mWidth),
	mHeight(texture.mHeight),
	mBPP(texture.mBPP),
	texSlot(texture.texSlot),
	nxFrames(texture.nxFrames),
	nyFrames(texture.nyFrames),
	totalFrames(texture.totalFrames),
	framePS(texture.framePS),
	isAnimtation(texture.isAnimtation),
	codeName(texture.codeName)
{
}

// constructor
Texture::Texture(std::string codename, const std::string& path, float numFrameX, float numFrameY, float numframePS)
{
	codeName = codename;
	nxFrames = numFrameX;
	nyFrames = numFrameY;
	framePS = numframePS;
	totalFrames = numFrameX * numFrameY;
	isAnimtation = (totalFrames == 1) ? false : true;

	mFilePath = path;
	mLocalBuffer = nullptr;
	mWidth = 0;
	mHeight = 0;
	mBPP = 0;
	mRendererID = 0;

	glGenTextures(1, &mRendererID);

	// flip the image upon loading to match OpenGL's coordinate system
	stbi_set_flip_vertically_on_load(1);

	// load the image data into the local buffer
	mLocalBuffer = stbi_load(path.c_str(), &mWidth, &mHeight, &mBPP, 4);

	glBindTexture(GL_TEXTURE_2D, mRendererID);

	// set texture options
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	// create the texture from the loaded image data
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, mWidth, mHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, mLocalBuffer);

	// unbind it upon finished modifying
	glBindTexture(GL_TEXTURE_2D, 0);

	// free the local image buffer if it was successfully loaded
	if (mLocalBuffer)
		stbi_image_free(mLocalBuffer);

	totalTexType += 1;
	texSlot = totalTexType;
}

// load the texture data based on the file path
void Texture::Init(const std::string& path)
{
	mFilePath = path;
	mLocalBuffer = nullptr;
	mWidth = 0;
	mHeight = 0;
	mBPP = 0;
	mRendererID = 0;

	glGenTextures(1, &mRendererID);

	// flip the image upon loading to match OpenGL's coordinate system
	stbi_set_flip_vertically_on_load(1);

	// load the image data into the local buffer
	mLocalBuffer = stbi_load(path.c_str(), &mWidth, &mHeight, &mBPP, 4);

	glBindTexture(GL_TEXTURE_2D, mRendererID);

	// set texture options
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	// create the texture from the loaded image data
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, mWidth, mHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, mLocalBuffer);

	// unbind it upon finished modifying
	glBindTexture(GL_TEXTURE_2D, 0);

	// free the local image buffer if it was successfully loaded
	if (mLocalBuffer)
		stbi_image_free(mLocalBuffer);


	totalTexType += 1;
	texSlot = totalTexType;
}

// cleans up the texture resources.
Texture::~Texture()
{
	glDeleteTextures(1, &mRendererID);
}

// Bind the texture to the renderer
void Texture::Bind(unsigned int slot) const
{
	glActiveTexture(GL_TEXTURE0 + slot);
	glBindTexture(GL_TEXTURE_2D, mRendererID);
}

// unbind the texture from the renderer
void Texture::Unbind() const
{
	glBindTexture(GL_TEXTURE_2D, 0);
}
