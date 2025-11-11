/*!****************************************************************
\file: InterruptionHandler.h
\author: Jeremy Lim Ting Jie, jeremytingjie.lim, 2301370
\brief: Header file for InterruptionHandler that declares the functions

Copyright (C) 2024 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents
without the prior written consent of DigiPen Institute of
Technology is prohibited.
*******************************************************************!*/
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "AudioManager.h"
#include "glhelper.h"
#include "engine.h"
/**
 * @class InterruptionHandler
 * @brief Handles window focus and iconification events, audio management, and input handling during focus transitions.
 *
 * This class is responsible for managing game behavior when the window gains or loses focus,
 * or when the window is minimized or restored. It handles pausing and resuming audio, as well as
 * enabling or disabling user input based on the window's focus state.
 */
class InterruptionHandler {
public:
    /**
     * @brief Initializes the interruption handler and sets GLFW callbacks for window focus and iconification events.
     *
     * @param window The GLFW window instance to set callbacks for.
     */
    static void Init(GLFWwindow* window);

    /**
     * @brief Callback function to handle window focus events.
     *
     * This function is called when the window gains or loses focus. It pauses or resumes the game
     * accordingly and restores or disables input based on the focus state.
     *
     * @param window The GLFW window instance.
     * @param focused Indicates if the window has gained (1) or lost (0) focus.
     */
    static void FocusCallback(GLFWwindow* window, int focused);

    /**
     * @brief Callback function to handle window iconification (minimization) events.
     *
     * This function is called when the window is minimized or restored. It pauses or resumes audio and
     * disables or restores input handling during minimization or restoration.
     *
     * @param window The GLFW window instance.
     * @param iconified Indicates if the window has been minimized (1) or restored (0).
     */
    static void IconifyCallback(GLFWwindow* window, int iconified);

    /**
     * @brief Pauses all audio when the window loses focus or is minimized.
     *
     * This function is called to pause all currently playing audio to prevent audio from continuing
     * when the game is not in focus.
     */
    static void PauseAudio();

    /**
     * @brief Resumes all audio when the window gains focus or is restored.
     *
     * This function is called to resume all audio that was previously paused when the window was
     * minimized or out of focus.
     */
    static void ResumeAudio();

    /**
     * @brief Disables input and processes game state when the window loses focus.
     *
     * This function is called when the window loses focus or is minimized. It saves the current input
     * states and disables further input processing until the window regains focus.
     */
    static void ProcessInputOnFocusLoss();

    /**
     * @brief Restores input handling when the window regains focus.
     *
     * This function restores input handling and clears any saved input states when the window regains focus
     * after being minimized or out of focus.
     */
    static void RestoreInputState();

}; 
// Static variables for tracking the state of audio and window iconification.
static bool isAudioPaused = false;
static bool isWindowIconified = false;
