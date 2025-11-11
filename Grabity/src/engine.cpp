/*!****************************************************************
\file: Engine.cpp
\author: Moahmed Rudhwan Bin Mohamed Afandi, mohamedridhwan.b, 2301367
\co-author: Goh Jun Jie, g.junjie, 2301293
            Lee Yu Jie Brandon , l.yujiebrandon , 2301232
            Johny Yong Jun Siang, j.yong, 2301301
            Jeremy Lim Ting Jie, jeremytingjie.lim, 2301370
            Teng Shi Heng, shiheng.teng, 2301269
\brief: Handle initialization, updating, rendering, and state management, 
        and coordinate these operations across the different managers and components.
        Includes various functions for ImGUI windows under EngineImGuiWindows namespace
        
        Moahmed Rudhwan Bin Mohamed Afandi (20%)
        Goh Jun Jie, g.junjie (16%)
        Lee Yu Jie Brandon (16%)
        Johny Yong Jun Siang (16%)
        Jeremy Lim Ting Jie (16%)
        Teng Shi Heng (16%)

Copyright (C) 2024 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents
without the prior written consent of DigiPen Institute of
Technology is prohibited.
*******************************************************************!*/
#include <engine.h>
#include <pch.h>
#include <AudioManager.h>
#include "GameObjectFactory.h"
//#include "CSVMapLoader.h"
#include "PhysicsSystem.h"
#include "RigidBodyComponent.h"
#include "ButtonComponent.h"
#include "UIComponent.h"
#include "UISystem.h"
#include "ExplosionComponent.h"
#include "ParticleSystem.h"
#include "EventSystem.h"

#ifdef _IMGUI
#include <imguimanager.h>
#include <imgui.h>
#include "ContentBrowser.h"
#include "FrameBuffer.h"
#endif // _IMGUI
#include "ImGuiConsole.h"

#ifdef _IMGUI
#include <iostream>
#endif // _IMGUI
#include "AIStateBase.h"

#ifdef _LOGGING
#include "GLWrapper.h"
#include "SystemLogging.h"
#include "SpawnerComponent.h"
#endif // _LOGGING

#include "GraphicsManager.h"
#include "TagManager.h"
#include "GLHelper.h"
#include "assetmanager.h"
#include <chrono>
#include <thread>
#include <filesystem>  // For std::filesystem::exists
#include "PlayerSceneControls.h"

#include "AnimationController.h"
#include "DespawnManager.h"



#pragma warning(disable : 4996)

struct FilePayload {
    char path[256];           // Buffer for the file path
    bool isPrefab;           // Flag to distinguish between prefabs and scenes
    bool isTexture;         // Flag for textures
    bool isSound;          // Flag for sound files
    char textureName[256];  // Buffer for texture name
    char soundName[256];   // Buffer for sound name
    int soundID;          // ID of the sound in the AssetManager
};


// Engine singleton instance
std::unique_ptr<Engine> Engine::instance = nullptr;
//AssetManager Engine::assetManager; // Add static AssetManager instance

PhysicsSystem physicsSystem;
AnimationController controller;

#ifdef _IMGUI
FrameBuffer frameBuffer;
#endif // _IMGUI

// Test flags. Comment out to disable test.
#define TEST_SOUND
#define TEST_UI
#define GAMEOBJECT_COMPONENTS
#define IMGUI
#define TEST_ASSETMAN 
#define LOGGING
#define TEST_CONTENTBROWSER



// Globals
//constexpr float targetFPS = 60.f;
//constexpr float frameDuration = 1000.0f / targetFPS;
std::string currentScene;
double accumulatedTime = (0.0);
double oldTime = glfwGetTime();

static size_t previousFrameHolderSize = 0;

//Testing select GO


namespace Utilities {

    //Cheat codes that can be activated in game with Function keys
    void CheatCodes() {
        //GOD MODE
        if (InputManager::IsKeyReleased(GLFW_KEY_F1)) {
            if (!Engine::GetInstance().isGodMode) {
                Engine::GetInstance().isGodMode = true;
                GameObject* player = GameObjectFactory::GetInstance().GetPlayerObject();
                player->GetComponent<RectColliderComponent>(TypeOfComponent::RECTCOLLIDER)->SetTrigger(true);

#ifdef _LOGGING
                ImGuiConsole::Cout("God Mode activated!");

#endif // _LOGGING
            }
            else if (Engine::GetInstance().isGodMode) {
                GameObject* player = GameObjectFactory::GetInstance().GetPlayerObject();
                player->GetComponent<RectColliderComponent>(TypeOfComponent::RECTCOLLIDER)->SetTrigger(false);
#ifdef _LOGGING
                ImGuiConsole::Cout("God Mode de-activated!");
#endif // _LOGGING
                Engine::GetInstance().isGodMode = false;
            }
        }

        //SET TIMER TO 10 SECONDS
        if (InputManager::IsKeyReleased(GLFW_KEY_F2)) {
            Engine& engine = Engine::GetInstance();
            engine.time = 10.0f;
#ifdef _LOGGING
            ImGuiConsole::Cout("Timer set to 10 seconds!");
#endif // _LOGGING
        }

        //RESET TIMER
        if (InputManager::IsKeyReleased(GLFW_KEY_F3)) {
            Engine& engine = Engine::GetInstance();
            engine.time = engine.maxTime;
#ifdef _LOGGING
            ImGuiConsole::Cout("Timer resetted!");
#endif // _LOGGING
        }

        //RESET PLAYER POSITION TO ORIGIN
        if (InputManager::IsKeyReleased(GLFW_KEY_F4)) {
            GameObject* player = GameObjectFactory::GetInstance().GetPlayerObject();
            TransformComponent* transform = player->GetComponent<TransformComponent>(TypeOfComponent::TRANSFORM);
            transform->SetLocalPosition(Vector2(0, 0));
#ifdef _LOGGING
            ImGuiConsole::Cout("Reset player back to origin!");
#endif // _LOGGING
        }

        //RESTORE PLAYER TO FULL HEALTH
        if (InputManager::IsKeyReleased(GLFW_KEY_F5)) {
            GameObject* player = GameObjectFactory::GetInstance().GetPlayerObject();
            HealthComponent* health = player->GetComponent<HealthComponent>(TypeOfComponent::HEALTH);
            health->SetHealth(50);
#ifdef _LOGGING
            ImGuiConsole::Cout("Player restored to full health!");
#endif // _LOGGING
        }

        if (InputManager::IsKeyReleased(GLFW_KEY_F6)) {
            GameObjectFactory& factory = GameObjectFactory::GetInstance();
            GameObject* newEnemy = factory.CreateFromLua("Assets/Lua/Prefabs/Heavy_Enemy.lua", "Heavy_Enemy_0");

            // Get the player object to set as the chase target
            GameObject* player = factory.GetPlayerObject();

            // Configure AI behavior
            if (player && newEnemy) {
                AIStateMachineComponent* aiComponent = newEnemy->GetComponent<AIStateMachineComponent>(TypeOfComponent::AISTATE);
                if (aiComponent) {
                    aiComponent->SetState("CHASE");
                    aiComponent->SetChaseTarget(player);
                    aiComponent->SetMoveSpeed(100.0f);
                }
            }
        }

        if (InputManager::IsKeyReleased(GLFW_KEY_F7)) {
            GameObjectFactory& factory = GameObjectFactory::GetInstance();
            GameObject* newBombEnemy = factory.CreateFromLua("Assets/Lua/Prefabs/Bomb_Enemy.lua", "Bomb_Enemy_0");

            // Get the player object to set as the chase target
            GameObject* player = factory.GetPlayerObject();

            // Configure AI behavior
            if (player && newBombEnemy) {
                AIStateMachineComponent* aiComponent = newBombEnemy->GetComponent<AIStateMachineComponent>(TypeOfComponent::AISTATE);
                if (aiComponent) {
                    aiComponent->SetState("CHASE");
                    aiComponent->SetChaseTarget(player);
                }
            }
        }

        if (InputManager::IsKeyReleased(GLFW_KEY_F8)) {
            GameObjectFactory& factory = GameObjectFactory::GetInstance();
            factory.CreateFromLua("Assets/Lua/Prefabs/Light_Enemy.lua", "Light_Enemy_0");
        }
    }
    //Function to convert a string to lowercase
    std::string ToLower(const std::string& str) {
        std::string lowerStr = str;
        std::transform(lowerStr.begin(), lowerStr.end(), lowerStr.begin(),
            [](unsigned char c) { return (char)std::tolower(c); });
        return lowerStr;
    }

    //To search component by names when trying to add component in run time
    std::vector<std::pair<TypeOfComponent, std::string>> componentNames = {
        {TypeOfComponent::TRANSFORM, "Transform"},
        {TypeOfComponent::SPRITE, "Sprite"},
        {TypeOfComponent::RECTCOLLIDER, "RectCollider"},
        {TypeOfComponent::AUDIO, "Audio"},
        {TypeOfComponent::TEXT, "Text"},
        {TypeOfComponent::RIGIDBODY, "RigidBody"},
        {TypeOfComponent::AISTATE, "AIState"},
        {TypeOfComponent::HEALTH,"Health"},
        {TypeOfComponent::SPAWNER,"Spawner"},
        {TypeOfComponent::BUTTON, "Button" }, 
		{TypeOfComponent::UI, "UI"},
        //////////////////////////////////////
        {TypeOfComponent::CANVAS_UI, "CanvasUI"},
        {TypeOfComponent::TEXT_UI, "TextUI"},
        {TypeOfComponent::SPRITE_UI, "SpriteUI"},
        //////////////////////////////////////
        {TypeOfComponent::PLAYER, "PlayerController"},
        {TypeOfComponent::EXPLOSION, "Explosion"},
        {TypeOfComponent::PARTICLE, "Particle"},

        {TypeOfComponent::PAUSEMENUBUTTON, "Pause Menu Button Script"},
        {TypeOfComponent::ANIMATOR, "Animator"},
        {TypeOfComponent::SLIDER, "Slider Component"},
        {TypeOfComponent::SPLITTING, "Splitting"},
        {TypeOfComponent::VIDEO, "Video"},
        {TypeOfComponent::VFX_FOLLOW, "VFX Follow Script"},

    };


#ifdef _IMGUI
    // display the component available to be attached to the gameobject
    void GUIComponentAdder(GameObject* selectedGO)
    {
        if (!selectedGO)
            return;

        // Dropdown for selecting components to add
        std::vector<const char*> componentCStrs;
        for (const auto& component : componentNames) {
            componentCStrs.push_back(component.second.c_str());
        }

        static int selectedComponentIndex = 0;  // Index for the dropdown
        ImGui::Combo("Component Lists", &selectedComponentIndex, componentCStrs.data(), (int)componentCStrs.size());


        // Check if the selected component already exists
        bool hasComponent = false;
        TypeOfComponent selectedComponent = componentNames[selectedComponentIndex].first;
        switch (selectedComponent) {
        case TypeOfComponent::TRANSFORM:
            hasComponent = selectedGO->GetComponent<TransformComponent>(TypeOfComponent::TRANSFORM) != nullptr;
            break;
        case TypeOfComponent::SPRITE:
            hasComponent = selectedGO->GetComponent<SpriteComponent>(TypeOfComponent::SPRITE) != nullptr;
            break;
        case TypeOfComponent::RECTCOLLIDER:
            hasComponent = selectedGO->GetComponent<RectColliderComponent>(TypeOfComponent::RECTCOLLIDER) != nullptr;
            break;
        case TypeOfComponent::AUDIO:
            hasComponent = selectedGO->GetComponent<AudioComponent>(TypeOfComponent::AUDIO) != nullptr;
            break;
        case TypeOfComponent::TEXT:
            hasComponent = selectedGO->GetComponent<TextComponent>(TypeOfComponent::TEXT) != nullptr;
            break;
        case TypeOfComponent::RIGIDBODY:
            hasComponent = selectedGO->GetComponent<RigidBodyComponent>(TypeOfComponent::RIGIDBODY) != nullptr;
            break;
        case TypeOfComponent::AISTATE:
            hasComponent = selectedGO->GetComponent<AIStateMachineComponent>(TypeOfComponent::AISTATE) != nullptr;
            break;
        case TypeOfComponent::HEALTH:
            hasComponent = selectedGO->GetComponent<HealthComponent>(TypeOfComponent::HEALTH) != nullptr;
            break;
        case TypeOfComponent::SPAWNER:
            hasComponent = selectedGO->GetComponent<SpawnerComponent>(TypeOfComponent::SPAWNER) != nullptr;
            break;
		case TypeOfComponent::BUTTON:
			hasComponent = selectedGO->GetComponent<ButtonComponent>(TypeOfComponent::BUTTON) != nullptr;
			break;
		case TypeOfComponent::UI:
			hasComponent = selectedGO->GetComponent<UIComponent>(TypeOfComponent::UI) != nullptr;
			break;
        case TypeOfComponent::CANVAS_UI:
            hasComponent = selectedGO->GetComponent<CanvasComponent>(TypeOfComponent::CANVAS_UI) != nullptr;
            break;
        case TypeOfComponent::TEXT_UI:
            hasComponent = selectedGO->GetComponent<UITextComponent>(TypeOfComponent::TEXT_UI) != nullptr;
            break;
        case TypeOfComponent::SPRITE_UI:
            hasComponent = selectedGO->GetComponent<UISpriteComponent>(TypeOfComponent::SPRITE_UI) != nullptr;
            break;
        case TypeOfComponent::PLAYER:
            hasComponent = selectedGO->GetComponent<PlayerControllerComponent>(TypeOfComponent::PLAYER) != nullptr;
            break;
        case TypeOfComponent::EXPLOSION:
            hasComponent = selectedGO->GetComponent<ExplosionComponent>(TypeOfComponent::EXPLOSION) != nullptr;
            break;
        case TypeOfComponent::PARTICLE:
            hasComponent = selectedGO->GetComponent<ParticleSystem>(TypeOfComponent::PARTICLE) != nullptr;
            break;
        case TypeOfComponent::PAUSEMENUBUTTON:
            hasComponent = selectedGO->GetComponent<PauseMenuButton>(TypeOfComponent::PAUSEMENUBUTTON) != nullptr;
            break;
        case TypeOfComponent::ANIMATOR:
            hasComponent = selectedGO->GetComponent<AnimatorComponent>(TypeOfComponent::ANIMATOR) != nullptr;
            break;
        case TypeOfComponent::SLIDER:
            hasComponent = selectedGO->GetComponent<SliderComponent>(TypeOfComponent::SLIDER) != nullptr;
            break;
        case TypeOfComponent::SPLITTING:
            hasComponent = selectedGO->GetComponent<SplittingComponent>(TypeOfComponent::SPLITTING) != nullptr;
            break;
        case TypeOfComponent::VIDEO:
            hasComponent = selectedGO->GetComponent<VideoComponent>(TypeOfComponent::VIDEO) != nullptr;
            break;
        case TypeOfComponent::VFX_FOLLOW:
            hasComponent = selectedGO->GetComponent<VfxFollowComponent>(TypeOfComponent::VFX_FOLLOW) != nullptr;
            break;
        default:
            break;
        }

        if (!hasComponent) {

            //Spawns the add component button only if the game object does not have the component
            // Add the selected component when the user clicks the "Add" button
            if (ImGui::Button("Add Component")) {
                switch (selectedComponent) {
                case TypeOfComponent::TRANSFORM:
                    selectedGO->AddComponent<TransformComponent>(TypeOfComponent::TRANSFORM);
                    break;
                case TypeOfComponent::SPRITE:
                    selectedGO->AddComponent<SpriteComponent>(TypeOfComponent::SPRITE);
                    break;
                case TypeOfComponent::RECTCOLLIDER:
                    selectedGO->AddComponent<RectColliderComponent>(TypeOfComponent::RECTCOLLIDER);
                    break;
                case TypeOfComponent::AUDIO:
                    selectedGO->AddComponent<AudioComponent>(TypeOfComponent::AUDIO);
                    break;
                case TypeOfComponent::TEXT:
                    selectedGO->AddComponent<TextComponent>(TypeOfComponent::TEXT);
                    break;
                case TypeOfComponent::RIGIDBODY:
                    selectedGO->AddComponent<RigidBodyComponent>(TypeOfComponent::RIGIDBODY);
                    break;
                case TypeOfComponent::AISTATE:
                    selectedGO->AddComponent<AIStateMachineComponent>(TypeOfComponent::AISTATE);
                    break;
                case TypeOfComponent::HEALTH:
                    selectedGO->AddComponent<HealthComponent>(TypeOfComponent::HEALTH);
                    break;
                case TypeOfComponent::SPAWNER:
                    selectedGO->AddComponent<SpawnerComponent>(TypeOfComponent::SPAWNER);
                    break;
				case TypeOfComponent::BUTTON:
					selectedGO->AddComponent<ButtonComponent>(TypeOfComponent::BUTTON);
					break;
				case TypeOfComponent::UI:
					selectedGO->AddComponent<UIComponent>(TypeOfComponent::UI);
					break;
                case TypeOfComponent::CANVAS_UI:
                    selectedGO->AddComponent<CanvasComponent>(TypeOfComponent::CANVAS_UI);
                    break;
                case TypeOfComponent::TEXT_UI:
                    selectedGO->AddComponent<UITextComponent>(TypeOfComponent::TEXT_UI);
                    break;
                case TypeOfComponent::SPRITE_UI:
                    selectedGO->AddComponent<UISpriteComponent>(TypeOfComponent::SPRITE_UI);
                    break;
                case TypeOfComponent::PLAYER:
                    selectedGO->AddComponent<PlayerControllerComponent>(TypeOfComponent::PLAYER);
                    break;
                case TypeOfComponent::EXPLOSION:
                    selectedGO->AddComponent<ExplosionComponent>(TypeOfComponent::EXPLOSION);
                    break;
                case TypeOfComponent::PARTICLE:
                    selectedGO->AddComponent<ParticleSystem>(TypeOfComponent::PARTICLE);
                    break;
                case TypeOfComponent::PAUSEMENUBUTTON:
                    selectedGO->AddComponent<PauseMenuButton>(TypeOfComponent::PAUSEMENUBUTTON);
                    break;
                case TypeOfComponent::ANIMATOR:
                    selectedGO->AddComponent<AnimatorComponent>(TypeOfComponent::ANIMATOR);
                    break;
                case TypeOfComponent::SLIDER:
                    selectedGO->AddComponent<SliderComponent>(TypeOfComponent::SLIDER);
                    break;
                case TypeOfComponent::SPLITTING:
                    selectedGO->AddComponent<SplittingComponent>(TypeOfComponent::SPLITTING);
                    break;
                case TypeOfComponent::VIDEO:
                    selectedGO->AddComponent<VideoComponent>(TypeOfComponent::VIDEO);
                    break;
                case TypeOfComponent::VFX_FOLLOW:
                    selectedGO->AddComponent<VfxFollowComponent>(TypeOfComponent::VFX_FOLLOW);
                    break;
                default:
                    break;
                }
            }
        }
    }
#endif // _IMGUI

    /**
 * @brief Retrieves a list of scene files from a specified directory.
 *
 * This function scans a directory for files with the `.lua` extension and returns a list of
 * their names (without the `.lua` extension). It is useful for loading scene files in the game.
 *
 * @param directory The directory to search for `.lua` scene files.
 * @return A vector of strings containing the names of the scene files (without extensions).
 */
    std::vector<std::string> getSceneFiles(const std::string& directory) {
        std::vector<std::string> sceneFiles;
        for (const auto& entry : std::filesystem::directory_iterator(directory)) {
            if (entry.path().extension() == ".lua") {
                sceneFiles.push_back(entry.path().stem().string()); // Get file name without extension
            }
        }
        return sceneFiles;
    }


    /**
     * @brief Retrieves a list of asset files from a specified directory.
     *
     * This function scans a directory for any files (with any extension) and returns a list of
     * their full paths. It is useful for loading game assets like textures, sounds, and more.
     *
     * @param directory The directory to search for asset files.
     * @return A vector of strings containing the full paths of the asset files.
     */
    std::vector<std::string> getAssetFiles(const std::string& directory) {
        std::vector<std::string> assetFiles;
        for (const auto& entry : std::filesystem::directory_iterator(directory)) {
            if (entry.path().has_extension())
                assetFiles.push_back(entry.path().string()); // Get file name without extension

        }
        return assetFiles;
    }

    /**
     * @brief Extracts the file name without the extension from a file path.
     *
     * This function takes a file path as input, extracts the file name, and removes its extension.
     * It handles paths that use either forward or backward slashes (`/` or `\\`).
     *
     * @param filePath The full path of the file.
     * @return The file name without its extension.
     */
     std::string extractFileName(const std::string& filePath) {
        // Find the last occurrence of '/' or '\'
        size_t pos = filePath.find_last_of("/\\");
        std::string fileName = (pos != std::string::npos) ? filePath.substr(pos + 1) : filePath;

        // Find the last dot in the file name
        size_t dotPos = fileName.find_last_of('.');
        if (dotPos != std::string::npos) {
            fileName = fileName.substr(0, dotPos); // Remove the extension
        }

        return fileName;
    }

     /**
      * @brief Extracts the part of the string after the first underscore.
      *
      * This function takes an input string and returns the part of the string after the first
      * underscore character (`'_'`). It is useful for extracting specific data encoded in a
      * naming convention.
      *
      * @param input The input string, typically a file name.
      * @return A substring from the input string after the first underscore. Returns an empty string
      *         if no underscore is found.
      */
    std::string extractSoundName(const std::string& input) {
        size_t pos = input.find('_'); // Find the first underscore
        if (pos != std::string::npos && pos + 1 < input.size()) {
            return input.substr(pos + 1); // Extract substring after the first underscore
        }
        return ""; // Return an empty string if no valid part is found
    }


    /**
     * @brief Extracts the integer value after the last underscore in a file path.
     *
     * This function looks for the last underscore (`'_'`) in a file path and extracts the
     * integer value immediately following it. It is useful for extracting specific information
     * encoded in the file name, such as a type or category identifier.
     *
     * @param filePath The file path or string from which to extract the number.
     * @return The integer value extracted from the file path, or -1 if no valid number is found.
     */
    int extractSoundType(const std::string& filePath) {
        size_t pos = filePath.find_last_of('_'); // Find the last underscore
        if (pos != std::string::npos && pos + 1 < filePath.size()) {
            // Extract substring after the last underscore and convert to integer
            return std::stoi(filePath.substr(pos + 1));
        }
        return -1; // Return -1 if no valid number is found
    }



    /**
     * @brief Extracts the numeric prefix from a string before the first underscore.
     *
     * This function extracts and returns the numeric prefix of a string, assuming the
     * prefix is located before the first underscore (`'_'`). If no numeric prefix is found,
     * it throws an exception. This is useful for processing strings that follow a naming
     * convention where the prefix represents an ID or index.
     *
     * @param input The input string, typically a file name or identifier.
     * @return The integer value of the numeric prefix before the first underscore.
     * @throws std::invalid_argument If no valid numeric prefix is found before the underscore.
     */
    int extractIndex(const std::string& input) {
        size_t pos = input.find('_'); // Find the first underscore
        if (pos != std::string::npos) {
            std::string prefix = input.substr(0, pos);
            // Validate the prefix is numeric
            if (!prefix.empty() && std::all_of(prefix.begin(), prefix.end(), ::isdigit)) {
#ifdef _LOGGING
                int i = std::stoi(prefix);
				ImGuiConsole::Cout("Audio ID: %d", i);
#endif // _LOGGING
                return std::stoi(prefix); //Convert valid numeric prefix to int
            }
        }
        throw std::invalid_argument("No valid numeric prefix found before underscore!");
    }

#ifdef _IMGUI
    /**
     * @brief Updates texture assets based on changes from a new list of textures.
     *
     * This function handles both the addition and removal of texture assets, and then
     * updates the associated Lua file with the new texture data. It compares the current
     * holder of textures with the previous holder and removes any textures no longer present.
     *
     * @param holder A vector of strings representing the current list of texture file paths.
     * @param prevholder A vector of strings representing the previous list of texture file paths.
     * @param previousHolderSize The previous size of the holder vector. This parameter is not used.
     */
    void UpdateTextureAssetChanges(const std::vector<std::string>& holder,
        const std::vector<std::string>& prevholder,
        size_t previousHolderSize) {
        (void)previousHolderSize;
        // Handle removed textures
        if (prevholder.size() > holder.size()) {
            std::vector<std::string> difference;
            std::vector<std::string> sortedHolder = holder;
            std::vector<std::string> sortedPrevHolder = prevholder;

            std::sort(sortedHolder.begin(), sortedHolder.end());
            std::sort(sortedPrevHolder.begin(), sortedPrevHolder.end());

            std::set_difference(sortedPrevHolder.begin(), sortedPrevHolder.end(),
                sortedHolder.begin(), sortedHolder.end(),
                std::back_inserter(difference));

            for (const auto& item : difference) {
                AssetManager::GetInstance().RemoveTexture(Utilities::extractFileName(item));
            }
        }

        // Update Lua file with new assets
        LuaManager luaManager("Assets/Lua/textures.lua");
        luaManager.ClearLuaFile();

        for (size_t i = 0; i < holder.size(); ++i) {
            std::string tableName = "Asset_" + std::to_string(i);
            std::vector<std::string> keys;
            LuaManager::LuaValueContainer values;

            std::string filePath = holder[i];
            keys.push_back("SpritePathName");
            values.push_back(filePath);

            std::string fileName = Utilities::extractFileName(holder[i]);
            keys.push_back("SpriteFileName");
            values.push_back(fileName);

            luaManager.LuaWrite(tableName, values, keys, "Texture");
        }

    }

#pragma region MergeSortAlgorithm
    /**
     * @brief Merges two sorted subarrays of a vector based on extracted numerical indices.
     * @param vec The vector of strings to be merged.
     * @param left The starting index of the first subarray.
     * @param mid The middle index, marking the end of the first subarray.
     * @param right The ending index of the second subarray.
     */
    void MergeByIndex(std::vector<std::string>& vec, int left, int mid, int right)
    {
        int n1 = mid - left + 1;
        int n2 = right - mid;

        std::vector<std::string> leftVec(vec.begin() + left, vec.begin() + mid + 1);
        std::vector<std::string> rightVec(vec.begin() + mid + 1, vec.begin() + right + 1);

        int i = 0, j = 0;
        int k = left;

        while (i < n1 && j < n2)
        {
            int indexA = Utilities::extractIndex(Utilities::extractFileName(leftVec[i]));
            int indexB = Utilities::extractIndex(Utilities::extractFileName(rightVec[j]));

            if (indexA <= indexB) {
                vec[k] = std::move(leftVec[i]);
                ++i;
            }
            else {
                vec[k] = std::move(rightVec[j]);
                ++j;
            }
            ++k;
        }

        //Copy remaining elements of leftVec, if any
        while (i < n1) {
            vec[k] = std::move(leftVec[i]);
            ++i;
            ++k;
        }

        //Copy remaining elements of rightVec, if any
        while (j < n2) {
            vec[k] = std::move(rightVec[j]);
            ++j;
            ++k;
        }
    }
    /**
     * @brief Recursively sorts a vector of strings using merge sort, based on extracted numerical indices.
     * @param vec The vector of strings to be sorted.
     * @param left The starting index of the sorting range.
     * @param right The ending index of the sorting range.
     */
    void MergeSortByIndex(std::vector<std::string>& vec, int left, int right) {
        if (left < right) {
            int mid = left + (right - left) / 2;

            //Recursive merging
            MergeSortByIndex(vec, left, mid);
            MergeSortByIndex(vec, mid + 1, right);

            //Merge the sorted halves
            MergeByIndex(vec, left, mid, right);
        }
    }
    /**
     * @brief Sorts a vector of strings based on numerical indices extracted from file names.
     * @param vec The vector of strings to be sorted.
     */
    void SortByIndex(std::vector<std::string>& vec) {
        if (!vec.empty()) {
            MergeSortByIndex(vec, 0, (int)vec.size() - 1);
        }
    }
#pragma endregion

    ///**
    // * @brief Custom implementation of the bubble sort algorithm to sort a vector of strings
    // *        based on numeric indices extracted from the filenames.
    // *
    // * This function sorts the vector of strings in ascending order by extracting numeric indices
    // * from the filenames using the Utilities::extractIndex function.
    // *
    // * @param vec A reference to the vector of strings to be sorted.
    // */
    ////Custom bubble sort implementation to sort based on extractIndex
    //void BubbleSortByIndex(std::vector<std::string>& vec) {

    //    int n1 = mid - left 
    //    size_t n = vec.size();
    //    for (size_t i = 0; i < n - 1; ++i) {
    //        for (size_t j = 0; j < n - i - 1; ++j) {
    //            // Extract indices for comparison
    //            int indexA = Utilities::extractIndex(Utilities::extractFileName(vec[j]));
    //            int indexB = Utilities::extractIndex(Utilities::extractFileName(vec[j + 1]));

    //            if (indexA > indexB) {
    //                std::swap(vec[j], vec[j + 1]);
    //            }
    //        }
    //    }
    //}


    /**
     * @brief Updates sound assets based on changes from a new list of sounds.
     *
     * This function handles both the addition and removal of sound assets, and then
     * updates the associated Lua file with the new sound data. It compares the current
     * holder of sounds with the previous holder and removes any sounds no longer present.
     *
     * @param holder A vector of strings representing the current list of sound file paths.
     * @param prevholder A vector of strings representing the previous list of sound file paths.
     * @param previousHolderSize The previous size of the holder vector. This parameter is not used.
     */
    void UpdateSoundAssetChanges(const std::vector<std::string>& holder,
        const std::vector<std::string>& prevholder,
        size_t previousHolderSize) {
        (void)previousHolderSize;
        // Handle removed textures
        if (prevholder.size() > holder.size()) {
            std::vector<std::string> difference;
            std::vector<std::string> sortedHolder = holder;
            std::vector<std::string> sortedPrevHolder = prevholder;

            std::sort(sortedHolder.begin(), sortedHolder.end());
            std::sort(sortedPrevHolder.begin(), sortedPrevHolder.end());

            std::set_difference(sortedPrevHolder.begin(), sortedPrevHolder.end(),
                sortedHolder.begin(), sortedHolder.end(),
                std::back_inserter(difference));

            for (const auto& item : difference) {
                //ImGuiConsole::Cout("REMOVING: " << Utilities::extractIndex(Utilities::extractFileName(item)));
                AssetManager::GetInstance().RemoveSound(Utilities::extractIndex(Utilities::extractFileName(item)));
            }
        }

        // Sort the holder to ensure consistent order
        std::vector<std::string> sortedHolder = holder;
        SortByIndex(sortedHolder);

        // Update Lua file with new assets
        LuaManager luaManager("Assets/Lua/sounds.lua");
        luaManager.ClearLuaFile();

        for (size_t i = 0; i < holder.size(); ++i) {
            std::string tableName = "Asset_" + std::to_string(i);
            std::vector<std::string> keys;
            LuaManager::LuaValueContainer values;

            std::string filePath = sortedHolder[i];  // Use sortedHolder instead of holder
            keys.push_back("SoundPathName");
            values.push_back(filePath);

            std::string fileName = Utilities::extractSoundName(sortedHolder[i]);
            keys.push_back("SoundFileName");
            values.push_back(fileName);

            int audioType = Utilities::extractSoundType(fileName);
            keys.push_back("SoundType");
            values.push_back(audioType);

            keys.push_back("Priority");
            values.push_back(0);


            luaManager.LuaWrite(tableName, values, keys, "Sound");
        }

    }



    /**
     * @brief Synchronizes the texture assets with the Lua file, updating the assets
     *        based on the data stored in the Lua file.
     *
     * This function reads the texture properties from the Lua file and loads the corresponding
     * textures using the AssetManager.
     *
     * @param holder A vector of strings representing the current list of texture file paths.
     */
    void SyncTextureAssetsWithLua(const std::vector<std::string>& holder) {
        LuaManager luaManager("Assets/Lua/textures.lua");

        for (size_t i = 0; i < holder.size(); ++i) {
            std::string tableName = "Asset_" + std::to_string(i);

            try {
                // Read properties from Lua
                std::string pathName = luaManager.LuaRead<std::string>(tableName, { "Texture", "SpritePathName" });
                std::string fileName = luaManager.LuaRead<std::string>(tableName, { "Texture", "SpriteFileName" });
                float animationFrame = 1.0f;
                float frameX = 1.0f;
                float frameY = 1.0f;

                AssetManager::GetInstance().LoadTexture(pathName, fileName, frameX, frameY, animationFrame);
            }
            catch (const std::exception& e) {
				ImGuiConsole::Cout("Error reading table %s: %s\n", tableName.c_str(), e.what());
            }
        }
    }



    /**
     * @brief Synchronizes the sound assets with the Lua file, updating the assets
     *        based on the data stored in the Lua file.
     *
     * This function reads the sound properties from the Lua file and loads the corresponding
     * sound assets using the AssetManager.
     *
     * @param holder A vector of strings representing the current list of sound file paths.
     */
    void SyncSoundAssetsWithLua(const std::vector<std::string>& holder) {
        LuaManager luaManager("Assets/Lua/sounds.lua");

        for (size_t i = 0; i < holder.size(); ++i) {
            std::string tableName = "Asset_" + std::to_string(i);

            try {
                // Read properties from Lua
                std::string pathName = luaManager.LuaRead<std::string>(tableName, { "Sound", "SoundPathName" });
                std::string fileName = luaManager.LuaRead<std::string>(tableName, { "Sound", "SoundFileName" });
                int audioType = luaManager.LuaRead<int>(tableName, { "Sound", "SoundType" });
                int priority = luaManager.LuaRead<int>(tableName, { "Sound", "Priority" });

                AssetManager::GetInstance().LoadAudios(pathName, fileName, audioType, priority);
            }
            catch (const std::exception& e) {
				ImGuiConsole::Cout("Error reading table %s: %s\n", tableName.c_str(), e.what());
            }
        }
    }
#endif // _IMGUI
    /**
 * @brief Loads texture assets from a Lua table file.
 *
 * This function reads texture asset data from the specified Lua file. It iterates through the Lua tables
 * representing texture assets, reads the relevant properties (path name, file name, animation frame,
 * frame dimensions), and loads the textures using the AssetManager.
 *
 * @param luaFilePath The path to the Lua file containing texture asset information.
 */
	// RIDHWAM: Function to load texture assets from Lua table file
    void LoadTextureAssetsWithLua(const std::string& luaFilePath) {
        LuaManager luaManager(luaFilePath);

        int numOfTextures = luaManager.countTables();
        for (int i = 0; i < numOfTextures; ++i) {
            std::string tableName = "Asset_" + std::to_string(i);

            try {
                // Read properties from Lua
                std::string pathName = luaManager.LuaRead<std::string>(tableName, { "Texture", "SpritePathName" });
                std::string fileName = luaManager.LuaRead<std::string>(tableName, { "Texture", "SpriteFileName" });
                float animationFrame = 1.0f;
                float frameX = 1.0f;
                float frameY = 1.0f;

                AssetManager::GetInstance().LoadTexture(pathName, fileName, frameX, frameY, animationFrame);
            }
            catch (const std::exception& e) {
				ImGuiConsole::Cout("Error reading table %s: %s\n", tableName.c_str(), e.what());
            }
        }
    }

    /**
 * @brief Loads sound assets from a Lua table file.
 *
 * This function reads sound asset data from the specified Lua file. It iterates through the Lua tables
 * representing sound assets, reads the relevant properties (path name, file name, audio type, priority),
 * and loads the sound assets using the AssetManager.
 *
 * @param luaFilePath The path to the Lua file containing sound asset information.
 */
    // RIDHWAM: Function to load sound assets from Lua table file
    void LoadSoundAssetsWithLua(const std::string& luaFilePath) {
        LuaManager luaManager(luaFilePath);

        int numOfSounds = luaManager.countTables();
        for (int i = 0; i < numOfSounds; ++i) {
            std::string tableName = "Asset_" + std::to_string(i);

            try {
                // Read properties from Lua
                std::string pathName = luaManager.LuaRead<std::string>(tableName, { "Sound", "SoundPathName" });
                std::string fileName = luaManager.LuaRead<std::string>(tableName, { "Sound", "SoundFileName" });
                int audioType = luaManager.LuaRead<int>(tableName, { "Sound", "SoundType" });
                int priority = luaManager.LuaRead<int>(tableName, { "Sound", "Priority" });

                AssetManager::GetInstance().LoadAudios(pathName, fileName, audioType, priority);
            }
            catch (const std::exception& e) {
				ImGuiConsole::Cout("Error reading table %s: %s\n", tableName.c_str(), e.what());
            }
        }
    }

}

#ifdef _IMGUI
namespace EngineImGuiWindows {

    void ShowTagManagerWindow() {
        TagManager& tagManager = TagManager::GetInstance();

        ImGui::Begin("Tag Manager");

        ImGui::Text("Available Tags:");
        ImGui::Separator();

        std::vector<std::string> allTags(tagManager.GetAllTags().begin(), tagManager.GetAllTags().end());

        for (const auto& tag : allTags) {
            ImGui::BulletText("%s", tag.c_str());
        }

        ImGui::Separator();

        static char newTagBuffer[64] = "";
        ImGui::InputText("New Tag", newTagBuffer, sizeof(newTagBuffer));
        if (ImGui::Button("Add Tag")) {
            std::string newTag(newTagBuffer);
            if (!newTag.empty() && tagManager.GetAllTags().find(newTag) == tagManager.GetAllTags().end()) {
                tagManager.AddTag(newTag);
                memset(newTagBuffer, 0, sizeof(newTagBuffer)); 
            }
            else {
                ImGui::TextColored(ImVec4(1, 0, 0, 1), "Tag already exists or invalid!");
            }
        }

        static int selectedTagIndex = -1;
        if (ImGui::BeginCombo("Remove Tag", selectedTagIndex == -1 ? "Select Tag" : allTags[selectedTagIndex].c_str())) {
            for (size_t i = 0; i < allTags.size(); i++) {
                bool isSelected = (selectedTagIndex == static_cast<int>(i));
                if (ImGui::Selectable(allTags[i].c_str(), isSelected)) {
                    selectedTagIndex = static_cast<int>(i);
                }
                if (isSelected) {
                    ImGui::SetItemDefaultFocus();
                }
            }
            ImGui::EndCombo();
        }

        if (selectedTagIndex != -1) {
            if (ImGui::Button("Remove Selected Tag")) {
                tagManager.RemoveTag(allTags[selectedTagIndex]);
                selectedTagIndex = -1;  
            }
        }

        ImGui::End();
    }

    // Showing all the layer available to assign the layer to the gameobject
    void ShowLayerManagerWindow() {
        LayerManager& layerManager = LayerManager::GetInstance();

        ImGui::Begin("Layer Manager");

        ImGui::Text("Available Layers:");
        ImGui::Separator();

        //Fetch all layers
        std::vector<std::string> allLayers = layerManager.GetAllLayers();
        for (const auto& layer : allLayers) {
            bool isActive = layerManager.IsLayerActive(layer);

            // Checkbox to enable/disable layers
            if (ImGui::Checkbox(layer.c_str(), &isActive)) {
                layerManager.SetLayerActive(layer, isActive);
            }
        }

        ImGui::Separator();

        //Add new layer
        static char newLayerBuffer[64] = "";
        ImGui::InputText("New Layer", newLayerBuffer, sizeof(newLayerBuffer));
        if (ImGui::Button("Add Layer")) {
            std::string newLayer(newLayerBuffer);
            if (!newLayer.empty() && !layerManager.IsLayerValid(newLayer)) {
                layerManager.AddLayer(newLayer);
                memset(newLayerBuffer, 0, sizeof(newLayerBuffer));  // Clear buffer
            }
            else {
                ImGui::TextColored(ImVec4(1, 0, 0, 1), "Layer already exists or invalid!");
            }
        }

        //Remove selected layer
        static int selectedLayerIndex = -1;
        if (ImGui::BeginCombo("Remove Layer", selectedLayerIndex == -1 ? "Select Layer" : allLayers[selectedLayerIndex].c_str())) {
            for (int i = 0; i < allLayers.size(); i++) {
                bool isSelected = (selectedLayerIndex == i);
                if (ImGui::Selectable(allLayers[i].c_str(), isSelected)) {
                    selectedLayerIndex = i;
                }
                if (isSelected) {
                    ImGui::SetItemDefaultFocus();
                }
            }
            ImGui::EndCombo();
        }

        if (selectedLayerIndex != -1 && allLayers[selectedLayerIndex] != "Default") {
            if (ImGui::Button("Remove Selected Layer")) {
                layerManager.RemoveLayer(allLayers[selectedLayerIndex]);
                selectedLayerIndex = -1;  // Reset selection after removal
            }
        }
        else if (selectedLayerIndex != -1) {
            ImGui::TextColored(ImVec4(1, 0, 0, 1), "Cannot remove Default layer!");
        }

        ImGui::End();
    }

    //Where we set up what components we can show and allow the users to edit
    void SelectedGOComponentWindow(GameObject* selectedGO) {

        GameObjectFactory& factory = GameObjectFactory::GetInstance();
        ImGui::Begin("GameObject Details");
        if (selectedGO == nullptr || selectedGO->GetName() == "")
        {
            ImGui::Text("Nothing has been selected.");
            ImGui::End();
            return;
        }
        //Display the current name
        ImGui::Text("Selected GameObject:");

        //Create a buffer to hold the name
        static char nameBuffer[128]; // Adjust size as needed
        std::strncpy(nameBuffer, selectedGO->GetName().c_str(), sizeof(nameBuffer));

        //Provide a text input field to modify the name
        if (ImGui::InputText("##GameObjectName", nameBuffer, sizeof(nameBuffer))) {
            selectedGO->SetName(std::string(nameBuffer));
        }

        TagManager& tagManager = TagManager::GetInstance();
        LayerManager& layerManager = LayerManager::GetInstance();

        //To implement the tag drop down here

#pragma region Tag Information
        const std::string& currentTag = selectedGO->GetTag();
        std::string selectedTag = currentTag.empty() ? "Untagged" : currentTag;
        ImGui::Text("%s Current Tag: %s", nameBuffer, selectedTag.c_str());
        ImGui::Text("Tag:");

        // Dropdown for tags
        if (ImGui::BeginCombo("##TagDropdown", selectedTag.c_str())) {
            for (const auto& tag : tagManager.GetAllTags()) {
                bool isSelected = (selectedTag == tag);
                if (ImGui::Selectable(tag.c_str(), isSelected)) {
                    selectedTag = tag;
                    selectedGO->SetTag(selectedTag);
                }
                if (isSelected) {
                    ImGui::SetItemDefaultFocus();
                }
            }
            ImGui::EndCombo();
        }
#pragma endregion

#pragma region LayerInformation

        // Layer selection
        ImGui::Separator();
        ImGui::Text("%s Current Layer: %s", nameBuffer, selectedGO->GetLayer().c_str());
        ImGui::Text("Layer:");

        std::string selectedLayer = selectedGO->GetLayer().empty() ? "Default" : selectedGO->GetLayer();
        if (ImGui::BeginCombo("##LayerDropdown", selectedLayer.c_str())) {
            for (const auto& layer : layerManager.GetAllLayers()) {
                bool isSelected = (selectedLayer == layer);
                if (ImGui::Selectable(layer.c_str(), isSelected)) {
                    selectedLayer = layer;
                    selectedGO->SetLayer(selectedLayer);
                }
                if (isSelected) {
                    ImGui::SetItemDefaultFocus();
                }
            }
            ImGui::EndCombo();
        }
        ImGui::Separator();
#pragma endregion

        static int selectedComponentIndex = 0;  // Index for the dropdown
  
        Utilities::GUIComponentAdder(selectedGO);

        // Retrieve and display the TransformComponent
        TransformComponent* transform = selectedGO->GetComponent<TransformComponent>(TypeOfComponent::TRANSFORM);
        if (transform) {
            if (ImGui::TreeNode("Transform Component"))
            {
                Vector2 pos = transform->GetLocalPosition();  // Get the current position
                if (ImGui::DragFloat("Position X", &pos.x, 1.0f)) {
                    transform->SetLocalPosition(pos);  // Set the updated position
                }
                if (ImGui::DragFloat("Position Y", &pos.y, 1.0f)) {
                    transform->SetLocalPosition(pos);  // Set the updated position
                }

                Vector2 scale = transform->GetLocalScale();  // Get the current position
                if (ImGui::DragFloat("Scale X", &scale.x, 0.1f)) {
                    transform->SetLocalScale(scale);  // Set the updated position
                }
                if (ImGui::DragFloat("Scale Y", &scale.y, 0.1f)) {
                    transform->SetLocalScale(scale);  // Set the updated position
                }

                float rotation = transform->GetLocalRotation();  // Get the current position
                if (ImGui::DragFloat("Rotation", &rotation, 1.0f)) {
                    transform->SetLocalRotation(rotation);  // Set the updated position
                }
                ImGui::TreePop();
            }
        }

        SpriteComponent* sprite = selectedGO->GetComponent<SpriteComponent>(TypeOfComponent::SPRITE);
        if (sprite) {
            if (ImGui::TreeNode("Sprite Component"))
            {
                // Display current texture preview
                auto currentSprite = sprite->GetCurrentSprite();
                if (currentSprite) {
                    unsigned int textureId = currentSprite->GetSpriteTexture()->GetTextureID();
                    ImGui::Text("Current Texture:");
                    ImGui::Image((void*)(intptr_t)textureId, ImVec2(64, 64), ImVec2(0, 1), ImVec2(1, 0));

                    // Create a drop target for textures
                    if (ImGui::BeginDragDropTarget()) {
                        if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("CONTENT_BROWSER_ITEM")) {
                            const FilePayload* filePayload = static_cast<const FilePayload*>(payload->Data);
                            if (filePayload->isTexture) {
                                std::string textureName(filePayload->textureName);

                                // Check if this is an animation file
                                std::string animPrefix = "Animation-";
                                if (textureName.substr(0, animPrefix.length()) == animPrefix) {
                                    // Extract the animation name (e.g., "Ame" from "Animation-Ame")
                                    std::string animName = textureName.substr(animPrefix.length());
                                    std::string animationKey = "Animation_" + animName;

                                    // Get the pre-loaded animation
                                    auto newSprite = AssetManager::GetInstance().GetSprite(animationKey);
                                    if (newSprite) {
                                        sprite->ChangeSprite(std::make_unique<SpriteAnimation>(newSprite));
                                        ImGuiConsole::Cout("Loaded animation: %s", animationKey.c_str());
                                    }
                                    else {
                                        ImGuiConsole::Cout("Failed to find pre-loaded animation: %s", animationKey.c_str());
                                    }
                                }
                                else {
                                    // Handle as regular texture
                                    auto newSprite = AssetManager::GetInstance().GetSprite(textureName);
                                    if (newSprite) {
                                        sprite->ChangeSprite(std::make_unique<SpriteAnimation>(newSprite));
                                        ImGuiConsole::Cout("Loaded texture: %s", textureName.c_str());
                                    }
                                }
                            }
                            else {
                                ContentBrowserPanel::GetInstance().ShowNotification("Invalid file format. Please drag and drop a valid texture file.");
                            }
                        }
                        ImGui::EndDragDropTarget();
                    }

                    // Animation controls for sprite sheets

                    ImGui::Separator();
                    ImGui::Text("Animation Settings");
                    currentSprite = sprite->GetCurrentSprite();
                    //auto currentSprite = sprite->GetCurrentSprite();
                    int frameX = (int)currentSprite->GetSpriteTexture()->GetNxFrames();
                    int frameY = (int)currentSprite->GetSpriteTexture()->GetNyFrames();
                    int totalFrame = (int)currentSprite->GetSpriteTexture()->GetTotalFrames();
                    double framesPS = currentSprite->GetSpriteTexture()->GetFramePs();
                    if (ImGui::DragInt("Number Of Frame X", &frameX, 1.0f))
                        currentSprite->GetSpriteTexture()->SetNxFrames(frameX);
                    if (ImGui::DragInt("Number Of Frame Y", &frameY, 1.0f))
                        currentSprite->GetSpriteTexture()->SetNyFrames(frameY);
                    if (ImGui::InputDouble("Frames per Second", &framesPS, 1.0f))
                        currentSprite->GetSpriteTexture()->SetFramePs(framesPS);
                    if (ImGui::DragInt("Total Frame", &totalFrame, 1.0f))
                        currentSprite->GetSpriteTexture()->SetTotalFrames(totalFrame);

                    // Reset animation button
                    if (ImGui::Button("Reset Animation")) {
                        currentSprite->ResetSpriteAnimation();
                    }
                }
                else {
                    ImGui::Text("No texture assigned");
                    // Drop target for empty sprite component
                    if (ImGui::BeginDragDropTarget()) {
                        if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("CONTENT_BROWSER_ITEM")) {
                            const FilePayload* filePayload = static_cast<const FilePayload*>(payload->Data);
                            if (filePayload->isTexture) {
                                std::string textureName(filePayload->textureName);

                                // Same animation check logic as above
                                std::string animPrefix = "Animation-";
                                if (textureName.substr(0, animPrefix.length()) == animPrefix) {
                                    std::string animName = textureName.substr(animPrefix.length());
                                    std::string animationKey = "Animation_" + animName;

                                    auto newSprite = AssetManager::GetInstance().GetSprite(animationKey);
                                    if (newSprite) {
                                        sprite->ChangeSprite(std::make_unique<SpriteAnimation>(newSprite));
                                    }
                                }
                                else {
                                    auto newSprite = AssetManager::GetInstance().GetSprite(textureName);
                                    if (newSprite) {
                                        sprite->ChangeSprite(std::make_unique<SpriteAnimation>(newSprite));
                                    }
                                }
                            }
                        }
                        ImGui::EndDragDropTarget();
                    }
                }


                // Editable layer value using a getter and setter
                int layer = sprite->GetLayer();  // Get the current layer value
                if (ImGui::DragInt("Layer", &layer, 1.0f)) {
                    sprite->SetLayer(layer);  // Update the layer if the value changes
                }


                // Editable flip options using getters and setters
                bool flipX = sprite->GetFlipX();
                if (ImGui::Checkbox("Flip X", &flipX)) {
                    sprite->SetFlipX(flipX);  // Update flipX if the value changes
                }

                bool flipY = sprite->GetFlipY();
                if (ImGui::Checkbox("Flip Y", &flipY)) {
                    sprite->SetFlipY(flipY);  // Update flipY if the value changes
                }

                if (ImGui::Button("Remove Sprite Component"))
                {
                    selectedGO->RemoveComponent(TypeOfComponent::SPRITE);
                }

                ImGui::TreePop();
            }
        }


        RectColliderComponent* collider = selectedGO->GetComponent<RectColliderComponent>(TypeOfComponent::RECTCOLLIDER);
        if (collider) {
            if (ImGui::TreeNode("RectCollider Component"))
            {
                std::vector<std::pair<Vector2, Vector2>>& colliders = collider->GetColliderData();
                int colliderCount = static_cast<int>(colliders.size());

                for (int i = 0; i < colliderCount; ++i) {
                    ImGui::Text("Collider %d:", i);

                    //Accessing value of i-collider
                    Vector2& size = colliders[i].first;
                    Vector2& center = colliders[i].second;

                    if (ImGui::DragFloat(("Size X##" + std::to_string(i)).c_str(), &size.x, 1.0f)) {
                        collider->UpdateColliderSize(i, size);
                    }
                    if (ImGui::DragFloat(("Size Y##" + std::to_string(i)).c_str(), &size.y, 1.0f)) {
                        collider->UpdateColliderSize(i, size);
                    }
                    if (ImGui::DragFloat(("Center X##" + std::to_string(i)).c_str(), &center.x, 1.0f)) {
                        collider->UpdateColliderCenter(i, center);
                    }
                    if (ImGui::DragFloat(("Center Y##" + std::to_string(i)).c_str(), &center.y, 1.0f)) {
                        collider->UpdateColliderCenter(i, center);
                    }

                    AABB aabb = collider->GetAABB(i);
                    ImGui::Text("AABB Min: (%.1f, %.1f)", aabb.min.x, aabb.min.y);
                    ImGui::Text("AABB Max: (%.1f, %.1f)", aabb.max.x, aabb.max.y);
                }
                ImGui::Checkbox("Is Trigger: ", &collider->isTrigger);


                if (ImGui::Button("Remove RectCollider Component"))
                {
                    selectedGO->RemoveComponent(TypeOfComponent::RECTCOLLIDER);
                }
                ImGui::TreePop();
            }
        }

        // In the SelectedGOComponentWindow function where AudioComponent is handled
        AudioComponent* audioComponent = selectedGO->GetComponent<AudioComponent>(TypeOfComponent::AUDIO);
        if (audioComponent) {
            if (ImGui::TreeNode("Audio Component"))
            {
                const std::vector<int>& audioClips = audioComponent->GetAudioClips();

                // Display existing audio clips
                for (int i = 0; i < audioClips.size(); ++i) {
                    int audioID = audioClips[i];
                    const std::string& audioName = AssetManager::GetInstance().GetAudioNameFromID(audioID);

                    ImGui::Text("Audio Clip: %s", audioName.c_str());

                    if (ImGui::Button(("Play " + audioName).c_str())) {
                        audioComponent->PlayAudio(audioID);
                    }

                    ImGui::SameLine();

                    if (ImGui::Button(("Stop " + audioName).c_str())) {
                        audioComponent->StopAudio();
                    }

                    // Add remove button for each audio clip
                    ImGui::SameLine();
                    if (ImGui::Button(("Remove##" + std::to_string(i)).c_str())) {
                        audioComponent->RemoveAudioClip(audioID);
                    }
                }
                
                // Drop target for audio files
                ImGui::Separator();
                ImGui::Text("Drag and drop audio files here:");
                if (ImGui::BeginDragDropTarget())
                {
                    if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("CONTENT_BROWSER_ITEM"))
                    {
                        const FilePayload* filePayload = static_cast<const FilePayload*>(payload->Data);
                        if (filePayload->isSound)
                        {
                            // Add the audio clip to the component
                            audioComponent->AddAudioClip(filePayload->soundID);
							ImGuiConsole::Cout("Added audio clip: %s (ID: %d)", filePayload->soundName, filePayload->soundID);
                        }
                        else {
                            ContentBrowserPanel::GetInstance().ShowNotification("Invalid file format. Please drag and drop a valid audio file.");
                        }
                    }
                    ImGui::EndDragDropTarget();
                }

                if (ImGui::Button("Remove Audio Component"))
                {
                    selectedGO->RemoveComponent(TypeOfComponent::AUDIO);
                }

                ImGui::TreePop();
            }
        }

        TextComponent* textComponent = selectedGO->GetComponent<TextComponent>(TypeOfComponent::TEXT);
        if (textComponent)
        {
            if (ImGui::TreeNode("Text Component"))
            {
                // To make it editable
                static char textBuffer[256];
                strncpy_s(textBuffer, sizeof(textBuffer), textComponent->GetText().c_str(), sizeof(textBuffer) - 1);
                textBuffer[sizeof(textBuffer) - 1] = '\0'; //Null terminating

                if (ImGui::InputText("Text Content", textBuffer, sizeof(textBuffer))) {
                    textComponent->SetText(std::string(textBuffer));
                }

                Vector2 pos = textComponent->GetPosition();  // Get the current position
                if (ImGui::DragFloat("Position X##", &pos.x, 1.0f)) {
                    textComponent->SetPosition(pos);  // Set the updated position
                }
                if (ImGui::DragFloat("Position Y##", &pos.y, 1.0f)) {
                    textComponent->SetPosition(pos);  // Set the updated position
                }

                // Editable font size
                float fontSize = textComponent->GetFontSize();
                if (ImGui::DragFloat("Size: ", &fontSize, 0.1f)) {
                    textComponent->SetFont(textComponent->GetFontType(), fontSize);  // Set the updated font size
                }
                if (ImGui::Button("Remove Text Component"))
                {
                    selectedGO->RemoveComponent(TypeOfComponent::TEXT);
                }
                ImGui::TreePop();
            }
        }
        RigidBodyComponent* rbComponent = selectedGO->GetComponent<RigidBodyComponent>(TypeOfComponent::RIGIDBODY);
        if (rbComponent)
        {
            if (ImGui::TreeNodeEx("Rigidbody Component"))
            {
                Vector2 velocity = rbComponent->GetVelocity();  // Get the current position
                if (ImGui::DragFloat("Vel X", &velocity.x, 1.0f)) {
                    rbComponent->SetVelocity(velocity);
                }
                if (ImGui::DragFloat("Vel Y", &velocity.y, 1.0f)) {
                    rbComponent->SetVelocity(velocity);
                }

                float mass = rbComponent->GetMass();  // Get the current position
                if (ImGui::DragFloat("Mass", &mass, 1.0f)) {
                    rbComponent->SetMass(mass);
                }

                float drag = rbComponent->GetDrag();
                
                if (ImGui::DragFloat("Drag", &drag, 1.0f)) {
                    rbComponent->SetDrag(drag);
                }

                ImGui::Text("Acceleration X: %.1f", rbComponent->GetAcceleration().x);
                ImGui::Text("Acceleration Y: %.1f", rbComponent->GetAcceleration().y);

                if (ImGui::Button("Remove RigidBody Component"))
                {
                    selectedGO->RemoveComponent(TypeOfComponent::RIGIDBODY);
                }

                ImGui::TreePop();
            }
        }

        AIStateMachineComponent* AIComponent = selectedGO->GetComponent<AIStateMachineComponent>(TypeOfComponent::AISTATE);
        if (AIComponent) {
            if (ImGui::TreeNodeEx("AI State Component", ImGuiTreeNodeFlags_DefaultOpen)) {
                // Fetch state and state name once
                const std::string& currentStateName = AIComponent->GetCurrentStateName();
                ImGui::Text("Current State: %s", currentStateName.c_str());

                // Combo box to allow selection of states
                std::vector<std::string> states = AIComponent->GetAllStates();
                if (!states.empty()) {
                    static int selectedState = (int)(std::find(states.begin(), states.end(), currentStateName) - states.begin()); // Initialize with current state

                    std::vector<const char*> stateNames;
                    for (const auto& state : states) {
                        stateNames.push_back(state.c_str());
                    }

                    // State selection combo
                    if (ImGui::Combo("Change State", &selectedState, stateNames.data(), static_cast<int>(stateNames.size()))) {
                        AIComponent->SetState(states[selectedState]); // Set the new state using string
                    }

                    // AI Move Speed
                    float movespeed = AIComponent->GetMoveSpeed();
                    if (ImGui::DragFloat("AI Move Speed", &movespeed, 1.0f, 0.0f, FLT_MAX)) {
                        AIComponent->SetMoveSpeed(movespeed);
                    }
                }

                // Target selection only if in "CHASE" state
                if (currentStateName == "CHASE") {
                    ImGui::Separator();
                    ImGui::Text("Change Chase Target:");

                    GameObject* currentTarget = AIComponent->GetChaseTarget();
                    if (currentTarget) {
                        ImGui::Text("Currently Chasing: %s", currentTarget->GetName().c_str());
                    }
                    else {
                        ImGui::Text("Currently Chasing: None");
                    }

                    static std::vector<GameObject*> availableTargets;
                    static std::vector<const char*> targetNames;

                    // Cache available targets and their names only once
                    if (availableTargets.empty()) {
                        const auto& allGameObjects = GameObjectFactory::GetInstance().GetAllGameObjects();
                        for (const auto& entry : allGameObjects) {
                            GameObject* gameObject = entry.second;
                            if (gameObject) {
                                targetNames.push_back(gameObject->GetName().c_str());
                                availableTargets.push_back(gameObject);
                            }
                        }
                    }

                    // Reset the selected target index when state changes
                    static int selectedTargetIndex = -1;
                    if (currentStateName == "CHASE") {
                        if (AIComponent) {
                            currentTarget = AIComponent->GetChaseTarget();
                            if (currentTarget) {
                                selectedTargetIndex = static_cast<int>(std::find(availableTargets.begin(), availableTargets.end(), currentTarget) - availableTargets.begin());
                            }
                        }
                    }

                    // Make sure index is valid
                    if (selectedTargetIndex < 0 || selectedTargetIndex >= availableTargets.size()) {
                        selectedTargetIndex = -1;
                    }

                    // Combo box to select a target
                    if (ImGui::Combo("Select Target", &selectedTargetIndex, targetNames.data(), static_cast<int>(targetNames.size()))) {
                        if (selectedTargetIndex >= 0 && selectedTargetIndex < availableTargets.size()) {
                            GameObject* selectedTarget = availableTargets[selectedTargetIndex];
                            AIComponent->SetChaseTarget(selectedTarget); // Set the new chase target
                        }
                    }
                }

                ImGui::Checkbox("Projectile: ", &AIComponent->isProjectile);

                if (ImGui::Button("Remove Ai State Machine Component"))
                {
                    selectedGO->RemoveComponent(TypeOfComponent::AISTATE);
                }

                ImGui::TreePop();
            }
        }

        PlayerControllerComponent* playerComponent = selectedGO->GetComponent<PlayerControllerComponent>(TypeOfComponent::PLAYER);
        if (playerComponent) {
            if (ImGui::TreeNode("Player Controller Component")) {
                float moveSpd = playerComponent->GetMoveSpd();
                if (ImGui::DragFloat("Move Speed", &moveSpd, 1.0f, 0.0f, FLT_MAX)) {
                    playerComponent->SetMoveSpd(moveSpd);
                }

                float handOffset = playerComponent->GetHandOffset();
                if (ImGui::DragFloat("Hand Offset: ", &handOffset, 0.1f)) {
                    playerComponent->SetHandOffset(handOffset);
                }

                GameObject* heldObject = playerComponent->GetHeldOBJ();
                if (heldObject) {
                    std::string heldObjectName = heldObject->GetName(); // Assuming GameObject has GetName()
                    ImGui::Text("Held Object: %s", heldObjectName.c_str());
                }
                else {
                    ImGui::Text("Held Object: None");
                }

                GameObject* dragObject = playerComponent->GetDraggingOBJ();
                if (dragObject) {
                    std::string dragOBJName = dragObject->GetName(); // Assuming GameObject has GetName()
                    ImGui::Text("Dragging Object: %s", dragOBJName.c_str());
                }
                else {
                    ImGui::Text("Dragging Object: None");
                }

                GameObject* collidedObj = playerComponent->GetCurrentlyColliding();
                if (collidedObj) {
                    std::string collidedObjName = collidedObj->GetName(); // Assuming GameObject has GetName()
                    ImGui::Text("Collided Object: %s", collidedObjName.c_str());
                }
                else {
                    ImGui::Text("Collided Object: None");
                }

                if (ImGui::Button("Remove Player Controller Component"))
                {
                    selectedGO->RemoveComponent(TypeOfComponent::PLAYER);
                }

                ImGui::TreePop();
            }
        }

        ButtonComponent* buttonComponent = selectedGO->GetComponent<ButtonComponent>(TypeOfComponent::BUTTON);
        if (buttonComponent) {
            if (ImGui::TreeNode("Button Component")) {
                // Dropdown to select the function
                const char* functionNames[] = { "Load Next Scene", "Exit Game", "Placeholder", "Restart Scene", "Resume", "Achievement", "AchievementBack", "Options", "Slider", "HowToPlay","Sound_POPUP", "CHANGEIMAGEFORWARD", "CHANGEIMAGEBACKWARD"};
                int functionIndex = static_cast<int>(buttonComponent->m_functionType);
                if (ImGui::Combo("Function", &functionIndex, functionNames, IM_ARRAYSIZE(functionNames))) {
                    buttonComponent->m_functionType = static_cast<ButtonFunctionType>(functionIndex);
                }

                // Only show the "Next Scene" dropdown if "Load Next Scene" is selected
                if (buttonComponent->m_functionType == ButtonFunctionType::LOAD_NEXT_SCENE) {
                    // Dropdown to select the next scene file
                    static std::string selectedScene = buttonComponent->pathNextScene;
                    static int currentSceneIndex = 0;

                    // Reload scene files every frame to reflect real-time changes
                    std::vector<std::string> sceneFiles = Utilities::getSceneFiles("Assets/Lua/Scenes");

                    // Check if current scene selection is still valid; if not, reset to the first scene
                    bool sceneFound = false;
                    for (int i = 0; i < sceneFiles.size(); ++i) {
                        if (sceneFiles[i] == selectedScene) {
                            currentSceneIndex = i;
                            sceneFound = true;
                            break;
                        }
                    }
                    if (!sceneFound) {
                        // Reset to default if selected scene is missing
                        currentSceneIndex = 0;
                        selectedScene = sceneFiles.empty() ? "" : sceneFiles[0];
                    }

                    // Dropdown to select scene file
                    if (ImGui::BeginCombo("Next Scene", selectedScene.c_str())) {
                        for (int i = 0; i < sceneFiles.size(); ++i) {
                            bool isSelected = (currentSceneIndex == i);
                            if (ImGui::Selectable(sceneFiles[i].c_str(), isSelected)) {
                                currentSceneIndex = i;
                                selectedScene = sceneFiles[i];
                                buttonComponent->pathNextScene = "Assets/Lua/Scenes/" + sceneFiles[i] + ".lua";
                            }
                            if (isSelected) {
                                ImGui::SetItemDefaultFocus();
                            }
                        }
                        ImGui::EndCombo();
                    }
                }

                if (ImGui::Button("Remove Button Component"))
                {
                    selectedGO->RemoveComponent(TypeOfComponent::BUTTON);
                }

                ImGui::TreePop();
            }
        }

        UIComponent* uiComponent = selectedGO->GetComponent<UIComponent>(TypeOfComponent::UI);
        if (uiComponent) {
            if (ImGui::TreeNode("UI Component")) {
                // Dropdown to select the UIComponentType
                const char* uiComponentNames[] = { "Health", "Bar", "Status", "Timer", "FPS", "Popup", "PopUpLeftRight"};
                int uiComponentIndex = static_cast<int>(uiComponent->type);

                if (ImGui::Combo("UI Component Type", &uiComponentIndex, uiComponentNames, IM_ARRAYSIZE(uiComponentNames))) {
                    uiComponent->type = static_cast<UIComponentType>(uiComponentIndex);
                }

                if (ImGui::Button("Remove UI Component"))
                {
                    selectedGO->RemoveComponent(TypeOfComponent::UI);
                }

                ImGui::TreePop();
            }
        }

        HealthComponent* healthComponent = selectedGO->GetComponent<HealthComponent>(TypeOfComponent::HEALTH);
        if (healthComponent) {
            if (ImGui::TreeNode("Health Component"))
            {
                ImGui::Separator();
                // Display current health
                int currentHealth = healthComponent->GetHealth();
                ImGui::Text("Current Health: %d", currentHealth);
                int maxHealth = healthComponent->GetMaxHealth();
                // Display health bar
                float healthRatio = static_cast<float>(currentHealth) / maxHealth;
                ImGui::ProgressBar(healthRatio, ImVec2(0.0f, 0.0f), "Health");

                // Button to decrease health by 10
                if (ImGui::Button("Take Damage (-10)")) {
                    healthComponent->TakeDamage(10);  // Reduce health by 10
                }

                if (ImGui::Button("Remove Health Component"))
                {
                    selectedGO->RemoveComponent(TypeOfComponent::HEALTH);
                }

                ImGui::TreePop();
            }
        }

        SpawnerComponent* spawnerComponent = selectedGO->GetComponent<SpawnerComponent>(TypeOfComponent::SPAWNER);
        if (spawnerComponent) {
            if (ImGui::TreeNode("Spawner Component")) {
                // Display and edit spawn intervals
                ImGui::Separator();
                float minInterval = spawnerComponent->GetMinInterval();
                float maxInterval = spawnerComponent->GetMaxInterval();

                if (ImGui::DragFloat("Min Spawn Interval", &minInterval, 0.1f, 0.1f, 60.0f)) {
                    spawnerComponent->SetSpawnInterval(minInterval, maxInterval);
                }

                if (ImGui::DragFloat("Max Spawn Interval", &maxInterval, 0.1f, 0.1f, 60.0f)) {
                    spawnerComponent->SetSpawnInterval(minInterval, maxInterval);
                }

                // Display timer for next spawn
                ImGui::Separator();
                ImGui::Text("Spawn Timer:");
                float nextSpawnTime = spawnerComponent->GetNextSpawnTime();
                ImGui::Text("Next Spawn In: %.2f seconds", nextSpawnTime);

                // Display and edit selected enemy types with a combo box
                ImGui::Separator();
                ImGui::Text("Selected Enemy Types:");
                const auto& availableEnemyTypes = spawnerComponent->GetAllAvailableEnemyTypes();
                auto& enemyTypes = spawnerComponent->GetEnemyTypes();

                // Iterate through the selected enemy types and display combo boxes
                for (size_t i = 0; i < enemyTypes.size(); ++i) {
                    ImGui::PushID(static_cast<int>(i)); // Create a unique ID for each combo box

                    int currentIndex = -1;  // Default to -1 in case the enemy type is not found
                    for (size_t j = 0; j < availableEnemyTypes.size(); ++j) {
                        if (enemyTypes[i] == availableEnemyTypes[j]) {
                            currentIndex = static_cast<int>(j);
                            break;
                        }
                    }

                    // Display a combo box for each selected enemy type
                    if (ImGui::Combo("Enemy Type", &currentIndex,
                        [](void* data, int idx, const char** out_text) {
                            const auto* types = static_cast<const std::vector<std::string>*>(data);
                            *out_text = (*types)[idx].c_str();
                            return true;
                        },
                        (void*)&availableEnemyTypes,
                        static_cast<int>(availableEnemyTypes.size()))) {

                        // Update the selected enemy type
                        if (currentIndex >= 0 && currentIndex < availableEnemyTypes.size()) {
                            enemyTypes[i] = availableEnemyTypes[currentIndex];
                            spawnerComponent->SetEnemyTypes(enemyTypes); // Update the enemy types in the SpawnerComponent
                        }
                    }

                    ImGui::PopID(); // End the combo box ID scope
                }

                // Add option to add a new enemy type
                if (ImGui::Button("Add Enemy Type")) {
                    if (!availableEnemyTypes.empty()) {
                        enemyTypes.push_back(availableEnemyTypes.front()); // Add the first available type by default
                        spawnerComponent->SetEnemyTypes(enemyTypes);
                    }
                }

                // Add option to remove an enemy type
                if (ImGui::Button("Remove Enemy Type") && !enemyTypes.empty()) {
                    enemyTypes.pop_back(); // Remove the last enemy type
                    spawnerComponent->SetEnemyTypes(enemyTypes);
                }



                if (ImGui::Button("Remove Spawner Component"))
                {
                    selectedGO->RemoveComponent(TypeOfComponent::SPAWNER);
                }

                ImGui::TreePop();
            }
        }

        ExplosionComponent* explosionComponent = selectedGO->GetComponent<ExplosionComponent>(TypeOfComponent::EXPLOSION);
        if (explosionComponent) {
            if (ImGui::TreeNode("Explosion Component")) {
                ImGui::Separator();

                // Display countdown time
                float countdownTime = explosionComponent->GetCountdownTime();
                ImGui::Text("Countdown Time: %.2f", countdownTime);
                if (ImGui::InputFloat("Countdown Time", &countdownTime, 0.1f)) {
                    // Update countdown time if modified in ImGui
                    explosionComponent->SetCountdownTime(countdownTime);
                }

                // Display damage radius
                float damageRadius = explosionComponent->GetDamageRadius();
                ImGui::Text("Damage Radius: %.2f", damageRadius);
                if (ImGui::InputFloat("Damage Radius", &damageRadius, 0.1f)) {
                    // Update damage radius if modified in ImGui
                    explosionComponent->SetDamageRadius(damageRadius);
                }

                // Display damage amount
                float damageAmount = explosionComponent->GetDamageAmount();
                ImGui::Text("Damage Amount: %.2f", damageAmount);
                if (ImGui::InputFloat("Damage Amount", &damageAmount, 1.0f)) {
                    // Update damage amount if modified in ImGui
                    explosionComponent->SetDamageAmount(damageAmount);
                }

                // Display if explosion has already occurred
                bool hasExploded = explosionComponent->HasExploded();
                ImGui::Text("Has Exploded: %s", hasExploded ? "Yes" : "No");

                // Button to manually trigger explosion
                if (ImGui::Button("Trigger Explosion")) {
                    explosionComponent->TriggerExplosion();
                }

                if (ImGui::Button("Remove Explosion Component"))
                {
                    selectedGO->RemoveComponent(TypeOfComponent::EXPLOSION);
                }

                ImGui::TreePop();
            }
        }

        ParticleSystem* particleSystem = selectedGO->GetComponent<ParticleSystem>(TypeOfComponent::PARTICLE);
        if (particleSystem) {
            if (ImGui::TreeNode("Particle Component")) {
                // Duration control
                float duration = particleSystem->GetDuration();
                if (ImGui::DragFloat("Duration", &duration, 0.1f, 0.0f, 100.0f, "%.1f", ImGuiSliderFlags_AlwaysClamp)) {
                    particleSystem->SetDuration(duration);
                }

                // Looping control
                bool isLooping = particleSystem->IsLooping();
                if (ImGui::Checkbox("Looping", &isLooping)) {
                    particleSystem->SetLooping(isLooping);
                }

                // Lifetime control
                float lifetime = particleSystem->GetLifetime();
                if (ImGui::DragFloat("Lifetime", &lifetime, 0.1f, 0.0f, 100.0f, "%.1f", ImGuiSliderFlags_AlwaysClamp)) {
                    particleSystem->SetLifetime(lifetime);
                }

                // Speed control
                float speed = particleSystem->GetSpeed();
                if (ImGui::DragFloat("Speed", &speed, 0.1f, 0.0f, 100.0f, "%.1f", ImGuiSliderFlags_AlwaysClamp)) {
                    particleSystem->SetSpeed(speed);
                }

                // Size control
                Vector2 size = particleSystem->GetSize();
                if (ImGui::DragFloat("SizeX", &size.x, 0.1f, 0.0f, 100.0f, "%.1f", ImGuiSliderFlags_AlwaysClamp)) {
                    particleSystem->SetSize(size);
                }

                if (ImGui::DragFloat("SizeY", &size.y, 0.1f, 0.0f, 100.0f, "%.1f", ImGuiSliderFlags_AlwaysClamp)) {
                    particleSystem->SetSize(size);
                }

                //// Color control
                //Vector3 color = particleSystem->GetColor();
                //float rgb[3] = { color.x, color.y, color.z };
                //if (ImGui::ColorEdit3("Color", rgb)) {
                //    particleSystem->SetColor(Vector3(rgb[0], rgb[1], rgb[2]));
                //}

                auto currentSprite = particleSystem->GetAnimation();
                if (currentSprite) {
                    unsigned int textureId = currentSprite->GetSpriteTexture()->GetTextureID();
                    ImGui::Text("Current Texture:");
                    ImGui::Image((void*)(intptr_t)textureId, ImVec2(64, 64), ImVec2(0, 1), ImVec2(1, 0));

                    // Create a drop target for textures
                    if (ImGui::BeginDragDropTarget()) {
                        if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("CONTENT_BROWSER_ITEM")) {
                            const FilePayload* filePayload = static_cast<const FilePayload*>(payload->Data);
                            if (filePayload->isTexture) {
                                std::string textureName(filePayload->textureName);

                                // Check if this is an animation file
                                std::string animPrefix = "Animation-";
                                if (textureName.substr(0, animPrefix.length()) == animPrefix) {
                                    // Extract the animation name (e.g., "Ame" from "Animation-Ame")
                                    std::string animName = textureName.substr(animPrefix.length());
                                    std::string animationKey = "Animation_" + animName;

                                    // Get the pre-loaded animation
                                    auto newSprite = AssetManager::GetInstance().GetSprite(animationKey);
                                    if (newSprite) {
                                        particleSystem->SetAnimation(std::make_unique<SpriteAnimation>(newSprite));
                                        ImGuiConsole::Cout("Loaded animation: %s", animationKey.c_str());
                                    }
                                    else {
                                        ImGuiConsole::Cout("Failed to find pre-loaded animation: %s", animationKey.c_str());
                                    }
                                }
                                else {
                                    // Handle as regular texture
                                    auto newSprite = AssetManager::GetInstance().GetSprite(textureName);
                                    if (newSprite) {
                                        particleSystem->SetAnimation(std::make_unique<SpriteAnimation>(newSprite));
                                        ImGuiConsole::Cout("Loaded texture: %s", textureName.c_str());
                                    }
                                }
                            }
                        }
                        ImGui::EndDragDropTarget();
                    }

                    // Animation controls for sprite sheets
                    currentSprite = particleSystem->GetAnimation();
                    int frameX = (int)currentSprite->GetSpriteTexture()->GetNxFrames();
                    int frameY = (int)currentSprite->GetSpriteTexture()->GetNyFrames();
                    int totalFrame = (int)currentSprite->GetSpriteTexture()->GetTotalFrames();
                    double framesPS = currentSprite->GetSpriteTexture()->GetFramePs();
                    if (ImGui::DragInt("Number Of Frame X", &frameX, 1.0f))
                        currentSprite->GetSpriteTexture()->SetNxFrames(frameX);
                    if (ImGui::DragInt("Number Of Frame Y", &frameY, 1.0f))
                        currentSprite->GetSpriteTexture()->SetNyFrames(frameY);
                    if (ImGui::InputDouble("Frames per Second", &framesPS, 1.0f))
                        currentSprite->GetSpriteTexture()->SetFramePs(framesPS);
                    if (ImGui::DragInt("Total Frame", &totalFrame, 1.0f))
                        currentSprite->GetSpriteTexture()->SetTotalFrames(totalFrame);

                    // Reset animation button
                    if (ImGui::Button("Reset Animation")) {
                        currentSprite->ResetSpriteAnimation();
                    }
                }
                else {
                    ImGui::Text("No texture assigned");
                    // Drop target for empty sprite component
                    if (ImGui::BeginDragDropTarget()) {
                        if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("CONTENT_BROWSER_ITEM")) {
                            const FilePayload* filePayload = static_cast<const FilePayload*>(payload->Data);
                            if (filePayload->isTexture) {
                                std::string textureName(filePayload->textureName);

                                // Same animation check logic as above
                                std::string animPrefix = "Animation-";
                                if (textureName.substr(0, animPrefix.length()) == animPrefix) {
                                    std::string animName = textureName.substr(animPrefix.length());
                                    std::string animationKey = "Animation_" + animName;

                                    auto newSprite = AssetManager::GetInstance().GetSprite(animationKey);
                                    if (newSprite) {
                                        particleSystem->SetAnimation(std::make_unique<SpriteAnimation>(newSprite));
                                    }
                                }
                                else {
                                    auto newSprite = AssetManager::GetInstance().GetSprite(textureName);
                                    if (newSprite) {
                                        particleSystem->SetAnimation(std::make_unique<SpriteAnimation>(newSprite));
                                    }
                                }
                            }
                        }
                        ImGui::EndDragDropTarget();
                    }

                }

                if (ImGui::Button("Remove Particle System Component"))
                {
                    selectedGO->RemoveComponent(TypeOfComponent::PARTICLE);
                }
                ImGui::TreePop();
            }
        }

        CanvasComponent* canvasComponent = selectedGO->GetComponent<CanvasComponent>(TypeOfComponent::CANVAS_UI);
        if (canvasComponent)
        {
            if (ImGui::TreeNode("Canvas Component"))
            {
                if (ImGui::Button("Remove Canvas Component"))
                {
                    selectedGO->RemoveComponent(TypeOfComponent::CANVAS_UI);
                }
                ImGui::TreePop();
            }
        }

        VideoComponent* videoComponent = selectedGO->GetComponent<VideoComponent>(TypeOfComponent::VIDEO);
        if (videoComponent) {
            if (ImGui::TreeNode("Video Component"))
            {
                // Display current scene
                int currentCutScene = videoComponent->GetCurrentScene();
                if (ImGui::InputInt("Current Scene", &currentCutScene)) {
                    if (currentCutScene < 1) {
                        ImGuiConsole::Cout("Warning: currentScene cannot be less than 1. Setting to 1.");
                        currentCutScene = 1;
                    }
                    if (currentCutScene > videoComponent->GetTotalScenes()) {
                       ImGuiConsole::Cout("Warning: currentScene exceeds totalScenes. Setting to last valid scene.");
                        currentCutScene = videoComponent->GetTotalScenes();
                    }
                    videoComponent->SetCurrentScene(currentCutScene);
                }

                // Control for switching interval
                float switchInterval = videoComponent->GetSwitchInterval();
                if (ImGui::DragFloat("Switch Interval (s)", &switchInterval, 0.1f, 0.1f, 10.0f)) {
                    videoComponent->SetSwitchInterval(switchInterval);
                }

                // Control for total scenes
                int totalScenes = videoComponent->GetTotalScenes();
                if (ImGui::InputInt("Total Scenes", &totalScenes)) {
                    totalScenes = std::max(1, totalScenes);  // Ensure at least 1 scene
                    videoComponent->SetTotalScenes(totalScenes);
                }

                if (ImGui::Button("Remove Video Component"))
                {
                    selectedGO->RemoveComponent(TypeOfComponent::VIDEO);
                }
                ImGui::TreePop();
            }
        }


        UITextComponent* uitextComponent = selectedGO->GetComponent<UITextComponent>(TypeOfComponent::TEXT_UI);
        if (uitextComponent)
        {
            if (ImGui::TreeNode("UI Text Component"))
            {
                // To make it editable
                static char textBuffer[256];
                strncpy_s(textBuffer, sizeof(textBuffer), uitextComponent->GetText().c_str(), sizeof(textBuffer) - 1);
                textBuffer[sizeof(textBuffer) - 1] = '\0'; //Null terminating

                if (ImGui::InputText("Text Content", textBuffer, sizeof(textBuffer))) {
                    uitextComponent->SetText(std::string(textBuffer));
                }

                Vector2 pos = uitextComponent->GetPosition();  // Get the current position
                if (ImGui::DragFloat("Position X##", &pos.x, 1.0f)) {
                    uitextComponent->SetPosition(pos);  // Set the updated position
                }
                if (ImGui::DragFloat("Position Y##", &pos.y, 1.0f)) {
                    uitextComponent->SetPosition(pos);  // Set the updated position
                }

                // Editable font size
                float fontSize = uitextComponent->GetFontSize();
                if (ImGui::DragFloat("Size: ", &fontSize, 0.1f)) {
                    uitextComponent->SetFont(uitextComponent->GetFontType(), fontSize);  // Set the updated font size
                }
                if (ImGui::Button("Remove Text UI Component")) {
                    selectedGO->RemoveComponent(TypeOfComponent::TEXT_UI);
                }
                ImGui::TreePop();
            }
        }

        UISpriteComponent* uisprite = selectedGO->GetComponent<UISpriteComponent>(TypeOfComponent::SPRITE_UI);
        if (uisprite) {
            if (ImGui::TreeNode("UI Sprite Component")) {
                // Display current texture preview
                auto currentSprite = uisprite->GetCurrentSprite();
                if (currentSprite && currentSprite->GetSpriteTexture()) {
                    unsigned int textureId = currentSprite->GetSpriteTexture()->GetTextureID();
                    ImGui::Text("Current Texture:");
                    ImGui::Image((void*)(intptr_t)textureId, ImVec2(64, 64), ImVec2(0, 1), ImVec2(1, 0));

                    // Create a drop target for textures
                    if (ImGui::BeginDragDropTarget()) {
                        if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("CONTENT_BROWSER_ITEM")) {
                            const FilePayload* filePayload = static_cast<const FilePayload*>(payload->Data);
                            if (filePayload->isTexture) {
                                std::string textureName(filePayload->textureName);

                                // Check if this is an animation file
                                std::string animPrefix = "Animation-";
                                if (textureName.substr(0, animPrefix.length()) == animPrefix) {
                                    // Extract the animation name (e.g., "Ame" from "Animation-Ame")
                                    std::string animName = textureName.substr(animPrefix.length());
                                    std::string animationKey = "Animation_" + animName;

                                    // Get the pre-loaded animation
                                    auto newSprite = AssetManager::GetInstance().GetSprite(animationKey);
                                    if (newSprite) {
                                        uisprite->ChangeSprite(std::make_unique<SpriteAnimation>(newSprite));
                                        ImGuiConsole::Cout("Loaded animation: %s", animationKey.c_str());
                                    }
                                    else {
                                        ImGuiConsole::Cout("Failed to find pre-loaded animation: %s", animationKey.c_str());
                                    }
                                }
                                else {
                                    // Handle as regular texture
                                    auto newSprite = AssetManager::GetInstance().GetSprite(textureName);
                                    if (newSprite) {
                                        uisprite->ChangeSprite(std::move(std::make_unique<SpriteAnimation>(newSprite)));
                                        ImGuiConsole::Cout("Loaded texture: %s", textureName.c_str());
                                    }
                                }
                            }
                            else {
                                ContentBrowserPanel::GetInstance().ShowNotification("Invalid file format. Please drag and drop a valid texture file.");
                            }
                        }
                        ImGui::EndDragDropTarget();
                    }

                    // Animation controls for sprite sheets

                    ImGui::Separator();
                    ImGui::Text("Animation Settings");
                    currentSprite = uisprite->GetCurrentSprite();
                    int frameX = (int)currentSprite->GetSpriteTexture()->GetNxFrames();
                    int frameY = (int)currentSprite->GetSpriteTexture()->GetNyFrames();
                    int totalFrame = (int)currentSprite->GetSpriteTexture()->GetTotalFrames();
                    double framesPS = currentSprite->GetSpriteTexture()->GetFramePs();
                    if (ImGui::DragInt("Number Of Frame X", &frameX, 1.0f))
                        currentSprite->GetSpriteTexture()->SetNxFrames(frameX);
                    if (ImGui::DragInt("Number Of Frame Y", &frameY, 1.0f))
                        currentSprite->GetSpriteTexture()->SetNyFrames(frameY);
                    if (ImGui::InputDouble("Frames per Second", &framesPS, 1.0f))
                        currentSprite->GetSpriteTexture()->SetFramePs(framesPS);
                    if (ImGui::DragInt("Total Frame", &totalFrame, 1.0f))
                        currentSprite->GetSpriteTexture()->SetTotalFrames(totalFrame);

                    // Reset animation button
                    if (ImGui::Button("Reset Animation")) {
                        currentSprite->ResetSpriteAnimation();
                    }
                }
                else {
                    ImGui::Text("No texture assigned");
                    // Drop target for empty sprite component
                    if (ImGui::BeginDragDropTarget()) {
                        if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("CONTENT_BROWSER_ITEM")) {
                            const FilePayload* filePayload = static_cast<const FilePayload*>(payload->Data);
                            if (filePayload->isTexture) {
                                std::string textureName(filePayload->textureName);

                                // Same animation check logic as above
                                std::string animPrefix = "Animation-";
                                if (textureName.substr(0, animPrefix.length()) == animPrefix) {
                                    std::string animName = textureName.substr(animPrefix.length());
                                    std::string animationKey = "Animation_" + animName;

                                    auto newSprite = AssetManager::GetInstance().GetSprite(animationKey);
                                    if (newSprite) {
                                        uisprite->ChangeSprite(std::make_unique<SpriteAnimation>(newSprite));
                                    }
                                }
                                else {
                                    auto newSprite = AssetManager::GetInstance().GetSprite(textureName);
                                    if (newSprite) {
                                        uisprite->ChangeSprite(std::make_unique<SpriteAnimation>(newSprite));
                                    }
                                }
                            }
                        }
                        ImGui::EndDragDropTarget();
                    }
                }

                // Editable layer value using a getter and setter
                int layer = uisprite->GetLayer();  // Get the current layer value
                if (ImGui::DragInt("Layer", &layer, 1.0f)) {
                    uisprite->SetLayer(layer);  // Update the layer if the value changes
                }


                // Editable flip options using getters and setters
                bool flipX = uisprite->GetFlipX();
                if (ImGui::Checkbox("Flip X", &flipX)) {
                    uisprite->SetFlipX(flipX);  // Update flipX if the value changes
                }

                bool flipY = uisprite->GetFlipY();
                if (ImGui::Checkbox("Flip Y", &flipY)) {
                    uisprite->SetFlipY(flipY);  // Update flipY if the value changes
                }


                bool Render = uisprite->GetIsRenderable();
                if (ImGui::Checkbox("Render", &Render)) {
                    uisprite->SetIsRenderable(Render);  // Update renderableUI if the value changes
                }
                if (ImGui::Button("Remove Sprite UI Component")) {
                    selectedGO->RemoveComponent(TypeOfComponent::SPRITE_UI);
                }
                ImGui::TreePop();
            }
        }
        PauseMenuButton* pauseButton = selectedGO->GetComponent<PauseMenuButton>(TypeOfComponent::PAUSEMENUBUTTON);
        if (pauseButton)
        {
            if (ImGui::TreeNode("Pause Menu Button Script"))
            {
                if (ImGui::Button("Remove Pause Button Script")) {
                    selectedGO->RemoveComponent(TypeOfComponent::PAUSEMENUBUTTON);
                }
                ImGui::TreePop();
            }
        }
        SliderComponent* sliderButton = selectedGO->GetComponent<SliderComponent>(TypeOfComponent::SLIDER);
        if (sliderButton)
        {
            if (ImGui::TreeNode("SliderComponent"))
            {
                if (ImGui::DragFloat("Min Position X Offset", &sliderButton->minPosXOffset, 0.1f, 0.0f, FLT_MAX, "%.2f")) {
                }

                ImGui::Text("CurrentValue: %.1f", sliderButton->currentValue);

                const char* volumeTypeItems[] = { "MASTER", "BGM", "SFX" };
                int currentVolumeType = static_cast<int>(sliderButton->volumeType);  // Convert enum to int for combo box
                if (ImGui::Combo("Volume Type", &currentVolumeType, volumeTypeItems, IM_ARRAYSIZE(volumeTypeItems))) {
                    sliderButton->volumeType = static_cast<VolumeType>(currentVolumeType);  // Convert int back to enum
                }

                if (ImGui::Button("Remove SliderComponent")) {
                    selectedGO->RemoveComponent(TypeOfComponent::SLIDER);
                }
                ImGui::TreePop();
            }
        }


        AnimatorComponent* animatorComponent = selectedGO->GetComponent<AnimatorComponent>(TypeOfComponent::ANIMATOR);
        if (animatorComponent) {
            if (ImGui::TreeNode("Animator Component")) {

                if (animatorComponent->animationController != nullptr)
                    ImGui::Text(animatorComponent->animationName.c_str());
                else
                    ImGui::Text("Drag and drop Animator Controller (Lua file) here:");

                // Set up drag-and-drop target
                if (ImGui::BeginDragDropTarget()) {
                    if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("CONTENT_BROWSER_ITEM")) { // Expecting a file payload
                        // Retrieve the file path from the payload
                        const char* filePath = (const char*)payload->Data;

                        // Load and parse the Lua file into an AnimatorController
                        AnimationController controller1;
                        try {
                            LoadFromLua(controller1, filePath);  // Assuming LoadFromLua parses the Lua file and sets up the controller

                            // Now that the controller is created from the Lua file, we move it into the AnimatorComponent
                            animatorComponent->SetAnimationController(std::make_unique<AnimationController>(std::move(controller1)));
                            animatorComponent->animationName = filePath;
                        }
                        catch (const std::exception& e) {
                            std::cerr << "Error loading Lua file: " << e.what() << std::endl;
                        }
                    }
                    else {
                        std::cerr << "Error: Payload is not a valid file!" << std::endl;
                    }
                    ImGui::EndDragDropTarget();
                }

                if (ImGui::Button("Remove Animator Component"))
                {
                    selectedGO->RemoveComponent(TypeOfComponent::ANIMATOR);
                }
                ImGui::TreePop();
            }
        }


        VfxFollowComponent* vfxFollowScript = selectedGO->GetComponent<VfxFollowComponent>(TypeOfComponent::VFX_FOLLOW);
        if (vfxFollowScript) {
            if (ImGui::TreeNode("VFX Follow Script Component"))
            {
                if (ImGui::Button("Remove VFX Follow Script"))
                {
                    selectedGO->RemoveComponent(TypeOfComponent::VFX_FOLLOW);
                }
                ImGui::TreePop();
            }
        }


        if (ImGui::Button("Despawn")) {
            factory.Despawn(selectedGO);  // Despawn the selected object
            selectedGO = nullptr;  // Clear selection after despawning
        }
        if (ImGui::Button("Save Prefab")) {
            std::string filePathLua;
            std::string name;
            if (selectedGO != nullptr) {
                name = selectedGO->GetName();
                std::string prefabPathPrefix = "Assets/Lua/Prefabs/";
                std::string prefabPathtype = ".lua";
                filePathLua = prefabPathPrefix + selectedGO->GetName() + prefabPathtype;
            }

            factory.GetInstance().SerializeGameObjectHierarchy(filePathLua, selectedGO, selectedGO->GetChildren());
        }

        if (selectedGO != nullptr) {
            if (selectedGO->GetParent() != nullptr) {
                if (ImGui::Button("Detach from parent")) {
                    selectedGO->UnsetParent();
                    factory.UpdateAllGameObjects();
                }
            }
        }

        ImGui::End();
        }

    // Showing all the Game Objects with respective name
    void DisplayGameObjectHierarchy(GameObject* object) {
        if (!object) return;

        int objectId = object->GetId();
        std::string displayName = object->GetName() + " (ID: " + std::to_string(objectId) + ") Header";

        // Create a TreeNode for this object, expandable to show children
        if (ImGui::TreeNode(displayName.c_str())) {
            // Make the GameObject name selectable
            if (ImGui::Selectable(object->GetName().c_str(), Engine::GetInstance().GetSelectedObject() == object)) {
                PlayerSceneControls::GetInstance().SetSelectedGameObject(object);
            }

            // Initiate drag-and-drop source for this GameObject
            if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_SourceAllowNullID)) {
                ImGui::SetDragDropPayload("GAMEOBJECT", &object, sizeof(GameObject*)); // Payload contains pointer to the GameObject
                ImGui::Text("Dragging %s", displayName.c_str()); // Tooltip text
                ImGui::EndDragDropSource();
            }

            // Accept drag-and-drop target for reparenting
            if (ImGui::BeginDragDropTarget()) {
                if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("GAMEOBJECT")) {
                    IM_ASSERT(payload->DataSize == sizeof(GameObject*));
                    GameObject* draggedObject = *(GameObject**)payload->Data;

                    // Set the dragged object as a child of this object
                    if (draggedObject != object) {
                        draggedObject->SetParent(object);
						ImGuiConsole::Cout("Set %s as a child of %s", draggedObject->GetName().c_str(), object->GetName().c_str());
                    }
                }

                ImGui::EndDragDropTarget();
            }

            // Recursively display all children of this GameObject
            for (GameObject* child : object->GetChildren()) {
                DisplayGameObjectHierarchy(child);
            }

            ImGui::TreePop(); // Close this nodez
        }
    }

    // display the gameobject hierarchy present in the scene
    void GameObjectListWindow() {
        ImGui::Begin("GameObject List");

        GameObjectFactory& factory = GameObjectFactory::GetInstance();
        const std::unordered_map<int, GameObject*>& gameObjectMap = factory.GetAllGameObjects();

        // Search input for filtering game objects
        static char searchBuffer[256] = "";
        ImGui::InputText("Search", searchBuffer, IM_ARRAYSIZE(searchBuffer));
        std::string searchQuery = Utilities::ToLower(searchBuffer);

        // Display game objects hierarchically, starting from top-level objects
        for (const auto& it : gameObjectMap) {
            GameObject* object = it.second;

            // Only display top-level objects in the search result
            if (object->GetParent() == nullptr) {
                std::string objectName = Utilities::ToLower(object->GetName());

                // Check if the object's name matches the search query
                if (searchQuery.empty() || objectName.find(searchQuery) != std::string::npos) {
                    DisplayGameObjectHierarchy(object);
                }
            }
        }

        ////CURRENTLY SPAWNING AN EMPTY GAME OBJECT IMMEDIATELY SETS THE GO AS THE CHILD OF GO 1
        if (ImGui::Button("Spawn empty gameobject")) {

            GameObject* newGO = factory.Create("Game");
            newGO->AddComponent<TransformComponent>(TypeOfComponent::TRANSFORM, Vector2{ 0, 0 }, Vector2{ 100, 100 }, 0.f);
            
        }


        if (ImGui::Button("Spawn 100 Light Enemy")) {
            for (int i = 0; i < 100; ++i) {
                factory.CreateFromLua("Assets/Lua/Prefabs/Light_Enemy.lua", "Light_Enemy_0");
            }
        }

        if (ImGui::Button("Spawn Heavy Enemy")) {
            //GameObjectFactory& factory = GameObjectFactory::GetInstance();
            GameObject* newEnemy = factory.CreateFromLua("Assets/Lua/Prefabs/Heavy_Enemy.lua", "Heavy_Enemy_0");

            // Get the player object to set as the chase target
            GameObject* player = factory.GetPlayerObject();

            // Configure AI behavior
            if (player && newEnemy) {
                AIStateMachineComponent* aiComponent = newEnemy->GetComponent<AIStateMachineComponent>(TypeOfComponent::AISTATE);
                if (aiComponent) {
                    aiComponent->SetState("CHASE");
                    aiComponent->SetChaseTarget(player);
                    aiComponent->SetMoveSpeed(100.0f);
                }
            }
        }

        if (ImGui::Button("Spawn Bomb Enemy")) {
            factory.CreateFromLua("Assets/Lua/Prefabs/Bomb_Enemy.lua", "Bomb_Enemy_0");
        }
        ImGui::End();
    }

    // display the game/editor scene window
    void SceneWindow() {
        Engine& engine = Engine::GetInstance();

        if (engine.cameraManager.GetCurrentMode() == CameraManager::CameraMode::EditorCamera)
            return;

        // Determine the title based on the current camera mode
        std::string title = (engine.cameraManager.GetCurrentMode() == CameraManager::CameraMode::EditorCamera) ? "Level Editor" : "Scene";

        // Assuming you have your texture ID from your graphics API (e.g., OpenGL, DirectX)
        ImVec2 imageSize(1600, 900); // Size of the image to display

        // Begin the ImGui window with the dynamic title
        ImGui::Begin(title.c_str()); // Use c_str() to convert std::string to const char*

        ImVec2 availableSize = ImGui::GetContentRegionAvail();

        // Ensure the aspect ratio is maintained
        float aspectRatio = imageSize.x / imageSize.y;
        if (availableSize.x / aspectRatio <= availableSize.y)
        {
            availableSize.y = availableSize.x / aspectRatio;
        }
        else
        {
            availableSize.x = availableSize.y * aspectRatio;
        }

        engine.availableSizeEngine = availableSize;

        // Calculate UV coordinates
        ImVec2 uv0(0.0f, 1.0f); // Top-left corner of the image
        ImVec2 uv1(1.0f, 0.0f); // Bottom-right corner of the image

        // Display the scene texture to the entire content region
        ImGui::Image((void*)(intptr_t)frameBuffer.GetTextureID(), availableSize, uv0, uv1);

        ImGui::End();

    }

    // display the control play to play/stop/pause to transit between editor scene and game scene
    void ControlPanel()
    {
        Engine& engine = Engine::GetInstance();

        ImGui::Begin("Control Panel");
        ImVec2 buttonSize(40, 30);
        float windowWidth = ImGui::GetWindowSize().x;
        float totalButtonWidth = buttonSize.x * 3 + ImGui::GetStyle().ItemSpacing.x * 2;
        float offsetX = (windowWidth - totalButtonWidth) * 0.5f;

        ImGui::SetCursorPosX(offsetX);
        if (!engine.isInGameScene) {
            //Play button when it is not in game scene
            if (ImGui::ImageButton("PlayButton",
                (void*)(uintptr_t)AssetManager::GetInstance().GetSprite("PlayIcon")->GetTextureID(),
                buttonSize))
            {
                ImGuiConsole::Cout("Current Scene: %s", currentScene.c_str());
                if (currentScene == "Assets/Lua/state.lua")
                {
                    ImGuiConsole::Cout("Loading State File");

                    ImGuiConsole::Cout("Original: %s", engine.originalScene.c_str());
                    if (engine.originalScene == "Assets/Lua/Scenes/VictoryScene.lua" ||
                        engine.originalScene == "Assets/Lua/Scenes/MainMenuScene.lua" ||
                        engine.originalScene == "Assets/Lua/Scenes/GameOverScene.lua")
                    {
                        ImGuiConsole::Cout("TRIGGERING");

                        variables::isRunningBack = false;
                        variables::isInteractable = true;
                        variables::volumeMenuOn = false;
                        engine.showCursor = true;
                    }
                }

                //AudioComponent* adComponent = GameObjectFactory::GetInstance().GetAllGameObjects().at(0)->GetComponent<AudioComponent>(TypeOfComponent::AUDIO);
                //const std::vector<int>& audioClips = adComponent->GetAudioClips();  // Get list of audio IDs
                //adComponent->PlayAudio(audioClips[0]);
#ifdef _IMGUI
                if (currentScene != "Assets/Lua/Scenes/GameScene.lua" &&
                    currentScene != "Assets/Lua/Scenes/CutScene.lua") //If next scenes is not game or cutscene, so it will play if go into main menu and others
                {
                    
                   //if (GameObjectFactory::GetInstance().GetAllGameObjects().at(2)->GetName() != "FastForwardInfo")
                   //{
                   //    AudioManager::GetInstance().PlayAudio(16);
                   //}
                }
#else
#endif
				engine.cameraManager.SetCameraMode(CameraManager::CameraMode::PlayerCamera);
                engine.isPaused = false;
                engine.isInGameScene = true;
                engine.SaveStateToLua();
                engine.time = engine.maxTime;
                // Unbind parent-child relationships using UnsetParent
                //GameObjectFactory& factory = GameObjectFactory::GetInstance();
                //for (auto& pair : factory.GetAllGameObjects()) {
                //    GameObject* object = pair.second;
                //    if (object) {
                //        object->UnsetParent(); // Use UnsetParent method
                //        ImGuiConsole::Cout("Parent unbound for GameObject: " << object->GetName());
                //    }
                //}
                

                if (currentScene == "Assets/Lua/Scenes/MainMenuScene.lua" || 
                    currentScene == "Assets/Lua/Scenes/GameOverScene.lua" || 
                    currentScene == "Assets/Lua/Scenes/VictoryScene.lua")
                {
                    variables::isRunningBack = false;
                    variables::isInteractable = true;
                    variables::volumeMenuOn = false;
                    engine.showCursor = true;

                }
                else if (currentScene == "Assets/Lua/Scenes/GameScene.lua")//Should only be in game scene
                {
                    variables::volumeMenuOn = false;
                    variables::isRunningBack = false;
                    variables::isInteractable = true;
                    engine.showCursor = false;
                }

            }
            ImGui::SameLine();
        }
        else {
            //Stops the game, reset to previous state
            if (ImGui::ImageButton("StopButton",
                (void*)(uintptr_t)AssetManager::GetInstance().GetSprite("StopIcon")->GetTextureID(),
                buttonSize))
            {
                engine.cameraManager.SetCameraMode(CameraManager::CameraMode::EditorCamera);
                engine.isPaused = false;
                engine.isInGameScene = false;
                AudioManager::GetInstance().StopAudio(8);
                AudioManager::GetInstance().StopAudio(17);
                engine.LoadStateFromLua();
                variables::isInteractable = true;
                variables::isRunningBack = false;
                variables::volumeMenuOn = false;
                engine.showCursor = true;
                AudioManager::GetInstance().StopAllAudio();
                
            }
            ImGui::SameLine();
        }

        //Pause the game 
        if (ImGui::ImageButton("PauseIcon",
            (void*)(uintptr_t)AssetManager::GetInstance().GetSprite("PauseIcon")->GetTextureID(),
            buttonSize))
        {
            if (!engine.isPaused)
            {
                AudioManager::GetInstance().PauseAllAudio();
                engine.isPaused = true;
            }
            else
            {
                AudioManager::GetInstance().ResumeAllAudio();
                engine.isPaused = false;
            }
            //engine.cameraManager.SetCameraMode(CameraManager::CameraMode::EditorCamera);
		}

        ImGui::End();
    }

    // Function to display the combined ImGui interface with tabs
    void AssetsAndDebugWindow() {
        ImGui::Begin("Asset Window");
        AssetManager& assetManager = AssetManager::GetInstance();

        // Test audio functionality
        if (ImGui::TreeNode("Audio")) {
            if (assetManager.audioObjects.empty()) {
                ImGui::Text("No audio objects available.");
            }
            else {
                for (int i = 0; i < assetManager.audioObjects.size(); i++) {
                    ImGui::Text("Audio Name: %s", assetManager.audioObjects.at(i)->GetAudioName().c_str());
                }
            }
            ImGui::TreePop();
        }

        // Shader section
        if (ImGui::TreeNode("Shaders")) {
            auto shaderInfos = assetManager.GetShaderInfo();

            if (shaderInfos.empty()) {
                ImGui::Text("No shaders loaded.");
            }
            else {
                for (const auto& shader : shaderInfos) {
                    if (ImGui::TreeNode(shader.name.c_str())) {
                        ImGui::Text("Path: %s", shader.path.c_str());
                        ImGui::Text("Type: %s", shader.type.c_str());

                        if (!shader.uniforms.empty()) {
                            if (ImGui::TreeNode("Uniforms")) {
                                for (const auto& [name, value] : shader.uniforms) {
                                    ImGui::Text("%s: %d", name.c_str(), value);
                                }
                                ImGui::TreePop();
                            }
                        }

                        ImGui::TreePop();
                    }
                }
            }
            ImGui::TreePop();
        }
        // Textures section
        if (ImGui::TreeNode("Textures")) {
            const auto& textures = assetManager.GetTextures();

            if (textures.empty()) {
                ImGui::Text("No textures loaded.");
            }
            else {
                ImGui::Text("Total Textures Loaded: %d", assetManager.GetTotalTextureLoaded());
                ImGui::Separator();

                for (const auto& [name, texture] : textures) {
                    if (ImGui::TreeNode(name.c_str())) {
                        // Display texture preview if it's available
                        ImGui::Text("Name: %s", name.c_str());

                        // Get texture ID and display a preview
                        unsigned int textureID = texture->GetTextureID();
                        ImVec2 previewSize(64, 64); // Adjust size as needed
                        ImGui::Image((void*)(intptr_t)textureID,
                            previewSize,
                            ImVec2(0, 1),   
                            ImVec2(1, 0));  

                        // Display additional texture information
                        int width = texture->GetWidth();
                        int height = texture->GetHeight();

                        ImGui::Text("Size: %dx%d", width, height);

                        // For sprite animations, display additional info
                        if (texture->GetNxFrames() > 1 || texture->GetNyFrames() > 1) {
                            ImGui::Text("Type: Sprite Animation");
                            ImGui::Text("Frames: %.0f x %.0f", texture->GetNxFrames(), texture->GetNyFrames());
                            ImGui::Text("Frame Rate: %.2f FPS", texture->GetFramePs());
                        }
                        else {
                            ImGui::Text("Type: Static Sprite");
                        }

                        ImGui::TreePop();
                    }

                }
            }
            ImGui::TreePop();
        }

        // Font section
        if (ImGui::TreeNode("Fonts")) {
            auto fontInfos = assetManager.GetFontInfo();

            if (fontInfos.empty()) {
                ImGui::Text("No fonts loaded.");
            }
            else {
                for (const auto& font : fontInfos) {
                    if (ImGui::TreeNode(font.name.c_str())) {
                        ImGui::Text("Path: %s", font.path.c_str());
                        ImGui::Text("Size: %d", font.size);
                        ImGui::Text("Type: %s", font.type.c_str());
                        ImGui::TreePop();
                    }
                }
            }
            ImGui::TreePop();
        }

        // Lua Files section
        if (ImGui::TreeNode("Lua Files")) {
            const auto& luaFiles = assetManager.GetLuaManagers();
            if (luaFiles.empty()) {
                ImGui::Text("No Lua files loaded.");
            }
            else {
                for (const auto& [name, manager] : luaFiles) {
                    // Create a tree node for each Lua file
                    if (ImGui::TreeNode(name.c_str())) {
                        // Display the file path
                        ImGui::Text("Path: %s.lua", name.c_str());

                        // Display object names in the Lua file
                        auto objectNames = manager->extractNames();
                        if (!objectNames.empty()) {
                            ImGui::Text("Objects in file:");
                            for (const auto& [id, objName] : objectNames) {
                                ImGui::BulletText("%s (ID: %d)", objName.c_str(), id);
                            }
                        }

                        ImGui::TreePop();
                    }     
                }
            }
            ImGui::TreePop();
        }
       
        if (ImGui::TreeNode("Prefabs")) {
            const auto& prefabs = assetManager.GetPrefabs();

            if (prefabs.empty()) {
                ImGui::Text("No prefabs loaded.");
            }
            else {
                ImGui::Text("Total Prefabs Loaded: %d", assetManager.GetTotalPrefabsLoaded());
                ImGui::Separator();

                std::string prefabPath = "Assets/Lua/Prefabs/";
                try {
                    for (const auto& entry : std::filesystem::directory_iterator(prefabPath)) {
                        if (entry.path().extension() == ".lua") {
                            std::string filename = entry.path().stem().string();
                            if (ImGui::TreeNode(filename.c_str())) {
                                ImGui::Text("Path: %s", entry.path().string().c_str());
                                ImGui::TreePop(); 
                            }
                        }
                    }
                }
                catch (const std::filesystem::filesystem_error& e) {
                    ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "Error accessing prefab directory: %s", e.what());
                }
            }

            ImGui::TreePop(); 
        }
    
        ImGui::End(); 
    }

    // display the gizmos available options
    void GizmoConfigurationsWindow()
    {
        ImGui::Begin("Gizmo Config");
        ImGui::Checkbox("Show Velocity", &Graphics::GizmosConfig::showVelocity);
        ImGui::SameLine();
        ImGui::Checkbox("Show Bounding Box", &Graphics::GizmosConfig::showBoundingBox);
        ImGui::Checkbox("Spatial Partitioning Collision", &GameObjectFactory::GetInstance().isSpatial);
        ImGui::Checkbox("Use Force", &GameObjectFactory::GetInstance().useForce);

        ImGui::End();
    }

    // display the time consumption for each system
    void SystemProcessTimeWindow()
    {
        SystemLogManager& logManager = SystemLogManager::GetInstance();
        auto logAverages = logManager.GetLogAverages();

        // Calculate the total time
        double totalTime = 0.0;
        for (const auto& log : logAverages) {
            totalTime += log.second;
        }

        ImGui::Begin("Log Averages");
        for (const auto& log : logAverages) {
            double percentage = (log.second / totalTime) * 100.0;
            ImGui::Text("%s: %.2fms (%.2f%%)", log.first.c_str(), log.second, percentage);
        }
        ImGui::End();
    }

    // display the fps
    void FPSWindow()
    {
        ImGui::Begin("FPS", nullptr, ImGuiWindowFlags_NoMove);
        ImGui::Text("Game is running at %.1f FPS", ImGui::GetIO().Framerate);
        ImGui::End();
    }

    // display saving and loading of the scene option
    void FilesWindow() {
        GameObjectFactory& factory = GameObjectFactory::GetInstance();
        const std::unordered_map<int, GameObject*>& gameObjectMap = factory.GetAllGameObjects();
        (void)gameObjectMap;
        ImGui::Begin("Files");

#pragma region SaveScene
        static char saveFileName[256] = "GameScene";  // Default file path
        ImGui::InputText("Save", saveFileName, IM_ARRAYSIZE(saveFileName));
        std::string saveFilePath = "Assets/Lua/Scenes/";
        saveFilePath += saveFileName;
        saveFilePath += ".lua";
        ImGui::SameLine();

        // Buttons for other functionalities
        if (ImGui::Button("Save Scene")) {
            factory.SerializeAllGameObjects(saveFilePath);
        }
#pragma endregion SaveScene

#pragma region LoadScene

        //auto start = std::chrono::high_resolution_clock::now();
        // Initialize default values
        static char loadFileName[256] = "GameScene";  // Default path
        static std::string selectedScene = "GameScene";  // Set default selection to "Scene1"
        static int currentSceneIndex = 0;

        // Reload scene files every frame to reflect real-time changes
        std::vector<std::string> sceneFiles = Utilities::getSceneFiles("Assets/Lua/Scenes");

        // Check if current scene selection is still valid; if not, reset to "Scene1"
        bool sceneFound = false;
        for (int i = 0; i < sceneFiles.size(); ++i) {
            if (sceneFiles[i] == selectedScene) {
                currentSceneIndex = i;
                sceneFound = true;
                break;
            }
        }
        if (!sceneFound) {
            // Reset to default if selected scene is missing
            currentSceneIndex = 0;
            selectedScene = sceneFiles.empty() ? "" : sceneFiles[0];
        }

        // Dropdown to select scene file
        if (ImGui::BeginCombo("Load", selectedScene.c_str())) {
            for (int i = 0; i < sceneFiles.size(); ++i) {
                bool isSelected = (currentSceneIndex == i);
                if (ImGui::Selectable(sceneFiles[i].c_str(), isSelected)) {
                    currentSceneIndex = i;
                    selectedScene = sceneFiles[i];
                    snprintf(loadFileName, sizeof(loadFileName), "%s", sceneFiles[i].c_str());
                }
                if (isSelected) {
                    ImGui::SetItemDefaultFocus();
                }
            }
            ImGui::EndCombo();
        }

        std::string loadFilePath = "Assets/Lua/Scenes/" + selectedScene + ".lua";
        ImGui::SameLine();

        if (ImGui::Button("Load Scene")) {
            Engine::GetInstance().LoadSceneFromLua(loadFilePath);
        }

        //auto end = std::chrono::high_resolution_clock::now();
        //std::chrono::duration<double> elapsed = end - start;
        //ImGuiConsole::Cout("Elapsed RUN time: " << elapsed.count() << " seconds\n";

#pragma endregion LoadScene


//#pragma region Prefab
//        // Initialize default prefab file name
//        static std::string selectedPrefab = "Ame";  // Default selection
//        static int currentPrefabIndex = 0;
//
//        // Reload prefab files every frame to reflect real-time changes
//        std::vector<std::string> prefabFiles = Utilities::getSceneFiles("Assets/Lua/Prefabs");
//
//        // Check if the selected prefab is still valid; if not, reset to "Ame" or first in the list
//        bool prefabFound = false;
//        for (int i = 0; i < prefabFiles.size(); ++i) {
//            if (prefabFiles[i] == selectedPrefab) {
//                currentPrefabIndex = i;
//                prefabFound = true;
//                break;
//            }
//        }
//        if (!prefabFound) {
//            // Reset selection if the prefab file is missing
//            currentPrefabIndex = 0;
//            selectedPrefab = prefabFiles.empty() ? "" : prefabFiles[0];
//        }
//
//        // Dropdown to select prefab file
//        if (ImGui::BeginCombo("Prefab", selectedPrefab.c_str())) {
//            for (int i = 0; i < prefabFiles.size(); ++i) {
//                bool isSelected = (currentPrefabIndex == i);
//                if (ImGui::Selectable(prefabFiles[i].c_str(), isSelected)) {
//                    currentPrefabIndex = i;
//                    selectedPrefab = prefabFiles[i];
//                }
//                if (isSelected) {
//                    ImGui::SetItemDefaultFocus();
//                }
//            }
//            ImGui::EndCombo();
//        }
//
//        ImGui::SameLine();
//        static bool editPrefabMode = false;
//        if (ImGui::Button("Edit Prefab")) {
//            editPrefabMode = true;
//         
//
//            //save scene to 
//            factory.SerializeAllGameObjects("Assets/Lua/state.lua"); 
//            Engine::GetInstance().SetSelectedObject(nullptr);
//
//
//            //Spawn the prefab
//            if ((static_cast<int>(gameObjectMap.size()) + 1) < 3000) {
//                std::string spawnPrefab = "Assets/Lua/Prefabs/" + selectedPrefab + ".lua";
//                if (std::filesystem::exists(spawnPrefab)) {
//                    /*GameObjectFactory::GetInstance().CreateFromLua(spawnPrefab, selectedPrefab);*/\
//                        if (Engine::GetInstance().GetSelectedObject() == nullptr) {
//                            LuaUtilities::LoadSceneFromLua(spawnPrefab);
//                        
//                        }
//                        else {
//                            ImGuiConsole::Cout("THERE IS A SELECTED OBJECT!!!!!\n";
//                        }
//
//                }
//
//            }
//
//        }
//        if (editPrefabMode) {
//            if (ImGui::Button("Back")) {
//                GameObject* selectedPrefabGO = gameObjectMap.at(0);
//                // Save changes to the prefab file before clearing the scene
//                if (selectedPrefabGO != nullptr) {
//                    std::string prefabPathPrefix = "Assets/Lua/Prefabs/";
//                    std::string prefabPath = prefabPathPrefix + selectedPrefab + ".lua";
//
//                    factory.GetInstance().SerializeGameObjectHierarchy(prefabPath, selectedPrefabGO, selectedPrefabGO->GetChildren());
//                }
//
//                LuaUtilities::LoadSceneFromLua("Assets/Lua/state.lua");
//            }
//        }
//#pragma endregion Prefab

        ImGui::End();
    }

    // display the console window
    void ConsoleWindow() {
		ImGuiConsole::Render("Console");
    }

    // display the event window
    void EventWindow() {
        static float startTime = 0.0f;
        static int numEnemies = 0;
        static bool showEvents = false;
        static char eventsFileName[256] = "Assets/Lua/events.dat";
        static EnemyType selectedType = EnemyType::Light;
        static EventType selectedEvent = EventType::SpawnEnemies;
        static const char* enemyTypeStr[] = { "Light Enemy", "Heavy Enemy", "Bomb Enemy" };
        static const char* eventStr[] = { "Spawn Enemies", "Mission" };

        ImGui::Begin("Event Window");

        if (ImGui::BeginCombo("Enemy Type", enemyTypeStr[static_cast<int>(selectedType)])) {
            for (int i = 0; i < IM_ARRAYSIZE(enemyTypeStr); i++) {
                bool isSelected = (selectedType == static_cast<EnemyType>(i));
                if (ImGui::Selectable(enemyTypeStr[i], isSelected)) {
                    selectedType = static_cast<EnemyType>(i);
                }
                if (isSelected) {
                    ImGui::SetItemDefaultFocus();
                }
            }
            ImGui::EndCombo();
        }

        if (ImGui::BeginCombo("Event Type", eventStr[static_cast<int>(selectedEvent)])) {
            for (int i = 0; i < IM_ARRAYSIZE(eventStr); i++) {
                bool isSelected = (selectedEvent == static_cast<EventType>(i));
                if (ImGui::Selectable(eventStr[i], isSelected)) {
                    selectedEvent = static_cast<EventType>(i);
                }
                if (isSelected) {
                    ImGui::SetItemDefaultFocus();
                }
            }
            ImGui::EndCombo();
        }

        ImGui::InputInt("Num Enemies", &numEnemies, 1);

        if (selectedEvent == EventType::SpawnEnemies) {
            ImGui::InputFloat("Start Time", &startTime, 1.f, 10.f, "%.0f");
        }

        if (ImGui::Button("Add Event")) {
            if (selectedEvent == EventType::SpawnEnemies) {
                // Notice the order: number of enemies then start time.
                GameEventComponent event(selectedType, numEnemies, startTime);
                EventSystem::GetInstance().AddEvent(event);
            }
            else if (selectedEvent == EventType::Mission) {
                GameMissionComponent mission(selectedType, numEnemies);
                EventSystem::GetInstance().AddMission(mission);
            }
        }

        ImGui::InputText("Filename: ", eventsFileName, IM_ARRAYSIZE(eventsFileName));

        if (ImGui::Button("Save Events")) {
            EventSystem::GetInstance().SaveEvents(std::string(eventsFileName));
        }
        ImGui::SameLine();
        if (ImGui::Button("Load Events")) {
            EventSystem::GetInstance().LoadEvents(std::string(eventsFileName));
        }
        ImGui::SameLine();
        if (ImGui::Button("Clear Events")) {
            EventSystem::GetInstance().ClearEvents();
        }
        ImGui::SameLine();
        if (ImGui::Button("Clear Missions")) {
            EventSystem::GetInstance().ClearMissions();
        }

        if (ImGui::Button("List Events")) {
            showEvents = !showEvents;
        }

        if (showEvents) {
            ImGui::Separator();
            ImGui::Text("Current Events:");

            auto eventsCopy = EventSystem::GetInstance().GetEventQueue();
            auto missionsCopy = EventSystem::GetInstance().GetMissionVector();
            if (eventsCopy.empty()) {
                ImGui::Text("No events in queue.");
            }
            else {
                while (!eventsCopy.empty()) {
                    const auto& ev = eventsCopy.top();
                    ImGui::Text("SpawnEvent: Type: %d, Start Time: %.2f, Num Enemies: %d",
                        static_cast<int>(ev.enemyType), ev.startTime, ev.numEnemies);
                    eventsCopy.pop();
                }
            }
            if (missionsCopy.empty()) {
                ImGui::Text("No missions in queue.");
            }
            else {
                for (const auto& mission : missionsCopy) {
                    ImGui::Text("Hunt: Type: %d, Num Enemies: %d, Completed: %d",
                        static_cast<int>(mission.enemyType), mission.numEnemies, mission.isCompleted);
                }
            }
        }


        ImGui::End();
    }

}
#endif // _IMGUI

// Constructor for the Engine class.
Engine::Engine()
    : clearColor{ 0.f, 0.f, 0.f, 1.0f },
    cameraManager(-800.0f, 800.0f, -450.0f, 450.0f, //player cam parameters
                   -1600.0f,1600.0f,-900.0f, 900.0f) //editor cam parameters
{
#ifdef _IMGUI
    IMGUIManager::GetInstance();
    ContentBrowserPanel::GetInstance();
#endif // _IMGUI
    AudioManager::GetInstance();
    AssetManager::GetInstance();
}

// Destructor for the Engine class.
Engine::~Engine() {
    Graphics::GraphicsManager& gMan = Graphics::GraphicsManager::GetInstance();
	gMan.Shutdown();

}

// Returns the singleton instance of the Engine class.
Engine& Engine::GetInstance() {
    if (instance == nullptr) {
        instance = std::make_unique<Engine>();
    }
    return *instance;
}

// Initializes the engine with the specified width and height.
void Engine::Init(GLint width, GLint height) {
	LuaManager luaManager("Assets/Lua/config.lua");
    int TargetFrameRate = luaManager.LuaReadFromWindow<unsigned int>("TargetFramerate");
    if (TargetFrameRate >= 0) {
       glfwWindowHint(GLFW_REFRESH_RATE, TargetFrameRate);
	   fixedDT= 1.0 / TargetFrameRate;
	   fixedDeltaTimeMilli = fixedDT * 1000.0;
    }

    Graphics::GraphicsManager& gMan = Graphics::GraphicsManager::GetInstance();
    (void)gMan;
    (void)height;
    (void)width;

    TagManager::GetInstance().PreloadTags(); //Preload tags
    LayerManager::GetInstance().PreloadLayers(); //Preload layers

#ifdef _IMGUI
    InitializeGLWrappers();
    ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_DockingEnable;

    //gMan.Init();
    if (luaManager.LuaReadFromWindow<bool>("Fullscreen")) {
        // Get the primary monitor
        GLFWmonitor* primaryMonitor = glfwGetPrimaryMonitor();
        const GLFWvidmode* mode = glfwGetVideoMode(primaryMonitor);

        frameBuffer.Init(mode->width, mode->height);
    }
    else
        frameBuffer.Init(width, height);
#pragma region InitTextures


    //Extract filepaths from directory
    std::vector<std::string> holder = Utilities::getAssetFiles("Assets/Textures/");



    // Check if there�s a change in the number of files then enter the if loop
    if (holder.size() != previousFrameHolderSize) {

        std::vector<std::string> keys;
        LuaManager::LuaValueContainer values;
        LuaManager luaManagerTmp("Assets/Lua/textures.lua");
        luaManagerTmp.ClearLuaFile();

        // Update Lua 
        for (size_t i = 0; i < holder.size(); ++i) {

            std::string tableName = "Asset_" + std::to_string(i);

            // Extract the file path
            std::string filePath = holder[i];
            keys.push_back("SpritePathName");
            values.push_back(filePath);

            // Extract the file name
            std::string fileName = Utilities::extractFileName(holder[i]);
            keys.push_back("SpriteFileName");
            values.push_back(fileName);

            // Write to Lua with the correct file path and file name
            luaManagerTmp.LuaWrite(tableName, values, keys, "Texture");

            keys.clear();
            values.clear();  // Clear for the next table
        }


        int numOfTextures = luaManagerTmp.countTables();
        //AssetManager* assetManag = AssetManager::GetInstance();

        for (int i = 0; i < numOfTextures; ++i) {
            // Construct the table name dynamically
            std::string tableName = "Asset_" + std::to_string(i);

            try {
                // Read properties from the Lua table
                std::string pathName = luaManagerTmp.LuaRead<std::string>(tableName, { "Texture", "SpritePathName" });
                std::string fileName = luaManagerTmp.LuaRead<std::string>(tableName, { "Texture" , "SpriteFileName" });
                float animationFrame = 1.0f;
                float frameX = 1.0f;
                float frameY = 1.0f;

                AssetManager::GetInstance().LoadTexture(pathName, fileName, frameX, frameY, animationFrame);


            }
            catch (const std::exception& e) {
				ImGuiConsole::Cout("Error reading table %s: %s\n", tableName.c_str(), e.what());
            }
        }


        // Update previous holder size after writing to Lua
        previousFrameHolderSize = holder.size();
    }
#pragma endregion InitTextures


#pragma region InitSounds
    static size_t previousSoundHolderSize = 0;
    static std::vector<std::string> soundPrevholder = Utilities::getAssetFiles("Assets/sounds/");
    // Extract filepaths from directory
    std::vector<std::string> soundHolder = Utilities::getAssetFiles("Assets/sounds/");

    if (soundHolder.size() != previousSoundHolderSize) {
		ImGuiConsole::Cout("CHANGE DETECTED!!!");
        // Update asset changes if needed
        Utilities::UpdateSoundAssetChanges(soundHolder, soundPrevholder, previousSoundHolderSize);

        // Sync assets with Lua and load textures
        Utilities::SyncSoundAssetsWithLua(soundHolder);

        // Update the state for the next frame
        soundPrevholder = soundHolder;
        previousSoundHolderSize = holder.size();

    }
#pragma endregion InitSounds
#else

#endif // _IMGUI
    Utilities::LoadSoundAssetsWithLua("Assets/Lua/sounds.lua");
    Utilities::LoadTextureAssetsWithLua("Assets/Lua/textures.lua");


    TagManager::GetInstance().PreloadTags(); //Preload tags
    LayerManager::GetInstance().PreloadLayers(); //Preload layers
    GameObjectFactory& factory = GameObjectFactory::GetInstance();
    (void)factory;

#pragma region InitScene

    //Create from scene1
    std::string nameofFile = "Assets/Lua/Scenes/OpeningScene.lua";  // Use the inputted file path
    LoadSceneFromLua(nameofFile);
    //Utilities::SpawnGrassObjects(factory, 20);
#pragma endregion InitScene
#ifdef _IMGUI
#else
#endif

}

// Updates the engine (e.g., handles input, updates game logic).
void Engine::Update() {

    if (!showCursor) {
        glfwSetInputMode(InputManager::ptrWindow, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);

    }
    else {
        glfwSetInputMode(InputManager::ptrWindow, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    }

    if (videoFinish)
    {
        EventSystem::GetInstance().ShutDown();
        videoFinish = false;
        LoadSceneFromLua("Assets/Lua/Scenes/GameScene.lua");

        isInGameScene = true;

        if (isInGameScene) {
            isPaused = false;
            time = maxTime;

            for (GameObject* player : GameObjectFactory::GetInstance().FindGameObjectsByTag("Player")) {
                auto* playController = player->GetComponent<PlayerControllerComponent>(TypeOfComponent::PLAYER);

                if (playController) {
                    playController->playBGM = false;
                    playController->changeBGM = false;
                }
            }
        }
    }
    else if (openingFinished)
    {
        openingFinished = false; //MUST PUT ELSE IT KEEPS LOADING
        showCursor = true;
        variables::isRunningBack = false;
        variables::isInteractable = true;
        AudioManager::GetInstance().PlayAudio(16);
        LoadSceneFromLua("Assets/Lua/Scenes/MainMenuScene.lua");
    }
    else if (fadeIntoCutScene)
    {
        fadeIntoCutScene = false;
        LoadSceneFromLua("Assets/Lua/Scenes/CutScene.lua");
    }
    
    AudioManager::GetInstance().Update();
	UpdateFixedTimeStep(glfwGetTime());

    // In your game's update loop
    DespawnManager::GetInstance().Update();


    static std::unordered_map<GameObject*, Vector2> enemyStoredVelocities;

    if (isInGameScene && InputManager::IsKeyReleased(GLFW_KEY_ESCAPE))
    {
        isPaused = ~isPaused;

        for (auto& [id, object] : GameObjectFactory::GetInstance().GetAllGameObjects())
        {
            if (!object) continue;

            AIStateMachineComponent* ai = object->GetComponent<AIStateMachineComponent>(TypeOfComponent::AISTATE);
            RigidBodyComponent* rb = object->GetComponent<RigidBodyComponent>(TypeOfComponent::RIGIDBODY);

            if (ai && rb)
            {
                if (isPaused)
                {
                    // Save current velocity and zero it out
                    enemyStoredVelocities[object] = rb->GetVelocity();
                    rb->SetVelocity(Vector2(0.f, 0.f));
                }
                else
                {
                    // Restore stored velocity
                    if (enemyStoredVelocities.find(object) != enemyStoredVelocities.end())
                    {
                        rb->SetVelocity(enemyStoredVelocities[object]);
                    }
                }
            }
        }

        if (!isPaused)
        {
            enemyStoredVelocities.clear(); // Clear memory when resuming
        }
    }

    GameObject* player = GameObjectFactory::GetInstance().GetPlayerObject();
    if (player) {

        if (InputManager::IsKeyPressed(GLFW_KEY_F9))
        {
            EventSystem::GetInstance().SetWinCondition(true);
        }
        if (InputManager::IsKeyPressed(GLFW_KEY_F10))
        {
            time = 0.f;
        }

        if (InputManager::IsKeyReleased(GLFW_KEY_ESCAPE))
        {
            showCursor = ~showCursor;
        }
        Utilities::CheatCodes();

        auto* healthComponent = player->GetComponent<HealthComponent>(TypeOfComponent::HEALTH);

        if (healthComponent)
        {
            if (player->GetComponent<HealthComponent>(TypeOfComponent::HEALTH)->GetHealth() <= 0) {
                showCursor = true;
                cameraManager.GetCurrentCamera()->SetCenter(Vector2(0.0f, 0.0f));
                EventSystem::GetInstance().ShutDown();
                LoadSceneFromLua("Assets/Lua/Scenes/GameOverScene.lua");
            }
        }

        if (time <= 0)
        {
            showCursor = true;
            cameraManager.GetCurrentCamera()->SetCenter(Vector2(0.0f, 0.0f));
            EventSystem::GetInstance().ShutDown();
            LoadSceneFromLua("Assets/Lua/Scenes/GameOverScene.lua");
        }
    }

	if (EventSystem::GetInstance().GetWinCondition())
	{
        showCursor = true;
		cameraManager.GetCurrentCamera()->SetCenter(Vector2(0.0f, 0.0f));
		EventSystem::GetInstance().ShutDown();
        LoadSceneFromLua("Assets/Lua/Scenes/VictoryScene.lua");
	}
#ifdef _IMGUI
    
#pragma region updateTextureluainRunTime

    static std::vector<std::string> prevholder = Utilities::getAssetFiles("Assets/Textures/");
    // Extract filepaths from directory
    std::vector<std::string> holder = Utilities::getAssetFiles("Assets/Textures/");

    if (holder.size() != previousFrameHolderSize) {
		ImGuiConsole::Cout("CHANGE DETECTED!!!");
        // Update asset changes if needed
        Utilities::UpdateTextureAssetChanges(holder, prevholder, previousFrameHolderSize);

        // Sync assets with Lua and load textures
        Utilities::SyncTextureAssetsWithLua(holder);

        // Update the state for the next frame
        prevholder = holder;
        previousFrameHolderSize = holder.size();

    }
#pragma endregion updateTextureluainRunTime



#pragma region updateSoundluaInRunTime
    static size_t previousSoundHolderSize = 0;
    static std::vector<std::string> soundPrevholder = Utilities::getAssetFiles("Assets/sounds/");
    // Extract filepaths from directory
    std::vector<std::string> soundHolder = Utilities::getAssetFiles("Assets/sounds/");

    if (soundHolder.size() != previousSoundHolderSize) {
		ImGuiConsole::Cout("CHANGE DETECTED!!!");
        // Update asset changes if needed
        Utilities::UpdateSoundAssetChanges(soundHolder, soundPrevholder, previousSoundHolderSize);

        // Sync assets with Lua and load textures
        Utilities::SyncSoundAssetsWithLua(soundHolder);

        // Update the state for the next frame
        soundPrevholder = soundHolder;
        previousSoundHolderSize = soundHolder.size();

    }

#pragma endregion updateSoundluaInRunTime





#endif // _IMGUI

    // Get the GLFW window
    GLFWwindow* window = InputManager::ptrWindow;
    (void)window;
    // Call the camera manager to handle input 
    // (Ridhwan: commented due to leveleditor camera enabling when game is paused or stopped)
    //cameraManager.HandleCameraToggleInput(window);


#ifdef _LOGGING
    std::unique_ptr<SystemLog> logMovement = std::make_unique<SystemLog>("Game Object Update System");
#endif
    // Player Movement
    GameObjectFactory& factory = GameObjectFactory::GetInstance();
    factory.UpdateAllGameObjects();

#ifdef _LOGGING
    logMovement.reset();
    std::unique_ptr<SystemLog> logCollision = std::make_unique<SystemLog>("Collision System");
#endif 

    if (isInGameScene && !isPaused)
        CollisionUpdate();

#ifdef _LOGGING
    logCollision.reset();
    std::unique_ptr<SystemLog> logPhysics = std::make_unique<SystemLog>("Physics System");
#endif
       physicsSystem.Update();

#ifdef _LOGGING
    logPhysics.reset();
#endif
    //playerRigidBody->SetVelocity(velocity);
    //playerRigidBody->Update();// called in physics system


#ifdef _LOGGING
    std::unique_ptr<SystemLog> logCamera = std::make_unique<SystemLog>("Camera System");
    //float zoomSpeed = 1.0f * static_cast<float>(InputManager::deltaTime);
    //float rotateSpeed = 50.0f * static_cast<float>(InputManager::deltaTime);
    float zoomSpeed = 1.0f * static_cast<float>(fixedDT * currentNumberOfSteps);
    float rotateSpeed = 50.0f * static_cast<float>(fixedDT * currentNumberOfSteps);
#endif

    // PLAYER CAMERA
    if (cameraManager.GetCurrentMode() == CameraManager::CameraMode::PlayerCamera) {
        // Retrieve the player object using the new helper function
        GameObject* playerObject = GameObjectFactory::GetInstance().GetPlayerObject();

        if (playerObject) {
            // Ensure the player has a TransformComponent before attempting to get position
            const auto* transformComponent = playerObject->GetComponent<TransformComponent>(TypeOfComponent::TRANSFORM);
            if (transformComponent) {
                //const auto& playerPosition = transformComponent->GetPosition();
                //cameraManager.GetPlayerCamera().HandleShake();
                // Update the player camera to bind it to the player's position
                cameraManager.GetPlayerCamera().Update();
            }
            else {
#ifdef _LOGGING
                // Handle case where the player doesn't have a transform component (if needed)
                ImGuiConsole::Cout("Player object does not have a TransformComponent.");
#endif //_LOGGING
            }
        }
        else {
            // Handle case where player object is null (if needed)
#ifdef _LOGGING
            //ImGuiConsole::Cout("Player object not found!");
#endif //_LOGGING
            //switch to another mode
            cameraManager.SetCameraMode(CameraManager::CameraMode::PlayerCamera);
        }
    }

#ifdef _IMGUI
    //EDITOR CAMERA
    else if (cameraManager.GetCurrentMode() == CameraManager::CameraMode::EditorCamera) {
        static bool isDragging = false;
        static double lastMouseX = 0.0, lastMouseY = 0.0;

        // Get the current mouse position
        double mouseX, mouseY;
        InputManager::GetMousePosition(mouseX, mouseY);
		// cameraManager.GetEditorCamera().HandleMovement(static_cast<float>(InputManager::deltaTime));
        cameraManager.GetEditorCamera().HandleMovement(static_cast<float>(fixedDT* currentNumberOfSteps));
        cameraManager.GetEditorCamera().HandleZoom(zoomSpeed);
        cameraManager.GetEditorCamera().HandleRotation(rotateSpeed);
        cameraManager.GetEditorCamera().HandleReset();
        //cameraManager.GetEditorCamera().HandleShake();

        if (InputManager::IsMouseButtonPressed(GLFW_MOUSE_BUTTON_MIDDLE)) {
            if (!isDragging) {
                // Begin dragging
                isDragging = true;
                lastMouseX = mouseX;
                lastMouseY = mouseY;
            }

            // Handle mouse drag movement
            cameraManager.GetEditorCamera().HandleMouseDrag(mouseX, mouseY, lastMouseX, lastMouseY, isDragging);
        }
        else {
            // Stop dragging
            isDragging = false;
        }


        // Update the debug camera's view matrix
		// cameraManager.GetEditorCamera().UpdateViewMatrix(static_cast<float>(InputManager::deltaTime));
        cameraManager.GetEditorCamera().UpdateViewMatrix(static_cast<float>(fixedDT * currentNumberOfSteps));
    }
#endif // _IMGUI
#ifdef _LOGGING
    logCamera.reset();
#endif
	// UI will be updated regardless of puase state
    UpdateUI();
#ifdef PARTICLES
   
#endif // PARTICLES
}

// Draws the current scene.
void Engine::Draw() {


#ifdef _LOGGING
    std::unique_ptr<SystemLog> logGraphics = std::make_unique<SystemLog>("Graphics Render System");
#endif // _LOGGING
    Engine& engine = Engine::GetInstance();
    Graphics::GraphicsManager& gMan = Graphics::GraphicsManager::GetInstance();
    glClearColor(engine.clearColor[0], engine.clearColor[1], engine.clearColor[2], engine.clearColor[3]);

#ifdef _IMGUI
    // Bind the framebuffer
    frameBuffer.Bind();
#endif // _IMGUI


    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    gMan.Render();

#ifdef _IMGUI
    // Unbind the framebuffer
    frameBuffer.Unbind();

#ifdef _LOGGING
    logGraphics.reset();
    std::unique_ptr<SystemLog> logUI = std::make_unique<SystemLog>("UI System");
#endif

    IMGUIManager& imguiManager = IMGUIManager::GetInstance();
    imguiManager.BeginFrame();
    ImGui::DockSpaceOverViewport(ImGui::GetMainViewport()->ID);

#ifdef IMGUI
    EngineImGuiWindows::ShowLayerManagerWindow();
    EngineImGuiWindows::ShowTagManagerWindow();
    EngineImGuiWindows::GameObjectListWindow();
    if (PlayerSceneControls::GetInstance().GetSelectedGameObject() != nullptr)
        selectedObject = PlayerSceneControls::GetInstance().GetSelectedGameObject();

    EngineImGuiWindows::SelectedGOComponentWindow(selectedObject);

    //ImGui::End();
#endif // GAMEOBJECT_COMPONENTS

#ifdef TEST_CONTENTBROWSER
    ContentBrowserPanel& contentbowser = ContentBrowserPanel::GetInstance();
    contentbowser.OnImGuiRender();

   
#endif

   

#ifdef TEST_SOUND
    EngineImGuiWindows::SceneWindow();
    EngineImGuiWindows::ControlPanel();
    EngineImGuiWindows::AssetsAndDebugWindow();
    EngineImGuiWindows::GizmoConfigurationsWindow();
    EngineImGuiWindows::FPSWindow();
    EngineImGuiWindows::SystemProcessTimeWindow();
    EngineImGuiWindows::FilesWindow();
    EngineImGuiWindows::ConsoleWindow();
	EngineImGuiWindows::EventWindow();

    RenderAnimationControllerUI(controller);

    PlayerSceneControls().GetInstance().Update();


#endif // TEST_SOUND


#ifdef _LOGGING

#endif
    imguiManager.EndFrame();

#ifdef _LOGGING
    logUI.reset();
#endif

#endif // _IMGUI


#ifdef PARTICLES    
    
#endif // PARTICLES
}

#ifdef _IMGUI
/*!****************************************************************
\func Engine::SaveStateToLua
\brief Save the current state of the game to a Lua file
*******************************************************************/
void Engine::SaveStateToLua()
{
	GameObjectFactory& factory = GameObjectFactory::GetInstance();
    factory.SerializeAllGameObjects(stateFile);
}


/*!****************************************************************
\func Engine::LoadStateFromLua
\brief Load the state of the game from a Lua file
*******************************************************************/
void Engine::LoadStateFromLua()
{
    originalScene = currentScene;
    std::string nameofFile = stateFile;  // Use the state file path
    LoadSceneFromLua(nameofFile);
}

/*!****************************************************************
\func Engine::GetMousePositionImGui
\brief Get the mouse position in ImGui coordinates
*******************************************************************/
ImVec2 Engine::GetMousePositionImGui(ImGuiWindow* window) {
    if (window == nullptr) {
        return ImVec2(-1.0f, -1.0f);
    }
    if (window->WasActive == false) {
        return ImVec2(-1.0f, -1.0f);
    }

    // Retrieve the current mouse position in screen coordinates
    ImVec2 mousePos = ImGui::GetMousePos();
    // Get the position of the content region's top-left corner in screen space
    ImVec2 contentPos = window->Pos;
    // Get the available size of the content region
    ImVec2 availableSize = window->Size;
    //ImGuiConsole::Cout("Avail PosX: " << availableSize.x << " , Y: " << availableSize.y);

    // Calculate the mouse position relative to the content region
    ImVec2 mousePosInContentRegion = ImVec2(mousePos.x - contentPos.x, mousePos.y - contentPos.y);


    // Check if the mouse is within the content region bounds
    if (mousePosInContentRegion.x < 0 || mousePosInContentRegion.x > Engine::GetInstance().availableSizeEngine.x ||
        mousePosInContentRegion.y < 0 || mousePosInContentRegion.y >  Engine::GetInstance().availableSizeEngine.y) {
        return ImVec2(-1.0f, -1.0f);
    }



    return mousePosInContentRegion; // Return the calculated position
}

/*!****************************************************************
\func Engine::MouseToScreenImGui
\brief Convert world coordinates to screen coordinates in ImGui
       centered at origin
*******************************************************************/
ImVec2 Engine::MouseToScreenImGui(const ImVec2& mousePos, const Camera& camera, float screenWidth, float screenHeight)
{
    (void)screenWidth;
    (void)screenHeight;
    // Get the center of the camera
    Vector2 cameraCenter{ 0, 0 };

    // Get the viewing range and pre-calculate half values
    Vector2 viewingRange = camera.GetViewingRange();
    float halfViewingRangeX = viewingRange.x * 0.5f;
    float halfViewingRangeY = viewingRange.y * 0.5f;

    // Calculate world coordinates based on the mouse position
    float normalizedX = (mousePos.x / availableSizeEngine.x) * viewingRange.x - halfViewingRangeX + (cameraCenter.x * 0.5f);
    float normalizedY = (1.0f - (mousePos.y / availableSizeEngine.y)) * viewingRange.y - halfViewingRangeY + (cameraCenter.y * 0.5f);


    //if (InputManager::IsMouseButtonPressed(0))
    //    std::cout << "Offset Position: (" << normalizedX * 2.0f << ", " << normalizedY * 2.0f <<  ")" << std::endl;

    return { normalizedX * 2.0f, (normalizedY + 30.f) * 2.0f };
}

#endif // _IMGUI

/*!****************************************************************
\func Engine::LoadSceneFromLua
\brief Load a scene from a Lua file
\param path - the file path of the Lua file
*******************************************************************/
void Engine::LoadSceneFromLua(const std::string path) {

    variables::isInteractable = true;
    variables::volumeMenuOn = false;

    auto start = std::chrono::high_resolution_clock::now();

  if (currentScene == path) {
        RestartScene();
        return;
    }
    else
    {
        LayerManager::GetInstance().ClearGameObjectLayerData();
    }
    
    if (path == "Assets/Lua/Scenes/VictoryScene.lua" || path == "Assets/Lua/Scenes/GameOverScene.lua")
    {
        AudioManager::GetInstance().PlayAudio(16);
    }
    currentScene = path;


    try {
        GameObjectFactory& factory = GameObjectFactory::GetInstance();

        // Clear existing game objects
        factory.Clear();

        LuaManager luaManager(path);
        // Extract id-name map along with parent IDs
        std::unordered_map<int, std::pair<std::string, int>> objectData = luaManager.extractNamesWithParentIDs();

        // Collect and sort IDs
        std::vector<int> sortedIds(objectData.size());
        std::transform(objectData.begin(), objectData.end(), sortedIds.begin(), [](const auto& pair) {
            return pair.first;
            });
        std::sort(sortedIds.begin(), sortedIds.end());

        // Create GameObjects from Lua data
        std::unordered_map<int, GameObject*> createdObjects;
        for (const int objectId : sortedIds) {
            const auto& [objectName, parentID] = objectData[objectId];
            GameObject* newObject = factory.CreateFromLua(path, objectName);
            createdObjects[objectId] = newObject;
        }

        // Link child objects to their parents
        for (const int objectId : sortedIds) {
            const auto& [objectName, parentID] = objectData[objectId];

            if (parentID != -1) { // -1 indicates no parent
                auto childIt = createdObjects.find(objectId);
                auto parentIt = createdObjects.find(parentID);
                if (childIt != createdObjects.end() && parentIt != createdObjects.end()) {
                    GameObject* child = childIt->second;
                    GameObject* parent = parentIt->second;

                    child->isDeserializing = true;
                    //child->SetParent(parent); // Use SetParent to establish hierarchy
                    child->SetParent(parent); // Use SetParent to establish hierarchy
                    child->isDeserializing = false;
                }
                else {
                    ImGuiConsole::Cout("Error: Could not find parent or child for ID: %d", objectId);
                }
            }
        }

        // Update all game objects
        factory.UpdateAllGameObjects();


        // Check for PlayerControllerComponent and set camera mode
        bool playerFound = false;
        for (const auto& [id, obj] : factory.GetAllGameObjects()) {
            if (obj->GetComponent<PlayerControllerComponent>(TypeOfComponent::PLAYER) != nullptr) {
#ifndef _IMGUI
                cameraManager.SetCameraMode(CameraManager::CameraMode::PlayerCamera);
                isInGameScene = true;
#endif
                playerFound = true;
                break;
            }
        }
#ifndef _IMGUI
        if (!playerFound) {
            isInGameScene = false;
        }
#endif
    }
    catch (const std::exception& e) {
        ImGuiConsole::Cout("Error loading scene: %s\n", e.what());
    }


    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed = end - start;
    ImGuiConsole::Cout("Elapsed time: %f seconds\n", elapsed.count());
    time = maxTime;
    // Hardcoded event loading for GameScene
    if (path == "Assets/Lua/Scenes/GameScene.lua")
    {
        EventSystem::GetInstance().LoadEvents("Assets/Lua/events.dat");
    }

}





/*!****************************************************************
* \func Engine::RestartScene
* \brief Restart the current scene
*******************************************************************/
void Engine::RestartScene() {
    LuaManager luaManager(currentScene);
    GameObjectFactory& factory = GameObjectFactory::GetInstance();

    // Extract id-name map along with parent IDs
    std::unordered_map<int, std::pair<std::string, int>> objectData = luaManager.extractNamesWithParentIDs();

    // Collect and sort IDs
    std::vector<int> sortedIds;
    for (const auto& pair : objectData) {
        sortedIds.push_back(pair.first);
    }
    std::sort(sortedIds.begin(), sortedIds.end());

    // Create GameObjects from Lua data
    std::unordered_map<int, GameObject*> createdObjects;
    for (const int objectId : sortedIds) {
        const auto& [tableName, parentID] = objectData[objectId];
        GameObject* existingObject = factory.GetObjectByID(objectId);
        const std::string objectName = luaManager.LuaReadFromName<std::string>(tableName, "name");

        if (existingObject && existingObject->GetName() == objectName) {
            factory.ResetObjectFromLua(currentScene, tableName, existingObject);
            createdObjects[objectId] = existingObject;
        }
        else {
            GameObject* newObject = factory.CreateFromLua(currentScene, tableName);
            createdObjects[objectId] = newObject;
        }
    }

    // Link child objects to their parents
    for (const int objectId : sortedIds) {
        const auto& [tableName, parentID] = objectData[objectId];

        if (parentID != -1) { // -1 indicates no parent
            GameObject* child = createdObjects[objectId];
            GameObject* parent = createdObjects[parentID];
            if (child && parent) {
                child->isDeserializing = true;
                child->SetParent(parent); // Use SetParent to establish hierarchy
                child->isDeserializing = false;
            }
            else {
				ImGuiConsole::Cout("Error: Could not find parent or child for ID: %d", objectId);
            }
        }
    }

    // Delete any extra objects that exist in the current level but not in the scene read from Lua
    auto allGameObjects = factory.GetAllGameObjects();
    for (auto& [id, obj] : allGameObjects) {
        if (createdObjects.find(id) == createdObjects.end()) {
            factory.Despawn(obj);
        }
    }

    // Update all game objects
    factory.UpdateAllGameObjects();

    // Loop through all GameObjects in the factory
    // MIGHT NEED TO UPDATE AFTER SHI HENG MERGE
    for (auto& [id, obj] : factory.GetAllGameObjects()) {
        if (obj->GetComponent<PlayerControllerComponent>(TypeOfComponent::PLAYER) != nullptr) {
#ifndef _IMGUI
            cameraManager.SetCameraMode(CameraManager::CameraMode::PlayerCamera);
            isInGameScene = true;
#endif // !_IMGUI
            break;  // Exit the loop once the player is found
        }
#ifndef _IMGUI
        else {
            isInGameScene = false;
        }
#endif // !_IMGUI
    }
}


/*!****************************************************************
\func Engine::Exit
\brief Exit the application
*******************************************************************/
void Engine::Exit() {
    EventSystem::GetInstance().ShutDown();
	glfwSetWindowShouldClose(InputManager::ptrWindow, GLFW_TRUE);
}

/*!****************************************************************
\func Engine::MouseToScreen
\brief Convert world coordinates to screen coordinates
*******************************************************************/
Vector2 Engine::MouseToScreen(const Vector2& mousePos, const Camera& camera, float screenWidth, float screenHeight)
{
    // Get the center of the camera
    Vector2 cameraCenter = { 0.f, 0.f };

    // Get the viewing range and pre-calculate half values
    Vector2 viewingRange = camera.GetViewingRange();
    float halfViewingRangeX = viewingRange.x * 0.5f;
    float halfViewingRangeY = viewingRange.y * 0.5f;

    // Calculate world coordinates based on the mouse position
    float normalizedX = (mousePos.x / screenWidth) * viewingRange.x - halfViewingRangeX + (cameraCenter.x * 0.5f);
    float normalizedY = (1.0f - (mousePos.y / screenHeight)) * viewingRange.y - halfViewingRangeY + (cameraCenter.y * 0.5f);

    return { normalizedX * 2.0f, normalizedY * 2.0f };
}


/*!****************************************************************
\func: Engine::UpdateFixedTimeStep
\param: double currentTime
\brief: Update the fixed time step variables (currentNumberOfSteps)
*******************************************************************/
void Engine::UpdateFixedTimeStep(double currentTime)
{
    // Update Fixed Time Step
    currentNumberOfSteps = 0;
    double deltaTime = currentTime - oldTime;
    oldTime = currentTime;
    accumulatedTime += deltaTime;
    while (accumulatedTime >= fixedDT) {
        accumulatedTime -= fixedDT;
        currentNumberOfSteps++;
    }
}