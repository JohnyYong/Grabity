/*!****************************************************************
\file: Shader.h
\author: Teng Shi Heng, shiheng.teng, 2301269
\brief:
    The `Shader` class manages the compilation and use of vertex
    and fragment shaders in OpenGL, providing methods for setting
    shader source code and uniform values.

Copyright (C) 2024 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents
without the prior written consent of DigiPen Institute of
Technology is prohibited.
*******************************************************************!*/

#pragma once

#include <string>
#include <unordered_map>
#include "Matrix4x4.h"

/*!****************************************************************
\brief
    Structure to hold the source code of a vertex and fragment shader.
*******************************************************************!*/
struct ShaderProgramSource
{
	std::string VertexSource;
	std::string FragmentSource;
};

/*!****************************************************************
\brief
    The `Shader` class handles shader compilation, linking, and
    uniform management for OpenGL rendering.
*******************************************************************!*/
class Shader
{
public:

    /*!****************************************************************
    \brief
        Default Constructor that initializes the shader program.
    *******************************************************************!*/
    Shader();

    /*!****************************************************************
    \brief
        Destructor that cleans up shader resources.
    *******************************************************************!*/
    ~Shader();

    /*!****************************************************************
    \brief
        Binds the shader program for use in rendering.
    *******************************************************************!*/
    void Bind() const;

    /*!****************************************************************
    \brief
        Unbinds the shader program.
    *******************************************************************!*/
    void Unbind() const;

    /*!****************************************************************
   \brief
       Loads and compiles the shader from the specified file path.

   \param filepath
       The path to the shader file.
   *******************************************************************!*/
    void SetShader(const std::string& filepath);

    /*!****************************************************************
    \brief
        Retrieves the renderer ID of the shader program.

    \return
        A const reference to the renderer ID.
    *******************************************************************!*/
    inline const unsigned int& GetShaderID() { return mRendererID; }

    /*!****************************************************************
    \brief
        Sets a uniform integer value in the shader.

    \param name
        The name of the uniform variable.

    \param v0
        The integer value to set.
    *******************************************************************!*/
    void SetUniform1i(const std::string& name, int v0);
    
    void SetUniform1iv(const std::string& name);

    /*!****************************************************************
    \brief
        Sets a uniform float value in the shader.

    \param name
        The name of the uniform variable.

    \param v0
        The float value to set.
    *******************************************************************!*/
    void SetUniform1f(const std::string& name, float v0);

    void SetUniform2f(const std::string& name, float v0, float v1);

    /*!****************************************************************
    \brief
        Sets a uniform float vector 3 in the shader.

    \param name
        The name of the uniform variable.

    \param v0, v1, v2
        The float values for the vector 3.
    *******************************************************************!*/
    void SetUniform3f(const std::string& name, float v0, float v1, float v2);

    /*!****************************************************************
    \brief
        Sets a uniform float vector 4 in the shader.

    \param name
        The name of the uniform variable.

    \param v0, v1, v2, v3
        The float values for the vector 4.
    *******************************************************************!*/
    void SetUniform4f(const std::string& name, float v0, float v1, float v2, float v3);

    /*!****************************************************************
    \brief
        Sets a uniform 4x4 matrix in the shader.

    \param name
        The name of the uniform variable.

    \param matrix
        The 4x4 matrix to set.
    *******************************************************************!*/
    void SetUniformMat4x4f(const std::string& name, Matrix4x4 matrix);

private:
    /*!****************************************************************
    \brief
        Parses the shader file and extracts the vertex and fragment
        source code.

    \param filepath
        The path to the shader file.

    \return
        A `ShaderProgramSource` struct containing the vertex and
        fragment shader source.
    *******************************************************************!*/
    ShaderProgramSource ParseShader(const std::string& filepath);

    /*!****************************************************************
   \brief
       Retrieves the location of a uniform variable in the shader.

   \param name
       The name of the uniform variable.

   \return
       The location of the uniform variable.
   *******************************************************************!*/
    int GetUniformLocation(const std::string& name);

    /*!****************************************************************
    \brief
        Compiles a shader from the given source code.

    \param type
        The type of shader (vertex or fragment).

    \param source
        The source code of the shader.

    \return
        The OpenGL ID of the compiled shader.
    *******************************************************************!*/
    unsigned int CompileShader(unsigned int type, const std::string& source);

    /*!****************************************************************
    \brief
        Creates a shader program by linking the vertex and fragment
        shaders.

    \param vertexShader
        The vertex shader source code.

    \param fragmentShader
        The fragment shader source code.

    \return
        The OpenGL ID of the created shader program.
    *******************************************************************!*/
    unsigned int CreateShader(const std::string& vertexShader, const std::string& fragmentShader);

    unsigned int mRendererID;                                   // OpenGL ID for the shader program
    std::string mFilePath;                                      // File path for the shader source
    std::unordered_map<std::string, int> mUniformLocationCache; // Cache for uniform locations
};