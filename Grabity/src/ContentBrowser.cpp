/*!****************************************************************
\file:      ContentBrowser.cpp
\author:    Lee Yu Jie Brandon , l.yujiebrandon , 2301232
\brief:     This file implements the ContentBrowser class functionality.
\details:   Provides a complete GUI system for asset management including:
            - File system navigation and browsing
            - Asset preview with thumbnails
            - Drag-and-drop functionality for game assets
            - Real-time asset manipulation
            - Integration with game viewport
            - Asset import/export capabilities
            - Directory management tools

Copyright (C) 2024 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents
without the prior written consent of DigiPen Institute of
Technology is prohibited.
*******************************************************************!*/
#ifdef _IMGUI

#include "ContentBrowser.h"
#include <imgui.h>
#include <ImGuiConsole.h>
#include <filesystem>
#include "assetmanager.h"
#include "GameObjectFactory.h"
#include "FrameBuffer.h"
#include "Texture.h"
#include "LuaConfig.h"

#include "PlayerSceneControls.h"

// Ensure APIENTRY is undefined before including Windows headers
#ifdef APIENTRY
#undef APIENTRY
#endif

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <commdlg.h>

/**
* @brief Structure to hold file drag-and-drop payload data.
* @details Contains information needed for drag-and-drop operations:
*          - File path information (256 chars max)
*          - File type flags (prefab, texture, sound)
*          - Asset names and identifiers
*          - Sound-specific data (ID tracking)
*/
struct FilePayload {
    char path[256];           // Buffer for the file path
    bool isPrefab;           // Flag to distinguish between prefabs and scenes
    bool isTexture;         // Flag for textures
    bool isSound;          // Flag for sound files
    char textureName[256];  // Buffer for texture name
    char soundName[256];   // Buffer for sound name
    int soundID;          // ID of the sound in the AssetManager
};

/** @brief Base path for all game assets */
static const std::filesystem::path s_AssetsPath = "Assets";

/** @brief Singleton instance pointer for the ContentBrowserPanel */
std::unique_ptr <ContentBrowserPanel> ContentBrowserPanel::instance = nullptr;

/** @brief External reference to the frame buffer for rendering */
extern FrameBuffer frameBuffer;

/**
* @brief Constructor for ContentBrowserPanel
* @details Initializes the content browser with:
*          - Sets the initial directory to the base assets path
*          - Prepares the browsing interface
*          - Sets up the file system navigation
*/
ContentBrowserPanel::ContentBrowserPanel() :
    m_CurrentDir(s_AssetsPath)
{

}

/**
* @brief Implementation of the singleton pattern for ContentBrowserPanel.
* @return Reference to the singleton instance.
*/
ContentBrowserPanel& ContentBrowserPanel::GetInstance() {
    if (instance == nullptr)
    {
        instance = std::make_unique<ContentBrowserPanel>();
    }
    return *instance;
}

/**
* @brief Checks if the mouse cursor is over a specific GameObject
* @param mouseWorldPos The mouse position in world coordinates
* @param gameObj Pointer to the GameObject to check
* @return true if mouse is over the GameObject, false otherwise
* @details Uses the GameObject's transform component to determine its bounds
*          and checks if the mouse position falls within these bounds
*/
bool ContentBrowserPanel::IsMouseOverGameObject(const Vector2& mouseWorldPos, GameObject* gameObj) {
    TransformComponent* trans = gameObj->GetComponent<TransformComponent>(TypeOfComponent::TRANSFORM);
    if (!trans) return false;

    Vector2 objPos = trans->GetLocalPosition();
    Vector2 objScale = trans->GetScale();

    return (mouseWorldPos.x >= objPos.x - objScale.x * 0.5f &&
        mouseWorldPos.x <= objPos.x + objScale.x * 0.5f &&
        mouseWorldPos.y >= objPos.y - objScale.y * 0.5f &&
        mouseWorldPos.y <= objPos.y + objScale.y * 0.5f);
}


/**
* @brief Helper function to create sprite GameObjects
* @param textureName Name of the texture to use
* @param mouseWorldPos World position for object placement
* @param factory Reference to GameObject factory
*/
void CreateNewSpriteGameObject(const std::string& textureName, const Vector2& mouseWorldPos, GameObjectFactory& factory)
{
    GameObject* newObject = factory.Create("Sprite_" + textureName);
    if (newObject) {
        newObject->AddComponent<TransformComponent>(
            TypeOfComponent::TRANSFORM,
            mouseWorldPos,
            Vector2(300.0f, 300.0f),
            0.0f
        );

        newObject->AddComponent<SpriteComponent>(
            TypeOfComponent::SPRITE,
            std::make_unique<SpriteAnimation>(AssetManager::GetInstance().GetSprite(textureName)),
            0,
            false,
            false
        );
        ImGuiConsole::Cout("Created new sprite GameObject");
    }
    else {
        ImGuiConsole::Cout("Failed to create GameObject");
    }
}


/**
* @brief Handles texture drag-and-drop operations
* @param filePayload Pointer to drag-drop payload
* @param mouseWorldPos World position of mouse cursor
* @param factory Reference to GameObject factory
* @details Manages texture replacement on existing objects or creates new sprite objects
*/
void HandleTextureDrop(const FilePayload* filePayload, const Vector2& mouseWorldPos, GameObjectFactory& factory)
{
    std::string textureName(filePayload->textureName);
    auto sprite = AssetManager::GetInstance().GetSprite(textureName);

    if (!sprite) {
        ImGuiConsole::Cout("Failed to get sprite: %s", textureName.c_str());
        return;
    }

    bool textureReplaced = false;
    for (const auto& [id, gameObj] : factory.GetAllGameObjects()) {
        if (ContentBrowserPanel::IsMouseOverGameObject(mouseWorldPos, gameObj)) {
            SpriteComponent* spriteComp = gameObj->GetComponent<SpriteComponent>(TypeOfComponent::SPRITE);
            if (spriteComp) {
                spriteComp->ChangeSprite(std::make_unique<SpriteAnimation>(sprite));
                textureReplaced = true;
                ImGuiConsole::Cout("Replaced texture on GameObject: %s", gameObj->GetName().c_str());
                break;
            }
        }
    }

    if (!textureReplaced) {
        CreateNewSpriteGameObject(textureName, mouseWorldPos, factory);
    }
}


/**
* @brief Handles prefab drag-and-drop operations
* @param filePayload Pointer to drag-drop payload
* @param mouseWorldPos World position of mouse cursor
* @param factory Reference to GameObject factory
* @details Creates new GameObjects from prefabs with proper hierarchy
*/
void HandlePrefabDrop(const FilePayload* filePayload, const Vector2& mouseWorldPos, GameObjectFactory& factory)
{
    std::string prefabName = std::filesystem::path(filePayload->path).stem().string();
    prefabName += "_0";
    GameObject* newObject = factory.CreateFromLua(filePayload->path, prefabName);

    if (newObject) {
        newObject->GetComponent<TransformComponent>(TypeOfComponent::TRANSFORM)->SetLocalPosition(mouseWorldPos);

        LuaManager luaManager(filePayload->path);
        std::unordered_map<int, std::string> idNameMap = luaManager.extractNames();
        idNameMap.erase(0);

        for (const auto& [childID, childName] : idNameMap) {
            if (childName != prefabName) {
                GameObject* childObject = factory.CreateFromLua(filePayload->path, childName);
                childObject->isDeserializing = true;
                childObject->SetParent(newObject);
                childObject->isDeserializing = false;
                factory.UpdateAllGameObjects();
            }
        }
    }
}


/**
* @brief Configures ImGui popup window parameters
* @details Sets up centered popup with:
*          - Fixed width (300px)
*          - Auto height
*          - Centered position relative to main viewport
*          - Forced positioning and sizing
*/
void SetupPopup() {
    ImGuiViewport* viewport = ImGui::GetMainViewport();
    ImVec2 work_pos = viewport->WorkPos;
    ImVec2 work_size = viewport->WorkSize;
    ImVec2 center(
        work_pos.x + work_size.x * 0.5f,
        work_pos.y + work_size.y * 0.5f
    );

    // Set fixed width for popup
    const float POPUP_WIDTH = 300.0f;
    ImGui::SetNextWindowSize(ImVec2(POPUP_WIDTH, 0), ImGuiCond_Always);
    ImGui::SetNextWindowPos(center, ImGuiCond_Always, ImVec2(0.5f, 0.5f));
}


/**
* @brief Renders the content browser interface using ImGui
* @details Handles multiple UI elements and interactions:
*          1. Content Browser Window:
*             - Directory navigation with back button
*             - Grid layout for file/folder display
*             - Thumbnail generation and display
*             - File type detection and appropriate icon loading
*
*          2. Drag and Drop Operations:
*             - Texture file handling
*             - Audio file handling
*             - Prefab and scene file handling
*
*          3. Level Editor Integration:
*             - Viewport display
*             - Asset placement and manipulation
*             - Real-time preview and modification
*/
void ContentBrowserPanel::OnImGuiRender()
{
    // Content Browser Window
    ImGui::Begin("Content Browser");

    //// Add asset buttons at the top of the window
    //if (ImGui::Button("New Folder")) {
    //    ImGui::OpenPopup("New Folder");
    //}
    //ImGui::SameLine();

    if (ImGui::Button("Import Asset")) {
        // Open file dialog using Windows API
        OPENFILENAMEA ofn;
        char szFile[260] = { 0 };
        ZeroMemory(&ofn, sizeof(ofn));
        ofn.lStructSize = sizeof(ofn);
        ofn.hwndOwner = nullptr;
        ofn.lpstrFile = szFile;
        ofn.nMaxFile = sizeof(szFile);
        ofn.lpstrFilter = "All Files\0*.*\0Images\0*.png;*.jpg;*.jpeg\0Audio\0*.wav;*.mp3\0";
        ofn.nFilterIndex = 1;
        ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR;

        if (GetOpenFileNameA(&ofn)) {
            // Get the destination path in the Assets folder
            std::filesystem::path sourcePath = ofn.lpstrFile;
            std::filesystem::path destPath = m_CurrentDir / sourcePath.filename();

            try {
                // Copy file to current directory
                std::filesystem::copy_file(sourcePath, destPath,
                    std::filesystem::copy_options::overwrite_existing);
                ShowNotification("Asset imported successfully!");
            }
            catch (const std::filesystem::filesystem_error& e) {
                std::string errorMsg = "Failed to import file: " + std::string(e.what());
                ShowNotification(errorMsg.c_str());
            }
        }
    }

    // Back button for directory navigation
    if (m_CurrentDir != std::filesystem::path(s_AssetsPath))
    {
        if (ImGui::Button("<- Back"))
        {
            m_CurrentDir = m_CurrentDir.parent_path();
        }
    }

    // Grid layout settings
    static float padding = 16.0f;
    static float thumbnailSize = 128.0f;
    float cellSize = padding + thumbnailSize;
    float panelWidth = ImGui::GetContentRegionAvail().x;
    int columnCount = int(panelWidth / cellSize);
    if (columnCount < 1)
        columnCount = 1;

    // Set up grid layout
    ImGui::Columns(columnCount, 0, false);

    // Iterate through directory contents
    for (auto& it : std::filesystem::directory_iterator(m_CurrentDir))
    {
        std::string pathString = it.path().string();
        auto relativePath = std::filesystem::relative(it.path(), s_AssetsPath);
        std::string filenameString = relativePath.filename().string();
        std::string extension = it.path().extension().string();

        ImGui::PushID(filenameString.c_str());

        // Get appropriate icon
        AssetManager& assetManager = AssetManager::GetInstance();
        unsigned int FolderIcon = assetManager.GetSprite("Folder_Icon")->GetTextureID();
        unsigned int FileIcon = assetManager.GetSprite("File_Icon")->GetTextureID();
        unsigned int textureId = it.is_directory() ? FolderIcon : FileIcon;

        // Check if file is a texture, audio, or other supported file
        bool isTextureFile = (extension == ".png" || extension == ".jpg" || extension == ".jpeg");
        bool isAudioFile = (extension == ".wav" || extension == ".mp3" || extension == ".ogg");
        std::string textureName;

        // Handle texture files
        if (isTextureFile) {
            textureName = std::filesystem::path(filenameString).stem().string();
            std::unique_ptr<SpriteAnimation> sprite;

            if (!assetManager.IsTextureLoaded(textureName)) {
                assetManager.LoadTexture(pathString, textureName, 1.0f, 1.0f, 0.0f);
            }

            sprite = std::make_unique<SpriteAnimation>(assetManager.GetSprite(textureName));
            if (sprite) {
                textureId = sprite->GetSpriteTexture()->GetTextureID();
            }
        }

        // Create image button
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));
        std::string buttonId = filenameString;
        bool clicked = ImGui::ImageButton(
            buttonId.c_str(),
            (ImTextureID)(uintptr_t)textureId,
            ImVec2(thumbnailSize, thumbnailSize),
            ImVec2(0.0f, 1.0f),
            ImVec2(1.0f, 0.0f),
            ImVec4(0.0f, 0.0f, 0.0f, 0.0f),
            ImVec4(1.0f, 1.0f, 1.0f, 1.0f)
        );
        ImGui::SetItemDefaultFocus();
        ImGui::PopStyleColor();

        // Right-click context menu
        if (ImGui::BeginPopupContextItem()) {
            if (ImGui::MenuItem("Delete")) {
                if (!it.is_directory()) {
                    try {
                        std::filesystem::remove(it.path());
                        if (isTextureFile) {
                            AssetManager::GetInstance().RemoveTexture(textureName);
                            ShowNotification("Texture deleted successfully!", false);
                        }
                        else if (isAudioFile) {
                            int audioID = extractIndex(filenameString);
                            AssetManager::GetInstance().RemoveSound(audioID);
                            ShowNotification("Audio file deleted successfully!", false);
                        }
                        else {
                            ShowNotification("File deleted successfully!", false);
                        }
                    }
                    catch (const std::filesystem::filesystem_error& e) {
                        std::string errorMsg = "Failed to delete file: " + std::string(e.what());
                        ShowNotification(errorMsg.c_str(), true);
                    }
                }
                else {
                    if (std::filesystem::is_empty(it.path())) {
                        try {
                            std::filesystem::remove(it.path());
                            ShowNotification("Directory deleted successfully!", false);
                        }
                        catch (const std::filesystem::filesystem_error& e) {
                            std::string errorMsg = "Failed to delete directory: " + std::string(e.what());
                            ShowNotification(errorMsg.c_str(), true);
                        }
                    }
                    else {
                        ShowNotification("Cannot delete non-empty directory!", true);
                    }
                }
            }
            ImGui::EndPopup();
        }

        // Handle directory navigation
        if (clicked && it.is_directory())
        {
            m_CurrentDir /= it.path().filename();
        }

        // Setup drag and drop for files
        if (isTextureFile || extension == ".lua" || isAudioFile)
        {
            if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_None))
            {
                static FilePayload payload;
                memset(&payload, 0, sizeof(FilePayload));

                if (isTextureFile)
                {
                    payload.isTexture = true;
                    payload.isPrefab = false;
                    payload.isSound = false;
                    strncpy_s(payload.textureName, textureName.c_str(), sizeof(payload.textureName) - 1);
                }
                else if (isAudioFile)
                {
                    if (extension != ".wav" && extension != ".mp3") {
                        std::string errorMsg = "Invalid audio file format: " + extension;
                        ShowNotification(errorMsg.c_str());
                        ImGui::EndDragDropSource();
                        continue;
                    }
                    payload.isTexture = false;
                    payload.isPrefab = false;
                    payload.isSound = true;

                    // Extract ID from filename (assuming format like "1_soundname")
                    std::string filename = std::filesystem::path(filenameString).stem().string();
                    int audioID = extractIndex(filename);
                    payload.soundID = audioID;

                    // Get the actual audio name using the ID
                    std::string soundName = AssetManager::GetInstance().GetAudioNameFromID(audioID);
                    if (soundName != "Cannot find audio") {
                        strncpy_s(payload.soundName, soundName.c_str(), sizeof(payload.soundName) - 1);
                        ImGui::Text("Dragging Audio: %s (ID: %d)", payload.soundName, audioID);
                    }
                    else {
                        std::cerr << "Failed to find audio with ID: " << audioID << std::endl;
                        ShowNotification("Invalid audio file name format!");
                        ImGui::EndDragDropSource();
                        continue;
                    }
                }
                else
                {
                    payload.isTexture = false;
                    payload.isSound = false;
                    std::string fullPath;
                    bool isPrefab = pathString.find("Prefabs") != std::string::npos;
                    // Explicitly check for Animator Controller or Scene in the path
                    if (pathString.find("AnimationController") != std::string::npos)
                    {
                        // Set path for Animator Controller Lua files
                        fullPath = "Assets/Lua/AnimationController/" + filenameString;
                    }
                    else if (isPrefab)
                    {
                        fullPath = "Assets/Lua/Prefabs/" + filenameString;
                    }
                    else {
                        // If the Lua file doesn't fit into the above categories, default to "Scenes"
                        fullPath = "Assets/Lua/Scenes/" + filenameString;
                    }
                    strncpy_s(payload.path, fullPath.c_str(), sizeof(payload.path) - 1);
                    payload.isPrefab = isPrefab;
                }

                ImGui::SetDragDropPayload("CONTENT_BROWSER_ITEM", &payload, sizeof(FilePayload));

                if (payload.isTexture) {
                    ImGui::Text("Dragging Texture: %s", payload.textureName);
                    ImGui::Image((void*)(intptr_t)textureId, ImVec2(32, 32), ImVec2(0, 1), ImVec2(1, 0));
                }
                else if (payload.isSound) {
                    ImGui::Text("Dragging Audio: %s", payload.soundName);
                    // Optionally show an audio icon here
                }
                else {
                    ImGui::Text(payload.isPrefab ? "Dragging Prefab: %s" : "Dragging Scene: %s", filenameString.c_str());
                }

                ImGui::EndDragDropSource();
            }
        }

        ImGui::PopID();
        ImGui::Text(filenameString.c_str());
        ImGui::NextColumn();
    }

    ImGui::Columns(1);

    ImGui::End();

    // Level Editor Window (Target)
    //Engine& engine = Engine::GetInstance();
    ImGui::Begin("Level Editor", nullptr, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);
    // Assuming you have your texture ID from your graphics API (e.g., OpenGL, DirectX)
    ImVec2 imageSize(1600, 900); // Size of the image to display

    ImVec2 availableSize = ImGui::GetContentRegionAvail();

    // Ensure the aspect ratio is maintained
    float aspectRatio = imageSize.x / imageSize.y;
    if (availableSize.x / aspectRatio <= availableSize.y)
    {
        availableSize.y = availableSize.x / aspectRatio;
    }
    else
    {
        availableSize.x = availableSize.y * aspectRatio;
    }

    // Calculate UV coordinates
    ImVec2 uv0(0.0f, 1.0f); // Top-left corner of the image
    ImVec2 uv1(1.0f, 0.0f); // Bottom-right corner of the image

    // Display the scene texture to the entire content region
    ImGui::Image((void*)(intptr_t)frameBuffer.GetTextureID(), availableSize, uv0, uv1);
    GameObjectFactory& factory = GameObjectFactory::GetInstance();
    Engine::GetInstance().scenewindow = ImGui::GetCurrentWindow();

    // Handle drag and drop in Level Editor
    if (ImGui::BeginDragDropTarget())
    {
        if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("CONTENT_BROWSER_ITEM"))
        {
            const FilePayload* filePayload = static_cast<const FilePayload*>(payload->Data);

            bool isWithinContent = false;
            Vector2 mousePos = PlayerSceneControls::GetInstance().GetMousePositionInLevelEditor(isWithinContent);
            Camera& editorCamera = Engine::GetInstance().cameraManager.GetEditorCamera();
            ImVec2 sceneWindowSize = ImGui::GetWindowSize();
            Vector2 mouseWorldPos = PlayerSceneControls::GetInstance().ScreenToWorld(mousePos, editorCamera, sceneWindowSize.x, sceneWindowSize.y);

            if (filePayload->isTexture)
            {
                HandleTextureDrop(filePayload, mouseWorldPos, factory);
            }
            else if (filePayload->isPrefab)
            {
                HandlePrefabDrop(filePayload, mouseWorldPos, factory);
            }
            else if (!filePayload->isSound)
            {
                if (std::string(filePayload->path).find("AnimationController") == std::string::npos)
                    Engine::GetInstance().LoadSceneFromLua(filePayload->path);
            }
        }
        ImGui::EndDragDropTarget();
    }
    ImGui::End();


    // Error popup
    if (showErrorPopup) {
        ImGui::OpenPopup("Error##Popup");
        showErrorPopup = false;
    }

    // Setup popup properties before BeginPopupModal
    SetupPopup();
    if (ImGui::BeginPopupModal("Error##Popup", nullptr,
        ImGuiWindowFlags_AlwaysAutoResize |
        ImGuiWindowFlags_NoMove |
        ImGuiWindowFlags_NoResize |
        ImGuiWindowFlags_NoSavedSettings)) {

        ImGui::Spacing();
        ImGui::Spacing();

        float textWidth = ImGui::CalcTextSize(popupMessage.c_str()).x;
        ImGui::SetCursorPosX((300.0f - textWidth) * 0.5f);

        ImGui::TextColored(ImVec4(1.0f, 0.3f, 0.3f, 1.0f), "%s", popupMessage.c_str());

        ImGui::Spacing();
        ImGui::Spacing();

        float buttonWidth = 120.0f;
        ImGui::SetCursorPosX((300.0f - buttonWidth) * 0.5f);

        if (ImGui::Button("OK", ImVec2(buttonWidth, 0))) {
            ImGui::CloseCurrentPopup();
        }

        ImGui::EndPopup();
    }

    // Success popup
    if (showSuccessPopup) {
        ImGui::OpenPopup("Success##Popup");
        showSuccessPopup = false;
    }

    // Setup popup properties before BeginPopupModal
    SetupPopup();
    if (ImGui::BeginPopupModal("Success##Popup", nullptr,
        ImGuiWindowFlags_AlwaysAutoResize |
        ImGuiWindowFlags_NoMove |
        ImGuiWindowFlags_NoResize |
        ImGuiWindowFlags_NoSavedSettings)) {

        ImGui::Spacing();
        ImGui::Spacing();

        float textWidth = ImGui::CalcTextSize(popupMessage.c_str()).x;
        ImGui::SetCursorPosX((300.0f - textWidth) * 0.5f);

        ImGui::TextColored(ImVec4(0.3f, 1.0f, 0.3f, 1.0f), "%s", popupMessage.c_str());

        ImGui::Spacing();
        ImGui::Spacing();

        float buttonWidth = 120.0f;
        ImGui::SetCursorPosX((300.0f - buttonWidth) * 0.5f);

        if (ImGui::Button("OK", ImVec2(buttonWidth, 0))) {
            ImGui::CloseCurrentPopup();
        }

        ImGui::EndPopup();
    }

}


/**
* @brief Helper function that converts coordinates from type ImVec2 to Vector2 coordinates.
* @param normalizedX The normalized X coordinate.
* @param normalizedY The normalized Y coordinate.
* @return Coordinates with type Vector2.
*/
Vector2 ContentBrowserPanel::ImVec2toVector2(float normalizedX, float normalizedY)
{


    return Vector2(normalizedX, normalizedY);
}


/**
* @brief Extracts the numeric ID from a filename in the format "NUMBER_name"
* @param input The filename to extract the ID from
* @return The extracted number, or -1 if any of these conditions are met:
*         - Empty input string
*         - No underscore in string
*         - No digits before underscore
*         - Invalid number format
* @details Safely parses numeric IDs from filenames, handling various error cases
*          and malformed inputs gracefully
*/
int ContentBrowserPanel::extractIndex(const std::string& input) {
    // Return -1 for empty strings
    if (input.empty()) {
        return -1;
    }

    // Find the underscore position
    size_t underscorePos = input.find('_');

    // If no underscore found, return -1
    if (underscorePos == std::string::npos) {
        return -1;
    }

    // Get the substring before the underscore
    std::string numberStr = input.substr(0, underscorePos);

    // Verify that we have at least one character
    if (numberStr.empty()) {
        return -1;
    }

    // Check if all characters before underscore are digits
    for (char c : numberStr) {
        if (!std::isdigit(c)) {
            return -1;
        }
    }

    try {
        // Convert string to integer
        return std::stoi(numberStr);
    }
    catch (...) {
        // Handle any conversion errors
        return -1;
    }
}


#endif // _IMGUI