/*!****************************************************************
\file: ImGuiConsole.cpp
\author: Mohamed Ridwhan Bin Mohamed Afandi (mohamedridhwan.b)
\brief: Definitions for funtions for ImGui in-game console.
        Contains functions to cout to the console and render the
        console, with clear and copy functions.

Copyright (C) 2024 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents
without the prior written consent of DigiPen Institute of
Technology is prohibited.
*******************************************************************!*/

#include "ImGuiConsole.h"



/*!****************************************************************
\namespace ImGuiConsole
\brief
	Contains functions to cout to the console and render the
	console, with clear and copy functions.
*******************************************************************/
namespace ImGuiConsole {
    ImGuiTextBuffer Buf;
    ImGuiTextFilter Filter;
    ImVector<int> LineOffsets;
    bool ScrollToBottom = true;

/*!****************************************************************
\func Cout
\brief
	Prints the formatted string to the console in ImGui.
*******************************************************************/
    void Cout(const char* fmt, ...) {
#ifdef _LOGGING
        va_list args;
        va_start(args, fmt);
		Buf.appendf("[%02d:%02d] ", (int)ImGui::GetTime() / 60, (int)ImGui::GetTime() % 60);
        Buf.appendfv(fmt, args);
        Buf.append("\n");
        va_end(args);
        ScrollToBottom = true;
#else
		(void)fmt;
#endif
    }


/*!****************************************************************
\func Clear
\brief
	Clears the console in ImGui.
*******************************************************************/
    void Clear() {
        Buf.clear();
        LineOffsets.clear();
    }

/*!****************************************************************
\func Render
\brief
	Renders the console in ImGui. Contains buttons to clear and copy
	the console.
*******************************************************************/
    void Render(const char* title) {
        if (!ImGui::Begin(title)) {
            ImGui::End();
            return;
        }

        // Main window
        ImGui::SameLine();
        if (ImGui::Button("Clear"))
            Clear();
        ImGui::SameLine();
        bool copy_to_clipboard = ImGui::Button("Copy");
        ImGui::SameLine();
        Filter.Draw("Filter", -120.0f);

        ImGui::Separator();
        ImGui::BeginChild("scrolling", ImVec2(0, 0), false, ImGuiWindowFlags_HorizontalScrollbar);

        if (copy_to_clipboard)
            ImGui::LogToClipboard();

        if (Filter.IsActive()) {
            const char* buf_begin = Buf.begin();
            const char* line = buf_begin;
            for (int line_no = 0; line != NULL; line_no++) {
                const char* line_end = (line_no < LineOffsets.Size) ? buf_begin + LineOffsets[line_no] : NULL;
                if (Filter.PassFilter(line, line_end))
                    ImGui::TextUnformatted(line, line_end);
                line = line_end && line_end[1] ? line_end + 1 : NULL;
            }
        }
        else {
            ImGui::TextUnformatted(Buf.begin());
        }

        if (ScrollToBottom)
            ImGui::SetScrollHereY(1.0f);
        ScrollToBottom = false;

        ImGui::EndChild();
        ImGui::End();
    }
}
