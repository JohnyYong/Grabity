/*!****************************************************************
\file: Shader.cpp
\author: Teng Shi Heng, shiheng.teng, 2301269
\brief:
    The `Shader` class member function definition where it manages
    the compilation and use of vertex and fragment shaders in OpenGL.
    At the same also providing methods for setting shader source code
    and uniform values.

Copyright (C) 2024 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents
without the prior written consent of DigiPen Institute of
Technology is prohibited.
*******************************************************************!*/

#include "Shader.h"


#ifdef _IMGUI
#include <iostream>
#endif // _IMGUI
#include "ImGuiConsole.h"
#include <fstream>
#include <sstream>

#include "glhelper.h"

// default constructor
Shader::Shader() : mFilePath(""), mRendererID(0)
{

}

// load and compile the shader from the specified file path.
void Shader::SetShader(const std::string& filepath)
{
    mFilePath = filepath;
    mRendererID = 0;
    ShaderProgramSource source = ParseShader(filepath);
    mRendererID = CreateShader(source.VertexSource, source.FragmentSource);
}

// destructor for shader
Shader::~Shader()
{

}

//  extracts the vertex and fragmen source code and return in a ShaderProgramSource struct
ShaderProgramSource Shader::ParseShader(const std::string& filepath)
{
	//RIDHWAN: Modified to ifstream to read the shader file
    std::ifstream stream(filepath);

    std::string line;
    std::stringstream ss[2];
    int type = 0; // starts from storing the source code the vertex shader program
    while (std::getline(stream, line))
    {
        if (line.find("#shader") != std::string::npos)
        {
            if (line.find("vertex") != std::string::npos)
                type = 0;
            else if (line.find("fragment") != std::string::npos)
                type = 1;
        }
        else
        {
            ss[type] << line << '\n';
        }
    }

    return { ss[0].str(),ss[1].str() };
}

// Compiles a shader from the given vertex or fragment shader source code
unsigned int Shader::CompileShader(unsigned int type, const std::string& source)
{
    unsigned int id = glCreateShader(type);
    const char* src = source.c_str();
    glShaderSource(id, 1, &src, nullptr);
    glCompileShader(id);

    int result;
    glGetShaderiv(id, GL_COMPILE_STATUS, &result);

    // if false to compile the shader program
    if (result == GL_FALSE)
    {
        int length;
        glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length);
        char* message = new char[length];
        glGetShaderInfoLog(id, length, &length, message);
#ifdef _LOGGING

		ImGuiConsole::Cout("Failed to compile %s shader! ", (type == GL_VERTEX_SHADER ? "vertex" : "Fragment"));
        ImGuiConsole::Cout(message);
#endif // _LOGGING
        glDeleteShader(id);
        return 0;
    }

    return id;
}

// create the shader program and returns back the generated shader program ID
unsigned int Shader::CreateShader(const std::string& vertexShader, const std::string& fragmentShader)
{
    // generate a shader program ID
    unsigned int program = glCreateProgram();

    // compile the vertex shader program and return back the ID
    unsigned int vs = CompileShader(GL_VERTEX_SHADER, vertexShader);

    // compile the vertex shader program and return back the ID
    unsigned int fs = CompileShader(GL_FRAGMENT_SHADER, fragmentShader);

    // attach both the compiled shaders to the program to link and validate
    glAttachShader(program, vs);
    glAttachShader(program, fs);

    // link the shaders into a complete shader program
    glLinkProgram(program);

    // validate the shader program to ensure it is correctly set up
    glValidateProgram(program);

    // clean up the individual shader objects as they are no longer needed
    glDeleteShader(vs);
    glDeleteShader(fs);

    return program;
}

//  binds the shader program
void Shader::Bind() const
{
    glUseProgram(mRendererID);
}

//  unbinds the shader program
void Shader::Unbind() const
{
    glUseProgram(0);
}

// sets a uniform integer in the shader.
void Shader::SetUniform1i(const std::string& name, int v0)
{
    glUniform1i(GetUniformLocation(name), v0);
}

void Shader::SetUniform1iv(const std::string& name)
{
    int sampler2D[32];
    for (unsigned index = 0; index < 32; ++index)sampler2D[index] = index;
    glUniform1iv(GetUniformLocation(name), 32, sampler2D);
}

// sets a uniform float in the shader.
void Shader::SetUniform1f(const std::string& name, float v0)
{
    glUniform1f(GetUniformLocation(name), v0);
}

// sets a uniform float vector 3 in the shader.
void Shader::SetUniform2f(const std::string& name, float v0, float v1)
{
    glUniform2f(GetUniformLocation(name), v0, v1);
}

// sets a uniform float vector 3 in the shader.
void Shader::SetUniform3f(const std::string& name, float v0, float v1, float v2)
{
    glUniform3f(GetUniformLocation(name), v0, v1, v2);
}

// sets a uniform float vector 4 in the shader.
void Shader::SetUniform4f(const std::string& name, float v0, float v1, float v2, float v3)
{
    glUniform4f(GetUniformLocation(name), v0, v1, v2, v3);
}

// sets a uniform 4x4 matrix in the shader.
void Shader::SetUniformMat4x4f(const std::string& name, Matrix4x4 matrix)
{
    // glm goes by column major instead of row major, need to switch the data position
    float m_matrix[4][4]{   { matrix(0,0), matrix(1,0), matrix(2,0), matrix(3,0)},
                            { matrix(0,1), matrix(1,1), matrix(2,1), matrix(3,1)},
                            { matrix(0,2), matrix(1,2), matrix(2,2), matrix(3,2)},
                            { matrix(0,3), matrix(1,3), matrix(2,3), matrix(3,3)}
    };
    glUniformMatrix4fv(GetUniformLocation(name), 1, GL_FALSE, &m_matrix[0][0]);
}

// get the uniform location stored in the shader program
int Shader::GetUniformLocation(const std::string& name)
{
    // if it was previously, just return the id to prevent multiple searches of the same time
    // to reduce the calling to the shaders which is costly
    if (mUniformLocationCache.find(name) != mUniformLocationCache.end())
        return mUniformLocationCache[name];

    // get the location of the variable
    int location = glGetUniformLocation(mRendererID, name.c_str());

#ifdef _LOGGING
    if (location == -1)
		ImGuiConsole::Cout("Warning: uniform '%s' doesn't exist!", name.c_str());
#endif // _LOGGING
    // store the found location into the unordered map if needed in the future
    mUniformLocationCache[name] = location;
    return location;
}