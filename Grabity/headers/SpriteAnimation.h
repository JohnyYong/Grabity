/*!****************************************************************
\file: SpriteAnimation.h
\author: Teng Shi Heng, shiheng.teng, 2301269
\brief:
    The `SpriteAnimation` class handles the loading, updating, and
    rendering of static sprite or sprite animations using a sprite sheet.

Copyright (C) 2024 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents
without the prior written consent of DigiPen Institute of
Technology is prohibited.
*******************************************************************!*/

#pragma once

#include <memory>
#include "Shader.h"
#include "Texture.h"
#include "glhelper.h"

/*!****************************************************************
\brief
    The `SpriteAnimation` class manages sprite animations, allowing
    for frame updates and rendering from a sprite sheet.

    It also caters for static sprite which might be moving into a
    new file in the next milestone.
*******************************************************************!*/
class SpriteAnimation
{

public:

    /*!****************************************************************
    \brief
        Default constructor for the `SpriteAnimation` class.
    *******************************************************************!*/
    SpriteAnimation();

    /*!****************************************************************
    \brief
        Default constructor for the `SpriteAnimation` class.
    *******************************************************************!*/
    SpriteAnimation(std::string codename, std::string spritePath, float numFrameX = 1, float numFrameY = 1, float numframePS = 1);

    /*!****************************************************************
    \brief
        Default constructor for the `SpriteAnimation` class.
    *******************************************************************!*/
    SpriteAnimation(std::shared_ptr<Texture> texture)
    {
        if (texture)
        {
            sprite = texture;

            uvX = 0;
            uvY = sprite->GetNyFrames() - 1.f;

            timeNow = timeOld = glfwGetTime();
            currFrames = 0;
        }
        else
        {
            sprite = nullptr;

            uvX = 0;
            uvY = 0;

            timeNow = timeOld = glfwGetTime();
            currFrames = 0;
        }
    }

    std::unique_ptr<SpriteAnimation> Clone() const {
        return std::make_unique<SpriteAnimation>(*this); // Uses copy constructor
    }


    /*!****************************************************************
    \brief
        Copy constructor for the `SpriteAnimation` class.
    *******************************************************************!*/
    SpriteAnimation(const SpriteAnimation& other);

    /*!****************************************************************
    \brief
        Initializes the sprite animation with a given sprite sheet
        and the number of frames in the X and Y directions.
        In the if the user wants a static sprite, he/she can just
        pass in only the sprite path.

    \param spritePath
        The path to the sprite sheet / sprite image file.

    \param numFrameX
        The number of frames in the horizontal direction (default is 1).

    \param numFrameY
        The number of frames in the vertical direction (default is 1).

    \param numframePS
        The number of frames per second (default is 1).
    *******************************************************************!*/
    void Init(std::string codename, std::string spritePath, float numFrameX = 1, float numFrameY = 1, float numframePS = 1);

    /*!****************************************************************
    \brief
        Updates the current sprite frame based on the elapsed time.

    \param dt
        The time elapsed since the last update (in seconds).
    *******************************************************************!*/
    void UpdateSprite(double& spriteAccumulatedTime);

    /*!****************************************************************
    \brief
        Resets the sprite animation to the initial state.
    *******************************************************************!*/
    void ResetSpriteAnimation();

    inline const float& Get_UV_X() { return uvX; }
    inline const float& Get_UV_Y() { return uvY; }
    inline void Set_UV_X(float x) { uvX = x; }

    inline const std::shared_ptr<Texture>& GetSpriteTexture() const { return sprite; }

    // New methods for animation control
    bool IsPlaying() const { return isPlaying; }
    void PlayAnimation() { isPlaying = true; }
    void PauseAnimation() { isPlaying = false; }
    float GetTextureWidth() const {
        return static_cast<float>(sprite->GetWidth());
    }

    float GetTextureHeight() const {
        return static_cast<float>(sprite->GetHeight());
    }


private:

private:
    std::shared_ptr<Texture> sprite; // The loaded sprite / sprite sheet

    double timeNow;       // Current time for frame updates
    double timeOld;       // Previous time for frame updates

    float currFrames;       // Number of vertical frames in the sprite sheet

    float uvX;            // Current U coordinate for slicing the animation frame
    float uvY;            // Current V coordinate for slicing the animation frame

    bool isPlaying = true;  // New member to track play state
};