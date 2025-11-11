/*!****************************************************************
\file:      assetmanager.cpp
\author:    Lee Yu Jie Brandon , l.yujiebrandon , 2301232
\co-author: Teng Shi Heng, shiheng.teng, 2301269
\brief:     Implementation of the AssetManager class functionality.
\details:   Implements the core asset management operations including
            resource loading, initialization, and access methods.

            Brandon implemeneted all the asset loading and getting functionality
            and initialisation of the assetmanager (90%)

            Shi Heng changed all the textures that being loaded as shared_ptr
            spriteanimation class to shared_ptr texture class such that all the
            spriteanimation created is unique to each gameobject. (10%)

Copyright (C) 2024 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents
without the prior written consent of DigiPen Institute of
Technology is prohibited.
*******************************************************************!*/

#include "assetmanager.h"
#include "AudioManager.h"
#include "Font.h"

#ifdef _IMGUI
#include <iostream>
#include <ImGuiConsole.h>
#endif // _IMGUI
#include "GameObjectFactory.h"

// Initialize the static instance pointer
std::unique_ptr<AssetManager> AssetManager::instance = nullptr;

/**
 * @brief Get the singleton instance of AssetManager.
 * @return Reference to the AssetManager instance.
 */
AssetManager& AssetManager::GetInstance()
{
    if (instance == nullptr)
    {
        instance = std::make_unique<AssetManager>();
    }
    return *instance;
}

/**
 * @brief Get the audio name associated with a given ID.
 * @param audioID The ID of the audio to look up.
 * @return The name of the audio, or "Cannot find audio" if not found.
 */
std::string AssetManager::GetAudioNameFromID(int theAudioID)
{
    auto it = audioObjects.find(theAudioID);
    if (it != audioObjects.end())
    {
        return it->second->GetAudioName();
    }
    return "Cannot find audio";
}

/**
 * @brief Construct a new Asset Manager object
 *
 * @details Initializes the asset manager by:
 *          1. Loading default textures and sprite animations
 *          2. Setting up audio resources
 *          3. Preloading Lua scripts and prefabs
 *          Each asset type is tracked with a counter for monitoring
 */
AssetManager::AssetManager()
{

    //Icons need to be initialized here
    m_Textures["Folder_Icon"] = std::make_shared<Texture>("Folder_Icon", "Assets/Textures/DirectoryIcon.png");
    ++totalTextureLoaded;

    m_Textures["File_Icon"] = std::make_shared<Texture>("File_Icon", "Assets/Textures/FileIcon.png");
    ++totalTextureLoaded;

    m_Textures["PlayIcon"] = std::make_shared<Texture>("PlayIcon", "Assets/Textures/PlayIcon.png");
    ++totalTextureLoaded;

    m_Textures["PauseIcon"] = std::make_shared<Texture>("PauseIcon", "Assets/Textures/PauseIcon.png");
    ++totalTextureLoaded;

    m_Textures["StopIcon"] = std::make_shared<Texture>("StopIcon", "Assets/Textures/StopIcon.png");
    ++totalTextureLoaded;


    m_Textures["ArrowUpIcon"] = std::make_shared<Texture>("ArrowIcon", "Assets/Textures/Arrow_Up.png");
    ++totalTextureLoaded;

    m_Textures["ArrowRightIcon"] = std::make_shared<Texture>("ArrowIcon", "Assets/Textures/Arrow_Right.png");
    ++totalTextureLoaded;


    PreloadLuaFiles();
    PreloadPrefabs();

}

/**
 * @brief Load an audio file.
 * @param audioName The name to associate with the audio.
 * @param filePath The path to the audio file.
 * @param type The type of the audio (e.g., sound effect, music).
 * @param priority The priority of the audio.
 * @return The ID of the loaded audio, or the existing ID if already loaded.
 */
int AssetManager::LoadAudio(const std::string& audioName, const std::string& filePath, AudioType type, int priority)
{
    if (audioNameToID.find(audioName) == audioNameToID.end())
    {
        int newID = audioID++;
        audioObjects[newID] = new Audio(audioName, filePath, type, priority);
        audioNameToID[audioName] = newID;
        return newID;
    }
    return audioNameToID[audioName];
}

/**
 * @brief Initialize graphics-related assets
 *
 * @details Sets up all shader programs and fonts:
 *          1. Configures texture shader with appropriate uniforms
 *          2. Sets up font shader for text rendering
 *          3. Initializes geometry shader for basic shapes
 *          4. Loads and configures all required fonts
 */
void AssetManager::InitializeGraphicsAssets()
{
    // Initialize shaders
    shader[Graphics::S_TEXTURE].SetShader("Assets/Shaders/Sprite.shader");
    shader[Graphics::S_TEXTURE].Bind();
    shader[Graphics::S_TEXTURE].SetUniform1i("u_Texture", 0);
    shader[Graphics::S_TEXTURE].Unbind();

    shader[Graphics::S_FONT].SetShader("Assets/Shaders/Font.shader");
    shader[Graphics::S_FONT].Bind();
    shader[Graphics::S_FONT].SetUniform1i("u_Texture", 0);
    shader[Graphics::S_FONT].Unbind();

    shader[Graphics::S_GEOMETRY].SetShader("Assets/Shaders/Geometry.shader");
    shader[Graphics::S_GEOMETRY].Bind();
    shader[Graphics::S_GEOMETRY].Unbind();

    // Initialize fonts
    font[Graphics::F_SLEEPYSANS].LoadFont("Assets/Fonts/sleepySans.ttf", 64);
    font[Graphics::F_ARIAL].LoadFont("Assets/Fonts/arial.ttf", 48);
    font[Graphics::F_TIMER].LoadFont("Assets/Fonts/SquadaOne-Regular.ttf", 64);

    // Initialize particles
    shader[Graphics::S_PARTICLE].SetShader("Assets/Shaders/Particle.shader");
    shader[Graphics::S_PARTICLE].Bind();
    shader[Graphics::S_PARTICLE].SetUniform1f("pointSize", 10.0f);
    shader[Graphics::S_PARTICLE].Unbind();
}


/**
 * @brief Preload all required Lua script files
 *
 * @details Loads script files from the Scenes directory:
 *          - Handles loading errors gracefully
 *          - Tracks loading progress
 *          - Reports success/failure through console
 *
 * @throw std::exception If a critical loading error occurs
 */
void AssetManager::PreloadLuaFiles()
{
    try {
        // Define the base path for Lua files
        const std::string luaBasePath = "Assets/Lua/Scenes/";

        // Preload all necessary Lua files
        // Scene files
        m_LuaFiles["Scene1"] = std::make_shared<LuaManager>(luaBasePath + "Scene1.lua");
        ++totalLuaFilesLoaded;

        //m_LuaFiles["Scene2"] = std::make_shared<LuaManager>(luaBasePath + "Scene2.lua");
        //++totalLuaFilesLoaded;

        //m_LuaFiles["Scene3"] = std::make_shared<LuaManager>(luaBasePath + "Scene3.lua");
        //++totalLuaFilesLoaded;
#ifdef _LOGGING
        // Add more Lua files as needed...
        ImGuiConsole::Cout("Successfully preloaded %d Lua files.\n", totalLuaFilesLoaded);
#endif // _LOGGING
    }
    catch (const std::exception& e) {
        ImGuiConsole::Cout("Error preloading Lua files: %s\n", e.what());
    }

}


/**
 * @brief Preload all required Lua script files
 *
 * @details Loads script files from the Scenes directory:
 *          - Handles loading errors gracefully
 *          - Tracks loading progress
 *          - Reports success/failure through console
 *
 * @throw std::exception If a critical loading error occurs
 */
std::shared_ptr<LuaManager> AssetManager::GetLuaFile(const std::string& name)
{
    auto it = m_LuaFiles.find(name);
    if (it != m_LuaFiles.end()) {
        return it->second;
    }
    ImGuiConsole::Cout("Lua file %s not found.\n", name.c_str());
    return nullptr;
}

/**
 * @brief Checks if a specific Lua file is currently loaded
 * @param name The name/identifier of the Lua file to check
 * @return true if the Lua file is loaded, false otherwise
 */
bool AssetManager::IsLuaFileLoaded(const std::string& name) const
{
    return m_LuaFiles.find(name) != m_LuaFiles.end();
}


/**
 * @brief Retrieves information about all registered shaders
 * @details Collects shader metadata including:
 *          - File paths
 *          - Shader types
 *          - Uniform variables
 *          - Shader names
 * @return Vector containing ShaderInfo structs for all shaders
 */
std::vector<ShaderInfo> AssetManager::GetShaderInfo() const {
    std::vector<ShaderInfo> shaderInfos;

    for (size_t i = 0; i < SHADER_COUNT; ++i) {
        ShaderInfo info;
        info.path = shaderPaths[i].first;
        info.type = shaderPaths[i].second;
        info.name = info.path.substr(info.path.find_last_of("/\\") + 1);

        // Add known uniforms based on shader type
        if (info.type == "S_TEXTURE" || info.type == "S_FONT") {
            info.uniforms.push_back({ "u_Texture", 0 });
        }

        shaderInfos.push_back(info);
    }

    return shaderInfos;
}


/**
 * @brief Retrieves information about all registered fonts
 * @details Collects font metadata including:
 *          - File paths
 *          - Font types
 *          - Font sizes
 *          - Font names
 * @return Vector containing FontInfo structs for all fonts
 */
std::vector<FontInfo> AssetManager::GetFontInfo() const {
    std::vector<FontInfo> fontInfos;

    for (size_t i = 0; i < FONT_COUNT; ++i) {
        FontInfo info;
        info.path = std::get<0>(fontPaths[i]);
        info.type = std::get<1>(fontPaths[i]);
        info.size = std::get<2>(fontPaths[i]);
        info.name = info.path.substr(info.path.find_last_of("/\\") + 1);

        fontInfos.push_back(info);
    }

    return fontInfos;
}

/**
 * @brief Preloads all predefined prefab assets
 * @details Loads prefab files from the Prefabs directory:
 *          - Attempts to load each predefined prefab
 *          - Tracks the number of successfully loaded prefabs
 *          - Handles loading errors gracefully
 * @throw std::exception If a critical loading error occurs
 */
void AssetManager::PreloadPrefabs()
{
    try {
        // Define the base path for prefab files, similar to Lua base path
        const std::string prefabBasePath = "Assets/Lua/Prefabs/";

        //// Preload all necessary prefabs, following Lua file loading pattern
        //m_Prefabs["Ame"] = std::make_shared<LuaManager>(prefabBasePath + "Ame.lua");
        //++totalPrefabsLoaded;

        //m_Prefabs["Ina"] = std::make_shared<LuaManager>(prefabBasePath + "Ina.lua");
        //++totalPrefabsLoaded;

        // Add more prefabs as needed...
#ifdef _LOGGING
        ImGuiConsole::Cout("Successfully preloaded %d prefabs.", totalPrefabsLoaded);
#endif // _LOGGING
    }
    catch (const std::exception& e) {
        ImGuiConsole::Cout("Error preloading prefabs: %s", e.what());
    }

}

/**
 * @brief Retrieves a loaded prefab by name
 * @param name The name/identifier of the prefab to retrieve
 * @return Shared pointer to the LuaManager for the prefab, nullptr if not found
 */
std::shared_ptr<LuaManager> AssetManager::GetPrefab(const std::string& name)
{
    auto it = m_Prefabs.find(name);
    if (it != m_Prefabs.end()) {
        return it->second;
    }
    ImGuiConsole::Cout("Prefab '%s' not found.", name.c_str());
    return nullptr;
}

/**
 * @brief Checks if a specific prefab is currently loaded
 * @param name The name/identifier of the prefab to check
 * @return true if the prefab is loaded, false otherwise
 */
bool AssetManager::IsPrefabLoaded(const std::string& name) const
{
    return m_Prefabs.find(name) != m_Prefabs.end();
}


/**
 * @brief Load a texture into the asset manager
 * @param pathName The file path to the texture
 * @param fileName The name to identify the texture
 * @param frameX Number of frames in X direction for animation
 * @param frameY Number of frames in Y direction for animation
 * @param animationFrame Animation frame rate
 * @details Special handling for predefined animations:
 *          - Animation_Ame: 6x5 frames at 30fps
 *          - Animation_Ina: 3x2 frames at 30fps
 *          - Animation_PlayerWalk: 9x1 frames at 25fps
 *          - Animation_Heavy_Enemy: 2x1 frames at 10fps
 *          - Animation_Light_Enemy: 2x1 frames at 10fps
 */
void AssetManager::LoadTexture(const std::string& pathName, const std::string& fileName, const float& frameX, const float& frameY, const float& animationFrame) {

    if (fileName == "Animation_Ame") {
        m_Textures[fileName] = std::make_shared<Texture>(fileName, pathName, 6.0f, 5.0f, 30.f);
        ++totalTextureLoaded;
    }
    else if (fileName == "Animation_Ina") {
        m_Textures[fileName] = std::make_shared<Texture>(fileName, pathName, 3.0f, 2.0f, 30.f);
        ++totalTextureLoaded;
    }
    else if (fileName == "Animation_PlayerWalk")
    {
        m_Textures[fileName] = std::make_shared<Texture>(fileName, pathName, 4.0f, 2.0f, 8.f);
        ++totalTextureLoaded;
    }
    else if (fileName == "Animation_Heavy_Enemy")
    {
        m_Textures[fileName] = std::make_shared<Texture>(fileName, pathName, 2.0f, 3.0f, 6.f);
        ++totalTextureLoaded;
    }
    else if (fileName == "Animation_Light_Enemy")
    {
        m_Textures[fileName] = std::make_shared<Texture>(fileName, pathName, 2.0f, 3.0f, 5.f);
        ++totalTextureLoaded;
    }
    else if (fileName == "Animation_Bomb_Enemy")
    {
        m_Textures[fileName] = std::make_shared<Texture>(fileName, pathName, 3.0f, 1.0f, 3.f);
        ++totalTextureLoaded;
    }
    else if (fileName == "explosin")
    {
        m_Textures[fileName] = std::make_shared<Texture>(fileName, pathName, 1.0f, 1.0f, 10.f);
        ++totalTextureLoaded;

    }
    else if (fileName == "Animation_GrabityTitle")
    {
        m_Textures[fileName] = std::make_shared<Texture>(fileName, pathName, 2.0f, 3.0f, 10.f);
        m_Textures[fileName]->SetTotalFrames(5);
        ++totalTextureLoaded;

    }
    else if (fileName == "Animation_Particle_Plant_B")
    {
        m_Textures[fileName] = std::make_shared<Texture>(fileName, pathName, 2.0f, 1.0f, 1.f);
        ++totalTextureLoaded;
    }
    else if (fileName == "Animation_hitVFX")
    {
        m_Textures[fileName] = std::make_shared<Texture>(fileName, pathName, 2.0f, 3.0f, 6.f);
        ++totalTextureLoaded;
    }
    else {
        m_Textures[fileName] = std::make_shared<Texture>(fileName, pathName, frameX, frameY, animationFrame);
        ++totalTextureLoaded;
    }

}


/**
 * @brief Load an audio file into the asset manager
 * @param pathName The file path to the audio
 * @param fileName The name to identify the audio
 * @param audioType The type of audio (as an integer)
 * @param priority The priority level for the audio
 */
void AssetManager::LoadAudios(const std::string& pathName, const std::string& fileName, const int& audioType, const int& priority) {

    LoadAudio(fileName, pathName, static_cast<AudioType>(audioType), priority);
}


/**
 * @brief Remove a texture from the asset manager
 * @param textureName The name of the texture to remove
 */
void AssetManager::RemoveTexture(const std::string& textureName) {
    auto it = m_Textures.find(textureName);
    if (it != m_Textures.end()) {
        // First update all GameObjects using this texture
        GameObjectFactory& factory = GameObjectFactory::GetInstance();
        for (const auto& [id, gameObj] : factory.GetAllGameObjects()) {
            if (SpriteComponent* sprite = gameObj->GetComponent<SpriteComponent>(TypeOfComponent::SPRITE)) {
                if (sprite->HasSprite(textureName)) {
                    sprite->RemoveSpriteByName(textureName);
                }
            }
        }

        // Then remove the texture from the asset manager
        m_Textures.erase(it);
        --totalTextureLoaded;
#ifdef _LOGGING
        ImGuiConsole::Cout("Texture \"%s\" removed successfully.", textureName.c_str());
#endif // _LOGGING
    }
}


/**
 * @brief Remove a sound from the asset manager
 * @param id The ID of the sound to remove
 */
void AssetManager::RemoveSound(const int& id) {
    // Map to store audio objects, indexed by their IDs.
    auto it = audioObjects.find(id);
    if (it != audioObjects.end()) {
        audioObjects.erase(it);
#ifdef _LOGGING
        ImGuiConsole::Cout("Audio ID:  \"%d\" removed successfully.", id);
#endif // _LOGGING
    }
    else {
#ifdef _LOGGING
        ImGuiConsole::Cout("Error: Audio ID: \"%d\" not found.", id);
#endif // _LOGGING
    }
}

