/*!****************************************************************
\file: SpriteAnimation.cpp
\author: Teng Shi Heng, shiheng.teng, 2301269
\brief:
    The `SpriteAnimation` class member function definition where it
    handles the loading, updating, and rendering of static sprite
    or sprite animations using a sprite sheet.

Copyright (C) 2024 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents
without the prior written consent of DigiPen Institute of
Technology is prohibited.
*******************************************************************!*/

#include "SpriteAnimation.h"
#include "assetmanager.h"
#include "glhelper.h"

// default constructor
SpriteAnimation::SpriteAnimation() : uvX(0), uvY(0),
timeNow(0), timeOld(0),
currFrames(0)
{

}

// default constructor
SpriteAnimation::SpriteAnimation(std::string codename, std::string spritePath, float numFrameX, float numFrameY, float numframePS)
{
    // load the texture
    if (AssetManager::GetInstance().IsTextureLoaded(codename))
        AssetManager::GetInstance().LoadTexture(codename, spritePath, numFrameX, numFrameY, numframePS);

    sprite = AssetManager::GetInstance().GetSprite(codename);

    uvX = 0;
    uvY = numFrameY - 1.f;

    timeNow = timeOld = glfwGetTime();
    currFrames = 0;
}

// copy constructor
SpriteAnimation::SpriteAnimation(const SpriteAnimation& other)
    : sprite(other.sprite),  // Copy texture
    timeNow(other.timeNow),
    timeOld(other.timeOld),
    currFrames(other.currFrames),
    uvX(other.uvX),
    uvY(other.uvY),
    isPlaying(other.isPlaying)
{
}


// initialise the sprite animation
void SpriteAnimation::Init(std::string codename, std::string spritePath, float numFrameX, float numFrameY, float numframePS)
{
    // load the texture
    if (AssetManager::GetInstance().IsTextureLoaded(codename))
        AssetManager::GetInstance().LoadTexture(codename, spritePath, numFrameX, numFrameY, numframePS);

    sprite = AssetManager::GetInstance().GetSprite(codename);

    uvX = 0;
    uvY = numFrameY - 1.f;

    timeNow = timeOld = glfwGetTime();
    currFrames = 0;
}

// update the sprite animation frame
void SpriteAnimation::UpdateSprite(double& spriteAccumulatedTime)
{
    // skip the update if it is an static image
    /*if (!isAnimtation)
        return;*/
    if (sprite)
    {

        if (sprite->GetNxFrames() != 1 || sprite->GetNyFrames() != 1)
            sprite->GetAnimation() = true;

        if (!sprite->GetAnimation() || !isPlaying)
        {
            spriteAccumulatedTime = 0;
            return;
        }

        // Calculate the time per frame
        double timePerFrame = 1.0 / sprite->GetFramePs();

        if (spriteAccumulatedTime >= timePerFrame) {
            spriteAccumulatedTime -= timePerFrame;

            uvX += 1.0f;
            currFrames += 1.0f;
            if (uvX >= sprite->GetNxFrames()) {
                uvX = 0.0f;
                uvY -= 1.f;
                if (uvY < 0.f)
                {
                    uvY = sprite->GetNyFrames() - 1.f;
                }
            }
            if (currFrames >= sprite->GetTotalFrames())
            {
                uvX = 0.0f;
                uvY = sprite->GetNyFrames() - 1.f;
                currFrames = 0.f;
            }
        }
    }
}
// reset the sprite animation data when changing states (e.g. Idle -> Moving)
void SpriteAnimation::ResetSpriteAnimation()
{
    uvX = 0;
    uvY = sprite->GetNyFrames() - 1.f;
}