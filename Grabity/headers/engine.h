/*!****************************************************************
\file: Engine.h
\author: Moahmed Rudhwan Bin Mohamed Afandi, mohamedridhwan.b, 2301367
\co-author: Goh Jun Jie, g.junjie, 2301293
            Lee Yu Jie Brandon , l.yujiebrandon , 2301232
            Johny Yong Jun Siang, j.yong, 2301301
            Jeremy Lim Ting Jie, jeremytingjie.lim, 2301370
            Teng Shi Heng, shiheng.teng, 2301269
\brief: Handle initialization, updating, rendering, and state management,
        and coordinate these operations across the different managers and components.

        Moahmed Rudhwan Bin Mohamed Afandi (20%)
        Goh Jun Jie, g.junjie (16%)
        Lee Yu Jie Brandon (16%)
        Johny Yong Jun Siang (16%)
        Jeremy Lim Ting Jie (16%)
        Teng Shi Heng (16%)

Copyright (C) 2024 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents
without the prior written consent of DigiPen Institute of
Technology is prohibited.
*******************************************************************!*/
#ifndef ENGINE_H
#define ENGINE_H

#include <pch.h>
#include "Assetmanager.h"
#ifdef _IMGUI
#include <ContentBrowser.h>
#include <imgui_internal.h>
#endif // _IMGUI
#include <memory>
#include "CameraManager.h"
#include "ParticleSystem.h"

class Engine {
    static std::unique_ptr<Engine> instance;
    Engine(const Engine&) = delete;
    Engine& operator=(const Engine&) = delete;
    GLfloat clearColor[4];
#ifdef _IMGUI
	std::string stateFile = "Assets/Lua/state.lua";
    GameObject* selectedObject = nullptr;
#endif // _IMGUI

public:
#ifdef _IMGUI
    ImVec2 availableSizeEngine;
#endif // _IMGUI

    /*!****************************************************************
    \func  Engine
    \brief Constructor for the Engine class.
    *******************************************************************!*/
    Engine();

    /*!****************************************************************
    \func  ~Engine
    \brief Destructor for the Engine class.
    *******************************************************************!*/
    ~Engine();
    float time = 450.f;
    float maxTime = 450.f;
    /*!****************************************************************
    \func  GetInstance
    \brief Returns the singleton instance of the Engine class.
    *******************************************************************!*/
    static Engine& GetInstance();

    /*!****************************************************************
    \func  Init
    \brief Initializes the engine with the specified width and height.
    \param width The width of the engine window.
    \param height The height of the engine window.
    *******************************************************************!*/
    void Init(GLint width, GLint height);

    /*!****************************************************************
    \func  Update
    \brief Updates the engine (e.g., handles input, updates game logic).
    *******************************************************************!*/
    void Update();

    /*!****************************************************************
    \func  Draw
    \brief Draws the current scene.
    *******************************************************************!*/
    void Draw();
    CameraManager cameraManager;

    /*!****************************************************************
    \func  SaveStateToLua
    \brief Saves the current engine state to a Lua file.
    *******************************************************************!*/
	void SaveStateToLua();

    /*!****************************************************************
    \func  LoadStateFromLua
    \brief Loads the engine state from a Lua file.
    *******************************************************************!*/
	void LoadStateFromLua();

    /*!****************************************************************
    \func  LoadSceneFromLua
    \brief Loads a scene from the specified Lua file.
    \param luaFilePath The path to the Lua file containing the scene data.
    *******************************************************************!*/
	void LoadSceneFromLua(const std::string luaFilePath);

    /*!****************************************************************
    * \func  RestartScene
    * \brief Restarts the current scene.
    *******************************************************************!*/
    void RestartScene();

    /*!****************************************************************
    \func  Exit
    \brief Exits the engine and cleans up resources.
    *******************************************************************!*/
	void Exit();

    /*!****************************************************************
    \func  ScreenToWorldFull
    \brief Converts mouse position to world coordinates based on the camera and screen size.
    \param mousePos The mouse position in screen space.
    \param camera The camera used to calculate the world position.
    \param screenWidth The width of the screen.
    \param screenHeight The height of the screen.
    \return The world coordinates corresponding to the mouse position.
    *******************************************************************!*/
    Vector2 ScreenToWorldFull(const Vector2& mousePos, const Camera& camera, float screenWidth, float screenHeight);


    /*!****************************************************************
    \func  ScreenToWorldFull
    \brief Converts mouse position to world coordinates based on the camera and screen size.
    \param mousePos The mouse position in screen space.
    \param camera The camera used to calculate the world position.
    \param screenWidth The width of the screen.
    \param screenHeight The height of the screen.
    \return The world coordinates corresponding to the mouse position.
    *******************************************************************!*/
    Vector2 MouseToScreen(const Vector2& mousePos, const Camera& camera, float screenWidth, float screenHeight);



    /*!****************************************************************
    \func: Engine::UpdateFixedTimeStep
    \param: double currentTime
	\brief: Update the fixed time step variables (currentNumberOfSteps) 
    *******************************************************************/
	void UpdateFixedTimeStep(double deltaTime);

#ifdef _IMGUI

    /*!****************************************************************
    \func  MouseToScreenImGui
    \brief Converts mouse position to screen coordinates in ImGui context.
    \param mousePos The mouse position in world space.
    \param camera The camera used to calculate the screen position.
    \param screenWidth The width of the screen.
    \param screenHeight The height of the screen.
    \return The screen coordinates corresponding to the mouse position in ImGui.
    *******************************************************************!*/
    ImVec2 MouseToScreenImGui(const ImVec2& mousePos, const Camera& camera, float screenWidth, float screenHeight);
    
    /*!****************************************************************
    \func  ScreenToWorldImGui
    \brief Converts mouse position to world coordinates in ImGui context.
    \param mousePos The mouse position in screen space.
    \param camera The camera used to calculate the world position.
    \param screenWidth The width of the screen.
    \param screenHeight The height of the screen.
    \return The world coordinates corresponding to the mouse position in ImGui.
    *******************************************************************!*/
    ImVec2 ScreenToWorldImGui(const ImVec2& mousePos, const Camera& camera, float screenWidth, float screenHeight);

    /*!****************************************************************
    \func  GetMousePositionImGui
    \brief Retrieves the mouse position within an ImGui window.
    \param window The ImGui window in which the mouse position is queried.
    \return The mouse position within the ImGui window.
    *******************************************************************!*/
    ImVec2 GetMousePositionImGui(ImGuiWindow* window);
    
    /*!****************************************************************
    \func  SetSelectedObject
    \brief Sets the selected game object.
    \param selectedObj The game object to set as selected.
    *******************************************************************!*/
    void SetSelectedObject(GameObject* selectedObj) { selectedObject = selectedObj; }

    /*!****************************************************************
    \func  GetSelectedObject
    \brief Gets the currently selected game object.
    \return The currently selected game object.
    *******************************************************************!*/
    GameObject* GetSelectedObject() { return selectedObject; }

#endif // _IMGUI

#ifdef _IMGUI
    GLboolean isPaused = false;
    bool isInGameScene = false;
	ImGuiWindow* scenewindow = nullptr;
#else
	GLboolean isPaused = false;
	bool isInGameScene = true;
#endif // _IMGUI

    GLboolean showCursor = true;
    std::string originalScene = "";


    /*!****************************************************************
    Public Member Variables
    *******************************************************************!*/
    double fixedDeltaTimeMilli = static_cast<double>(16.666666666666667);
    double fixedDT = static_cast<double>(0.0166666666666667);
    int currentNumberOfSteps = 0;
    bool videoFinish = false;
    bool openingFinished = false;
    bool fadeIntoCutScene = false;
    bool isGodMode = false;

};
#endif // ENGINE_H
