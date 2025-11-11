/*!****************************************************************
\file: Audio.h
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
#pragma once
#include "fmod.hpp"
#include <string>

enum class AudioType {
	BGM, //Loops by default
	NO_LOOP, //Mainly for SFX
	LOOP //Mainly for SFX
};

class Audio {
	FMOD::Sound* audio;
	std::string name;
	AudioType audioType;
	int priority;

public:
	/**
	 * @brief Constructs an Audio object with the specified properties.
	 * @param audioName The name of the audio file (identifier).
	 * @param filePath The file path to the audio file.
	 * @param type The type of the audio (e.g., music, sound effect).
	 * @param priority The priority level of the audio (higher priority is handled first).
	 */
	Audio(const std::string& audioName, const std::string& filePath, AudioType type, int priority);

	/**
	 * @brief Destructor for the Audio object. Cleans up resources if necessary.
	 */
	~Audio();

	/**
	 * @brief Retrieves the FMOD::Sound object associated with this Audio.
	 * @return A pointer to the FMOD::Sound object.
	 */
	FMOD::Sound* GetAudio() const;

	/**
	 * @brief Retrieves the type of the audio.
	 * @return The AudioType of the audio (e.g., music, sound effect).
	 */
	AudioType GetType() const;

	/**
	 * @brief Retrieves the priority of the audio.
	 * @return An integer representing the priority of the audio.
	 */
	int GetPriority() const;

	/**
	 * @brief Retrieves the name of the audio.
	 * @return A string containing the name of the audio.
	 */
	std::string GetAudioName() const;
};
