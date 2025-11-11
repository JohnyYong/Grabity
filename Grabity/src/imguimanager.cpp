/*!****************************************************************
\file: Imguimanager.cpp
\author: Moahmed Rudhwan Bin Mohamed Afandi, mohamedridhwan.b, 2301367
\brief: This file contains the implementation of the IMGUIManager class
which is a wrapper for the ImGui library. It initializes the ImGui context
and provides functions to begin and end an ImGui frame.

Copyright (C) 2024 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents
without the prior written consent of DigiPen Institute of
Technology is prohibited.
*******************************************************************!*/
#ifdef _IMGUI
#include "Imguimanager.h"

// Singleton instance
IMGUIManager& IMGUIManager::GetInstance() {
    static IMGUIManager instance;
    return instance;
}

// Constructor: Initialize ImGui context
IMGUIManager::IMGUIManager() {
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(InputManager::ptrWindow, true);
    ImGui_ImplOpenGL3_Init("#version 450");
}

// Destructor: Cleanup ImGui context
IMGUIManager::~IMGUIManager() {
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}

// Begin a new ImGui frame
void IMGUIManager::BeginFrame() {
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
}

// End the ImGui frame and render
void IMGUIManager::EndFrame() {
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

#endif // _IMGUI