/*!****************************************************************
\file: AudioManager.h
\author: Johny Yong Jun Siang, j.yong, 2301301
\brief: Declares the AudioManager class, which manages audio playbackand system initialization using the FMOD library. This class
        handles background music (BGM) and sound effects (SFX) through volume control, audio channel management, and basic audio operations
        such as play, pause, resume, and stop. It uses a singleton pattern to ensure only one audio system instance exists. The class also
        provides functionality to list active audio channels and update audio volumes.

Copyright (C) 2024 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents
without the prior written consent of DigiPen Institute of
Technology is prohibited.
*******************************************************************!*/

#pragma once
#include <map>
#include <fmod.hpp>
#include <string>
#include <vector>
#include "Assetmanager.h"

//Forward declarations
class Audio;

struct FadeOutInfo {
    FMOD::Channel* channel;
    float fadeDuration;    // Total time to fade out
    float elapsedTime;     // Time passed since the fade started
    float initialVolume;   // Starting volume
};

struct FadeInInfo {
    FMOD::Channel* channel;
    float fadeDuration;    // Total time to fade in
    float elapsedTime;     // Time passed since fade started
    float targetVolume;    // Final volume after fade-in
};

struct LowPassTransition {
    FMOD::Channel* channel = nullptr;
    float startFrequency = 0.f;
    float targetFrequency = 0.f;
    float elapsed = 0.f;
    float duration = 1.f;
    bool active = false;
};


class AudioManager {
    FMOD::System* audioSystem;
    
    std::map<int, std::vector<FMOD::Channel*>> audioChannels;
    std::vector<FadeInInfo> fadingInChannels;


    float bgmVolume;
    float sfxVolume;
    float masterVolume;
    float bgmVolumeRaw;  
    float sfxVolumeRaw;

    /**
     * @brief Updates the volume settings for background music (BGM) and sound effects (SFX).
     */
    void UpdateVolumes();

    int currentID = 0;  // Track the next available audio ID

    int queuedBGM = -1;
    float queuedBGMDelay = 0.0f;

public:
    FMOD::Channel* bgmChannel;
    std::vector<FadeOutInfo> fadingOutChannels;
    FMOD::DSP* lowPassDSP = nullptr;
    LowPassTransition lpTransition;
    std::map<int, float> sfxRawVolumes; // Raw volume set per audioID
    float bgmRawOverride = -1.0f;  // -1 means no override

    /**
     * @brief Constructs an AudioManager object and initializes the audio system.
     */
    AudioManager();

    /**
     * @brief Destructor that releases the audio system resources.
     */
    ~AudioManager();

    /**
     * @brief Retrieves the singleton instance of the AudioManager class.
     * @return A reference to the singleton instance.
     */
    static AudioManager& GetInstance();
    AudioManager(AudioManager const&) = delete;
    void operator=(AudioManager const&) = delete;

    /**
     * @brief Initializes the FMOD audio system with a specified maximum number of channels.
     * @param maxChannels The maximum number of audio channels the system can handle (default is 100).
     */
    void InitSystem(int maxChannels = 100);

    /**
     * @brief Releases the resources of the audio system.
     */
    void ReleaseSystem();
    
    /**
     * @brief Retrieves the FMOD audio system.
     * @return A pointer to the FMOD::System object.
     */
    FMOD::System* GetSystem() const;

    /**
     * @brief Retrieves the map of all audio objects currently loaded in the system.
     * @return A constant reference to the map of audio objects.
     */
    const std::map<int, Audio*>& GetAudioObjects() const;

    /**
     * @brief Retrieves the current background music (BGM) volume.
     * @return The current BGM volume.
     */
    float GetBGMVolume() const { return bgmVolume; };

    /**
     * @brief Retrieves the current sound effects (SFX) volume.
     * @return The current SFX volume.
     */
    float GetSFXVolume() const { return sfxVolume; };

    void SetMasterVolume(float vol);

    /**
     * @brief Sets the volume for background music (BGM).
     * @param vol The desired BGM volume.
     */
    void SetBGMVolume(float vol);

    /**
     * @brief Sets the volume for sound effects (SFX).
     * @param vol The desired SFX volume.
     */
    void SetSFXVolume(float vol);

    /**
     * @brief Lists all currently active audio channels.
     */
    void ListActiveChannels() const;

    //Standard operations

    /**
     * @brief Pauses the audio playback for a specific audio ID.
     * @param audioID The ID of the audio to pause.
     */
    void PauseAudio(int audioID);

    /**
     * @brief Plays an audio based on the given audio ID and stores the playback channel in the provided output.
     * @param audioID The ID of the audio to play.
     * @param channelOut A pointer to a channel that will store the audio playback channel.
     */
    void PlayAudio(int audioID, FMOD::Channel** channelOut);

    /**
     * @brief Plays an audio based on the given audio ID.
     * @param audioID The ID of the audio to play.
     */
    void PlayAudio(int audioID);

    void PlayAudioImmediately(int audioID);

    /**
     * @brief Resumes the audio playback for a specific audio ID.
     * @param audioID The ID of the audio to resume.
     */
    void ResumeAudio(int audioID);

    /**
     * @brief Stops the audio playback for a specific audio ID.
     * @param audioID The ID of the audio to stop.
     */
    void StopAudio(int audioID);

    //M2

    /**
     * @brief Pauses all audio currently being played.
     */
    void PauseAllAudio();

    /**
     * @brief Resumes all paused audio.
     */
    void ResumeAllAudio();

    /**
     * @brief Updates the audio system, including volume adjustments and channel management.
     */
    void Update();
    /**
     * @brief Checks if a specific audio ID is currently playing.
     * @param audioID The ID of the audio to check.
     * @return True if the audio is playing, false otherwise.
     */
    bool IsPlaying(int audioID);

    //M5
    /**
     * @brief Gradually fades out the volume of the specified audio channel over a given duration.
     *        Once the fade-out is complete, the audio will be stopped.
     * @param channel A pointer to the FMOD audio channel to fade out.
     * @param duration The duration (in seconds) over which the fade-out occurs.
     */
    void FadeOutAudio(FMOD::Channel* channel, float duration);
    /**
     * @brief Cleans up audio channels that are no longer in use, removing inactive or stopped channels.
     *        This helps optimize performance by releasing unused resources.
     */
    void CleanUpChannels();
    /**
     * @brief Gradually fades in the volume of the specified audio channel over a given duration.
     *        The volume starts from zero and reaches the specified target volume.
     * @param channel A pointer to the FMOD audio channel to fade in.
     * @param duration The duration (in seconds) over which the fade-in occurs.
     * @param targetVolume The final volume level to reach after the fade-in is complete.
     */
    void FadeInAudio(FMOD::Channel* channel, float duration, float targetVolume);
    /**
     * @brief Sets the volume of a specific audio channel associated with the given audio ID.
     * @param audioID The ID of the audio whose volume is to be adjusted.
     * @param volume The desired volume level.
     */
    void SetChannelVolume(int audioID, float volume);

    /**
     * @brief Applies a smooth low-pass filter to the specified audio channel over a given duration.
     * @param channel The FMOD channel to which the low-pass filter will be applied.
     * @param targetFrequency The target cutoff frequency (in Hz) for the low-pass filter.
     * @param duration The duration (in seconds) over which the low-pass filter will be applied smoothly.
     */
    void ApplyLowPassFilterSmooth(FMOD::Channel* channel, float targetFrequency, float duration);

    /**
     * @brief Immediately removes the low-pass filter from the specified audio channel.
     * @param channel The FMOD channel from which the low-pass filter will be removed.
     */
    void RemoveLowPassFilter(FMOD::Channel* channel);

    /**
     * @brief Smoothly removes the low-pass filter from the specified audio channel over a given duration.
     * @param channel The FMOD channel from which the low-pass filter will be removed.
     * @param duration The duration (in seconds) over which the low-pass filter will be smoothly removed.
     */
    void RemoveLowPassFilterSmooth(FMOD::Channel* channel, float duration);
    /**
     * @brief Stops all audio
     */
    void StopAllAudio();
};