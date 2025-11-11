/*!****************************************************************
\file: VideoComponent.h
\author: Johny Yong Jun Siang, j.yong, 2301301

\brief
    Declares the VideoComponent, which updates the SpriteComponent
    to change textures at a fixed interval, creating a video-like effect.

Copyright (C) 2025 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents
without the prior written consent of DigiPen Institute of
Technology is prohibited.
*******************************************************************/

#pragma once
#include "Component.h"
#include "LuaConfig.h"
#include <string>

class VideoComponent : public Component {

public:
    /*!****************************************************************
    \brief Default constructor for VideoComponent.
    *******************************************************************/
    VideoComponent();

    /*!****************************************************************
    \brief Constructor that assigns the video component to a parent GameObject.
    \param parent Pointer to the parent GameObject.
    *******************************************************************/
    VideoComponent(GameObject* parent);
    //Destructor
    ~VideoComponent();
    /*!****************************************************************
    \brief Updates the sprite at a fixed time interval.
    *******************************************************************/
    void Update() override;

    /*!****************************************************************
    \brief Serializes the video component's data to a Lua file.
    \param luaFilePath Path to the Lua script file.
    \param tableName Name of the Lua table where the data will be stored.
    *******************************************************************/
    void Serialize(const std::string& luaFilePath, const std::string& tableName) override;

    /*!****************************************************************
    \brief Deserializes the video component's data from a Lua file.
    \param luaFilePath Path to the Lua script file.
    \param tableName Name of the Lua table from which the data will be loaded.
    *******************************************************************/
    void Deserialize(const std::string& luaFilePath, const std::string& tableName) override;

    /*!****************************************************************
    \brief Provides debugging information for the video component.
    \return A string containing debug information.
    *******************************************************************/
    std::string DebugInfo() const override;


    //Get setters
    int GetCurrentScene() const { return currentScene; }
    void SetCurrentScene(int scene) { currentScene = scene; }

    float GetSwitchInterval() const { return switchInterval; }
    void SetSwitchInterval(float interval) { switchInterval = interval; }

    int GetTotalScenes() const { return totalScenes; }
    void SetTotalScenes(int scenes) { totalScenes = scenes; }

private:
    float elapsedTime; /*!< Tracks elapsed time for sprite switching */
    int currentScene;  /*!< Tracks the current scene (0 = Scene1, 1 = Scene2) */
    float switchInterval = 3.0f; // Time in seconds to switch textures
    int totalScenes = 16; // Number of scenes from 
    bool firstScenePlayed = false;
    int previousScene = -1;

};
