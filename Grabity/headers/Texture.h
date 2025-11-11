/*!****************************************************************
\file: Texture.h
\author: Teng Shi Heng, shiheng.teng, 2301269
\brief:
    The `Texture` class manages the loading, binding, and
    cleanup of texture resources for use in graphical applications.

Copyright (C) 2024 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents
without the prior written consent of DigiPen Institute of
Technology is prohibited.
*******************************************************************!*/

#pragma once

#include <string>

class Texture
{
public:
    // Constructors & Destructor
    Texture();
    Texture(std::string codename, const std::string& path, float numFrameX = 1, float numFrameY = 1, float numframePS = 1);
    Texture(const Texture& texture);
    ~Texture();

    // Texture Operations
    void Init(const std::string& path);
    void Bind(unsigned int slot = 0) const;
    void Unbind() const;

    // Getters
    inline const int& GetWidth() const { return mWidth; }
    inline const int& GetHeight() const { return mHeight; }
    inline const unsigned int& GetTextureID() const { return mRendererID; }
    inline const unsigned int& GetTextureSlot() const { return texSlot; }
    inline const unsigned int& GetTotalTextureType() const { return totalTexType; }
    inline double& GetFramePs() { return framePS; }
    inline float& GetNxFrames() { return nxFrames; }
    inline float& GetNyFrames() { return nyFrames; }
    inline float& GetTotalFrames() { return totalFrames; }
    inline bool& GetAnimation() { return isAnimtation; }

    // Setters
    inline void SetNxFrames(int frames) { nxFrames = static_cast<float>(frames); }
    inline void SetNyFrames(int frames) { nyFrames = static_cast<float>(frames); }
    inline void SetTotalFrames(int frames) { totalFrames = static_cast<float>(frames); }
    void SetFramePs(double newFrameRate) { framePS = newFrameRate; }

    std::string codeName;
private:
    unsigned int mRendererID;
    static unsigned int totalTexType;
    unsigned int texSlot;

    std::string mFilePath;
    unsigned char* mLocalBuffer;

    int mWidth;
    int mHeight;
    int mBPP;

    double framePS;
    float nxFrames;
    float nyFrames;
    float totalFrames;
    bool isAnimtation;
};
