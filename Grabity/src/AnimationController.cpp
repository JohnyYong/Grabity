/*!****************************************************************
\file: AnimationController.cpp
\author: Teng Shi Heng, shiheng.teng, 2301269

\brief
    This source file implements the AnimationController class,
    which manages animation states, transitions, and updates
    animation parameters dynamically. It also provides functions
    for loading and saving animation data using Lua scripting.

Copyright (C) 2025 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents
without the prior written consent of DigiPen Institute of
Technology is prohibited.
*******************************************************************/
#include "AnimationController.h"

#include <filesystem>
#include <fstream>
#include <imgui.h>
#include "assetmanager.h"
#include "ContentBrowser.h"

#pragma warning(disable : 4996)

struct FilePayload {
    char path[256];           // Buffer for the file path
    bool isPrefab;           // Flag to distinguish between prefabs and scenes
    bool isTexture;         // Flag for textures
    bool isSound;          // Flag for sound files
    char textureName[256];  // Buffer for texture name
    char soundName[256];   // Buffer for sound name
    int soundID;          // ID of the sound in the AssetManager
};

std::vector<const char*> testSTR{ "INT","BOOL","FLOAT" };
std::vector<const char*> otherSTR{ "Less Than","More Than" };

/*!****************************************************************
\brief
    Sets an animation parameter value by name.
\param paramName
    Name of the parameter.
\param value
    float value to set.
*******************************************************************/
void AnimationController::SetParameter(const std::string& paramName, float value)
{
    for (auto& param : parameters) {
        if (param.first == paramName) {  // If the parameter already exists, update it
            param.second = AnimationParameter(value);
            return;
        }
    }
    // If parameter does not exist, create it
    parameters.push_back(std::make_pair(paramName, AnimationParameter(value)));
}

/*!****************************************************************
\brief
    Sets an animation parameter value by name.
\param paramName
    Name of the parameter.
\param value
    bool value to set.
*******************************************************************/
void AnimationController::SetParameter(const std::string& paramName, bool value)
{
    for (auto& param : parameters) {
        if (param.first == paramName) {
            param.second = AnimationParameter(value);
            return;
        }
    }
    parameters.push_back(std::make_pair(paramName, AnimationParameter(value)));
}

/*!****************************************************************
\brief
    Sets an animation parameter value by name.
\param paramName
    Name of the parameter.
\param value
    int value to set.
*******************************************************************/
void AnimationController::SetParameter(const std::string& paramName, int value)
{
    for (auto& param : parameters) {
        if (param.first == paramName) {
            param.second = AnimationParameter(value);
            return;
        }
    }
    parameters.push_back(std::make_pair(paramName, AnimationParameter(value)));
}

/*!****************************************************************
\brief
    Retrieves an animation parameter by name.
\param paramName
    Name of the parameter.
\return
    The requested animation parameter.
*******************************************************************/
AnimationParameter AnimationController::GetParameter(const std::string& paramName)
{
    for (const auto& param : parameters) {
        if (param.first == paramName) {
            return param.second;
        }
    }
    return AnimationParameter(); // Return a default parameter if not found
}
/*!****************************************************************
\brief
    Adds a new animation state to the controller.
\param name
    Name of the new state.
\param texture
    unique_ptr sprite animation associated with the state.
*******************************************************************/
void AnimationController::AddState(const std::string& name, std::unique_ptr<SpriteAnimation> texture)
{
    states.emplace_back(std::make_unique<AnimationState>(name, std::move(texture)));
}

/*!****************************************************************
\brief
    Adds a transition between animation states.
\param fromStateIndex
    Index of the initial state.
\param toStateIndex
    Index of the target state.
\param conditionParam
    Condition parameter name and value.
\param type
    Type of the condition parameter.
\param threshold
    Threshold value for transition.
\param transitionTime
    Duration of the transition.
*******************************************************************/
void AnimationController::AddTransition(int fromStateIndex, int toStateIndex,
    std::pair<std::string, std::string> conditionParam,
    ParameterType type, float threshold, float transitionTime)
{

    if (fromStateIndex < 0 || fromStateIndex >= states.size() ||
        toStateIndex < 0 || toStateIndex >= states.size())
    {
        std::cerr << "Error: Invalid state indices provided.\n";
        return;
    }

    transitions.push_back({ fromStateIndex, toStateIndex, conditionParam, type, threshold, transitionTime });
}

/*!****************************************************************
\brief
    Updates the animation state based on transitions and parameters.
*******************************************************************/
void AnimationController::Update()
{
    for (auto& transition : transitions)
    {
        AnimationParameter param = GetParameter(transition.conditionParam.first);
        std::string compare = transition.conditionParam.second;
        float threshold = transition.threshold;

        bool conditionMet = false;

        switch (param.type)
        {
        case PARAMETER_FLOAT:
            if (compare == "More Than") conditionMet = (param.floatValue > threshold);
            else if (compare == "Less Than") conditionMet = (param.floatValue < threshold);
            break;

        case PARAMETER_INT:
            if (compare == "More Than") conditionMet = (param.intValue > threshold);
            else if (compare == "Less Than") conditionMet = (param.intValue < threshold);
            break;

        case PARAMETER_BOOL:
            if (compare == "True") conditionMet = param.boolValue;
            else if (compare == "False") conditionMet = !param.boolValue;
            break;
        }


        if (transition.fromStateIndex == currentStateIndex && conditionMet)
        {
            currentStateIndex = transition.toStateIndex;
        }
    }
}

/*!****************************************************************
\brief
    Sets the active animation state by name.
\param stateName
    Name of the state to activate.
*******************************************************************/
void AnimationController::SetState(const std::string& stateName)
{
    for (auto& state : states)
    {
        if (state->name == stateName)
        {
            currentState = state.get();
            break;
        }
    }
}


/*!****************************************************************
\brief
    Saves animation controller data to a Lua script file.
\param controller
    Reference to the animation controller to save.
\param filePath
    Path to the output Lua script file.
*******************************************************************/
void SaveAnimationControllerToLua(const AnimationController& controller, const std::string& filePath)
{
    std::ofstream luaFile(filePath);
    if (!luaFile.is_open())
    {
        std::cerr << "Failed to open file: " << filePath << std::endl;
        return;
    }

    luaFile << "AnimationController = {}\n\n";

    // Save parameters
    luaFile << "AnimationController.Parameters = {\n";
    for (const auto& param : controller.parameters)
    {
        luaFile << "  [\"" << param.first << "\"] = { type = \"";
        switch (param.second.type)
        {
        case PARAMETER_INT:
            luaFile << "int\", value = " << param.second.intValue;
            break;
        case PARAMETER_FLOAT:
            luaFile << "float\", value = " << param.second.floatValue;
            break;
        case PARAMETER_BOOL:
            luaFile << "bool\", value = " << (param.second.boolValue ? "true" : "false");
            break;
        }
        luaFile << " },\n";
    }
    luaFile << "}\n\n";

    // Save states
    luaFile << "AnimationController.States = {\n";
    for (const auto& state : controller.states)
    {
        luaFile << "  { name = \"" << state->name
            << "\", texture = \"" << (state->texture->GetSpriteTexture()->codeName.empty() ? "None" : state->texture->GetSpriteTexture()->codeName.c_str())
            << "\", FramesX = " << state->texture->GetSpriteTexture()->GetNxFrames()
            << ", FramesY = " << state->texture->GetSpriteTexture()->GetNyFrames()
            << ", FramesTotal = " << state->texture->GetSpriteTexture()->GetTotalFrames()
            << ", FramesPS = " << state->texture->GetSpriteTexture()->GetFramePs()
            << " }, \n";
    }
    luaFile << "}\n\n";

    // Save transitions
    luaFile << "AnimationController.Transitions = {\n";
    for (const auto& transition : controller.transitions)
    {
        luaFile << "  { from = \"" << controller.states[transition.fromStateIndex]->name << "\", to = \"" << controller.states[transition.toStateIndex]->name << "\", ";
        luaFile << "condition = { param = \"" << transition.conditionParam.first << "\", compare = \"" << transition.conditionParam.second << "\", ";
        luaFile << "threshold = " << transition.threshold << " }, time = " << transition.transitionTime << " },\n";
    }
    luaFile << "}\n";

    luaFile.close();
}

/*!****************************************************************
\brief
    Loads animation data from a Lua script file.
\param controller
    Reference to the animation controller to load data into.
\param luaFilePath
    Path to the Lua script file.
*******************************************************************/
void LoadFromLua(AnimationController& controllerTarget, const std::string& luaFilePath)
{
    LuaManagerAlpha luaManager(luaFilePath);

    // Load parameters
    sol::table luaParameters = luaManager.lua["AnimationController"]["Parameters"];
    for (auto& pair : luaParameters) {
        std::string paramName = pair.first.as<std::string>();
        sol::table paramData = pair.second.as<sol::table>();
        std::string paramType = paramData["type"].get<std::string>();
        if (paramType == "int") {
            int value = paramData["value"];
            controllerTarget.SetParameter(paramName, value);
        }
        else if (paramType == "float") {
            float value = paramData["value"];
            controllerTarget.SetParameter(paramName, value);
        }
        else if (paramType == "bool") {
            bool value = paramData["value"];
            controllerTarget.SetParameter(paramName, value);
        }
    }

    // Load states
    sol::table luaStates = luaManager.lua["AnimationController"]["States"];
    int index = 0;
    for (auto& state : luaStates)
    {
        sol::table stateTable = state.second.as<sol::table>();
        std::string stateName = stateTable["name"].get<std::string>();
        std::string stateTextureAssigned = stateTable["texture"].get<std::string>();
        if (stateTextureAssigned != "None")
            controllerTarget.AddState(stateName, std::make_unique<SpriteAnimation>(AssetManager::GetInstance().GetSprite(stateTextureAssigned)));
        else
            controllerTarget.AddState(stateName);
        controllerTarget.states[index]->texture->GetSpriteTexture()->SetNxFrames(stateTable["FramesX"].get<int>());
        controllerTarget.states[index]->texture->GetSpriteTexture()->SetNyFrames(stateTable["FramesY"].get<int>());
        controllerTarget.states[index]->texture->GetSpriteTexture()->SetTotalFrames(stateTable["FramesTotal"].get<int>());
        controllerTarget.states[index]->texture->GetSpriteTexture()->GetFramePs() = stateTable["FramesPS"].get<double>();
        ++index;
    }

    // Load transitions
    sol::table luaTransitions = luaManager.lua["AnimationController"]["Transitions"];
    for (auto& transition : luaTransitions)
    {
        sol::table transitionTable = transition.second.as<sol::table>();
        std::string fromStateName = transitionTable["from"].get<std::string>();
        std::string toStateName = transitionTable["to"].get<std::string>();
        sol::table conditionTable = transitionTable["condition"];
        std::string paramName = conditionTable["param"].get<std::string>();
        std::string compare = conditionTable["compare"].get<std::string>();
        float threshold = conditionTable["threshold"].get<float>();
        float transitionTime = transitionTable["time"].get<float>();

        int fromStateIndex = -1;
        int toStateIndex = -1;
        for (int i = 0; i < controllerTarget.states.size(); ++i)
        {
            if (controllerTarget.states[i]->name == fromStateName) fromStateIndex = i;
            if (controllerTarget.states[i]->name == toStateName) toStateIndex = i;
        }

        if (fromStateIndex != -1 && toStateIndex != -1)
        {
            ParameterType paramType = PARAMETER_FLOAT;
            controllerTarget.AddTransition(fromStateIndex, toStateIndex, { paramName, compare }, paramType, threshold, transitionTime);
        }
    }
}

/*!****************************************************************
\brief
    To handle adding and removing parameters
\param controller
    Reference to the current animation controller used in the engine.
*******************************************************************/
void RenderParameterSection(AnimationController& controller)
{
    static char textBuffer[128] = { 0 };
    static int parameterTypeIndex = 0;

    ImGui::Text("Add Parameter");
    ImGui::InputText("Parameter Name", textBuffer, sizeof(textBuffer));
    ImGui::Combo("Type", &parameterTypeIndex, testSTR.data(), (int)testSTR.size());

    if (ImGui::Button("Confirm"))
    {
        ParameterType selectedType = static_cast<ParameterType>(parameterTypeIndex);

        // Initialize the value based on the selected type
        switch (selectedType) {
        case PARAMETER_INT:
            controller.SetParameter(textBuffer, 0);  // Default integer value
            break;
        case PARAMETER_BOOL:
            controller.SetParameter(textBuffer, false);  // Default boolean value
            break;
        case PARAMETER_FLOAT:
            controller.SetParameter(textBuffer, 0.0f);  // Default float value
            break;
        }
    }

    ImGui::Separator();
    ImGui::Text("Existing Parameters:");

    for (auto it = controller.parameters.begin(); it != controller.parameters.end();)
    {
        ImGui::PushID(it->first.c_str());

        switch (it->second.type) {
        case PARAMETER_INT:
            ImGui::InputInt(it->first.c_str(), &it->second.intValue);
            break;
        case PARAMETER_FLOAT:
            ImGui::InputFloat(it->first.c_str(), &it->second.floatValue);
            break;
        case PARAMETER_BOOL:
            ImGui::Checkbox(it->first.c_str(), &it->second.boolValue);
            break;
        }

        ImGui::SameLine();
        if (ImGui::Button("Remove")) {
            it = controller.parameters.erase(it);
        }
        else {
            ++it;
        }

        ImGui::PopID();
    }
}

/*!****************************************************************
\brief
    To handle drag-and-drop texture assignment
\param controller
    Reference to the current animation controller used in the engine.
*******************************************************************/
void RenderTextureDragDrop(AnimationState* state)
{
    if (state->texture) {
        ImGui::Text("Texture:");
        ImGui::Image((void*)(intptr_t)state->texture->GetSpriteTexture()->GetTextureID(), ImVec2(64, 64), ImVec2(0, 1), ImVec2(1, 0));

        if (ImGui::BeginDragDropTarget()) {
            if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("CONTENT_BROWSER_ITEM"))
            {
                const FilePayload* filePayload = static_cast<const FilePayload*>(payload->Data);
                if (filePayload->isTexture) {
                    std::string textureName(filePayload->textureName);

                    // Check if this is an animation file
                    std::string animPrefix = "Animation-";
                    if (textureName.substr(0, animPrefix.length()) == animPrefix) {
                        // Extract the animation name (e.g., "Ame" from "Animation-Ame")
                        std::string animName = textureName.substr(animPrefix.length());
                        std::string animationKey = "Animation_" + animName;

                        // Get the pre-loaded animation
                        auto newSprite = AssetManager::GetInstance().GetSprite(animationKey);
                        if (newSprite) {
                            state->texture = std::make_unique<SpriteAnimation>(newSprite);  // Assign animation texture
                            std::cout << "Loaded animation: " << animationKey << std::endl;
                        }
                        else {
                            std::cout << "Failed to find pre-loaded animation: " << animationKey << std::endl;
                        }
                    }
                    else {
                        // Handle as regular texture
                        auto newSprite = AssetManager::GetInstance().GetSprite(textureName);
                        if (newSprite) {
                            state->texture = std::make_unique<SpriteAnimation>(newSprite);  // Assign regular texture
                            std::cout << "Loaded texture: " << textureName << std::endl;
                        }
                    }
                }
                else {
                    //ContentBrowserPanel::GetInstance().ShowErrorPopup("Invalid file format. Please drag and drop a valid texture file.");
                }
            }
            ImGui::EndDragDropTarget();
        }
    }
    else
    {
        ImGui::Text("No texture assigned");

        if (ImGui::BeginDragDropTarget())
        {
            if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("CONTENT_BROWSER_ITEM"))
            {
                const FilePayload* filePayload = static_cast<const FilePayload*>(payload->Data);
                if (filePayload->isTexture)
                {
                    std::string textureName(filePayload->textureName);

                    // Check if this is an animation file
                    std::string animPrefix = "Animation-";
                    if (textureName.substr(0, animPrefix.length()) == animPrefix)
                    {
                        // Extract the animation name (e.g., "Ame" from "Animation-Ame")
                        std::string animName = textureName.substr(animPrefix.length());
                        std::string animationKey = "Animation_" + animName;

                        // Get the pre-loaded animation
                        auto newSprite = AssetManager::GetInstance().GetSprite(animationKey);
                        if (newSprite)
                        {
                            state->texture = std::make_unique<SpriteAnimation>(newSprite);  // Assign animation texture
                            std::cout << "Loaded animation: " << animationKey << std::endl;
                        }
                        else
                        {
                            std::cout << "Failed to find pre-loaded animation: " << animationKey << std::endl;
                        }
                    }
                    else
                    {
                        // Handle as regular texture
                        auto newSprite = AssetManager::GetInstance().GetSprite(textureName);
                        if (newSprite)
                        {
                            state->texture = std::make_unique<SpriteAnimation>(newSprite);  // Assign regular texture
                            std::cout << "Loaded texture: " << textureName << std::endl;
                        }
                    }
                }
                else
                {
                    //ContentBrowserPanel::GetInstance().ShowErrorPopup("Invalid file format. Please drag and drop a valid texture file.");
                }
            }
            ImGui::EndDragDropTarget();
        }
    }
}

/*!****************************************************************
\brief
    To handle animation frame control
\param controller
    Reference to the current animation controller used in the engine.
*******************************************************************/
void RenderAnimationControls(AnimationState* state)
{
    if (state->texture->GetSpriteTexture()->GetNxFrames() > 1 || state->texture->GetSpriteTexture()->GetNyFrames() > 1)
    {
        ImGui::Text("Animation Settings");
        ImGui::InputFloat("Frames X", &state->texture->GetSpriteTexture()->GetNxFrames());
        ImGui::InputFloat("Frames Y", &state->texture->GetSpriteTexture()->GetNyFrames());
        ImGui::InputFloat("Frames Total", &state->texture->GetSpriteTexture()->GetTotalFrames());
        ImGui::InputDouble("FPS", &state->texture->GetSpriteTexture()->GetFramePs(), 1.0f);

        if (ImGui::Button("Reset Animation"))
        {
            state->texture->ResetSpriteAnimation();
        }
    }
}

/*!****************************************************************
\brief
    to handle state selection, texture assignment, and animation properties
\param controller
    Reference to the current animation controller used in the engine.
*******************************************************************/
void RenderStatesSection(AnimationController& controller)
{
    if (ImGui::Button("Add New State"))
    {
        controller.AddState("New State");
    }

    int index = 0;
    for (auto& state : controller.states)  // Fix: Use auto& to iterate unique_ptr
    {
        ImGui::PushID(index);

        AnimationState* rawState = state.get();  // Extract raw pointer

        // Render input field for state name
        char stateNameBuffer[128];
        strncpy(stateNameBuffer, rawState->name.c_str(), sizeof(stateNameBuffer));
        if (ImGui::InputText("State Name", stateNameBuffer, sizeof(stateNameBuffer))) {
            rawState->name = std::string(stateNameBuffer);  // Update the state name if it changed
        }

        ImGui::Text("State: %s", rawState->name.c_str());

        RenderTextureDragDrop(rawState);

        if (rawState->texture) {
            RenderAnimationControls(rawState);
        }

        if (ImGui::Button("Remove State")) {
            controller.states.erase(controller.states.begin() + index);
            ImGui::PopID();
            return;
        }

        ImGui::Separator();
        ImGui::PopID();
        ++index;
    }
}

/*!****************************************************************
\brief
    To handle state transitions
\param controller
    Reference to the current animation controller used in the engine.
*******************************************************************/
void RenderTransitionsSection(AnimationController& controller)
{
    static int fromIndex = 0, toIndex = 0, conditionIndex = 0, compareIndex = 0;
    static int intValue = 0;
    static float floatValue = 0.0f;
    static bool boolValue = false;

    // Select "From" and "To" states using the name directly from the AnimationState structs
    ImGui::Combo("From State", &fromIndex, [](void* data, int idx, const char** out_text) {
        auto& states = *reinterpret_cast<std::vector<std::unique_ptr<AnimationState>>*>(data);
        if (idx < 0 || idx >= (int)states.size()) return false;  // Safety check
        *out_text = states[idx]->name.c_str();  // Dereference unique_ptr
        return true;
        }, &controller.states, (int)controller.states.size());

    ImGui::Combo("To State", &toIndex, [](void* data, int idx, const char** out_text) {
        auto& states = *reinterpret_cast<std::vector<std::unique_ptr<AnimationState>>*>(data);
        if (idx < 0 || idx >= (int)states.size()) return false;  // Safety check
        *out_text = states[idx]->name.c_str();  // Dereference unique_ptr
        return true;
        }, &controller.states, (int)controller.states.size());



    // Ensure conditionIndex is within bounds
    if (controller.parameters.empty()) return;

    ImGui::Combo("Condition", &conditionIndex, [](void* data, int idx, const char** out_text) {
        *out_text = ((std::vector<std::pair<std::string, AnimationParameter>>*)data)->at(idx).first.c_str();
        return true;
        }, &controller.parameters, (int)controller.parameters.size());

    auto& param = controller.parameters[conditionIndex];

    switch (param.second.type) {
    case PARAMETER_INT:
        ImGui::Combo("Compare", &compareIndex, otherSTR.data(), (int)otherSTR.size());
        ImGui::InputInt("Value", &intValue);
        break;
    case PARAMETER_FLOAT:
        ImGui::Combo("Compare", &compareIndex, otherSTR.data(), (int)otherSTR.size());
        ImGui::InputFloat("Value", &floatValue);
        break;
    case PARAMETER_BOOL:
        ImGui::Checkbox("Is True", &boolValue);
        break;
    }

    // Add transition
    if (ImGui::Button("Add Transition"))
    {
        switch (param.second.type)
        {
        case PARAMETER_INT:
            controller.AddTransition(
                fromIndex,
                toIndex,
                { param.first, otherSTR[compareIndex] }, // Use correct index
                PARAMETER_INT,
                (float)intValue,
                0.f
            );
            break;
        case PARAMETER_FLOAT:
            controller.AddTransition(
                fromIndex,
                toIndex,
                { param.first, otherSTR[compareIndex] }, // Use correct index
                PARAMETER_FLOAT,
                floatValue,
                0.f
            );
            break;
        case PARAMETER_BOOL:
            controller.AddTransition(
                fromIndex,
                toIndex,
                { param.first, boolValue ? "True" : "False" },
                PARAMETER_BOOL,
                boolValue,
                0.f
            );
            break;
        }
    }
}

/*!****************************************************************
\brief
    Render the animation controller UI (ImGui)
\param controller
    Reference to the current animation controller used in the engine.
*******************************************************************/
void RenderAnimationControllerUI(AnimationController& controller)
{
    ImGui::Begin("Animation Controller");

    if (ImGui::CollapsingHeader("Parameters", ImGuiTreeNodeFlags_DefaultOpen))
    {
        RenderParameterSection(controller);
    }

    if (ImGui::CollapsingHeader("States", ImGuiTreeNodeFlags_DefaultOpen))
    {
        RenderStatesSection(controller);
    }

    if (ImGui::CollapsingHeader("Transitions", ImGuiTreeNodeFlags_DefaultOpen))
    {
        RenderTransitionsSection(controller);

        ImGui::Text("Transitions:");

        //show all the transitions created for this animation controller
        for (size_t i = 0; i < controller.transitions.size(); ++i)
        {
            auto& transition = controller.transitions[i];
            ImGui::PushID((int)i);
            ImGui::Text("From: %s -> To: %s", controller.states[transition.fromStateIndex]->name.c_str(), controller.states[transition.toStateIndex]->name.c_str());

            switch (transition.type)
            {
            case PARAMETER_INT:
                ImGui::Text("Condition: %s %s %d", transition.conditionParam.first.c_str(), transition.conditionParam.second.c_str(), transition.threshold);
                break;
            case PARAMETER_FLOAT:
                ImGui::Text("Condition: %s %s %f", transition.conditionParam.first.c_str(), transition.conditionParam.second.c_str(), transition.threshold);
                break;
            case PARAMETER_BOOL:
                ImGui::Text("Condition: %s %s", transition.conditionParam.first.c_str(), transition.conditionParam.second.c_str());
                break;
            }

            ImGui::SliderFloat("Transition Time", &transition.transitionTime, 0.1f, 2.0f);
            if (ImGui::Button("Remove Transition")) {
                controller.transitions.erase(controller.transitions.begin() + i);
                ImGui::PopID();
                --i;  // Adjust the index since we removed an element
                continue;
            }

            ImGui::PopID();
        }

    }

    static char saveFileName[256] = "Scene1";  // Default file path
    ImGui::InputText("Save", saveFileName, IM_ARRAYSIZE(saveFileName));
    std::string saveFilePath = "Assets/Lua/AnimationController/";
    saveFilePath += saveFileName;
    saveFilePath += ".lua";
    ImGui::SameLine();

    // Buttons for other functionalities
    if (ImGui::Button("Save Controller"))
    {
        //Create new luafile
        SaveAnimationControllerToLua(controller, saveFilePath);
    }

    // Initialize default values
    static char loadFileName[256] = "Scene1";  // Default path
    static std::string selectedAnimator = "Scene1";  // Set default selection to "Scene1"
    static int currentAnimatorIndex = 0;

    // Reload animator controller files every frame to reflect real-time changes
    std::vector<std::string> animatorFiles;
    for (const auto& entry : std::filesystem::directory_iterator("Assets/Lua/AnimationController"))
    {
        if (entry.path().extension() == ".lua")
        {
            animatorFiles.push_back(entry.path().stem().string()); // Get file name without extension
        }
    }

    // Check if current animator controller selection is still valid; if not, reset to "Scene1"
    bool animatorFound = false;
    for (int i = 0; i < animatorFiles.size(); ++i)
    {
        if (animatorFiles[i] == selectedAnimator)
        {
            currentAnimatorIndex = i;
            animatorFound = true;
            break;
        }
    }
    if (!animatorFound)
    {
        // Reset to default if selected animator controller is missing
        currentAnimatorIndex = 0;
        selectedAnimator = animatorFiles.empty() ? "" : animatorFiles[0];
    }

    // Dropdown to select animator controller file
    if (ImGui::BeginCombo("Load", selectedAnimator.c_str()))
    {
        for (int i = 0; i < animatorFiles.size(); ++i)
        {
            bool isSelected = (currentAnimatorIndex == i);
            if (ImGui::Selectable(animatorFiles[i].c_str(), isSelected))
            {
                currentAnimatorIndex = i;
                selectedAnimator = animatorFiles[i];
                snprintf(loadFileName, sizeof(loadFileName), "%s", animatorFiles[i].c_str());
            }
            if (isSelected) {
                ImGui::SetItemDefaultFocus();
            }
        }
        ImGui::EndCombo();
    }

    std::string loadFilePath = "Assets/Lua/AnimationController/" + selectedAnimator + ".lua";
    ImGui::SameLine();

    if (ImGui::Button("Load AnimationController"))
    {
        LoadFromLua(controller, loadFilePath);
    }

    ImGui::End();
}
