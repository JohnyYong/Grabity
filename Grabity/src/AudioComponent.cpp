/*!****************************************************************
\file: AudioComponent.cpp
\author: Johny Yong Jun Siang, j.yong, 2301301
\brief: File defining the implementations of the AudioComponent class methods declared in AudioComponent.h. This includes functionality for
        adding, playing, stopping, and managing audio clips for a game object. The PlayAudio method ensures that only one audio clip is played
        at a time by stopping any currently playing audio before playing a new one. The Update method handles checking whether the audio is still
        playing and clears the channel when audio playback finishes. Serialization, De-Serialization, and debug information are provided for game
        state management and debugging.

Copyright (C) 2024 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents
without the prior written consent of DigiPen Institute of
Technology is prohibited.
*******************************************************************!*/

#include "AudioComponent.h"

#ifdef _IMGUI
#include <iostream>
#include "ImGuiConsole.h"
#endif // _IMGUI
#include <algorithm>  // for std::find

/**
 * @brief Default constructor for the AudioComponent.
 */
AudioComponent::AudioComponent()
{
}

/**
 * @brief Constructor that initializes the AudioComponent with a parent GameObject.
 * @param parent The parent GameObject associated with this component.
 */
AudioComponent::AudioComponent(GameObject* parent) : Component(parent), currentChannel(nullptr), audioClipIDs(0) {}

/**
 * @brief Destructor for the AudioComponent. Stops any audio currently playing and clears the audio clip list.
 */
AudioComponent::~AudioComponent() {
    StopAudio();  // Stop all audio when the component is destroyed
    audioClipIDs.clear();  // Should clear itself but to be safe
}

/**
 * @brief Adds a reference to an audio clip by its ID.
 * @param audioID The ID of the audio clip to add.
 */
void AudioComponent::AddAudioClip(int audioID) {
    audioClipIDs.push_back(audioID);
}

/**
 * @brief Plays an audio clip by its ID. Stops any currently playing audio before starting the new clip.
 * @param audioID The ID of the audio clip to play.
 */
void AudioComponent::PlayAudio(int audioID) {
    if (currentChannel) {
        bool isPlaying = false;
        currentChannel->isPlaying(&isPlaying);

        if (isPlaying) {
            currentChannel->stop();
        }
    }

    AudioManager::GetInstance().PlayAudio(audioID, &currentChannel);
    AudioManager::GetInstance().SetChannelVolume(audioID, 0.7f);

    if (!currentChannel) {
		ImGuiConsole::Cout("Failed to play audio ID: %d", audioID);
    }
}

/**
 * @brief Stops the currently playing audio, if any, and clears the audio channel.
 */
void AudioComponent::StopAudio() {
    if (currentChannel) {
        currentChannel->stop();
        currentChannel = nullptr;  // Clear the channel once the audio is stopped
    }
}


/**
 * @brief Updates the AudioComponent. Checks whether the audio is still playing and clears the channel if playback has finished.
 */
void AudioComponent::Update() {

    if (!GetActive()) { return; }

    if (currentChannel) {
        bool isPlaying = false;
        FMOD_RESULT result = currentChannel->isPlaying(&isPlaying);

        // If the channel is no longer playing, clear it
        if (result != FMOD_OK || !isPlaying) {
            currentChannel = nullptr;
        }
    }
}

/**
 * @brief Serializes the AudioComponent's data, including audio clip IDs, to a Lua file.
 * @param luaFilePath The file path to the Lua file.
 * @param tableName The table name under which data will be stored.
 */
void AudioComponent::Serialize(const std::string& luaFilePath, const std::string& tableName) {
    LuaManager luaManager(luaFilePath);

    //Temp holders
    int index = 0;
    std::vector<std::string> keys;
    LuaManager::LuaValueContainer values;

    for (auto id : audioClipIDs) {

        keys.push_back("audioClip" + std::to_string(index));
        values.push_back(id);

        ++index;
    }

    luaManager.LuaWrite(tableName, values, keys, "Audio");

}

/**
 * @brief Deserializes the AudioComponent's data, including audio clip IDs, from a Lua file.
 * @param luaFilePath The file path to the Lua file.
 * @param tableName The table name from which data will be loaded.
 */
void AudioComponent::Deserialize(const std::string& luaFilePath, const std::string& tableName)
{
    LuaManager luaManager(luaFilePath);

    luaManager.LuaReadFromAudioClips(tableName, audioClipIDs);
}

/**
 * @brief Provides a string containing debug information about the AudioComponent.
 * @return A string with the list of audio clip IDs.
 */
std::string AudioComponent::DebugInfo() const {
    std::string debugInfo = "AudioComponent: \n";
    for (const auto& audioName : audioClipIDs) {
        debugInfo += "Audio Clip: " + audioName;
    }
    return debugInfo;
}

/**
 * @brief Removes an audio clip from the component by its ID. If the clip is currently playing, it is stopped first.
 * @param audioID The ID of the audio clip to remove.
 */
void AudioComponent::RemoveAudioClip(int audioID) {
    // Stop the audio if it's currently playing and matches the ID being removed
    if (currentChannel) {
        bool isPlaying = false;
        currentChannel->isPlaying(&isPlaying);
        if (isPlaying) {
            currentChannel->stop();
            currentChannel = nullptr;
        }
    }

    // Find and remove the audio ID from the vector
    auto it = std::find(audioClipIDs.begin(), audioClipIDs.end(), audioID);
    if (it != audioClipIDs.end()) {
        audioClipIDs.erase(it);
    }
}
