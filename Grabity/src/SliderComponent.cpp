/*!****************************************************************
\file: SliderComponent.cpp
\author: Goh Jun Jie, 2301293
\co-author: 
\brief: Defines the functions declared in SliderComponent.h. It implements the functionality for a slidable gameobject

Copyright (C) 2024 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents
without the prior written consent of DigiPen Institute of
Technology is prohibited.
*******************************************************************!*/

#include "SliderComponent.h"
#include "GLHelper.h"
#include "GameObjectFactory.h"
#include "GameObject.h"


SliderComponent::SliderComponent(GameObject* parentObj) : parent(parentObj), Component(parentObj), minPosX(0.0f), maxPosX(0.0f), minPosXOffset(0.0f){

    if (parent) {
        TransformComponent* transform = parent->GetComponent<TransformComponent>(TypeOfComponent::TRANSFORM);
        if (transform) { 
            maxPosX = transform->GetLocalPosition().x;
        }
    }
}
SliderComponent::SliderComponent() : Component(nullptr), minPosX(0.0f), maxPosX(0.0f), minPosXOffset(0.0f) {

}



/*!****************************************************************
* \brief Updates the slider's state each frame.
*
* Handles user interaction and updates the slider's value accordingly.
*********************************************************************/
void SliderComponent::Update() {
    double mouseX, mouseY;
    InputManager::GetMousePosition(mouseX, mouseY); // Get current mouse position
    Engine& engine = Engine::GetInstance();
    minPosX = maxPosX - minPosXOffset - 60.f;
    //std::cout << " minPosX: " << minPosX << " maxPosX: " << maxPosX << std::endl;


#ifdef _IMGUI
        ImVec2 worldPos = engine.MouseToScreenImGui(engine.GetMousePositionImGui(engine.scenewindow),
            *engine.cameraManager.GetCurrentCamera(),
            engine.scenewindow->Size.x,
            engine.scenewindow->Size.y);
#else
        glfwGetCursorPos(InputManager::ptrWindow, &mouseX, &mouseY);
        Vector2 worldPos = engine.MouseToScreen(Vector2(static_cast<float>(mouseX), static_cast<float>(mouseY)),
            *engine.cameraManager.GetCurrentCamera(),
            static_cast<float>(InputManager::GetWidth()),
            static_cast<float>(InputManager::GetHeight()));

#endif // _IMGUI

    if (InputManager::IsMouseButtonPressed(0)) { // Check if left mouse button is down
        TransformComponent* transform = parent->GetComponent<TransformComponent>(TypeOfComponent::TRANSFORM);
        RectColliderComponent* collider = parent->GetComponent<RectColliderComponent>(TypeOfComponent::RECTCOLLIDER);

        if (collider) {
            AABB box = collider->GetAABB(0);
        
            if (!isDragging) {
                if (worldPos.x >= box.min.x && worldPos.x <= box.max.x &&
                    worldPos.y >= box.min.y && worldPos.y <= box.max.y) {
                    isDragging = true;  // Start dragging
                }
            }

            if (isDragging) {
                float newX = std::clamp(worldPos.x, minPosX, maxPosX);  // Clamp to the valid range

                // Update the position of the slider
                Vector2 newLocalPosition(newX, transform->GetLocalPosition().y);
                transform->SetLocalPosition(newLocalPosition);

                // Normalize the value to [0, 100]
                float range = maxPosX - minPosX;
                if (range > 0.0f) {
                    float normalizedValue = (newX - minPosX) / range;  // Value between 0 and 1
                    currentValue = normalizedValue;  // Scale to 0-1 range

                    // Update audio volumes
                    if (volumeType == VolumeType::MASTER) {
                        AudioManager::GetInstance().SetMasterVolume(currentValue);
                    }
                    else if (volumeType == VolumeType::BGM) {
                        AudioManager::GetInstance().SetBGMVolume(currentValue);
                    }
                    else if (volumeType == VolumeType::SFX) {
                        AudioManager::GetInstance().SetSFXVolume(currentValue);
                    }

                }

            }
        }
    }
    else {
        isDragging = false; // Stop dragging when mouse is released
    }

    
}

/*!****************************************************************
\func VolumeTypeToString
\brief Converts a VolumeType enum value to its corresponding string representation.

\param volumeType The VolumeType enum value to convert.
\return A string representation of the given VolumeType.
*******************************************************************/
std::string VolumeTypeToString(VolumeType volumeType) {
    switch (volumeType) {
    case VolumeType::MASTER: return "MASTER";
    case VolumeType::BGM: return "BGM";
    case VolumeType::SFX: return "SFX";
    default: return "Unknown";
    }
}

/*!****************************************************************
\func StringToVolumeType
\brief Converts a string representation of a volume type to its corresponding VolumeType enum value.

\param volumeTypeStr The string representation of the volume type.
\return The corresponding VolumeType enum value. Defaults to VolumeType::BGM if the string is unknown.
*******************************************************************/
VolumeType StringToVolumeType(const std::string& volumeTypeStr) {
    if (volumeTypeStr == "MASTER") {
        return VolumeType::MASTER;
    }
    else if (volumeTypeStr == "BGM") {
        return VolumeType::BGM;
    }
    else if (volumeTypeStr == "SFX") {
        return VolumeType::SFX;
    }
    else {
        return VolumeType::BGM;  // Default to BGM if unknown
    }
}

/*!****************************************************************
\func Serialize
\brief Serializes the slider component to a Lua file.
*******************************************************************!*/
void SliderComponent::Serialize(const std::string& luaFilePath, const std::string& tableName) {
    LuaManager luaManager(luaFilePath);
    std::vector<std::string> keys = { "maxPosXOffset", "currentValue", "volumeType"};
    std::string volumeTypeString = VolumeTypeToString(volumeType);
    LuaManager::LuaValueContainer values = { minPosXOffset, currentValue, volumeTypeString };
    luaManager.LuaWrite(tableName, values, keys, "SliderComponent");
}

/*!****************************************************************
\func Deserialize
\brief Deserializes the slider component from a Lua file.
*******************************************************************!*/
void SliderComponent::Deserialize(const std::string& luaFilePath, const std::string& tableName) {
    LuaManager luaManager(luaFilePath);

    minPosXOffset = luaManager.LuaRead<float>(tableName, { "SliderComponent", "maxPosXOffset" });
    currentValue = luaManager.LuaRead<float>(tableName, { "SliderComponent", "currentValue" });
    std::string volumeTypeStr = luaManager.LuaRead<std::string>(tableName, { "SliderComponent", "volumeType" });
    volumeType = StringToVolumeType(volumeTypeStr);


}

