/*!****************************************************************
\file: PlayerSceneControls.cpp
\author: Teng Shi Heng, shiheng.teng, 2301269
\brief:
    The definition for PlayerSceneControls class functions

Copyright (C) 2024 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents
without the prior written consent of DigiPen Institute of
Technology is prohibited.
*******************************************************************!*/

#include "PlayerSceneControls.h"
#include "engine.h"
#include "GameObjectFactory.h"
#include "glhelper.h"
#include <cmath>

// Singleton instance of PlayerSceneControls.
std::unique_ptr<PlayerSceneControls> PlayerSceneControls::instance = nullptr;

// Constructor for PlayerSceneControls.
PlayerSceneControls::PlayerSceneControls() : selectedObj(nullptr), mode(None), isDragging(false)
{
    translate[0].colliderBox = { 50,100 };
    translate[1].colliderBox = { 50,100 };
    scale[0].colliderBox = { 50,100 };
    scale[1].colliderBox = { 50,100 };
    rotate.colliderBox = { 50,100 };
}

//  Destructor for PlayerSceneControls.
PlayerSceneControls::~PlayerSceneControls()
{
}

// Retrieves the current editor mode.
GameObjectEditorMode const& PlayerSceneControls::GetSelectedObjectMode()
{
    return mode;
}

// Retrieves the UI controls for the translation mode.
std::array<ControlsUI, 2> const& PlayerSceneControls::GetTranslateModeUI()
{
    return translate;
}

// Retrieves the UI controls for the scaling mode.
std::array<ControlsUI, 2> const& PlayerSceneControls::GetScaleModeUI()
{
    return scale;
}

// Retrieves the UI controls for the rotation mode.
ControlsUI const& PlayerSceneControls::GetRotateModeUI()
{
    return rotate;
}

// Retrieves the mouse position in the game world.
Vector2 PlayerSceneControls::GetMousePositionInGameWorld()
{
    return mousePositionInGameWorld;
}

// Retrieves the singleton instance of PlayerSceneControls.
PlayerSceneControls& PlayerSceneControls::GetInstance()
{
	if (instance == nullptr)
		instance = std::make_unique<PlayerSceneControls>();

	return *instance;
}

// Sets the selected GameObject in the editor.
void PlayerSceneControls::SetSelectedGameObject(GameObject* selected)
{
	selectedObj = selected;
}

// Gets the currently selected GameObject.
GameObject* PlayerSceneControls::GetSelectedGameObject()
{
	return selectedObj;
}

#ifdef  _IMGUI

// Converts screen-space mouse position to world-space within the level editor.
Vector2 PlayerSceneControls::GetMousePositionInLevelEditor(bool& withinRange) 
{
    // Check if the "Level Editor" window is active
    if (Engine::GetInstance().cameraManager.GetCurrentMode() == CameraManager::CameraMode::EditorCamera)
    {
        if (!ImGui::Begin("Level Editor")) {
            ImGui::End();
            withinRange = false;
            return Vector2(-1.0f, -1.0f); // Return an invalid position if the window is not open
        }
    }
    else
    {
        if (!ImGui::Begin("Scene")) {
            ImGui::End();
            withinRange = false;
            return Vector2(-1.0f, -1.0f); // Return an invalid position if the window is not open
        }
    }

    // Retrieve the current mouse position in screen coordinates
    Vector2 mousePos{ ImGui::GetMousePos().x,ImGui::GetMousePos().y };
    // Get the position of the content region's bot-left corner in screen space
    Vector2 contentPos{ ImGui::GetCursorScreenPos().x,ImGui::GetCursorScreenPos().y };
    // Get the available size of the content region
    Vector2 availableSize{ ImGui::GetWindowSize().x,ImGui::GetWindowSize().y };

    // Calculate the mouse position relative to the content region
    Vector2 mousePosInContentRegion = Vector2(mousePos.x - contentPos.x, mousePos.y - contentPos.y);

    withinRange = ((mousePos.x - contentPos.x >= 0) && (mousePos.x - contentPos.x <= availableSize.x));
    ImGui::End();

    return mousePosInContentRegion; // Return the calculated position
}

// Converts screen-space coordinates to world-space coordinates.
Vector2 PlayerSceneControls::ScreenToWorld(const Vector2& mousePos, const Camera& camera, float screenWidth, float screenHeight)
{
    // Get the center of the camera (including any shift in the camera center)
    Vector2 cameraCenter = camera.GetCenter();

    // Get the viewing range (the size of the world in camera coordinates)
    Vector2 viewingRange = camera.GetViewingRange();

    // Calculate the world coordinates based on the mouse position and camera properties
    float worldX = (mousePos.x - screenWidth * 0.5f) * (viewingRange.x / (screenWidth * 0.5f)) + cameraCenter.x;
    float worldY = (-mousePos.y - screenHeight * 0.5f) * (viewingRange.y / (screenHeight * 0.5f)) + cameraCenter.y;

    return { worldX, worldY }; // Return the calculated world position
}

// Determines if the mouse is hovering over a GameObject.
bool PlayerSceneControls::IsMouseOverGameObject(const Vector2& mouseWorldPos, GameObject* gameObj, float& distance)
{
    // Retrieve the TransformComponent of the game object
    TransformComponent* trans = gameObj->GetComponent<TransformComponent>(TypeOfComponent::TRANSFORM);

    // Check if the TransformComponent exists; return false if not found
    if (!trans) return false;

    // Get the position, scale, and rotation of the game object
    Vector2 objPos = trans->GetPosition();
    Vector2 objScale = trans->GetScale();
    float rotationAngle = trans->GetRotation();

    // Translate the mouse position relative to the object's origin
    Vector2 relativeMousePos = mouseWorldPos - objPos;

    // Apply a reverse rotation transformation to the mouse position
    float cosAngle = cos(-rotationAngle);
    float sinAngle = sin(-rotationAngle);
    Vector2 rotatedMousePos = {
        relativeMousePos.x * cosAngle - relativeMousePos.y * sinAngle, // Rotate X coordinate
        relativeMousePos.x * sinAngle + relativeMousePos.y * cosAngle  // Rotate Y coordinate
    };

    // Calculate the distance between the mouse position and the object's origin
    distance = Vector2::Distance({ 0,0 }, relativeMousePos);

    // Check if the rotated mouse position is within the unrotated bounding box of the object
    return (rotatedMousePos.x >= -objScale.x * 0.5f &&  // Check left boundary
        rotatedMousePos.x <= objScale.x * 0.5f &&       // Check right boundary
        rotatedMousePos.y >= -objScale.y * 0.5f &&      // Check bottom boundary
        rotatedMousePos.y <= objScale.y * 0.5f);        // Check top boundary

}

// Updates the state of the editor, handling input and object interactions.
void PlayerSceneControls::Update()
{
    // Check if the current camera mode is EditorCamera
    if (Engine::GetInstance().cameraManager.GetCurrentMode() == CameraManager::CameraMode::EditorCamera)
    {
        // Open ImGui window for "Level Editor"
        ImGui::Begin("Level Editor");

        // Retrieve the size of the ImGui "Level Editor" window
        Vector2 sceneWindowSize{ ImGui::GetWindowSize().x, ImGui::GetWindowSize().y };

        // Close the ImGui window context
        ImGui::End();

        bool wihtinRange;
        // Get the mouse position within the Level Editor window
        Vector2 mousePos = GetMousePositionInLevelEditor(wihtinRange);
        if (!wihtinRange) return;

        // Retrieve the editor camera instance
        Camera& editorCamera = Engine::GetInstance().cameraManager.GetEditorCamera();

        // Convert the screen-space mouse position to world-space coordinates
        mousePositionInGameWorld = ScreenToWorld(mousePos, editorCamera, sceneWindowSize.x, sceneWindowSize.y);

        // Check if the left mouse button is pressed
        if (InputManager::IsMouseButtonPressed(GLFW_MOUSE_BUTTON_LEFT))
        {
            // On the first mouse click
            if (firstAttempt)
            {
                //ImGuiConsole::Cout("Click");

                // Retrieve all game objects from the factory
                GameObjectFactory& factory = GameObjectFactory::GetInstance();
                const auto& gameObjects = factory.GetAllGameObjects();

                std::vector<std::pair<float, GameObject*>> possibleGameObjects;
                float distance;
                firstAttempt = false;

                // Iterate through all game objects to check mouse overlap
                for (auto& [id, gameObj] : gameObjects)
                {
                    if (IsMouseOverGameObject(mousePositionInGameWorld, gameObj, distance))
                    {
                        possibleGameObjects.emplace_back(distance, gameObj);
                        isFirstClickObject = true;
                    }
                }

                // Handle object selection
                if (isFirstClickObject && !isDragging)
                {
                    // Sort objects by proximity to the mouse (if multiple)
                    if (possibleGameObjects.size() > 1)
                    {
                        std::sort(possibleGameObjects.begin(), possibleGameObjects.end(),
                            [](std::pair<float, GameObject*>& lhs, std::pair<float, GameObject*>& rhs)
                            { return lhs.first < rhs.first; });
                    }

                    // Update selected object and its editor state
                    if (selectedObj != possibleGameObjects.front().second)
                    {
                        selectedObj = possibleGameObjects.front().second;
                        mode = GameObjectEditorMode::Translate;

                        // Initialize manipulators to the object's position
                        TransformComponent* trans = selectedObj->GetComponent<TransformComponent>(TypeOfComponent::TRANSFORM);
                        translate[0].pos = translate[1].pos = scale[0].pos = scale[1].pos = rotate.pos = trans->GetPosition();
                        translate[0].pos.y += translate[0].colliderBox.y;
                        translate[1].pos.x += translate[1].colliderBox.x;
                        scale[0].pos.y += scale[0].colliderBox.y;
                        scale[1].pos.x += scale[1].colliderBox.x;
                    }

                    // Start dragging the selected object
                    isDragging = true;
                    lastGameObjOffset = mousePositionInGameWorld;
                }
                else
                {
                    // Clear selection if no object is clicked
                    selectedObj = nullptr;
                    mode = GameObjectEditorMode::None;
                }
            }
        }
        else
        {
            // Reset dragging and selection flags
            isDragging = false;
            isFirstClickObject = false;
            firstAttempt = true;
        }

        // Handle dragging transformations
        if (isDragging)
        {
            TransformComponent* trans = selectedObj->GetComponent<TransformComponent>(TypeOfComponent::TRANSFORM);
            Vector2 offset = { mousePositionInGameWorld.x - lastGameObjOffset.x, mousePositionInGameWorld.y - lastGameObjOffset.y };

            switch (mode)
            {
            case Translate:
                // Update position during translation
                trans->SetLocalPosition({ trans->GetLocalPosition().x + offset.x, trans->GetLocalPosition().y + offset.y });
                break;

            case Rotate:
                // Update rotation based on mouse movement
                trans->SetLocalRotation(trans->GetLocalRotation() + offset.x * 0.1f);
                break;

            case Scale:
                // Update scale while dragging
                if (PlayerSceneControls::GetInstance().GetSelectedGameObject()->GetParent())
                    trans->SetLocalScale({ trans->GetLocalScale().x + offset.x / trans->GetScale().x,
                                           trans->GetLocalScale().y + offset.y / trans->GetScale().y });
                else
                    trans->SetLocalScale({ trans->GetLocalScale().x + offset.x, trans->GetLocalScale().y + offset.y });
                break;

            default:
                break;
            }

            // Update last offset for dragging
            lastGameObjOffset = mousePositionInGameWorld;
        }

        // Keyboard shortcuts to change transformation mode
        if (InputManager::IsKeyPressed(GLFW_KEY_8))
        {
            mode = Translate;
        }
        else if (InputManager::IsKeyPressed(GLFW_KEY_9))
        {
            mode = Rotate;
        }
        else if (InputManager::IsKeyPressed(GLFW_KEY_0))
        {
            mode = Scale;
        }
        if (InputManager::IsKeyPressed(GLFW_KEY_DELETE) && selectedObj != nullptr) {
            GameObjectFactory& factory = GameObjectFactory::GetInstance();
            factory.Despawn(selectedObj); 
        
        }
    }
    else
    {
        // Handle scene camera mode (not editor)
        ImGui::Begin("Scene");
        Vector2 sceneWindowSize{ ImGui::GetWindowSize().x, ImGui::GetWindowSize().y };
        ImGui::End();

        bool wihtinRange;
        Vector2 mousePos = GetMousePositionInLevelEditor(wihtinRange);
        if (!wihtinRange) return;

        Camera& playerCamera = Engine::GetInstance().cameraManager.GetPlayerCamera();
        mousePositionInGameWorld = ScreenToWorld(mousePos, playerCamera, sceneWindowSize.x, sceneWindowSize.y);
    }

}
#endif // _IMGUI