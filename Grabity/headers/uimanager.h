#pragma once
#include <pch.h>

namespace UI {
    class ImGuiLayer {
    public:
        void Init();
        void Begin();
        void End();
        void Shutdown();
    };
}
