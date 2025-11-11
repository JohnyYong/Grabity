/*!****************************************************************
\file: InterruptionHandler.cpp
\author: Jeremy Lim Ting Jie, jeremytingjie.lim, 2301370
\brief: This file implements the `InterruptionHandler` class, which 
        manages interruptions to the game caused by window focus loss, 
        minimization, or restoration. It handles pausing and resuming 
        the game state, audio playback, and input handling to ensure 
        a seamless user experience when the game window state changes. 

Copyright (C) 2024 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents
without the prior written consent of DigiPen Institute of
Technology is prohibited.
*******************************************************************!*/
#include "InterruptionHandler.h"

#ifdef _IMGUI
#include <iostream>
#include "ImGuiConsole.h"
#endif // _IMGUI
#define UNUSED(x) (void)(x)
static std::unordered_map<int, bool> keyStates;  // Stores key states (pressed/released)
static bool isInputEnabled = true;              // Tracks if input is currently enabled

/**
 * @brief Handles the change in window focus state.
 *
 * This method is called when the window gains or loses focus. It pauses or resumes
 * game audio and restores the input state based on whether the window is focused or not.
 *
 * @param window The GLFW window that triggered the callback.
 * @param focused The focus state of the window (1 for focused, 0 for unfocused).
 */
void InterruptionHandler::FocusCallback(GLFWwindow* window, int focused) {
    UNUSED(window);
    if (focused) {
        Engine& engine = Engine::GetInstance();
#ifdef _LOGGING
        ImGuiConsole::Cout("Window regained focus.");
#endif // _LOGGING
        engine.isPaused = false;
        if (isAudioPaused) {
            ResumeAudio();
            isAudioPaused = false;
        }
        RestoreInputState();
    }
    else {
        Engine& engine = Engine::GetInstance();
#ifdef _LOGGING
        ImGuiConsole::Cout("Window lost focus.");
#endif // _LOGGING
        engine.isPaused = true;
        if (!isAudioPaused) {
            PauseAudio();
            isAudioPaused = true;
        }
        ProcessInputOnFocusLoss();

        //minimize window
        glfwIconifyWindow(window);
    }
}

/**
 * @brief Handles changes in window iconification (minimization/restoration) state.
 *
 * This method is called when the window is minimized or restored. It pauses the audio
 * when the window is minimized and restores it when the window is restored.
 *
 * @param window The GLFW window that triggered the callback.
 * @param iconified The iconification state of the window (1 for minimized, 0 for restored).
 */
void InterruptionHandler::IconifyCallback(GLFWwindow* window, int iconified) {
    UNUSED(window);
    if (iconified) {
#ifdef _LOGGING
        ImGuiConsole::Cout("Window minimized.");
#endif // _LOGGING
        isWindowIconified = true;
        if (!isAudioPaused) {
            PauseAudio();
            isAudioPaused = true;
        }
        ProcessInputOnFocusLoss();
    }
    else {
#ifdef _LOGGING
        ImGuiConsole::Cout("Window restored.");
#endif // _LOGGING
        if (isWindowIconified) {
            isWindowIconified = false;
            if (isAudioPaused) {
                ResumeAudio();
                isAudioPaused = false;
            }
            RestoreInputState();
        }
    }
}

/**
 * @brief Pauses all currently playing audio.
 *
 * This method pauses all audio currently being played by the `AudioManager` instance.
 */
void InterruptionHandler::PauseAudio() {
    AudioManager::GetInstance().PauseAllAudio();
#ifdef _LOGGING
    ImGuiConsole::Cout("Audio paused.");
#endif // _LOGGING
}

/**
 * @brief Resumes all previously paused audio.
 *
 * This method resumes all audio that was previously paused by the `AudioManager` instance.
 */
void InterruptionHandler::ResumeAudio() {
    AudioManager::GetInstance().ResumeAllAudio();
#ifdef _LOGGING
    ImGuiConsole::Cout("Audio resumed.");
#endif // _LOGGING
}

/**
 * @brief Disables input and processes the game state when the window loses focus.
 *
 * This method disables input handling and saves the current key states when the window
 * loses focus. The game state is processed accordingly to handle focus loss.
 */
void InterruptionHandler::ProcessInputOnFocusLoss() {
    if (isInputEnabled) {
#ifdef _LOGGING
        ImGuiConsole::Cout("Disabling input on focus loss.");
#endif // _LOGGING

        // Save the current key states
        keyStates = InputManager::GetKeyStates();

        // Disable input handling in the InputManager
        InputManager::DisableInput();

        isInputEnabled = false;
    }

    // Optionally, notify the player
#ifdef _LOGGING
    ImGuiConsole::Cout("Game paused due to focus loss.");
#endif // _LOGGING
}

/**
 * @brief Restores input handling when the window regains focus.
 *
 * This method restores input handling and clears saved key states when the window
 * regains focus, enabling the game to resume as normal.
 */
void InterruptionHandler::RestoreInputState() {
    if (!isInputEnabled) {
#ifdef _LOGGING
        ImGuiConsole::Cout("Restoring input state after focus regain.");
#endif // _LOGGING

        // Restore input handling
        InputManager::EnableInput();

        // Clear saved states to prevent stale data
        keyStates.clear();

        isInputEnabled = true;
    }

    // Optionally notify the player
#ifdef _LOGGING
	ImGuiConsole::Cout("Game resumed after focus regain.");
#endif // _LOGGING
}

/**
 * @brief Initializes the interruption handler by setting GLFW callbacks.
 *
 * This method sets the appropriate GLFW callbacks for focus and iconification events.
 *
 * @param window The GLFW window for which the callbacks are being set.
 */
void InterruptionHandler::Init(GLFWwindow* window) {
    // Set GLFW callbacks
    glfwSetWindowFocusCallback(window, FocusCallback);
    glfwSetWindowIconifyCallback(window, IconifyCallback);
}
