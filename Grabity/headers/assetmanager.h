/*!****************************************************************
\file:      assetmanager.h
\author:    Lee Yu Jie Brandon , l.yujiebrandon , 2301232
\brief:     Defines the core asset management system for the game engine.
\details:   This header file defines the AssetManager class which serves
            as a unified interface for loading, retrieving, and managing
            various game assets. It handles:
            - Texture and sprite animation management
            - Audio file loading and retrieval
            - Shader management and initialization
            - Font loading and access
            - Lua script and prefab management

Copyright (C) 2024 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents
without the prior written consent of DigiPen Institute of
Technology is prohibited.
*******************************************************************!*/

#pragma once
#include <unordered_map>
#include <memory>
#include <string>
#include "SpriteAnimation.h"
#include "Shader.h"
#include "Font.h"
#include "graphicsmanager.h"
#include "LuaConfig.h"
#include "Audio.h"
#include <fmod.hpp>
#include <map>


/**
 * @struct ShaderInfo
 * @brief Contains metadata about a shader asset
 */
struct ShaderInfo {
    std::string name;
    std::string path;
    std::string type;
    std::vector<std::pair<std::string, int>> uniforms;
};

/**
 * @struct FontInfo
 * @brief Contains metadata about a font asset
 */
struct FontInfo {
    std::string name;
    std::string path;
    int size;
    std::string type;
};

/**
 * @class AssetManager
 * @brief Singleton manager for all game assets
 *
 * @details This class provides centralized management for:
 *          - Textures and sprite animations
 *          - Audio files (BGM and SFX)
 *          - Shaders and fonts
 *          - Lua scripts and prefabs
 *
 * The singleton pattern ensures consistent asset management across the application.
 */
class AssetManager {
private:
    // Maps texture names to their sprite animation objects
    std::unordered_map<std::string, std::shared_ptr<Texture>> m_Textures;

    // Maps Lua script names to their manager objects
    std::unordered_map<std::string, std::shared_ptr<LuaManager>> m_LuaFiles;

    // Map to store prefabs, indexed by their names.
    std::unordered_map<std::string, std::shared_ptr<LuaManager>> m_Prefabs;

    // Singleton instance pointer
    static std::unique_ptr<AssetManager> instance;

    // Counter for generating unique audio IDs.
    int audioID = 0;

    // Array of shader objects, indexed by ShaderType
    Shader shader[Graphics::S_TOTAL];

    // Array of font objects, indexed by FontType
    Font font[Graphics::F_TOTAL];

    // Loads all required Lua script files at startup
    void PreloadLuaFiles();

    // Loads all required prefab files at startup
    void PreloadPrefabs();


    // Constants for array sizes (adjust these based on your Graphics enum sizes)
    static constexpr size_t SHADER_COUNT = 3; // Number of shader types
    static constexpr size_t FONT_COUNT = 3;   // Number of font types

    // Shader path and type mappings
    const std::pair<std::string, std::string> shaderPaths[SHADER_COUNT] = {
        {"Shaders/Sprite.shader", "S_TEXTURE"},
        {"Shaders/Font.shader", "S_FONT"},
        {"Shaders/Geometry.shader", "S_GEOMETRY"}
    };

    // Font configurations: path, type identifier, and size
    const std::tuple<std::string, std::string, int> fontPaths[FONT_COUNT] = {
        {"Fonts/sleepySans.ttf", "F_SLEEPYSANS", 64},
        {"Fonts/arial.ttf", "F_ARIAL", 48},
        {"Fonts/SquadaOne-Regular.ttf", "F_TIMER", 64}
    };

    // Counter for tracking loaded prefabs 
    int totalPrefabsLoaded{ 0 };

    // Counter for tracking loaded textures
    int totalTextureLoaded = 0;

    // Counter for tracking loaded Lua files
    int totalLuaFilesLoaded{ 0 };

public:


    // Default constructor.
    AssetManager();

    /**
   * @brief Get the singleton instance of the AssetManager.
   * @return Reference to the AssetManager instance.
   */
    static AssetManager& GetInstance();

    // Prevent copying
    AssetManager(const AssetManager&) = delete;
    AssetManager& operator=(const AssetManager&) = delete;


    /*****************************************************
    * Audio
    *****************************************************/

    // Map to store audio objects, indexed by their IDs.
    std::map<int, Audio*> audioObjects;

    // Map to store audio IDs, indexed by their names.
    std::map<std::string, int> audioNameToID;

    /**
     * @brief Get the audio name associated with a given ID.
     * @param audioID The ID of the audio.
     * @return The name of the audio.
     */
    std::string GetAudioNameFromID(int audioID);

    /**
     * @brief Load an audio file.
     * @param name The name to associate with the audio.
     * @param filePath The path to the audio file.
     * @param type The type of the audio (e.g., sound effect, music).
     * @param priority The priority of the audio.
     * @return The ID of the loaded audio.
     */
    int LoadAudio(const std::string& name, const std::string& filePath, AudioType type, int priority);

    /**
    * @brief Load an audio file into the asset manager
    * @param pathName The file path to the audio
    * @param fileName The name to identify the audio
    * @param audioType The type of audio (as an integer)
    * @param priority The priority level for the audio
    */
    void LoadAudios(const std::string& pathName, const std::string& fileName, const int& audioType, const int& priority);

    /**
     * @brief Remove a sound from the asset manager
     * @param id The ID of the sound to remove
     */
    void RemoveSound(const int& soundeName);


    /*****************************************************
    * Textures
    *****************************************************/

    /**
     * @brief Gets a sprite animation by its identifier
     * @param id The identifier of the sprite to retrieve
     * @return Shared pointer to the requested sprite animation
     */
    inline std::shared_ptr<Texture> GetSprite(const std::string& id) { return m_Textures[id]; }

    /**
    * @brief Gets all loaded textures
    * @return Constant reference to the texture map
    */
    const std::unordered_map<std::string, std::shared_ptr<Texture>>& GetTextures() const { return m_Textures; }

    /**
    * @brief Gets the total number of loaded textures
    * @return Number of loaded textures
    */
    int GetTotalTextureLoaded() const { return totalTextureLoaded; }

    /**
     * @brief Initializes all graphics-related assets (shaders and fonts)
     */
    void InitializeGraphicsAssets();

    /**
     * @brief Check if a texture is already loaded
     * @param name The name of the texture to check
     * @return true if the texture is loaded, false otherwise
     */
    bool IsTextureLoaded(const std::string& name) const {
        return m_Textures.find(name) != m_Textures.end();
    }

    /**
     * @brief Remove a texture from the asset manager
     * @param name The name of the texture to remove
     */
    void UnloadTexture(const std::string& name) {
        auto it = m_Textures.find(name);
        if (it != m_Textures.end()) {
            m_Textures.erase(it);
            --totalTextureLoaded;
        }
    }

    /**
     * @brief Load a texture into the asset manager
     * @param pathName The file path to the texture
     * @param fileName The name to identify the texture
     * @param frameX Number of frames in X direction for animation
     * @param frameY Number of frames in Y direction for animation
     * @param animationFrame Animation frame rate
     */
    void LoadTexture(const std::string& pathName, const std::string& fileName, const float& frameX, const float& frameY, const float& animationFrame);

    /**
     * @brief Remove a texture from the asset manager
     * @param textureName The name of the texture to remove
     */
    void RemoveTexture(const std::string& textureName);


    /*****************************************************
    * Shaders
    *****************************************************/

    /**
    * @brief Gets a shader by its type
    * @param type The type of shader to retrieve
    * @return Reference to the requested shader
    */
    Shader& GetShader(Graphics::ShaderType type) { return shader[type]; }

    // Helper method to get shader info
    std::vector<ShaderInfo> GetShaderInfo() const;


    /*****************************************************
    * Fonts
    *****************************************************/

    /**
    * @brief Gets a font by its type
    * @param type The type of font to retrieve
    * @return Reference to the requested font
    */
    Font& GetFont(Graphics::FontType type) { return font[type]; }

    // Helper method to get font info
    std::vector<FontInfo> GetFontInfo() const;


    /*****************************************************
   * Lua Files
   *****************************************************/

   /**
   * @brief Retrieves a Lua script manager by name
   * @param name The name/identifier of the Lua script to retrieve
   * @return Shared pointer to the LuaManager, nullptr if not found
   */
    std::shared_ptr<LuaManager> GetLuaFile(const std::string& name);

    /**
    * @brief Checks if a specific Lua script is loaded
    * @param name The name/identifier of the Lua script to check
    * @return true if the script is loaded, false otherwise
    */
    bool IsLuaFileLoaded(const std::string& name) const;

    /**
    * @brief Gets the total number of Lua scripts currently loaded
    * @return Number of loaded Lua scripts
    */
    int GetTotalLuaFilesLoaded() const { return totalLuaFilesLoaded; }

    /**
    * @brief Gets all loaded Lua script managers
    * @return Constant reference to the map of Lua managers
    * @note Map is indexed by script names and contains shared pointers to LuaManager objects
    */
    const std::unordered_map<std::string, std::shared_ptr<LuaManager>>& GetLuaManagers() const { return  m_LuaFiles; }


    /*****************************************************
   * Prefabs
   *****************************************************/

   /**
   * @brief Retrieves a prefab by name
   * @param name The name/identifier of the prefab to retrieve
   * @return Shared pointer to the LuaManager handling the prefab, nullptr if not found
   */
    std::shared_ptr<LuaManager> GetPrefab(const std::string& name);

    /**
    * @brief Checks if a specific prefab is loaded
    * @param name The name/identifier of the prefab to check
    * @return true if the prefab is loaded, false otherwise
    */
    bool IsPrefabLoaded(const std::string& name) const;

    /**
    * @brief Gets the total number of prefabs currently loaded
    * @return Number of loaded prefabs
    */
    int GetTotalPrefabsLoaded() const { return totalPrefabsLoaded; }

    /**
    * @brief Gets all loaded prefabs
    * @return Constant reference to the map of prefab managers
    * @note Map is indexed by prefab names and contains shared pointers to LuaManager objects
    */
    const std::unordered_map<std::string, std::shared_ptr<LuaManager>>& GetPrefabs() const { return m_Prefabs; }


};