/*!****************************************************************
\file: Audio.cpp
\author: Johny Yong Jun Siang, j.yong, 2301301
\brief: Declares the Audio class for managing audio assets using the FMOD library.
		Includes audio data with its type (background music,looping sound effects, or non-looping sound effects),
		priority for audio, andadata like name and file path. It provides functionality to access audio properties and the FMOD::Sound
		object, allowing straightforward integration with audio systems.

Copyright (C) 2024 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents
without the prior written consent of DigiPen Institute of
Technology is prohibited.
*******************************************************************!*/
#ifdef _IMGUI
#include <iostream>
#endif // _IMGUI
#include "ImGuiConsole.h"
#include "AudioManager.h"
#include "Assetmanager.h"
/**
 * @brief Constructs an Audio object with the specified properties and loads the audio using FMOD.
 * @param audioName The name of the audio file (identifier).
 * @param filePath The file path to the audio file.
 * @param type The type of the audio (e.g., NO_LOOP or looping sound effects).
 * @param audioPriority The priority level of the audio (higher priority is handled first).
 */
Audio::Audio(const std::string& audioName, const std::string& filePath, AudioType type, int audioPriority)
	: name(audioName), audioType(type), priority(audioPriority), audio(nullptr)
{
	FMOD_MODE mode = (audioType == AudioType::NO_LOOP) ? FMOD_DEFAULT : FMOD_LOOP_NORMAL;
	FMOD_RESULT result = AudioManager::GetInstance().GetSystem()->createSound(filePath.c_str(), mode, nullptr, &audio);
	if (result != FMOD_OK) { 
#ifdef _LOGGING
		ImGuiConsole::Cout("Audio loading error: %s", name.c_str());
#endif // _LOGGING
	} 
	else { 
#ifdef _LOGGING

		ImGuiConsole::Cout("Audio creation success: %s", name.c_str());
#endif // _LOGGING
	}
}
/**
 * @brief Destructor for the Audio object. Releases the FMOD::Sound object if it exists.
 */
Audio::~Audio() {
	if (audio) { audio->release(); }
}
/**
 * @brief Retrieves the FMOD::Sound object associated with this Audio.
 * @return A pointer to the FMOD::Sound object.
 */
FMOD::Sound* Audio::GetAudio() const {
	return audio;
}
/**
 * @brief Retrieves the type of the audio.
 * @return The AudioType of the audio (e.g., NO_LOOP or looping sound effects).
 */
AudioType Audio::GetType() const {
	return audioType;
}
/**
 * @brief Retrieves the priority of the audio.
 * @return An integer representing the priority of the audio.
 */
int Audio::GetPriority() const {
	return priority;
}
/**
 * @brief Retrieves the name of the audio.
 * @return A string containing the name of the audio.
 */
std::string Audio::GetAudioName() const {
	return name;
}