/*!****************************************************************
\file: GraphicsManager.cpp
\author: Mohamed Ridhwan Bin Mohamed Afandi, mohamedridhwan.b, 2301367
\co-author: Teng Shi Heng, shiheng.teng, 2301269

\brief
    This file defines the GraphicsManager and GraphicsRender
    classes member function defintion to render gameobjects into the
    game world based on the camera's orthographic perspective and view

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

#include "graphicsmanager.h"
#include <glhelper.h>
#include <filemanager.h>
#include "Camera.h"
#include "PlayerCamera.h"

#ifdef _IMGUI
#include <iostream>
#endif // _IMGUI
#include <fstream>
#include <sstream>
#include "engine.h"
#include "GameObjectFactory.h"
#include "Vector2.h"
#include "Matrix4x4.h"
#include "CameraManager.h"
#include <cstring>
#include <cstdio>
#include "assetmanager.h"
#include "PlayerSceneControls.h"

#define GIZMOSYSTEM



namespace Graphics
{

    std::unique_ptr<GraphicsManager> GraphicsManager::instance = nullptr;

    // instantiate flags
    bool GizmosConfig::showVelocity = false;
    bool GizmosConfig::showBoundingBox = false;

    std::array<Vertex, MaxVertexCount> vertices; // Pre-allocate vertex buffer
    std::array<Vertex, MaxBatchSize> lineVertices; // Pre-allocate vertex buffer
    std::array<Vertex, MaxBatchSize> velocityVertices; // Pre-allocate velocity buffer
    std::array<Vertex, MaxBatchSize> pointVertices; // Pre-allocate velocity buffer

    // Render quads using the provided vertices and index count.
    template<size_t N>
    void GraphicsRender::RenderQuads(const Vertex* vertices, uint32_t const& indexCount)
    {
        // Bind buffers and upload vertex data for rendering the current batch
        allGeomtryVertexData[G_QUAD].GetVertexArray().Bind();
        allGeomtryVertexData[G_QUAD].GetVertexBuffer().Bind();
        glBufferSubData(GL_ARRAY_BUFFER, 0, N * sizeof(Vertex), vertices);
        allGeomtryVertexData[G_QUAD].GetIndexBuffer().Bind();

        // Render all quads in the current batch
        glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, nullptr);

        // Unbind the buffers
        allGeomtryVertexData[G_QUAD].GetVertexArray().Unbind();
        allGeomtryVertexData[G_QUAD].GetVertexBuffer().Unbind();
        allGeomtryVertexData[G_QUAD].GetIndexBuffer().Unbind();
    }

    // Render fonts using the provided vertices and index count.
    template<size_t N>
    void GraphicsRender::RenderFonts(const Vertex* vertices, uint32_t const& indexCount)
    {
        // Flush current batch of text rendering
        allGeomtryVertexData[G_FONT].GetVertexArray().Bind();
        allGeomtryVertexData[G_FONT].GetVertexBuffer().Bind();
        allGeomtryVertexData[G_FONT].GetIndexBuffer().Bind();
        glBufferSubData(GL_ARRAY_BUFFER, 0, N * sizeof(Vertex), vertices);
        glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, nullptr);

        // Unbind the buffers after drawing
        allGeomtryVertexData[G_FONT].GetVertexArray().Unbind();
        allGeomtryVertexData[G_FONT].GetVertexBuffer().Unbind();
        allGeomtryVertexData[G_FONT].GetIndexBuffer().Unbind();
    }

    // Render lines using the provided vertices and index count.
    template<size_t N>
    void GraphicsRender::RenderLines(const Vertex* vertices, uint32_t const& indexCount)
    {

        allGeomtryVertexData[G_LINE_DYNAMIC].GetVertexArray().Bind();
        allGeomtryVertexData[G_LINE_DYNAMIC].GetVertexBuffer().Bind();
        glBufferSubData(GL_ARRAY_BUFFER, 0, N * sizeof(Vertex), vertices);
        allGeomtryVertexData[G_LINE_DYNAMIC].GetIndexBuffer().Bind();

        glDrawElements(GL_LINES, indexCount, GL_UNSIGNED_INT, nullptr);

        allGeomtryVertexData[G_LINE_DYNAMIC].GetVertexArray().Unbind();
        allGeomtryVertexData[G_LINE_DYNAMIC].GetVertexBuffer().Unbind();
        allGeomtryVertexData[G_LINE_DYNAMIC].GetIndexBuffer().Unbind();
    }

    // Render points using the provided vertices and index count.
    template<size_t N>
    void GraphicsRender::RenderPoints(const Vertex* vertices, uint32_t const& indexCount)
    {

        // Render velocity center points (GL_POINTS)
        allGeomtryVertexData[G_POINT].GetVertexArray().Bind();
        allGeomtryVertexData[G_POINT].GetVertexBuffer().Bind();
        allGeomtryVertexData[G_POINT].GetIndexBuffer().Bind();
        glPointSize(10.f);
        glBufferSubData(GL_ARRAY_BUFFER, 0, N * sizeof(Vertex), vertices);
        glDrawArrays(GL_POINTS, 0, indexCount);

        allGeomtryVertexData[G_POINT].GetVertexArray().Unbind();
        allGeomtryVertexData[G_POINT].GetVertexBuffer().Unbind();
        allGeomtryVertexData[G_POINT].GetIndexBuffer().Unbind();
    }

    // initialise all the class member variables
    void GraphicsRender::Init()
    {
        // Get asset manager instance and initialize graphics assets
        AssetManager& assetManager = AssetManager::GetInstance();
        assetManager.InitializeGraphicsAssets();

        shader[S_TEXTURE] = assetManager.GetShader(S_TEXTURE);
        shader[S_FONT] = assetManager.GetShader(S_FONT);
        shader[S_GEOMETRY] = assetManager.GetShader(S_GEOMETRY);


        // Enable blending for transparency
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        //Initialize geometry data for all types of geometry
        for (int index = 0; index < GeometryType::G_TOTAL; index++)
            allGeomtryVertexData[index].SetGeometryData(index);

        font[F_SLEEPYSANS] = assetManager.GetFont(F_SLEEPYSANS);
        font[F_ARIAL] = assetManager.GetFont(F_ARIAL);
        font[F_TIMER] = assetManager.GetFont(F_TIMER);
    }

    // render all the gameobjects onto the screen based on the number of game objects stored in the
    // game object factory and also render out the game object collider box and direction of movement
    // if the gizmos is being activated
    void GraphicsRender::Render()
    {
        // Clear the color and depth buffers to prepare for the new frame
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Access the engine and factory instances
        Engine& engine = Engine::GetInstance();
        GameObjectFactory& Fact = GameObjectFactory::GetInstance();
        const auto& gameObjects = Fact.GetAllGameObjects();

        bool isCanvasFound = false;

        std::vector<std::reference_wrapper<GameObject>> SpriteGameobjects;
        std::vector<std::reference_wrapper<GameObject>> TextGameobjects;

        std::vector<std::reference_wrapper<GameObject>> uiSpriteGameobjects;
        std::vector<std::reference_wrapper<GameObject>> uiTextGameobjects;

        std::vector<std::reference_wrapper<GameObject>> particleGameobjects;

        std::vector<std::string> allActiveLayer = LayerManager::GetInstance().GetActiveLayers();
        for (std::string str : allActiveLayer)
        {
            if (str == "UI")
            {
                std::vector<GameObject*> UI = LayerManager::GetInstance().GetSpecifiedLayer(str);
                for (GameObject* object : UI)
                {
                    CanvasComponent* canvas = object->GetComponent<CanvasComponent>(TypeOfComponent::CANVAS_UI);
                    if (canvas)
                    {
                        isCanvasFound = true;
                        UISpriteComponent* uispriteCanvas = object->GetComponent<UISpriteComponent>(TypeOfComponent::SPRITE_UI);
                        UITextComponent* uitextCanvas = object->GetComponent<UITextComponent>(TypeOfComponent::TEXT_UI);
                        if (uispriteCanvas)
                            uiSpriteGameobjects.push_back(*object);
                        if (uitextCanvas)
                            uiTextGameobjects.push_back(*object);
                        for (GameObject* obj : object->GetChildren())
                        {
                            if (obj->GetLayer() == "Default")
                            {
                                UISpriteComponent* uisprite = obj->GetComponent<UISpriteComponent>(TypeOfComponent::SPRITE_UI);
                                UITextComponent* uitext = obj->GetComponent<UITextComponent>(TypeOfComponent::TEXT_UI);
                                if (uisprite)
                                    uiSpriteGameobjects.push_back(*obj);
                                if (uitext)
                                    uiTextGameobjects.push_back(*obj);
                            }
                        }
                    }
                    else
                    {
                        UISpriteComponent* uisprite = object->GetComponent<UISpriteComponent>(TypeOfComponent::SPRITE_UI);
                        UITextComponent* uitext = object->GetComponent<UITextComponent>(TypeOfComponent::TEXT_UI);
                        if (uisprite)
                            uiSpriteGameobjects.push_back(*object);
                        if (uitext)
                            uiTextGameobjects.push_back(*object);
                    }
                }
            }
            else
            {
                std::vector<GameObject*> others = LayerManager::GetInstance().GetSpecifiedLayer(str);
                for (GameObject* obj : others)
                {
                    SpriteComponent* sprite = obj->GetComponent<SpriteComponent>(TypeOfComponent::SPRITE);
                    TextComponent* text = obj->GetComponent<TextComponent>(TypeOfComponent::TEXT);
                    ParticleSystem* particle = obj->GetComponent<ParticleSystem>(TypeOfComponent::PARTICLE);
                    if (sprite)
                        SpriteGameobjects.push_back(*obj);
                    if (text)
                        TextGameobjects.push_back(*obj);
                    if (particle)
                        particleGameobjects.push_back(*obj);
                }
            }
        }

        std::sort(SpriteGameobjects.begin(), SpriteGameobjects.end(), [](GameObject& lhs, GameObject& rhs) {
            return lhs.GetComponent<SpriteComponent>(TypeOfComponent::SPRITE)->GetLayer() < rhs.GetComponent<SpriteComponent>(TypeOfComponent::SPRITE)->GetLayer();
            });

        std::sort(uiSpriteGameobjects.begin(), uiSpriteGameobjects.end(), [](GameObject& lhs, GameObject& rhs) {
            return lhs.GetComponent<UISpriteComponent>(TypeOfComponent::SPRITE_UI)->GetLayer() < rhs.GetComponent<UISpriteComponent>(TypeOfComponent::SPRITE_UI)->GetLayer();
            });

        // Get the current camera from the CameraManager within the engine
        Camera* currentCamera = engine.cameraManager.GetCurrentCamera();

        // Get view and projection matrices from the current camera and combine them
        Matrix4x4 viewMatrix = currentCamera->GetViewMatrix();
        Matrix4x4 projectionMatrix = currentCamera->GetProjectionMatrix();
        Matrix4x4 viewProjMatrix = projectionMatrix * viewMatrix;


#pragma region GameObject_Rendering
        // Bind the texture shader and set up the uniform variables
        shader[S_TEXTURE].Bind();
        shader[S_TEXTURE].SetUniformMat4x4f("u_ViewProj", viewProjMatrix);
        shader[S_TEXTURE].SetUniform1iv("u_Texture"); // Set texture unit

        // Initialize the buffer pointer and index count for vertex data
        Vertex* buffer = vertices.data();
        uint32_t indexCount = 0;

        // Initialize texture slot tracking variables
        unsigned int texSlotIndex = 0;
        std::unordered_map<unsigned int, unsigned int> texSlotUsed;
        buffer = vertices.data();  // Reset buffer pointer for the batch

        // Loop over each game object to render sprites
        for (auto it = SpriteGameobjects.begin(); it != SpriteGameobjects.end(); ++it)
        {
            GameObject* gameOBJ1 = &it->get();
            SpriteComponent* sprite = gameOBJ1->GetComponent<SpriteComponent>(TypeOfComponent::SPRITE);

            // Skip if there�s no sprite component or sprite data
            if (!sprite || !sprite->GetCurrentSprite()) continue;

            unsigned int texID = sprite->GetCurrentSprite()->GetSpriteTexture()->GetTextureID();

            // Check if the texture ID has been assigned a slot index
            if (texSlotUsed.find(texID) == texSlotUsed.end())
            {
                // If no slots are available, flush the current batch before assigning a new slot
                if (texSlotIndex >= 32)
                {
                    RenderQuads<MaxVertexCount>(vertices.data(), indexCount);

                    // Reset buffer, index count, and texture slot tracking for the next batch
                    buffer = vertices.data();
                    indexCount = 0;
                    texSlotIndex = 0;
                    texSlotUsed.clear();
                }

                // Assign a new slot to the texture ID
                texSlotUsed[texID] = texSlotIndex++;
            }

            // Bind the texture to the assigned texture slot
            glBindTextureUnit(texSlotUsed[texID], texID);

            // Retrieve the transform component and apply transformations for position, scale, and rotation
            TransformComponent* trans = gameOBJ1->GetComponent<TransformComponent>(TypeOfComponent::TRANSFORM);
            Vector2 position = trans->GetPosition();
            Vector2 scale = trans->GetScale();
            float rotate = trans->GetRotation();

            // Create transformation matrices for translation, rotation, and scale
            Matrix4x4 translate4x4(Matrix4x4::Translation(position.x, position.y, 0.0f));
            Matrix4x4 scale4x4;

            // Adjust scale matrix based on sprite flip settings
            if (sprite->GetFlipX())
                scale4x4 = Matrix4x4::Scale(-scale.x, scale.y, 1.f);
            else if (sprite->GetFlipY())
                scale4x4 = Matrix4x4::Scale(-scale.x, -scale.y, 1.f);
            else
                scale4x4 = Matrix4x4::Scale(scale.x, scale.y, 1.f);

            Matrix4x4 rotate4x4(Matrix4x4::RotationZ(rotate));

            // Combine transformations into a single matrix
            Matrix4x4 transform = translate4x4 * rotate4x4 * scale4x4;

            // Populate the buffer with vertex data for the current quad
            buffer = CreateTextureQuad(buffer, transform, 0.f,
                (float)texSlotUsed[texID], sprite->GetCurrentSprite()->Get_UV_X(),
                sprite->GetCurrentSprite()->Get_UV_Y(), sprite->GetCurrentSprite()->GetSpriteTexture()->GetNxFrames(),
                sprite->GetCurrentSprite()->GetSpriteTexture()->GetNyFrames(), sprite->GetRGB());

            // Increase the index count for the quad (6 vertices per quad)
            indexCount += 6;

            // If max index count reached, submit the batch and reset for the next set of quads
            if (indexCount >= MaxIndexCount)
            {
                // Reset buffer, index count, and texture slot tracking for the next batch
                buffer = vertices.data();
                indexCount = 0;
                texSlotIndex = 0;
                texSlotUsed.clear();
            }
        }

        // If any quads are left in the buffer, submit them as a final batch
        if (indexCount > 0) {
            RenderQuads<MaxVertexCount>(vertices.data(), indexCount);
        }

        // Reset texture slot tracking variables for the next render cycle
        texSlotIndex = 0;
        texSlotUsed.clear();

#pragma region Particle
        {
            buffer = vertices.data();
            indexCount = 0;
            texSlotIndex = 0;

            // Iterate through all game objects to check for bounding boxes
            for (auto it = particleGameobjects.begin(); it != particleGameobjects.end(); ++it)
            {
                // Get collider and transform components
                ParticleSystem* collider = it->get().GetComponent<ParticleSystem>(TypeOfComponent::PARTICLE);

                if (!collider) continue;  // Skip if no collider

                // Calculate the collider's position and size
                auto particles = collider->GetParticlePoolData();

                // Create lines for bounding box edges
                for (size_t BigParticle = 0; BigParticle < particles.size(); ++BigParticle)
                {

                    unsigned int texID = particles[BigParticle].Animation->GetSpriteTexture()->GetTextureID();
                    SpriteAnimation* sprite = particles[BigParticle].Animation.get();

                    // Check if the texture ID has been assigned a slot index
                    if (texSlotUsed.find(texID) == texSlotUsed.end())
                    {
                        // If no slots are available, flush the current batch before assigning a new slot
                        if (texSlotIndex >= 32)
                        {
                            RenderQuads<MaxVertexCount>(vertices.data(), indexCount);

                            // Reset buffer, index count, and texture slot tracking for the next batch
                            buffer = vertices.data();
                            indexCount = 0;
                            texSlotIndex = 0;
                            texSlotUsed.clear();
                        }

                        // Assign a new slot to the texture ID
                        texSlotUsed[texID] = texSlotIndex++;
                    }

                    // Bind the texture to the assigned texture slot
                    glBindTextureUnit(texSlotUsed[texID], texID);

                    // Create transformation matrices for translation, rotation, and scale
                    Matrix4x4 translate4x4(Matrix4x4::Translation(particles[BigParticle].x, particles[BigParticle].y, 0.0f));
                    Matrix4x4 scale4x4;

                    // Adjust scale matrix based on sprite flip settings
                    scale4x4 = Matrix4x4::Scale(particles[BigParticle].currentSize.x * 100, particles[BigParticle].currentSize.y * 100, 1.f);

                    Matrix4x4 rotate4x4(Matrix4x4::RotationZ(0));

                    // Combine transformations into a single matrix
                    Matrix4x4 transform = translate4x4 * rotate4x4 * scale4x4;
                    buffer = CreateTextureQuad(buffer, transform, 0,
                        (float)texSlotUsed[texID], sprite->Get_UV_X(),
                        sprite->Get_UV_Y(), sprite->GetSpriteTexture()->GetNxFrames(),
                        sprite->GetSpriteTexture()->GetNyFrames(), Vector4(1.f, 1.1f, 1.f, 1.f));

                    indexCount += 6;

                    // If the batch size exceeds the max limit, submit and reset
                    if (indexCount >= MaxBatchSize)
                    {
                        RenderQuads<MaxBatchSize>(vertices.data(), indexCount);

                        indexCount = 0;  // Reset vertex count
                        buffer = vertices.data();
                    }

                }
            }

            // Submit any remaining vertices for bounding boxes
            if (indexCount > 0)
            {
                RenderQuads<MaxBatchSize>(vertices.data(), indexCount);
            }
        }

        // Unbind the texture shader
        shader[S_TEXTURE].Unbind();

#pragma endregion Particle

        // Bind the font shader program to render text
        shader[S_FONT].Bind();

        // Set the texture array and view-projection matrix for the shader
        shader[S_FONT].SetUniform1iv("u_Texture");
        shader[S_FONT].SetUniformMat4x4f("u_ViewProj", viewProjMatrix * Matrix4x4());

        // Initialize buffer and state variables
        buffer = vertices.data(); // Start buffer at the beginning of the vertices data
        indexCount = 0;           // Reset index count for new batch of text
        texSlotIndex = 0;         // Start from the first texture slot
        float advancePosX;        // Variable to keep track of the X position as text is rendered

        // Iterate over all game objects to render text for each one
        for (auto it = TextGameobjects.begin(); it != TextGameobjects.end(); ++it)
        {
            // Retrieve the TransformComponent and TextComponent for each game object
            TransformComponent* trans = it->get().GetComponent<TransformComponent>(TypeOfComponent::TRANSFORM);
            TextComponent* text = it->get().GetComponent<TextComponent>(TypeOfComponent::TEXT);

            // If either the TransformComponent or TextComponent is missing, skip this object
            if (!trans || !text) continue;

            // Start with the X position from the text component
            advancePosX = text->GetPosition().x;

            // Get the string of text to be rendered
            std::string textToPrint = text->GetText();

            // Loop through each character in the text string
            for (const char& c : textToPrint)
            {
                // Retrieve the character data (e.g., texture, size, etc.)
                Character ch;

                // Determine which font dictionary to use based on the font type
                if (text->GetFontType() == "F_SLEEPYSANS")
                    ch = font[F_SLEEPYSANS].GetCharacterDictionary().at(c);
                else if (text->GetFontType() == "F_ARIAL")
                    ch = font[F_ARIAL].GetCharacterDictionary().at(c);
                else
                    ch = font[F_TIMER].GetCharacterDictionary().at(c);

                unsigned int texID = ch.textureID; // Get the texture ID for the current character

                // Check if this texture has already been assigned a slot (to avoid reassigning)
                if (texSlotUsed.find(texID) == texSlotUsed.end())
                {
                    // If all texture slots are used, flush the current batch and reset
                    if (texSlotIndex >= 32)
                    {
                        RenderFonts<MaxVertexCount>(vertices.data(), indexCount);

                        // Reset buffers and texture slot indices for the next batch
                        buffer = vertices.data();
                        indexCount = 0;
                        texSlotIndex = 0;
                        texSlotUsed.clear();
                    }

                    // Assign the texture to the next available texture slot
                    texSlotUsed[texID] = texSlotIndex++;
                }

                // Bind the character's texture to the assigned texture slot
                glBindTextureUnit(texSlotUsed[texID], texID);

                // Calculate the position and size of the current character's quad
                float xpos = advancePosX + ch.bearing.x * text->GetFontSize(); // X position adjusted by bearing
                float ypos = text->GetPosition().y - (ch.size.y - ch.bearing.y) * text->GetFontSize(); // Y position adjusted by bearing
                float w = ch.size.x * text->GetFontSize(); // Character width adjusted by font size
                float h = ch.size.y * text->GetFontSize(); // Character height adjusted by font size

                // Add the character's quad (vertices) to the buffer
                buffer = CreateFontQuad(buffer, Vector2(xpos, ypos), 0.f, (float)texSlotUsed[texID], w, h, text->GetRGB());

                // Increase index count by 6 (two triangles per quad)
                indexCount += 6;

                // Advance the cursor for the next character based on its width
                advancePosX += (ch.advance / 64) * text->GetFontSize(); // Adjust by character's advance value

                // If we've reached the maximum index count, flush the batch and reset
                if (indexCount >= MaxIndexCount)
                {
                    RenderFonts<MaxVertexCount>(vertices.data(), indexCount);

                    // Reset buffers and texture slot indices for the next batch
                    buffer = vertices.data();
                    indexCount = 0;
                    texSlotIndex = 0;
                    texSlotUsed.clear();
                }
            }
        }

        // Final flush to render any remaining text if there is data left
        if (indexCount > 0)
        {
            RenderFonts<MaxVertexCount>(vertices.data(), indexCount);
        }

        texSlotIndex = 0;
        texSlotUsed.clear();

        shader[S_FONT].Unbind();

#pragma endregion GameObject_Rendering

#pragma region GameUI_Rendering
        // only render the canvas border of the camera fov when canvas is present and the scene is in editor mode
        if (engine.cameraManager.GetCurrentMode() == CameraManager::CameraMode::EditorCamera && isCanvasFound)
        {
            shader[S_GEOMETRY].Bind();
            shader[S_GEOMETRY].SetUniformMat4x4f("u_ViewProj", viewProjMatrix);

            Vertex* lineBuffer = lineVertices.data();  // Line buffer for drawing bounding boxes
            uint32_t batchVertexCount = 0; // To track the number of vertices per batch

            // Calculate the collider's position and size
            Vector2 size = engine.cameraManager.GetPlayerCamera().GetViewingRange() - Vector2(75.f, 0.f);
            Vector3 colliderCenter = { engine.cameraManager.GetPlayerCamera().GetCenter().x,engine.cameraManager.GetPlayerCamera().GetCenter().y,0.f };
            Vector2 halfSize = size; // Half size for corner calculations

            // Define the 4 corners of the bounding box
            Vector3 corners[4] = {
                { -halfSize.x,  halfSize.y, 1.f },
                {  halfSize.x,  halfSize.y, 1.f },
                {  halfSize.x, -halfSize.y, 1.f },
                { -halfSize.x, -halfSize.y, 1.f }
            };

            // Create lines for bounding box edges
            for (int i = 0; i < 4; ++i) {
                Vector3 start = corners[i] + colliderCenter;
                Vector3 end = corners[(i + 1) % 4] + colliderCenter;
                lineBuffer = CreateLine(lineBuffer, start, end, 0.f);
                batchVertexCount += 2;

            }

            RenderLines<MaxBatchSize>(lineVertices.data(), batchVertexCount);
        }

        // Bind the texture shader and set up the uniform variables
        shader[S_TEXTURE].Bind();
        shader[S_TEXTURE].SetUniformMat4x4f("u_ViewProj", viewProjMatrix);
        shader[S_TEXTURE].SetUniform1iv("u_Texture"); // Set texture unit

        // Initialize the buffer pointer and index count for vertex data
        buffer = vertices.data();
        indexCount = 0;

        // Initialize texture slot tracking variables
        texSlotIndex = 0;
        buffer = vertices.data();  // Reset buffer pointer for the batch

        // Loop over each game object to render sprites
        for (auto it = uiSpriteGameobjects.begin(); it != uiSpriteGameobjects.end(); ++it)
        {
            GameObject* gameOBJ1 = &it->get();

            if (gameOBJ1->GetParent() == nullptr || gameOBJ1->GetParent()->GetComponent<CanvasComponent>(TypeOfComponent::CANVAS_UI) == nullptr)
                continue;

            UISpriteComponent* sprite = gameOBJ1->GetComponent<UISpriteComponent>(TypeOfComponent::SPRITE_UI);

            // Skip if there�s no sprite component or sprite data
            if (!sprite || !sprite->GetCurrentSprite() || !sprite->GetIsRenderable() || !sprite->GetCurrentSprite()->GetSpriteTexture()) continue;

            unsigned int texID = sprite->GetCurrentSprite()->GetSpriteTexture()->GetTextureID();

            // Check if the texture ID has been assigned a slot index
            if (texSlotUsed.find(texID) == texSlotUsed.end())
            {
                // If no slots are available, flush the current batch before assigning a new slot
                if (texSlotIndex >= 32)
                {
                    RenderQuads<MaxVertexCount>(vertices.data(), indexCount);

                    // Reset buffer, index count, and texture slot tracking for the next batch
                    buffer = vertices.data();
                    indexCount = 0;
                    texSlotIndex = 0;
                    texSlotUsed.clear();
                }

                // Assign a new slot to the texture ID
                texSlotUsed[texID] = texSlotIndex++;
            }

            // Bind the texture to the assigned texture slot
            glBindTextureUnit(texSlotUsed[texID], texID);

            // Retrieve the transform component and apply transformations for position, scale, and rotation
            TransformComponent* trans = gameOBJ1->GetComponent<TransformComponent>(TypeOfComponent::TRANSFORM);
            Vector2 position = trans->GetPosition() + engine.cameraManager.GetPlayerCamera().GetCenter();
            Vector2 scale = trans->GetScale();
            float rotate = trans->GetRotation();

            // Create transformation matrices for translation, rotation, and scale
            Matrix4x4 translate4x4(Matrix4x4::Translation(position.x, position.y, 0.0f));
            Matrix4x4 scale4x4;

            // Adjust scale matrix based on sprite flip settings
            if (sprite->GetFlipX())
                scale4x4 = Matrix4x4::Scale(-scale.x, scale.y, 1.f);
            else if (sprite->GetFlipY())
                scale4x4 = Matrix4x4::Scale(-scale.x, -scale.y, 1.f);
            else
                scale4x4 = Matrix4x4::Scale(scale.x, scale.y, 1.f);

            Matrix4x4 rotate4x4(Matrix4x4::RotationZ(rotate));

            // Combine transformations into a single matrix
            Matrix4x4 transform = translate4x4 * rotate4x4 * scale4x4;

            // Populate the buffer with vertex data for the current quad
            buffer = CreateTextureQuad(buffer, transform, 0.f,
                (float)texSlotUsed[texID], sprite->GetCurrentSprite()->Get_UV_X(),
                sprite->GetCurrentSprite()->Get_UV_Y(), sprite->GetCurrentSprite()->GetSpriteTexture()->GetNxFrames(),
                sprite->GetCurrentSprite()->GetSpriteTexture()->GetNyFrames(), sprite->GetColor());

            // Increase the index count for the quad (6 vertices per quad)
            indexCount += 6;

            // If max index count reached, submit the batch and reset for the next set of quads
            if (indexCount >= MaxIndexCount)
            {
                RenderQuads<MaxVertexCount>(vertices.data(), indexCount);

                // Reset buffer, index count, and texture slot tracking for the next batch
                buffer = vertices.data();
                indexCount = 0;
                texSlotIndex = 0;
                texSlotUsed.clear();
            }
        }

        // If any quads are left in the buffer, submit them as a final batch
        if (indexCount > 0)
        {
            RenderQuads<MaxVertexCount>(vertices.data(), indexCount);
        }

        // Reset texture slot tracking variables for the next render cycle
        texSlotIndex = 0;
        texSlotUsed.clear();

        // Unbind the texture shader
        shader[S_TEXTURE].Unbind();

        shader[S_FONT].Bind();
        // Initialize buffer and state variables
        buffer = vertices.data(); // Start buffer at the beginning of the vertices data
        indexCount = 0;           // Reset index count for new batch of text
        texSlotIndex = 0;         // Start from the first texture slot

        // Iterate over all game objects to render text for each one
        for (auto it = uiTextGameobjects.begin(); it != uiTextGameobjects.end(); ++it)
        {
            // Retrieve the TransformComponent and TextComponent for each game object
            GameObject* gameOBJ = &it->get();

            if (gameOBJ->GetParent() == nullptr || gameOBJ->GetParent()->GetComponent<CanvasComponent>(TypeOfComponent::CANVAS_UI) == nullptr)
                continue;

            TransformComponent* trans = gameOBJ->GetComponent<TransformComponent>(TypeOfComponent::TRANSFORM);
            UITextComponent* text = gameOBJ->GetComponent<UITextComponent>(TypeOfComponent::TEXT_UI);

            if (text)
            {
                if (!text->GetRenderable())
                {
                    continue;
                }
            }

            // If either the TransformComponent or TextComponent is missing, skip this object
            if (!trans || !text) continue;

            // Start with the X position from the text component
            advancePosX = text->GetPosition().x;

            // Get the string of text to be rendered
            std::string textToPrint = text->GetText();

            // Loop through each character in the text string
            for (const char& c : textToPrint)
            {
                // Retrieve the character data (e.g., texture, size, etc.)
                Character ch;

                // Determine which font dictionary to use based on the font type
                if (text->GetFontType() == "F_SLEEPYSANS")
                    ch = font[F_SLEEPYSANS].GetCharacterDictionary().at(c);
                else if (text->GetFontType() == "F_ARIAL")
                    ch = font[F_ARIAL].GetCharacterDictionary().at(c);
                else
                    ch = font[F_TIMER].GetCharacterDictionary().at(c);

                unsigned int texID = ch.textureID; // Get the texture ID for the current character

                // Check if this texture has already been assigned a slot (to avoid reassigning)
                if (texSlotUsed.find(texID) == texSlotUsed.end())
                {
                    // If all texture slots are used, flush the current batch and reset
                    if (texSlotIndex >= 32)
                    {
                        RenderFonts<MaxVertexCount>(vertices.data(), indexCount);

                        // Reset buffers and texture slot indices for the next batch
                        buffer = vertices.data();
                        indexCount = 0;
                        texSlotIndex = 0;
                        texSlotUsed.clear();
                    }

                    // Assign the texture to the next available texture slot
                    texSlotUsed[texID] = texSlotIndex++;
                }

                // Bind the character's texture to the assigned texture slot
                glBindTextureUnit(texSlotUsed[texID], texID);

                // Calculate the position and size of the current character's quad
                float xpos = advancePosX + ch.bearing.x * text->GetFontSize(); // X position adjusted by bearing
                float ypos = text->GetPosition().y - (ch.size.y - ch.bearing.y) * text->GetFontSize(); // Y position adjusted by bearing
                float w = ch.size.x * text->GetFontSize(); // Character width adjusted by font size
                float h = ch.size.y * text->GetFontSize(); // Character height adjusted by font size

                // Add the character's quad (vertices) to the buffer
                buffer = CreateFontQuad(buffer, Vector2(xpos, ypos) + engine.cameraManager.GetPlayerCamera().GetCenter(), 0.f, (float)texSlotUsed[texID], w, h, text->GetRGB());

                // Increase index count by 6 (two triangles per quad)
                indexCount += 6;

                // Advance the cursor for the next character based on its width
                advancePosX += (ch.advance / 64) * text->GetFontSize(); // Adjust by character's advance value

                // If we've reached the maximum index count, flush the batch and reset
                if (indexCount >= MaxIndexCount)
                {
                    RenderFonts<MaxVertexCount>(vertices.data(), indexCount);

                    // Reset buffers and texture slot indices for the next batch
                    buffer = vertices.data();
                    indexCount = 0;
                    texSlotIndex = 0;
                    texSlotUsed.clear();
                }
            }
        }

        // Final flush to render any remaining text if there is data left
        if (indexCount > 0)
        {
            RenderFonts<MaxVertexCount>(vertices.data(), indexCount);
            texSlotUsed.clear();
        }

        // Unbind the font shader after all text has been rendered
        shader[S_FONT].Unbind();
        ////////////////////////////////////////////////////////////////////////////////////////////////

#pragma endregion  GameUI_Rendering

#pragma region Gizmos_Rendering

        // Bind the geometry shader for rendering
        shader[S_GEOMETRY].Bind();
        shader[S_GEOMETRY].SetUniformMat4x4f("u_ViewProj", viewProjMatrix);

        // Bounding Box Rendering: This section visualizes the collider boxes of objects
        if (GizmosConfig::showBoundingBox)
        {
            Vertex* lineBuffer = lineVertices.data();  // Line buffer for drawing bounding boxes
            uint32_t batchVertexCount = 0; // To track the number of vertices per batch

            // Iterate through all game objects to check for bounding boxes
            for (auto it = gameObjects.begin(); it != gameObjects.end(); ++it)
            {
                // Get collider and transform components
                GameObject* gameOBJ1 = it->second;
                RectColliderComponent* collider = gameOBJ1->GetComponent<RectColliderComponent>(TypeOfComponent::RECTCOLLIDER);
                TransformComponent* trans = gameOBJ1->GetComponent<TransformComponent>(TypeOfComponent::TRANSFORM);

                if (!collider) continue;  // Skip if no collider

                // Calculate the collider's position and size
                Vector2 size = collider->GetColliderData()[0].first;
                Vector2 offsetCenter = collider->GetColliderData()[0].second;
                Vector3 colliderCenter = { trans->GetPosition().x + offsetCenter.x, trans->GetPosition().y + offsetCenter.y, 0.f };
                Matrix4x4 rotationMatrix = Matrix4x4::RotationZ(trans->GetRotation());
                Vector2 halfSize = size * 0.5f; // Half size for corner calculations

                // Define the 4 corners of the bounding box
                Vector3 corners[4] = {
                    { -halfSize.x,  halfSize.y, 1.f },
                    {  halfSize.x,  halfSize.y, 1.f },
                    {  halfSize.x, -halfSize.y, 1.f },
                    { -halfSize.x, -halfSize.y, 1.f }
                };

                // Create lines for bounding box edges
                for (int i = 0; i < 4; ++i) {
                    Vector3 start = rotationMatrix * corners[i] + colliderCenter;
                    Vector3 end = rotationMatrix * corners[(i + 1) % 4] + colliderCenter;
                    lineBuffer = CreateLine(lineBuffer, start, end, 0.f);
                    batchVertexCount += 2;

                    // If the batch size exceeds the max limit, submit and reset
                    if (batchVertexCount >= MaxBatchSize)
                    {
                        RenderLines<MaxBatchSize>(lineVertices.data(), batchVertexCount);

                        batchVertexCount = 0;  // Reset vertex count
                        lineBuffer = lineVertices.data();
                    }
                }
            }

            // Submit any remaining vertices for bounding boxes
            if (batchVertexCount > 0)
            {
                RenderLines<MaxBatchSize>(lineVertices.data(), batchVertexCount);
            }
        }

        // Velocity Rendering: Visualize object velocities as lines and points
        if (GizmosConfig::showVelocity)
        {
            Vertex* velBuffer = velocityVertices.data();  // Buffer for velocity line vertices
            Vertex* pointBuffer = pointVertices.data();   // Buffer for velocity center points
            uint32_t batchVelVertexCount = 0;  // Vertex count for velocity lines
            uint32_t batchPointVertexCount = 0; // Vertex count for center points

            // Iterate through game objects to visualize velocity
            for (auto it = gameObjects.begin(); it != gameObjects.end(); ++it)
            {
                GameObject* gameOBJ1 = it->second;
                RigidBodyComponent* rigidBody = gameOBJ1->GetComponent<RigidBodyComponent>(TypeOfComponent::RIGIDBODY);
                TransformComponent* trans = gameOBJ1->GetComponent<TransformComponent>(TypeOfComponent::TRANSFORM);
                SpriteComponent* sprite = gameOBJ1->GetComponent<SpriteComponent>(TypeOfComponent::SPRITE);

                if (!rigidBody) continue;  // Skip if no rigid body (no velocity)

                // Get the object's velocity
                Vector2 velocity = rigidBody->GetVelocity();

                // Add velocity line vertices (start and end points)
                velBuffer->position = { trans->GetPosition().x, trans->GetPosition().y, (float)sprite->GetLayer() };
                velBuffer->color = { 0.0f, 0.2f, 0.7f, 1.0f };
                velBuffer++;

                velBuffer->position = { trans->GetPosition().x + (velocity.x * trans->GetScale().x * 0.01f),
                                        trans->GetPosition().y + (velocity.y * trans->GetScale().y * 0.01f),
                                        1.f };
                velBuffer->color = { 0.0f, 0.2f, 0.7f, 1.0f };
                velBuffer++;

                // Add point at the center of the velocity vector
                pointBuffer->position = { trans->GetPosition().x,
                                          trans->GetPosition().y,
                                          1.f };
                pointBuffer->color = { 0.0f, 0.2f, 0.7f, 1.0f };
                pointBuffer++;

                // Update vertex counts
                batchVelVertexCount += 2;  // 2 vertices for the velocity line
                batchPointVertexCount += 1; // 1 vertex for the center point

                // If the batch size exceeds the max limit, submit and reset
                if (batchVelVertexCount >= MaxBatchSize)
                {
                    RenderLines<MaxBatchSize>(velocityVertices.data(), batchVelVertexCount);
                    RenderPoints<MaxBatchSize>(pointVertices.data(), batchPointVertexCount);

                    // Reset counters and buffers for the next batch
                    batchVelVertexCount = 0;
                    batchPointVertexCount = 0;
                    velBuffer = velocityVertices.data();  // Buffer for velocity line vertices
                    pointBuffer = pointVertices.data();   // Buffer for velocity center points
                }
            }

            // Render any remaining vertices
            if (batchVelVertexCount > 0)
            {
                RenderLines<MaxBatchSize>(velocityVertices.data(), batchVelVertexCount);
                RenderPoints<MaxBatchSize>(pointVertices.data(), batchPointVertexCount);

                // Reset counters and buffers for the next batch
                batchVelVertexCount = 0;
                batchPointVertexCount = 0;
            }
        }

        shader[S_GEOMETRY].Unbind();

#pragma endregion Gizmos_Rendering

#pragma region PlayerSelect_Rendering
        //////////////////////////////////////////////////

        // only render the relevant selected object UI when in editor scene mode and an object is being selected
        if (PlayerSceneControls::GetInstance().GetSelectedObjectMode() != GameObjectEditorMode::None &&
            engine.cameraManager.GetCurrentMode() == CameraManager::CameraMode::EditorCamera)
        {
            shader[S_GEOMETRY].Bind();

            if (PlayerSceneControls::GetInstance().GetSelectedGameObject() != nullptr)
            {
                GameObject* gameOBJ1 = PlayerSceneControls::GetInstance().GetSelectedGameObject();
                if (gameOBJ1 == nullptr || gameOBJ1->GetName() == "") {
                    return;
                }
                TransformComponent* trans = gameOBJ1->GetComponent<TransformComponent>(TypeOfComponent::TRANSFORM);

                if (trans == nullptr)
                    return;

                // Calculate the collider's position and size
                Vector2 size = trans->GetScale();
                Vector3 colliderCenter = { trans->GetPosition().x, trans->GetPosition().y, 0.f };
                Matrix4x4 rotationMatrix = Matrix4x4::RotationZ(trans->GetRotation());
                Vector2 halfSize = size * 0.5f; // Half size for corner calculations

                // Define the 4 corners of the bounding box
                Vector3 corners[4] = {
                    { -halfSize.x,  halfSize.y, 0.f },
                    {  halfSize.x,  halfSize.y, 0.f },
                    {  halfSize.x, -halfSize.y, 0.f },
                    { -halfSize.x, -halfSize.y, 0.f }
                };

                // Create lines for bounding box edges
                Vertex* lineBuffer = lineVertices.data();
                for (int i = 0; i < 4; ++i) {
                    Vector3 start = rotationMatrix * corners[i] + colliderCenter;
                    Vector3 end = rotationMatrix * corners[(i + 1) % 4] + colliderCenter;
                    if (gameOBJ1->GetComponent<SpriteComponent>(TypeOfComponent::SPRITE) != nullptr)
                        lineBuffer = CreateLine(lineBuffer, start, end, 0.f);
                    else if (gameOBJ1->GetComponent<UISpriteComponent>(TypeOfComponent::SPRITE_UI) != nullptr)
                        lineBuffer = CreateLine(lineBuffer, start, end, 0.f);

                }

                RenderLines<MaxBatchSize>(lineVertices.data(), 8);
            }

            shader[S_GEOMETRY].Unbind();

            shader[S_TEXTURE].Bind();
            shader[S_TEXTURE].SetUniformMat4x4f("u_ViewProj", viewProjMatrix);
            shader[S_TEXTURE].SetUniform1iv("u_Texture"); // Set texture unit

            // Initialize the buffer pointer and index count for vertex data
            buffer = vertices.data();
            indexCount = 0;

            // Initialize texture slot tracking variables
            buffer = vertices.data();  // Reset buffer pointer for the batch

            // render different kind of response base on the mode to change the game object properties
            if (PlayerSceneControls::GetInstance().GetSelectedObjectMode() == GameObjectEditorMode::Rotate)
            {
                // Bind the texture to the assigned texture slot
                glBindTextureUnit(0, AssetManager::GetInstance().GetSprite("ArrowRightIcon")->GetTextureID());

                // Retrieve the transform component and apply transformations for position, scale, and rotation
                Vector2 position = PlayerSceneControls::GetInstance().GetRotateModeUI().pos;
                Vector2 scale = PlayerSceneControls::GetInstance().GetRotateModeUI().colliderBox;
                float rotate = 0;

                // Create transformation matrices for translation, rotation, and scale
                Matrix4x4 translate4x4(Matrix4x4::Translation(position.x + scale.x * 0.5f, position.y, 0.0f));
                Matrix4x4 scale4x4 = Matrix4x4::Scale(scale.x, scale.y, 1.f);
                Matrix4x4 rotate4x4(Matrix4x4::RotationZ(rotate));

                Matrix4x4 transform = translate4x4 * rotate4x4 * scale4x4;
                buffer = CreateTextureQuad(buffer, transform, 0.f, 0.f, 0.f, 0.f, 1.f, 1.f, Vector4{ 1.0f, 1.0f, 1.0f, 1.0f });
                indexCount += 6;

                // Create transformation matrices for translation, rotation, and scale
                translate4x4 = Matrix4x4::Translation(position.x - scale.x * 0.5f, position.y, 0.0f);
                scale4x4 = Matrix4x4::Scale(-scale.x, scale.y, 1.f);

                transform = translate4x4 * rotate4x4 * scale4x4;
                buffer = CreateTextureQuad(buffer, transform, 0.f, 0.f, 0.f, 0.f, 1.f, 1.f, Vector4{ 1.0f, 1.0f, 1.0f, 1.0f });
                indexCount += 6;
            }
            else
            {
                // Bind the texture to the assigned texture slot
                glBindTextureUnit(0, AssetManager::GetInstance().GetSprite("ArrowUpIcon")->GetTextureID());
                glBindTextureUnit(1, AssetManager::GetInstance().GetSprite("ArrowRightIcon")->GetTextureID());

                // Retrieve the transform component and apply transformations for position, scale, and rotation
                Vector2 position = PlayerSceneControls::GetInstance().GetTranslateModeUI()[0].pos;
                Vector2 scale = PlayerSceneControls::GetInstance().GetTranslateModeUI()[0].colliderBox;
                float rotate = 0;

                // Create transformation matrices for translation, rotation, and scale
                Matrix4x4 translate4x4(Matrix4x4::Translation(position.x, position.y, 0.0f));
                Matrix4x4 scale4x4 = Matrix4x4::Scale(scale.x, scale.y, 1.f);
                Matrix4x4 rotate4x4(Matrix4x4::RotationZ(rotate));

                Matrix4x4 transform = translate4x4 * rotate4x4 * scale4x4;
                buffer = CreateTextureQuad(buffer, transform, 0.f, 0.f, 0.f, 0.f, 1.f, 1.f, Vector4{ 1.0f, 1.0f, 1.0f, 1.0f });
                indexCount += 6;

                // Retrieve the transform component and apply transformations for position, scale, and rotation
                position = PlayerSceneControls::GetInstance().GetTranslateModeUI()[1].pos;
                scale = PlayerSceneControls::GetInstance().GetTranslateModeUI()[1].colliderBox;
                rotate = 0;

                // Create transformation matrices for translation, rotation, and scale
                translate4x4 = Matrix4x4::Translation(position.x, position.y, 0.0f);
                scale4x4 = Matrix4x4::Scale(scale.x, scale.y, 1.f);
                rotate4x4 = Matrix4x4::RotationZ(rotate);


                transform = translate4x4 * rotate4x4 * scale4x4;
                buffer = CreateTextureQuad(buffer, transform, 0.f, 1.f, 0.f, 0.f, 1.f, 1.f, Vector4{ 1.0f, 1.0f, 1.0f, 1.0f });
                indexCount += 6;
            }

            RenderQuads<MaxVertexCount>(vertices.data(), indexCount);

            // Reset texture slot tracking variables for the next render cycle
            texSlotIndex = 0;
            texSlotUsed.clear();

            // Unbind the texture shader
            shader[S_TEXTURE].Unbind();

            shader[S_FONT].Bind();
            // Initialize buffer and state variables
            buffer = vertices.data(); // Start buffer at the beginning of the vertices data
            indexCount = 0;           // Reset index count for new batch of text
            texSlotIndex = 0;         // Start from the first texture slot

            // Start with the X position from the text component
            advancePosX = engine.cameraManager.GetEditorCamera().GetCenter().x - engine.cameraManager.GetEditorCamera().GetViewingRange().x;
            float posY = engine.cameraManager.GetEditorCamera().GetCenter().y + engine.cameraManager.GetEditorCamera().GetViewingRange().y * 0.9f;

            // Render the text of the current mode that the user has chosen
            std::string textToPrint = "Current Mode: ";
            switch (PlayerSceneControls::GetInstance().GetSelectedObjectMode())
            {
            case GameObjectEditorMode::Translate:
                textToPrint += "Translate";
                break;
            case GameObjectEditorMode::Scale:
                textToPrint += "Scale";
                break;
            case GameObjectEditorMode::Rotate:
                textToPrint += "Rotate";
                break;
            default:
                break;
            }

            // Loop through each character in the text string
            for (const char& c : textToPrint)
            {
                // Retrieve the character data (e.g., texture, size, etc.)
                Character ch;

                ch = font[F_ARIAL].GetCharacterDictionary().at(c);

                unsigned int texID = ch.textureID; // Get the texture ID for the current character

                // Check if this texture has already been assigned a slot (to avoid reassigning)
                if (texSlotUsed.find(texID) == texSlotUsed.end())
                {
                    // If all texture slots are used, flush the current batch and reset
                    if (texSlotIndex >= 32)
                    {
                        RenderFonts<MaxVertexCount>(vertices.data(), indexCount);

                        // Reset buffers and texture slot indices for the next batch
                        buffer = vertices.data();
                        indexCount = 0;
                        texSlotIndex = 0;
                        texSlotUsed.clear();
                    }

                    // Assign the texture to the next available texture slot
                    texSlotUsed[texID] = texSlotIndex++;
                }

                // Bind the character's texture to the assigned texture slot
                glBindTextureUnit(texSlotUsed[texID], texID);

                // Calculate the position and size of the current character's quad
                float xpos = advancePosX + ch.bearing.x * 2.f; // X position adjusted by bearing
                float ypos = posY - (ch.size.y - ch.bearing.y) * 2.f; // Y position adjusted by bearing
                float w = ch.size.x * 2.f; // Character width adjusted by font size
                float h = ch.size.y * 2.f; // Character height adjusted by font size

                // Add the character's quad (vertices) to the buffer
                buffer = CreateFontQuad(buffer, Vector2(xpos, ypos), 0.f, (float)texSlotUsed[texID], w, h, { 255.f,0,0 });

                // Increase index count by 6 (two triangles per quad)
                indexCount += 6;

                // Advance the cursor for the next character based on its width
                advancePosX += (ch.advance / 64) * 2.f; // Adjust by character's advance value
            }

            // Final flush to render any remaining text if there is data left
            if (indexCount > 0)
            {
                RenderFonts<MaxVertexCount>(vertices.data(), indexCount);
                texSlotUsed.clear();
            }

            // Unbind the font shader after all text has been rendered
            shader[S_FONT].Unbind();
        }

#pragma endregion PlayerSelect_Rendering
    }

    // free up the resources used
    void GraphicsRender::Shutdown()
    {
    }

    // default constructor
    GraphicsManager::GraphicsManager()
    {
        Init();
    }

    // destructor
    GraphicsManager::~GraphicsManager()
    {
        Shutdown();
    }

    // get the singleton instance of the GraphicsManager
    GraphicsManager& GraphicsManager::GetInstance()
    {
        if (instance == nullptr)
            instance = std::make_unique<GraphicsManager>();

        return *instance;
    }

    // initialize the graphics manager
    void GraphicsManager::Init()
    {
        // intialise the renderer
        renderer.Init();
    }

    // renderering game objects by calling the renderer to render
    void GraphicsManager::Render()
    {
        renderer.Render();
    }

    // free up the resources used
    void GraphicsManager::Shutdown()
    {
        renderer.Shutdown();
    }
}
