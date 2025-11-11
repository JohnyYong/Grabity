/*!****************************************************************
\file: ImGuiConsole.h
\author: Mohamed Ridwhan Bin Mohamed Afandi (mohamedridhwan.b)
\brief: Declarations for funtions for ImGui in-game console.
		Contains functions to cout to the console and render the 
		console, with clear and copy functions.

Copyright (C) 2024 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents
without the prior written consent of DigiPen Institute of
Technology is prohibited.
*******************************************************************!*/


#pragma once
#include <imgui.h>

namespace ImGuiConsole {
	void Cout(const char* fmt, ...);
    void Clear();
    void Render(const char* title);
#ifdef _LOGGING
    // Internal variables
    extern ImGuiTextBuffer Buf;
    extern ImGuiTextFilter Filter;
    extern bool ScrollToBottom;
#endif
}
