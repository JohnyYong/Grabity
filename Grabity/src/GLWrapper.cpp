/*!****************************************************************
\file: GLWrapper.cpp
\author: Moahmed Rudhwan Bin Mohamed Afandi, mohamedridhwan.b, 2301367
\brief: This file contains the implementation of the GLWrapper
functions that are used to wrap OpenGL functions to log their calls 
and durations.When InitializeGLWrappers is called, the functions
are wrapped for MAX_EVENTSnumber of calls. After MAX_EVENTS number
of calls, the wrappers are terminated.The log file is named
gl_log_sample.log and is written to the same directory as the
executable.

Copyright (C) 2024 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents
without the prior written consent of DigiPen Institute of
Technology is prohibited.
*******************************************************************!*/
#ifdef _LOGGING

#ifdef _IMGUI
#include <iostream>
#endif // _IMGUI
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "GLWrapper.h"
#include "filemanager.h"
#include <unordered_map>
#include <sstream>

FileManager logFile("gl_log_sample.log");

#define MAX_EVENTS 1000
uint16_t eventIndex = 0;

float lastFrameTime=0.0f, frameDuration = 0.0f;
float callTime = 0.0f, callDuration = 0.0f;

std::unordered_map<GLenum, std::string> GLenumToString = {
    {GL_COLOR_BUFFER_BIT, "GL_COLOR_BUFFER_BIT"},
    {GL_DEPTH_BUFFER_BIT, "GL_DEPTH_BUFFER_BIT"},
    {GL_STENCIL_BUFFER_BIT, "GL_STENCIL_BUFFER_BIT"},
    {GL_TRIANGLES, "GL_TRIANGLES"},
    {GL_LINES, "GL_LINES"},
    {0, "Clear bit"}
};

std::string GetGLenumName(GLenum value) {
    auto it = GLenumToString.find(value);
    if (it != GLenumToString.end()) {
        return it->second;
    }
    return "Unknown GLenum: " + std::to_string(value);
}

// Wraps the glClear function to log the frame and call duration with mask information.
typedef void (APIENTRY* glClear_t)(GLbitfield mask);
glClear_t original_glClear = nullptr;
void APIENTRY glClear_wrapper(GLbitfield mask) {
    if (logFile.IsOpen() && eventIndex < MAX_EVENTS) {
		frameDuration = (static_cast<float>(glfwGetTime()) - lastFrameTime) * 1000.0f; // in milliseconds
        lastFrameTime = static_cast<float>(glfwGetTime());
		callDuration = (static_cast<float>(glfwGetTime()) - callTime) * 1000.0f; // in milliseconds
		callTime = static_cast<float>(glfwGetTime());
        logFile.Write("\n\nFrame took " + std::to_string(frameDuration) + "ms \n" + std::to_string(callDuration) + "ms - glClear called with mask : " + GetGLenumName(mask) + "\n");
        eventIndex++;
    }
    if (original_glClear) {
        original_glClear(mask);
    }
    if (eventIndex >= MAX_EVENTS) {
        TerminateGLWrappers();
    }
}

// Wraps the glDrawArrays function to log the call duration and parameters (mode, first, count).
typedef void (APIENTRY* glDrawArrays_t)(GLenum mode, GLint first, GLsizei count);
glDrawArrays_t original_glDrawArrays = nullptr;
void APIENTRY glDrawArrays_wrapper(GLenum mode, GLint first, GLsizei count) {
    if (logFile.IsOpen() && eventIndex < MAX_EVENTS) {
        callDuration = (static_cast<float>(glfwGetTime()) - callTime) * 1000.0f; // in milliseconds
        callTime = static_cast<float>(glfwGetTime());
        logFile.Write(std::to_string(callDuration) + "ms - glDrawArrays called with mode: " + GetGLenumName(mode) + ", first: " + std::to_string(first) + ", count: " + std::to_string(count) + "\n");
        eventIndex++;
    }
    if (original_glDrawArrays) {
        original_glDrawArrays(mode, first, count);
    }
    if (eventIndex >= MAX_EVENTS) {
        TerminateGLWrappers();
    }
}

// Wraps the glDrawElements function to log the call duration and parameters (mode, count, type, indices).
typedef void (APIENTRY* glDrawElements_t)(GLenum mode, GLsizei count, GLenum type, const void* indices);
glDrawElements_t original_glDrawElements = nullptr;
void APIENTRY glDrawElements_wrapper(GLenum mode, GLsizei count, GLenum type, const void* indices) {
    if (logFile.IsOpen() && eventIndex < MAX_EVENTS) {
        callDuration = (static_cast<float>(glfwGetTime()) - callTime) * 1000.0f; // in milliseconds
        callTime = static_cast<float>(glfwGetTime());
        logFile.Write(std::to_string(callDuration) + "ms - glDrawElements called with mode: " + GetGLenumName(mode) + ", count: " + std::to_string(count) + ", type: " + GetGLenumName(type) + ", indices: " + std::to_string(reinterpret_cast<std::uintptr_t>(indices)) + "\n");
        eventIndex++;
    }
    if (original_glDrawElements) {
        original_glDrawElements(mode, count, type, indices);
    }
    if (eventIndex >= MAX_EVENTS) {
        TerminateGLWrappers();
    }
}

// Wraps the glCompileShader function to log the call duration and shader parameter.
typedef void (APIENTRY* glCompileShader_t)(GLuint shader);
glCompileShader_t original_glCompileShader = nullptr;
void APIENTRY glCompileShader_wrapper(GLuint shader) {
	if (logFile.IsOpen() && eventIndex < MAX_EVENTS) {
		callDuration = (static_cast<float>(glfwGetTime()) - callTime) * 1000.0f; // in milliseconds
		callTime = static_cast<float>(glfwGetTime());
		logFile.Write(std::to_string(callDuration) + "ms - glCompileShader called with shader: " + std::to_string(shader) + "\n");
		eventIndex++;
	}
	if (original_glCompileShader) {
		original_glCompileShader(shader);
	}
	if (eventIndex >= MAX_EVENTS) {
		TerminateGLWrappers();
	}
}

// Wraps the glAttachShader function to log the call duration and parameters (program, shader).
typedef void (APIENTRY* glAttachShader_t)(GLuint program, GLuint shader);
glAttachShader_t original_glAttachShader = nullptr;
void APIENTRY glAttachShader_wrapper(GLuint program, GLuint shader) {
    if (logFile.IsOpen() && eventIndex < MAX_EVENTS) {
        callDuration = (static_cast<float>(glfwGetTime()) - callTime) * 1000.0f; // in milliseconds
        callTime = static_cast<float>(glfwGetTime());
        logFile.Write(std::to_string(callDuration) + "ms - glAttachShader called with program: " + std::to_string(program) + ", shader: " + std::to_string(shader) + "\n");
        eventIndex++;
    }
    if (original_glAttachShader) {
        original_glAttachShader(program, shader);
    }
    if (eventIndex >= MAX_EVENTS) {
        TerminateGLWrappers();
    }
}

// Wraps the glLinkProgram function to log the call duration and program parameter.
typedef void (APIENTRY* glLinkProgram_t)(GLuint program);
glLinkProgram_t original_glLinkProgram = nullptr;
void APIENTRY glLinkProgram_wrapper(GLuint program) {
    if (logFile.IsOpen() && eventIndex < MAX_EVENTS) {
        callDuration = (static_cast<float>(glfwGetTime()) - callTime) * 1000.0f; // in milliseconds
        callTime = static_cast<float>(glfwGetTime());
        logFile.Write(std::to_string(callDuration) + "ms - glLinkProgram called with program: " + std::to_string(program) + "\n");
        eventIndex++;
    }
    if (original_glLinkProgram) {
        original_glLinkProgram(program);
    }
    if (eventIndex >= MAX_EVENTS) {
        TerminateGLWrappers();
    }
}

// Wraps the glValidateProgram function to log the call duration and program parameter.
typedef void (APIENTRY* glValidateProgram_t)(GLuint program);
glValidateProgram_t original_glValidateProgram = nullptr;
void APIENTRY glValidateProgram_wrapper(GLuint program) {
    if (logFile.IsOpen() && eventIndex < MAX_EVENTS) {
        callDuration = (static_cast<float>(glfwGetTime()) - callTime) * 1000.0f; // in milliseconds
        callTime = static_cast<float>(glfwGetTime());
        logFile.Write(std::to_string(callDuration) + "ms - glValidateProgram called with program: " + std::to_string(program) + "\n");
        eventIndex++;
    }
    if (original_glValidateProgram) {
        original_glValidateProgram(program);
    }
    if (eventIndex >= MAX_EVENTS) {
        TerminateGLWrappers();
    }
}


// Wraps the glGetUniformLocation function to log the call duration and parameters (program, name).
typedef GLint(APIENTRY* glGetUniformLocation_t)(GLuint program, const GLchar* name);
glGetUniformLocation_t original_glGetUniformLocation = nullptr;
GLint APIENTRY glGetUniformLocation_wrapper(GLuint program, const GLchar* name) {
    if (logFile.IsOpen() && eventIndex < MAX_EVENTS) {
        callDuration = (static_cast<float>(glfwGetTime()) - callTime) * 1000.0f; // in milliseconds
        callTime = static_cast<float>(glfwGetTime());
        logFile.Write(std::to_string(callDuration) + "ms - glGetUniformLocation called with program: " + std::to_string(program) + ", name: " + std::string(name) + "\n");
        eventIndex++;
    }
    GLint location = -1;
    if (original_glGetUniformLocation) {
        location = original_glGetUniformLocation(program, name);
    }
    if (eventIndex >= MAX_EVENTS) {
        TerminateGLWrappers();
    }
    return location;
}


// Wraps the glCreateShader function to log the call duration and shader type parameter.
typedef GLuint(APIENTRY* glCreateShader_t)(GLenum type);
glCreateShader_t original_glCreateShader = nullptr;
GLuint APIENTRY glCreateShader_wrapper(GLenum type) {
    if (logFile.IsOpen() && eventIndex < MAX_EVENTS) {
        callDuration = (static_cast<float>(glfwGetTime()) - callTime) * 1000.0f; // in milliseconds
        callTime = static_cast<float>(glfwGetTime());
        logFile.Write(std::to_string(callDuration) + "ms - glCreateShader called with type: " + GetGLenumName(type) + "\n");
        eventIndex++;
    }
    GLuint shader = 0;
    if (original_glCreateShader) {
        shader = original_glCreateShader(type);
    }
    if (eventIndex >= MAX_EVENTS) {
        TerminateGLWrappers();
    }
    return shader;
}

// Wraps the glUseProgram function to log the call duration and program parameter.
typedef void(APIENTRY* glUseProgram_t)(GLuint program);
glUseProgram_t original_glUseProgram = nullptr;
void APIENTRY glUseProgram_wrapper(GLuint program) {
    if (logFile.IsOpen() && eventIndex < MAX_EVENTS) {
        callDuration = (static_cast<float>(glfwGetTime()) - callTime) * 1000.0f; // in milliseconds
        callTime = static_cast<float>(glfwGetTime());
        logFile.Write(std::to_string(callDuration) + "ms - glUseProgram called with program: " + std::to_string(program) + "\n");
        eventIndex++;
    }
    if (original_glUseProgram) {
        original_glUseProgram(program);
    }
    if (eventIndex >= MAX_EVENTS) {
        TerminateGLWrappers();
    }
}


// Wraps the glDeleteShader function to log the call duration and shader parameter.
typedef void(APIENTRY* glDeleteShader_t)(GLuint shader);
glDeleteShader_t original_glDeleteShader = nullptr;
void APIENTRY glDeleteShader_wrapper(GLuint shader) {
    if (logFile.IsOpen() && eventIndex < MAX_EVENTS) {
        callDuration = (static_cast<float>(glfwGetTime()) - callTime) * 1000.0f; // in milliseconds
        callTime = static_cast<float>(glfwGetTime());
        logFile.Write(std::to_string(callDuration) + "ms - glDeleteShader called with shader: " + std::to_string(shader) + "\n");
        eventIndex++;
    }
    if (original_glDeleteShader) {
        original_glDeleteShader(shader);
    }
    if (eventIndex >= MAX_EVENTS) {
        TerminateGLWrappers();
    }
}

// Initializes the OpenGL wrapper functions.
void InitializeGLWrappers() {
    original_glClear = (glClear_t)glad_glClear;
    glad_glClear = glClear_wrapper;

    original_glDrawArrays = (glDrawArrays_t)glad_glDrawArrays;
    glad_glDrawArrays = glDrawArrays_wrapper;

    original_glDrawElements = (glDrawElements_t)glad_glDrawElements;
    glad_glDrawElements = glDrawElements_wrapper;

    original_glCompileShader = (glCompileShader_t)glad_glCompileShader;
    glad_glCompileShader = glCompileShader_wrapper;

    original_glAttachShader = (glAttachShader_t)glad_glAttachShader;
    glad_glAttachShader = glAttachShader_wrapper;

    original_glLinkProgram = (glLinkProgram_t)glad_glLinkProgram;
    glad_glLinkProgram = glLinkProgram_wrapper;

    original_glValidateProgram = (glValidateProgram_t)glad_glValidateProgram;
    glad_glValidateProgram = glValidateProgram_wrapper;

    original_glGetUniformLocation = (glGetUniformLocation_t)glad_glGetUniformLocation;
    glad_glGetUniformLocation = glGetUniformLocation_wrapper;

    original_glCreateShader = (glCreateShader_t)glad_glCreateShader;
    glad_glCreateShader = glCreateShader_wrapper;

    original_glUseProgram = (glUseProgram_t)glad_glUseProgram;
    glad_glUseProgram = glUseProgram_wrapper;

    original_glDeleteShader = (glDeleteShader_t)glad_glDeleteShader;
    glad_glDeleteShader = glDeleteShader_wrapper;
}

// Terminates the OpenGL wrapper functions.
void TerminateGLWrappers() {
    glad_glClear = original_glClear;
    glad_glDrawArrays = original_glDrawArrays;
    glad_glDrawElements = original_glDrawElements;
    glad_glCompileShader = original_glCompileShader;
    glad_glAttachShader = original_glAttachShader;
    glad_glLinkProgram = original_glLinkProgram;
    glad_glValidateProgram = original_glValidateProgram;
    glad_glGetUniformLocation = original_glGetUniformLocation;
    glad_glCreateShader = original_glCreateShader;
    glad_glUseProgram = original_glUseProgram;
    glad_glDeleteShader = original_glDeleteShader;
}
#endif // _LOGGING