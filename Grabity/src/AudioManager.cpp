/*!****************************************************************
\file: AudioManager.h
\author: Johny Yong Jun Siang, j.yong, 2301301
\co-author: Lee Yu Jie Brandon, l.yujiebrandon, 2301232
\brief: Defines functions declared in AudioManager.h, which manages audio playbackand system initialization using the FMOD library.
        Handles background music (BGM) and sound effects (SFX) through volume control, audio channel management, and basic audio operations
        such as play, pause, resume, and stop. It uses a singleton pattern to ensure only one audio system instance exists. The audio manager 
        maintains a pool of active channels, lists active channels for debugging, and updates the audio system each frame

        Johny created and defined all the functions (90%)
        Brandon who implemented Assets Manager moved the loading of audios into AssetManager instead (10%)

Copyright (C) 2024 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents
without the prior written consent of DigiPen Institute of
Technology is prohibited.
*******************************************************************!*/
#include "AudioManager.h"
#include "Assetmanager.h"
#include "Audio.h"
#include <chrono>
#include <thread>
#ifdef _IMGUI
#include <iostream>
#include "ImGuiConsole.h"
#endif // _IMGUI

// Retrieves the singleton instance of the AudioManager class.
AudioManager& AudioManager::GetInstance()
{
    static AudioManager instance;
    return instance;
}

// Constructs an AudioManager object and initializes the audio system.
AudioManager::AudioManager() : audioSystem(nullptr), bgmChannel(nullptr), bgmVolume(1.f), sfxVolume(1.f), masterVolume(1.0f), bgmVolumeRaw(1.0f), sfxVolumeRaw(1.0f) {
    InitSystem();
}

// Destructor that releases the audio system resources.
AudioManager::~AudioManager() {
    ReleaseSystem();
}

// Initializes the FMOD audio system with a specified maximum number of channels.
void AudioManager::InitSystem(int maxChannels) {
    FMOD_RESULT result = FMOD::System_Create(&audioSystem);
    if (result != FMOD_OK) {
        ImGuiConsole::Cout("FMOD System creation failure");
    }

    audioSystem->init(maxChannels, FMOD_INIT_NORMAL, nullptr);

    // Optional: improve real-time fade response
    audioSystem->setSoftwareFormat(0, FMOD_SPEAKERMODE_DEFAULT, 0);
    audioSystem->setDSPBufferSize(1024, 4); // Smaller buffer for responsiveness
}


// Releases the resources of the audio system.
void AudioManager::ReleaseSystem() {
    for (auto& audio : AssetManager::GetInstance().audioObjects) {
        delete audio.second;
    }
    AssetManager::GetInstance().audioObjects.clear();
    audioSystem->close();
    audioSystem->release();
}

// Retrieves the FMOD audio system.
FMOD::System* AudioManager::GetSystem() const {
    return audioSystem;
}

// Plays an audio based on the given audio ID and stores the playback channel in the provided output.
void AudioManager::PlayAudio(int audioID, FMOD::Channel** channelOut) {
    std::map<int, Audio*>::iterator it = AssetManager::GetInstance().audioObjects.find(audioID);

    if (it != AssetManager::GetInstance().audioObjects.end()) {
        Audio* audio = it->second;
        FMOD::Channel* channel = nullptr;

        if (audio->GetType() == AudioType::BGM && bgmChannel) {
            FadeOutAudio(bgmChannel, 1);
            bgmChannel = nullptr;
        }

        //Normal play audio
        audioSystem->playSound(audio->GetAudio(), nullptr, false, &channel);

        //Setting of volume before playing
        if (channel) {
            if (audio->GetType() != AudioType::BGM) {
                channel->setVolume(sfxVolume);
                audioChannels[audioID].push_back(channel);
            }
            else {
                channel->setVolume(bgmVolume);
                bgmChannel = channel;
            }

            channel->setPriority(audio->GetPriority());

            if (channelOut) {
                *channelOut = channel;
            }
        }
    }
}

void AudioManager::PlayAudio(int audioID) {
    AudioManager::GetInstance().GetSystem()->mixerSuspend();
    AudioManager::GetInstance().GetSystem()->mixerResume();

    auto it = AssetManager::GetInstance().audioObjects.find(audioID);
    if (it != AssetManager::GetInstance().audioObjects.end()) {
        Audio* audio = it->second;

        if (audio->GetType() == AudioType::BGM && bgmChannel) {
            FMOD::Sound* currentSound = nullptr;
            bgmChannel->getCurrentSound(&currentSound);

            if (currentSound == audio->GetAudio()) {
                // Same BGM is already playing
                ImGuiConsole::Cout("PlayAudio: BGM %d already playing, skipping replay.", audioID);
                return;
            }

            // Otherwise fade out the old one and queue the new
            FadeOutAudio(bgmChannel, 0.4f);
            queuedBGM = audioID;
            return;
        }

        // Play immediately if SFX or no BGM playing
        PlayAudioImmediately(audioID);
    }
}

void AudioManager::Update() {
    CleanUpChannels();
    audioSystem->update();

    float deltaTime = 1.0f / 60.0f;
    bool bgmFadeCompleted = false;

    // --- Fade-out logic ---
    for (auto it = fadingOutChannels.begin(); it != fadingOutChannels.end(); ) {
        FadeOutInfo& fade = *it;
        fade.elapsedTime += deltaTime;

        bool isPlaying = false;
        fade.channel->isPlaying(&isPlaying);

        if (!isPlaying) {
            it = fadingOutChannels.erase(it);
            continue;
        }

        float t = fade.elapsedTime / fade.fadeDuration;
        if (t >= 1.0f) {
            fade.channel->setVolume(0.01f);
            fade.channel->stop();  // <-- force stop
            it = fadingOutChannels.erase(it);

            if (fade.channel == bgmChannel) {
                bgmChannel = nullptr;
                bgmFadeCompleted = true;
            }
        }
        else {
            float newVolume = fade.initialVolume * (1.0f - t);
            fade.channel->setVolume(newVolume);
            ++it;
        }
    }

    // --- Fade-in logic ---
    for (auto it = fadingInChannels.begin(); it != fadingInChannels.end(); ) {
        FadeInInfo& fade = *it;
        fade.elapsedTime += deltaTime;

        bool isPlaying = false;
        fade.channel->isPlaying(&isPlaying);

        if (!isPlaying) {
            it = fadingInChannels.erase(it);
            continue;
        }

        float t = fade.elapsedTime / fade.fadeDuration;
        if (t >= 1.0f) {
            fade.channel->setVolume(fade.targetVolume);
            fade.channel->setPaused(false); // Ensure it's unpaused at end
            it = fadingInChannels.erase(it);
        }
        else {
            float newVolume = fade.targetVolume * t;

            if (newVolume > 0.01f) {
                bool isPaused = true;
                fade.channel->getPaused(&isPaused);
                if (isPaused)
                    fade.channel->setPaused(false); // Unpause gradually during fade-in
            }

            fade.channel->setVolume(newVolume);
            ++it;
        }
    }

    // --- Low-pass transition logic ---
    if (lpTransition.active && lpTransition.channel && lowPassDSP) {
        lpTransition.elapsed += deltaTime;
        float t = std::min(lpTransition.elapsed / lpTransition.duration, 1.0f);
        float interpolatedCutoff = lpTransition.startFrequency +
            (lpTransition.targetFrequency - lpTransition.startFrequency) * t;

        lowPassDSP->setParameterFloat(FMOD_DSP_LOWPASS_CUTOFF, interpolatedCutoff);

        if (t >= 1.0f) {
            lpTransition.active = false;

            // If fading back to full clarity, remove DSP
            if (lpTransition.targetFrequency >= 19900.0f) {
                lpTransition.channel->removeDSP(lowPassDSP);
#ifdef _IMGUI
                ImGuiConsole::Cout("Low-pass filter removed after smooth transition.");
#endif
            }
        }
    }

    // --- Handle queued BGM after fade-out ---
    if (bgmFadeCompleted && queuedBGM != -1) {
        PlayAudioImmediately(queuedBGM);
        queuedBGM = -1;
    }
}


void AudioManager::PlayAudioImmediately(int audioID) {

    ImGuiConsole::Cout("Trying Play Audio: %d", audioID);

    auto it = AssetManager::GetInstance().audioObjects.find(audioID);

    if (it != AssetManager::GetInstance().audioObjects.end()) {
        Audio* audio = it->second;
        FMOD::Channel* channel = nullptr;

        // Start the sound in a paused state
        audioSystem->playSound(audio->GetAudio(), nullptr, true, &channel);

        if (channel) {
            if (audio->GetType() != AudioType::BGM) {
                channel->setVolume(sfxVolume);

                // Special pop-fix for IDs 30 and 31 only
                if (audioID == 30 || audioID == 31) {
                    channel->setPaused(true);
                    FadeInAudio(channel, 1.0f, 0.1f); // Fade in
                }
                else {
                    channel->setPaused(false); // Standard SFX
                }

                audioChannels[audioID].push_back(channel);
            }

            else {
                channel->setVolume(0.0f); // Stay silent
                channel->setMode(FMOD_LOOP_NORMAL | FMOD_2D);
                channel->setPriority(255);
                if (bgmChannel && bgmChannel != channel) {
                    bgmChannel->stop(); // Ensure previous BGM is dead
                }
                bgmChannel = channel;
                //unsigned long long dspClock;
                //channel->getDSPClock(nullptr, &dspClock);
                //channel->setDelay(dspClock + 512, 0, false); // Delay playback slightly
                FadeInAudio(bgmChannel, 1.f, bgmVolume); // Fade in
            }

            channel->setPriority(audio->GetPriority());
        }
    }


}

// Pauses all audio currently being played.
void AudioManager::PauseAllAudio() {
    //Pause the background music channel if it's playing
    if (bgmChannel) {
        bgmChannel->setPaused(true);
    }

    //Pause all other audio channels
    for (auto& pair : audioChannels) {  
        for (FMOD::Channel* channel : pair.second) {  //Iterate over each channel in the vector
            if (channel) {
                channel->setPaused(true);  //Pause each channel
            }
        }
    }
}

// Resumes all paused audio.
void AudioManager::ResumeAllAudio()
{
    //Pause the background music channel if it's playing
    if (bgmChannel) {
        bgmChannel->setPaused(false);
    }

    //Pause all other audio channels
    for (auto& pair : audioChannels) {
        for (FMOD::Channel* channel : pair.second) {  //Iterate over each channel in the vector
            if (channel) {
                channel->setPaused(false);  //Pause each channel
            }
        }
    }
}


// Pauses the audio playback for a specific audio ID.
void AudioManager::PauseAudio(int audioID) {
    auto it = audioChannels.find(audioID);

    ImGuiConsole::Cout("BGM has paused! Restarting...\n");

    //Checks if it is the BGM
    if (AssetManager::GetInstance().audioObjects.find(audioID) != AssetManager::GetInstance().audioObjects.end() && AssetManager::GetInstance().audioObjects[audioID]->GetType() == AudioType::BGM) {
        if (bgmChannel) {
            bgmChannel->setPaused(true);
        }
    }

    if (it != audioChannels.end())
    {
        for (FMOD::Channel* channel : it->second)
        {
            if (channel)
            {
                channel->setPaused(true);
            }
        }
    }
}

// Resumes the audio playback for a specific audio ID.
void AudioManager::ResumeAudio(int audioID) {
    auto it = audioChannels.find(audioID);  // Find channels by ID
    //Checks if it is the BGM
    if (AssetManager::GetInstance().audioObjects.find(audioID) != AssetManager::GetInstance().audioObjects.end() && AssetManager::GetInstance().audioObjects[audioID]->GetType() == AudioType::BGM) {
        if (bgmChannel) {
            bgmChannel->setPaused(false);
        }
    }

    // Resume all other channels associated with this audio ID
    if (it != audioChannels.end()) {
        for (FMOD::Channel* channel : it->second) {
            if (channel) {
                channel->setPaused(false);
            }
        }
    }
}

// Stops the audio playback for a specific audio ID.
void AudioManager::StopAudio(int audioID) {
    auto it = audioChannels.find(audioID);  // Find channels by ID
    //Checks if it is the BGM
    if (AssetManager::GetInstance().audioObjects.find(audioID) != AssetManager::GetInstance().audioObjects.end() && AssetManager::GetInstance().audioObjects[audioID]->GetType() == AudioType::BGM) {
        if (bgmChannel) {
            bgmChannel->stop();
            bgmChannel = nullptr; // Clear the BGM channel after stopping
            ImGuiConsole::Cout("BGM has stopped! Restarting...\n");
        }
    }

    //For all others
    if (it != audioChannels.end()) {
        for (FMOD::Channel* channel : it->second) {
            if (channel) {
                channel->stop();
            }
        }
    }

}

// Stops all currently playing audio including BGM and SFX
void AudioManager::StopAllAudio() {
    // Stop BGM if active
    if (bgmChannel) {
        bgmChannel->stop();
        bgmChannel = nullptr;
        ImGuiConsole::Cout("StopAllAudio: BGM channel stopped.");
    }

    // Stop all other SFX channels
    for (auto& pair : audioChannels) {
        for (FMOD::Channel* channel : pair.second) {
            if (channel) {
                channel->stop();
            }
        }
    }

    // Clear the channel map
    audioChannels.clear();

    // Clear any fading or low-pass transitions
    fadingOutChannels.clear();
    fadingInChannels.clear();
    lpTransition = { nullptr, 0.f, 0.f, 0.f, 0.f, false };

    ImGuiConsole::Cout("StopAllAudio: All channels stopped and cleared.");
}


// Updates the volume settings for background music (BGM) and sound effects (SFX).
void AudioManager::UpdateVolumes() {
    for (auto& [audioID, channels] : audioChannels) {
        float raw = sfxRawVolumes.count(audioID) ? sfxRawVolumes[audioID] : 1.0f;

        for (FMOD::Channel* channel : channels) {
            if (channel) {
                channel->setVolume(sfxVolume * raw);  // Use global * raw
            }
        }
    }

    if (bgmChannel) {
        float finalBGMVolume = (bgmRawOverride >= 0.0f) ? (bgmVolume * bgmRawOverride) : bgmVolume;
        bgmChannel->setVolume(finalBGMVolume);
    }

}


void AudioManager::SetMasterVolume(float vol) {
    //float scale = vol / masterVolume; // Calculate scale factor
    //masterVolume = vol;

    //// Scale existing volumes instead of resetting them
    //bgmVolume *= scale;
    //sfxVolume *= scale;


    masterVolume = vol;

    // Recalculate final volumes
    bgmVolume = masterVolume * bgmVolumeRaw;
    sfxVolume = masterVolume * sfxVolumeRaw;

    UpdateVolumes();
}

void AudioManager::SetBGMVolume(float vol) {
    bgmVolumeRaw = vol;
    bgmVolume = masterVolume * bgmVolumeRaw;
    if (bgmChannel) {
        bgmChannel->setVolume(bgmVolume);
    }
}

void AudioManager::SetSFXVolume(float vol) {
    sfxVolumeRaw = vol;
    sfxVolume = masterVolume * sfxVolumeRaw;

    UpdateVolumes();
    ImGuiConsole::Cout("SetSFXVolume: raw = %.2f, final = %.2f", sfxVolumeRaw, sfxVolume);

}
//
//
//// Sets the volume for background music (BGM).
//void AudioManager::SetBGMVolume(float vol)
//{
//    bgmVolumeRaw = vol;
//    bgmVolume = masterVolume * bgmVolumeRaw;
//    if (bgmChannel) { bgmChannel->setVolume(bgmVolume); }
//}
//
//// Sets the volume for sound effects (SFX).
//void AudioManager::SetSFXVolume(float vol)
//{
//    sfxVolumeRaw = vol;
//    sfxVolume = masterVolume * sfxVolumeRaw;
//    UpdateVolumes();
//}


// Retrieves the map of all audio objects currently loaded in the system.
const std::map<int, Audio*>& AudioManager::GetAudioObjects() const
{
    return AssetManager::GetInstance().audioObjects;
}

// Lists all currently active audio channels.
void AudioManager::ListActiveChannels() const {
    if (!audioSystem) {
        ImGuiConsole::Cout("Audio system not initialized.");
        return;
    }

    int numChannelsPlaying = 0;
    FMOD_RESULT result = audioSystem->getChannelsPlaying(&numChannelsPlaying);
    if (result != FMOD_OK) {
        ImGuiConsole::Cout("Failed to get the number of channels playing.");
        return;
    }

    ImGuiConsole::Cout("Active Channels:");

    for (int i = 0; i < 100; ++i) {
        FMOD::Channel* channel = nullptr;
        result = audioSystem->getChannel(i, &channel);

        if (result == FMOD_OK && channel) {
            bool isPlaying = false;
            result = channel->isPlaying(&isPlaying);

            if (result == FMOD_OK && isPlaying) {
                FMOD::Sound* sound = nullptr;
                result = channel->getCurrentSound(&sound);

                if (result == FMOD_OK && sound) {
                    char soundName[256];
                    unsigned int length = 0;

                    result = sound->getName(soundName, sizeof(soundName));
                    if (result == FMOD_OK) {
                        result = sound->getLength(&length, FMOD_TIMEUNIT_MS);
                        if (result == FMOD_OK) {
							ImGuiConsole::Cout("Channel %d: Playing sound \"%s\" with length %d ms", i, soundName, length);
                        }
                        else {
							ImGuiConsole::Cout("Failed to get length of sound on channel %d", i);
                        }
                    }
                    else {
						ImGuiConsole::Cout("Failed to get name of sound on channel %d", i);
                    }
                }
                else {
					ImGuiConsole::Cout("No sound assigned to channel %d", i);
                }
            }
            else {
				ImGuiConsole::Cout("Channel %d is not playing", i);
            }
        }
        else {
			ImGuiConsole::Cout("Failed to retrieve channel %d", i);
        }
    }
}

// Updates the audio system, including volume adjustments and channel management.
//void AudioManager::Update() {
//    if (audioSystem) {
//        audioSystem->mixerSuspend();
//        audioSystem->mixerResume();
//    }
// audioSystem->update(); 
//}

/**
 * @brief Checks if a specific audio ID is currently playing.
 * @param audioID The ID of the audio to check.
 * @return True if the audio is playing, false otherwise.
 */
bool AudioManager::IsPlaying(int audioID) {
    // If checking BGM
    if (AssetManager::GetInstance().audioObjects.find(audioID) != AssetManager::GetInstance().audioObjects.end() &&
        AssetManager::GetInstance().audioObjects[audioID]->GetType() == AudioType::BGM) {
        
        if (bgmChannel) {
            bool isPlaying = false;
            bgmChannel->isPlaying(&isPlaying);
            return isPlaying;
        }
        return false;
    }

    // If checking SFX
    auto it = audioChannels.find(audioID);
    if (it != audioChannels.end()) {
        for (FMOD::Channel* channel : it->second) {
            bool isPlaying = false;
            channel->isPlaying(&isPlaying);
            if (isPlaying) return true;  // If any channel is playing this sound, return true
        }
    }
    return false;
}

void AudioManager::FadeOutAudio(FMOD::Channel* channel, float duration) {
    if (channel) {
        float currentVolume = 1.0f;
        FMOD_RESULT result = channel->getVolume(&currentVolume);
        if (result != FMOD_OK) {
            ImGuiConsole::Cout("FadeOutAudio: Failed to get volume of channel, error code: %d", result);
            return;
        }

        fadingOutChannels.push_back({ channel, duration, 0.0f, currentVolume });
        ImGuiConsole::Cout("FadeOutAudio: Started fading out channel with initial volume %.2f over %.2f seconds.", currentVolume, duration);
    }
    else {
        ImGuiConsole::Cout("FadeOutAudio: Attempted to fade out a nullptr channel.");
    }
}

void AudioManager::CleanUpChannels() {
    for (auto it = audioChannels.begin(); it != audioChannels.end(); ) {
        auto& channelList = it->second;

        for (auto channelIt = channelList.begin(); channelIt != channelList.end(); ) {
            FMOD::Channel* channel = *channelIt;
            bool isPlaying = false;

            if (channel) {
                channel->isPlaying(&isPlaying);
            }

            if (!isPlaying) {
                channelIt = channelList.erase(channelIt);
            }
            else {
                ++channelIt;
            }
        }

        if (channelList.empty()) {
            it = audioChannels.erase(it);
        }
        else {
            ++it;
        }
    }
}

void AudioManager::FadeInAudio(FMOD::Channel* channel, float duration, float targetVolume) {
    if (channel) {
        channel->setVolume(0.0f);
        channel->setPaused(true); // Start fully paused
        fadingInChannels.push_back({ channel, duration, 0.0f, targetVolume });

        ImGuiConsole::Cout("FadeInAudio: Started fading in channel to volume %.2f over %.2f seconds.", targetVolume, duration);
    }
    else {
        ImGuiConsole::Cout("FadeInAudio: Attempted to fade in a nullptr channel.");
    }
}



void AudioManager::SetChannelVolume(int audioID, float volume) {
    auto* audio = AssetManager::GetInstance().audioObjects[audioID];
    if (!audio) {
        ImGuiConsole::Cout("SetChannelVolume: Audio ID %d not found", audioID);
        return;
    }

    if (audio->GetType() == AudioType::BGM) {
        bgmRawOverride = volume;
        if (bgmChannel) {
            bgmChannel->setVolume(bgmVolume * volume);
        }
    }
    else {
        sfxRawVolumes[audioID] = volume;
        auto it = audioChannels.find(audioID);
        if (it != audioChannels.end()) {
            for (FMOD::Channel* channel : it->second) {
                if (channel) {
                    channel->setVolume(sfxVolume * volume);
                }
            }
        }
    }
}


// Applies a smooth low-pass filter to the specified audio channel over a given duration.
void AudioManager::ApplyLowPassFilterSmooth(FMOD::Channel* channel, float targetFrequency, float duration) {
    if (!channel) return;

    targetFrequency = std::clamp(targetFrequency, 1500.0f, 20000.0f);

    if (!lowPassDSP) {
        audioSystem->createDSPByType(FMOD_DSP_TYPE_LOWPASS, &lowPassDSP);
    }

    int numDSPs = 0;
    channel->getNumDSPs(&numDSPs);
    bool alreadyAttached = false;

    for (int i = 0; i < numDSPs; ++i) {
        FMOD::DSP* existingDSP = nullptr;
        channel->getDSP(i, &existingDSP);
        if (existingDSP == lowPassDSP) {
            alreadyAttached = true;
            break;
        }
    }

    if (!alreadyAttached) {
        channel->addDSP(0, lowPassDSP);

        lowPassDSP->setParameterFloat(FMOD_DSP_LOWPASS_CUTOFF, 20000.0f);
    }

    float currentFreq = 0.0f;
    FMOD_RESULT result = lowPassDSP->getParameterFloat(FMOD_DSP_LOWPASS_CUTOFF, &currentFreq, nullptr, 0);

    if (result != FMOD_OK || currentFreq < 100.0f) {
        currentFreq = 20000.0f;
        lowPassDSP->setParameterFloat(FMOD_DSP_LOWPASS_CUTOFF, currentFreq);
    }

    lpTransition = {
        channel,
        currentFreq,
        targetFrequency,
        0.0f,
        duration,
        true
    };

#ifdef _IMGUI
#endif
}

//  Smoothly removes the low-pass filter from the specified audio channel over a given duration.
void AudioManager::RemoveLowPassFilterSmooth(FMOD::Channel* channel, float duration) {
    if (!channel || !lowPassDSP) return;
    ApplyLowPassFilterSmooth(channel, 20000.0f, duration);  // Fade to full clarity
}

// Immediately removes the low-pass filter from the specified audio channel.
void AudioManager::RemoveLowPassFilter(FMOD::Channel* channel) {
    if (!channel || !lowPassDSP) return;

    bool isActive = false;
    lowPassDSP->getActive(&isActive);
    if (isActive) {
        channel->removeDSP(lowPassDSP);
#ifdef _IMGUI
        ImGuiConsole::Cout("Low-pass filter removed.");
#endif
    }
}
