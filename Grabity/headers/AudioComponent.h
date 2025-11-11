/*!****************************************************************
\file: AudioComponent.h
\author: Johny Yong Jun Siang
\par: j.yong\@digipen.edu
\brief: Header file declaring the AudioComponent class, with inherits from the base Component class. 
        Responsible for managing audio playback per game objects, including storing references to their audio clips by their
        IDs and controlling current channels. Also include function to add more audio clips reference and getting the current audio clips.


Copyright (C) 2024 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents
without the prior written consent of DigiPen Institute of
Technology is prohibited.
*******************************************************************!*/
#pragma once
#include "Component.h"
#include "AudioManager.h"
#include <vector>
#include "LuaConfig.h"

class AudioComponent : public Component {
private:
    FMOD::Channel* currentChannel = nullptr; ///< The FMOD channel currently playing the audio.
    std::vector<int> audioClipIDs; ///< Stores the IDs of audio clips associated with this component.

public:
    /**
     * @brief Default constructor for the AudioComponent.
     */
    AudioComponent();

    /**
     * @brief Constructor that initializes the AudioComponent with a parent GameObject.
     * @param parent The parent GameObject associated with this component.
     */
    AudioComponent(GameObject* parent);

    /**
     * @brief Destructor for the AudioComponent. Cleans up any allocated resources.
     */
    ~AudioComponent();

    /**
     * @brief Adds a new audio clip to the component by its ID.
     * @param audioID The ID of the audio clip to add.
     */
    void AddAudioClip(int audioID);

    /**
     * @brief Retrieves the list of audio clip IDs associated with this component.
     * @return A const reference to the vector of audio clip IDs.
     */
    const std::vector<int>& GetAudioClips() const { return audioClipIDs; };

    /**
     * @brief Plays an audio clip by its ID.
     * @param audioID The ID of the audio clip to play.
     */
    void PlayAudio(int audioID);

    /**
     * @brief Stops the currently playing audio.
     */
    void StopAudio();

    /**
     * @brief Updates the AudioComponent. This is called during the game's update cycle.
     */
    void Update() override;

    /**
     * @brief Serializes the AudioComponent's data to a Lua file.
     * @param luaFilePath The file path to the Lua file.
     * @param tableName The table name under which data will be stored.
     */
    void Serialize(const std::string& luaFilePath, const std::string& tableName) override;

    /**
     * @brief Deserializes the AudioComponent's data from a Lua file.
     * @param luaFilePath The file path to the Lua file.
     * @param tableName The table name from which data will be loaded.
     */
    void Deserialize(const std::string& luaFilePath, const std::string& tableName) override;

    /**
     * @brief Provides a debug string with details about the AudioComponent's state.
     * @return A string containing debug information.
     */
    std::string DebugInfo() const override;

    /**
     * @brief Removes an audio clip from the component by its ID.
     * @param audioID The ID of the audio clip to remove.
     */
    void RemoveAudioClip(int audioID);  // Add this declaration
};
