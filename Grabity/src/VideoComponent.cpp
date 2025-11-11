/*!****************************************************************
\file: VideoComponent.cpp
\author: Johny Yong Jun Siang, j.yong, 2301301

\brief
    This source file implements the VideoComponent class, which
    updates the SpriteComponent to change textures at a fixed interval.

Copyright (C) 2025 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents
without the prior written consent of DigiPen Institute of
Technology is prohibited.
*******************************************************************/

#include "VideoComponent.h"
#include "GameObject.h"
#include "Time.h"  // Assumed to provide delta time
#include "SpriteComponent.h"
#include "AssetManager.h"
#include <iostream>
#include "engine.h"
#include "UISystem.h"

/*!****************************************************************
\brief
    Default constructor for VideoComponent.
*******************************************************************/
VideoComponent::VideoComponent() : Component(nullptr), elapsedTime(0.0f), currentScene(1), switchInterval(2.f), totalScenes(16) {
}

/*!****************************************************************
\brief
    Constructor that assigns the video component to a parent GameObject.
\param parent
    Pointer to the parent GameObject.
*******************************************************************/
VideoComponent::VideoComponent(GameObject* parent)
    : Component(parent), elapsedTime(0.0f), currentScene(1), switchInterval(2.f), totalScenes(16) {
}

VideoComponent::~VideoComponent()
{
    elapsedTime = 0.0f;
    currentScene = 1;
    previousScene = -1;
}

/*!****************************************************************
\brief
    Updates the sprite at a fixed time interval.
*******************************************************************/
void VideoComponent::Update() {
    float deltaTime = (float)InputManager::GetDeltaTime();
    auto* parent = GetParentGameObject();
    if (!parent) return;


    if (parent->GetTag() == "CutScene") {
        //if (!AudioManager::GetInstance().IsPlaying(18))
        //{
        //    AudioManager::GetInstance().PlayAudio(18);
        //}

        static bool isFadingOut = false;
        static float fadeTimer = 0.0f;
        static bool queuedSceneSwitch = false;
        static bool firstFrame = true;

        if (previousScene != currentScene) {
            elapsedTime = 0.0f;       // Reset timing when scene changes
            firstFrame = true;        // Flag to delay switching
            previousScene = currentScene;
        }
        // Trigger early fade-out when holding CTRL
        if (InputManager::IsKeyDown(GLFW_KEY_LEFT_CONTROL) && !isFadingOut) {
            isFadingOut = true;
            fadeTimer = 0.0f;
            queuedSceneSwitch = false; // We don't continue the video
            AudioManager::GetInstance().FadeOutAudio(AudioManager::GetInstance().bgmChannel, 1.f);
            return;
        }

        switchInterval = (currentScene == 6) ? 1.0f : 3.0f;
        if (totalScenes <= 0) totalScenes = 1;

        auto* spriteComponent = parent->GetComponent<SpriteComponent>(TypeOfComponent::SPRITE);
        if (!spriteComponent) return;

        if (firstFrame) {
            firstFrame = false;
            return;
        }

        // --- Handle fade-out ---
        if (isFadingOut) {
            fadeTimer += deltaTime;
            float alpha = std::max(1.0f - fadeTimer / 1.0f, 0.0f);

            // Fade out main sprite
            Vector4 color = spriteComponent->GetRGB();
            color.w = alpha;
            spriteComponent->SetRGB(color);

            // Fade out FastForwardInfo sprites
            std::vector<GameObject*> ffObjects = GameObjectFactory::GetInstance().FindGameObjectsByTag("FastForwardInfo");
            for (GameObject* obj : ffObjects) {
                if (!obj) continue;
                SpriteComponent* ffSprite = obj->GetComponent<SpriteComponent>(TypeOfComponent::SPRITE);
                if (ffSprite) {
                    Vector4 ffColor = ffSprite->GetRGB();
                    ffColor.w = alpha;
                    ffSprite->SetRGB(ffColor);
                }
            }

            if (fadeTimer >= 1.0f) {
                if (queuedSceneSwitch) {
                    currentScene = (currentScene % totalScenes) + 1;
                    std::string sceneName = "Scene" + std::to_string(currentScene);
                    spriteComponent->ChangeSprite(std::make_unique<SpriteAnimation>(AssetManager::GetInstance().GetSprite(sceneName)));
                    std::cout << "Switched (after fade-out) to: " << sceneName << std::endl;

                    Vector4 colorReset = spriteComponent->GetRGB();
                    colorReset.w = 1.0f;
                    spriteComponent->SetRGB(colorReset);
                }
                else {
                    Engine::GetInstance().videoFinish = true;
                }

                isFadingOut = false;
                queuedSceneSwitch = false;
                fadeTimer = 0.0f;
            }
            return;
        }

        // Reset fast forward info alpha every frame (in case scene switched)
        std::vector<GameObject*> ffObjects = GameObjectFactory::GetInstance().FindGameObjectsByTag("FastForwardInfo");
        for (GameObject* obj : ffObjects) {
            if (!obj) continue;
            SpriteComponent* ffSprite = obj->GetComponent<SpriteComponent>(TypeOfComponent::SPRITE);
            if (ffSprite) {
                Vector4 color = ffSprite->GetRGB();
                color.w = 1.0f;
                ffSprite->SetRGB(color);
            }
        }

        // --- Handle normal scene switching ---
        elapsedTime += deltaTime;
        if (elapsedTime >= switchInterval) {
            elapsedTime = 0.0f;

            if (currentScene == totalScenes) {
                if (!isFadingOut) {
                    isFadingOut = true;
                    fadeTimer = 0.0f;
                    queuedSceneSwitch = false;
                    AudioManager::GetInstance().FadeOutAudio(AudioManager::GetInstance().bgmChannel, 1.f);
                }
                return;
            }

            if (currentScene == 7 || currentScene == 14) {
                isFadingOut = true;
                fadeTimer = 0.0f;
                queuedSceneSwitch = true;
                return;
            }

            currentScene = (currentScene % totalScenes) + 1;
            std::string sceneName = "Scene" + std::to_string(currentScene);
            spriteComponent->ChangeSprite(std::make_unique<SpriteAnimation>(AssetManager::GetInstance().GetSprite(sceneName)));
            std::cout << "Switched to: " << sceneName << std::endl;
        }

    }
    else if (parent->GetTag() == "FadeAble" && parent->GetName() != "FadeObject") {
        static float fadeTimer = 0.0f;
        static float holdTimer = 0.0f;
        static bool fadingIn = true;
        static bool holding = false;
        static bool fadingOut = false;

        auto* spriteComponent = GetParentGameObject()->GetComponent<SpriteComponent>(TypeOfComponent::SPRITE);
        if (!spriteComponent) return;

        if (fadingIn) {
            fadeTimer += deltaTime;
            float alpha = std::min(fadeTimer / 2.0f, 1.0f);
            Vector4 color = spriteComponent->GetRGB();
            color.w = alpha;
            spriteComponent->SetRGB(color);

            if (alpha >= 1.0f) {
                fadingIn = false;
                holding = true;
                fadeTimer = 0.0f;
            }
            return;
        }

        if (holding) {
            holdTimer += deltaTime;
            if (holdTimer >= 2.0f) {
                holding = false;
                fadingOut = true;
                fadeTimer = 0.0f;
            }
            return;
        }

        if (fadingOut) {
            fadeTimer += deltaTime;
            float alpha = std::max(1.0f - (fadeTimer / 2.0f), 0.0f);
            Vector4 color = spriteComponent->GetRGB();
            color.w = alpha;
            spriteComponent->SetRGB(color);

            if (alpha <= 0.0f) {
                Engine::GetInstance().openingFinished = true;
                fadingIn = true;
                holding = false;
                fadingOut = false;
                fadeTimer = 0.0f;
                holdTimer = 0.0f;
            }
            return;
        }
    }
    else if (parent->GetTag() == "FadeAble" && parent->GetName() == "FadeObject") {
        auto* uiSprite = parent->GetComponent<UISpriteComponent>(TypeOfComponent::SPRITE_UI);
        auto* transform = parent->GetComponent<TransformComponent>(TypeOfComponent::TRANSFORM);
        static float fadeTimer = 0.0f;
        static bool fadeComplete = false;

        if (variables::runFadeIntoCutscene && uiSprite && transform && !fadeComplete) {
            Vector4 currentColor = uiSprite->GetColor();
            currentColor.w = 0.0f;
            uiSprite->SetColor(currentColor);
            transform->SetLocalPosition(Vector2(0.0f, 0.0f));

            fadeTimer += deltaTime;
            float alpha = std::min(fadeTimer / 1.0f, 1.0f);
            currentColor.w = alpha;
            uiSprite->SetColor(currentColor);

            if (alpha >= 1.0f) {
                fadeComplete = true;
                variables::runFadeIntoCutscene = false;
                Engine::GetInstance().fadeIntoCutScene = true;
            }
        }

        if (!variables::runFadeIntoCutscene) {
            fadeTimer = 0.0f;
            fadeComplete = false;
        }
    }
}

/*!****************************************************************
\brief
    Serializes the video component's data to a Lua file.
\param luaFilePath
    Path to the Lua script file.
\param tableName
    Name of the Lua table where the data will be stored.
*******************************************************************/
void VideoComponent::Serialize(const std::string& luaFilePath, const std::string& tableName) {
    LuaManager luaManager(luaFilePath);

    // Define the keys and values to store
    std::vector<std::string> keys = { "elapsedTime", "currentScene", "switchInterval", "totalScenes" };
    LuaManager::LuaValueContainer values = { elapsedTime, currentScene, switchInterval, totalScenes };

    // Write data to Lua
    luaManager.LuaWrite(tableName, values, keys, "Video");
}

/*!****************************************************************
\brief
    Deserializes the video component's data from a Lua file.
\param luaFilePath
    Path to the Lua script file.
\param tableName
    Name of the Lua table from which the data will be loaded.
*******************************************************************/
void VideoComponent::Deserialize(const std::string& luaFilePath, const std::string& tableName) {
    LuaManager luaManager(luaFilePath);

    // Read stored data from Lua
    elapsedTime = luaManager.LuaRead<float>(tableName, { "Video", "elapsedTime" });
    currentScene = luaManager.LuaRead<int>(tableName, { "Video", "currentScene" });
    switchInterval = luaManager.LuaRead<float>(tableName, { "Video", "switchInterval" });
    totalScenes = luaManager.LuaRead<int>(tableName, { "Video", "totalScenes" });

    // Ensure totalScenes is never 0
    if (totalScenes <= 0) {
        ImGuiConsole::Cout("Warning: Deserialized totalScenes was <= 0. Resetting to 1.");
        totalScenes = 1;
    }
}
/*!****************************************************************
\brief
    Provides debugging information for the video component.
\return
    A string containing debug information.
*******************************************************************/
std::string VideoComponent::DebugInfo() const {
    return "VideoComponent: Scene = " + std::to_string(currentScene) + ", Elapsed Time = " + std::to_string(elapsedTime);
}
