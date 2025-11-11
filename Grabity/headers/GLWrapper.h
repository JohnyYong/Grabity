/*!****************************************************************
\file: GLWrapper.h
\author: Moahmed Rudhwan Bin Mohamed Afandi, mohamedridhwan.b, 2301367
\brief: Declaration of functions for GLWrapper

Copyright (C) 2024 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents
without the prior written consent of DigiPen Institute of
Technology is prohibited.
*******************************************************************!*/
#ifdef _IMGUI
#pragma once

/*!****************************************************************
\func  InitializeGLWrappers
\brief Initializes the OpenGL wrapper functions.
\details This function sets up all necessary OpenGL contexts and prepares the graphics system for rendering. It should be called before any OpenGL-related operations.
*******************************************************************!*/
void InitializeGLWrappers();

/*!****************************************************************
\func  TerminateGLWrappers
\brief Terminates the OpenGL wrapper functions.
\details This function cleans up and releases all OpenGL-related resources and contexts. It should be called when OpenGL rendering is no longer needed to avoid resource leaks.
*******************************************************************!*/
void TerminateGLWrappers();


#endif // _IMGUI