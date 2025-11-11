#pragma once
#ifndef PCH_H
#define PCH_H

// Standard Library

#ifdef _IMGUI
#include <iostream>
#endif // _IMGUI
#include <vector>
#include <unordered_map>
#include <memory>
#include <string>
#include <string.h> //for strncpy_s
#include <typeindex>
#include <cmath>   
#include <algorithm> 
#include <cstdlib> 
#include <iomanip>
#include <array>
#include <stdexcept>
//Math Library
#include "Maths.h"
//Camera
//#include "Camera.h"
// OpenGL
#include <glad.h>
#include <GLFW/glfw3.h>

// ImGui
#ifdef _IMGUI
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#endif // _IMGUI

// Fmod
#include <fmod.hpp>
//#include <fmod_errors.h> //only included when needed to fetch audio errors

// FreeType
#include <ft2build.h>
#include FT_FREETYPE_H

#endif // PCH_H
