/*!****************************************************************
\file: GraphicsManager.h
\author: Mohamed Ridhwan Bin Mohamed Afandi, mohamedridhwan.b, 2301367
\co-author: Teng Shi Heng, shiheng.teng, 2301269

\brief
    This header file defines the GraphicsManager and GraphicsRender
    classes, which are responsible for managing and rendering graphics
    in the application. It includes the management of shaders, fonts,
    and geometric data used for rendering.

    Ridhwan created the file and the singleton for the graphics manager 
    and the Gizmos config. (5%)

    Shi Heng implemented the graphic rendering functionality, including
    shader management, transformation matrix calculation and rendering processes,
    building on Ridhwan's foundation (95%).

Copyright (C) 2024 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents
without the prior written consent of DigiPen Institute of
Technology is prohibited.
*******************************************************************/
#ifndef GRAPHICSMANAGER_H
#define GRAPHICSMANAGER_H

#include "objectmanager.h"
#include <vector>
#include <memory>
#include <string>
#include <unordered_map>
#include <sstream>

#ifdef _IMGUI
#include <iostream>
#endif // _IMGUI
#include <fstream>
#include "Texture.h"
#include "Shader.h"
#include "Font.h"
#include "Geometry.h"

namespace Graphics 
{
    /*!****************************************************************
    \brief
        Enumeration type on the kinds of shaders supported.
    *******************************************************************!*/
    enum ShaderType 
    {
        S_TEXTURE,
        S_FONT,
        S_GEOMETRY,
        S_PARTICLE,
        S_TOTAL
    };

    /*!****************************************************************
    \brief
        Enumeration type on the kinds of custom font supported.
    *******************************************************************!*/
    enum FontType 
    {
        F_SLEEPYSANS,
        F_ARIAL,
		F_TIMER,
        F_TOTAL
    };

    /*!****************************************************************
    \brief
        The 'GraphicsRender' that in charges of rendering things that 
        are needed to be in the game.
    *******************************************************************!*/
    class GraphicsRender
    {
    public:
        /*!****************************************************************
        \brief
            Initialize the GraphicsRender
        *******************************************************************!*/
        void Init();

        /*!****************************************************************
        \brief
            Render the game objects and other things such as gizmos for the
            gameobject
        *******************************************************************!*/
        void Render();

        /*!****************************************************************
        \brief
            Free up the resources used
        *******************************************************************!*/
        void Shutdown();

   // private:
        Shader shader[S_TOTAL];                                 // total number of shaders that is loaded
        Font font[F_TOTAL];                                     // total number of fonts that is loaded

        Geometry allGeomtryVertexData[GeometryType::G_TOTAL];   // total number of geometry types that is created

        /*!****************************************************************
        \brief
            Render quads using the provided vertices and index count.
        \param vertices
            Pointer to the array of vertices.
        \param indexCount
            Number of indices to be rendered.
        *******************************************************************!*/
        template<size_t N>
        void RenderQuads(const Vertex* vertices, uint32_t const& indexCount);

        /*!****************************************************************
        \brief
            Render fonts using the provided vertices and index count.
        \param vertices
            Pointer to the array of vertices.
        \param indexCount
            Number of indices to be rendered.
        *******************************************************************!*/
        template<size_t N>
        void RenderFonts(const Vertex* vertices, uint32_t const& indexCount);

        /*!****************************************************************
        \brief
            Render lines using the provided vertices and index count.
        \param vertices
            Pointer to the array of vertices.
        \param indexCount
            Number of indices to be rendered.
        *******************************************************************!*/
        template<size_t N>
        void RenderLines(const Vertex* vertices, uint32_t const& indexCount);

        /*!****************************************************************
        \brief
            Render points using the provided vertices and index count.
        \param vertices
            Pointer to the array of vertices.
        \param indexCount
            Number of indices to be rendered.
        *******************************************************************!*/
        template<size_t N>
        void RenderPoints(const Vertex* vertices, uint32_t const& indexCount);
    };

    /*!****************************************************************
    \brief
        The `GraphicsManager` class handles the graphics system within
        the application as a singleton.

        It is responsible for initializing, rendering, and shutting down
        graphics resources, utilizing the `GraphicsRender` component for
        rendering operations. 
    *******************************************************************!*/
    class GraphicsManager 
    {
    public:
        static GraphicsManager& GetInstance();

        /*!****************************************************************
        \brief
            Initialize the GraphicsManager
        *******************************************************************!*/
        void Init();

        /*!****************************************************************
        \brief
            Rendering the objects by calling the GraphicsRender Render() function
        *******************************************************************!*/
        void Render();

        /*!****************************************************************
        \brief
            Shutting down of the graphics resources
        *******************************************************************!*/
        void Shutdown();

        /*!****************************************************************
        \brief
            Default constructor
        *******************************************************************!*/
        GraphicsManager();

        /*!****************************************************************
        \brief
            Destructor to free the resources if any
        *******************************************************************!*/
        ~GraphicsManager();

    //private:
        static std::unique_ptr<GraphicsManager> instance;
        GraphicsRender renderer;
    };

    /*!****************************************************************
    \brief
        The `Texture` class handles hte loading and rendering of textures
        by binding it to the shader before drawing the gameobject and
        unbind it once is finished using.
    *******************************************************************!*/
    struct GizmosConfig {
        static bool showVelocity;
        static bool showBoundingBox;
    };

}

#endif // GRAPHICSMANAGER_H
