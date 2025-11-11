/*!****************************************************************
\file: Imguimanager.h
\author: Moahmed Rudhwan Bin Mohamed Afandi, mohamedridhwan.b, 2301367
\brief: Declaration of functions for IMGUIManager class

Copyright (C) 2024 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents
without the prior written consent of DigiPen Institute of
Technology is prohibited.
*******************************************************************!*/
#ifndef IMGUIMANAGER_H
#define IMGUIMANAGER_H

#include <pch.h>
#include <glhelper.h>
#include <imgui.h>

/*!****************************************************************
\class IMGUIManager
\brief Singleton class for managing ImGui context and rendering.
*******************************************************************!*/
class IMGUIManager {
public:

    /*!****************************************************************
    \func  IMGUIManager::GetInstance
    \brief Retrieves the singleton instance of the IMGUIManager class.
    \return A reference to the singleton instance.
    *******************************************************************!*/
    static IMGUIManager& GetInstance();

    /*!****************************************************************
    \func  IMGUIManager::~IMGUIManager
    \brief Destructor that cleans up the ImGui context and associated
           resources.
    *******************************************************************!*/
    ~IMGUIManager();

    /*!****************************************************************
    \func  IMGUIManager::BeginFrame
    \brief Begins a new ImGui frame for rendering UI elements.
    *******************************************************************!*/
    static void BeginFrame();

    /*!****************************************************************
    \func  IMGUIManager::EndFrame
    \brief Ends the ImGui frame and renders the UI.
    *******************************************************************!*/
    static void EndFrame();

private:

    /*!****************************************************************
    \func  IMGUIManager::IMGUIManager
    \brief Constructor that initializes the ImGui context and sets
           up ImGui for OpenGL.
    *******************************************************************!*/
    IMGUIManager();
    IMGUIManager(const IMGUIManager&) = delete;
    IMGUIManager& operator=(const IMGUIManager&) = delete;
};

#endif // IMGUIMANAGER_H