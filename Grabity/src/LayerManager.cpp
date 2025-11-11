/*!****************************************************************
\file: LayerManager.cpp
\author: Johny Yong Jun Siang, j.yong, 2301301
\brief: Defines the LayerManager class, which manages game object
        layers within the engine. The LayerManager allows for
        adding, removing, and validating layers, as well as
        controlling the active state of each layer. Layers are
        associated with game objects through unique IDs, enabling
        functionalities such as toggling rendering and physics
        operations based on layer activity. It uses a singleton
        pattern to ensure only one LayerManager instance exists.

Copyright (C) 2024 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents
without the prior written consent of DigiPen Institute of
Technology is prohibited.
*******************************************************************!*/
#include "LayerManager.h"
#include <fstream>
#include "GameObjectFactory.h"

//Preloading previously saved layers
/**
 * @brief Preloads previously saved layers from a file.
 * @return True if layers are successfully loaded, false otherwise.
 */
bool LayerManager::PreloadLayers()
{
    std::ifstream file("Assets/Lua/layers.txt");

    if (!file.is_open())
    {
        std::cout << "Failed to open file\n";
        return false;
    }

    std::string layer;
    while (std::getline(file, layer))
    {
        if (!layer.empty() && !IsLayerValid(layer))
        {
            AddLayer(layer);
        }
    }

    file.close();
    return true;
}
/**
 * @brief Adds a new layer to the system and saves it to a file.
 * @param newLayer The name of the layer to be added.
 * @return True if the layer is added successfully, false otherwise.
 */
bool LayerManager::AddLayer(const std::string& newLayer) {
    if (layerStates.count(newLayer)) {
        std::cout << "Layer " << newLayer << " already exists. Failed to add\n";
        return false;
    }

    layerStates[newLayer] = true; //Active by default
    std::cout << "Layer " << newLayer << " added successfully\n";
#pragma region SavingIntoFile
    const std::string layerFilePath = "Assets/Lua/layers.txt";

    std::ifstream inputFile(layerFilePath);
    if (!inputFile.is_open()) {
        std::cerr << "Failed to open layer file for reading: " << layerFilePath << std::endl;
        return false;
    }

    std::string line;
    while (std::getline(inputFile, line)) {
        if (line == newLayer) {
            std::cout << "Layer " << newLayer << " already exists in the file. Skipping append.\n";
            return true;
        }
    }
    inputFile.close();

    //Append the new layer to the file
    std::ofstream outputFile(layerFilePath, std::ios::app);
    if (!outputFile.is_open()) {
        std::cerr << "Failed to open layer file for writing: " << layerFilePath << std::endl;
        return false;
    }

    outputFile << newLayer << '\n';
    outputFile.close();
#pragma endregion

    return true;
}
/**
 * @brief Removes an existing layer from the system and updates the file.
 * @param layerName The name of the layer to be removed.
 * @return True if the layer is removed successfully, false otherwise.
 */
bool LayerManager::RemoveLayer(const std::string& layerName) {
    if (!layerStates.count(layerName)) {
        std::cout << "Layer " << layerName << " does not exist. Failed to remove\n";
        return false;
    }

    if (layerName == "Default") {
        std::cout << "Cannot remove the Default layer.\n";
        return false;
    }

    ////Assign "Default" to all GameObjects currently using the removed layer
    //for (auto& pair : gameObjectLayers) {
    //    if (pair.second == layerName) {
    //        pair.second = "Default";
    //    }
    //}

    //Force update to all gameobject whose layer that got removed
    for (auto& pair : GameObjectFactory::GetInstance().GetAllGameObjects()) {
        if (pair.second->GetLayer() == layerName) {
            pair.second->SetLayer("Default");
        }
    }

    layerStates.erase(layerName);
    std::cout << "Layer " << layerName << " removed successfully. Current number of layers: " << layerStates.size() << std::endl;

#pragma region SaveIntoFile
    const std::string layerFilePath = "Assets/Lua/layers.txt";
    std::ifstream inputFile(layerFilePath);

    if (!inputFile.is_open()) {
        std::cerr << "Failed to open layer file for reading: " << layerFilePath << std::endl;
        return false;
    }

    // Read the entire file content into memory and filter out the layer to remove
    std::vector<std::string> lines;
    std::string line;
    while (std::getline(inputFile, line)) {
        if (line != layerName) {  // Only keep lines that are NOT the layer to be removed
            lines.push_back(line);
        }
    }

    inputFile.close();

    // Open the same file for writing (overwrite mode)
    std::ofstream outputFile(layerFilePath, std::ios::trunc);
    if (!outputFile.is_open()) {
        std::cerr << "Failed to open layer file for writing: " << layerFilePath << std::endl;
        return false;
    }

    // Write the updated content back to the same file
    for (const std::string& savedLine : lines) {
        outputFile << savedLine << '\n';
    }

    outputFile.close();
#pragma endregion

    return true;
}
/**
 * @brief Checks if a given layer exists in the system.
 * @param layerName The name of the layer to validate.
 * @return True if the layer exists, false otherwise.
 */
bool LayerManager::IsLayerValid(const std::string& layerName) const {
    return layerStates.find(layerName) != layerStates.end();
}
/**
 * @brief Sets the active state of a specific layer.
 * @param layerName The name of the layer to modify.
 * @param isActive True to activate the layer, false to deactivate it.
 */
void LayerManager::SetLayerActive(const std::string& layerName, bool isActive) {
    if (layerStates.find(layerName) != layerStates.end()) {
        layerStates[layerName] = isActive;
        std::cout << "Layer " << layerName << " is now " << (isActive ? "active" : "inactive") << std::endl;
    }
    else {
        std::cout << "Layer " << layerName << " does not exist!\n";
    }
}
/**
 * @brief Checks if a specific layer is currently active.
 * @param layerName The name of the layer to check.
 * @return True if the layer is active, false otherwise.
 */

bool LayerManager::IsLayerActive(const std::string& layerName) const {
    auto it = layerStates.find(layerName);
    if (it != layerStates.end()) {
        return it->second;
    }
    return false; //inactive if the layer does not exist
}
/**
 * @brief Retrieves all layers currently managed by the system.
 * @return A vector containing the names of all layers.
 */
std::vector<std::string> LayerManager::GetAllLayers() const {
    std::vector<std::string> layers;
    for (const auto& pair : layerStates) {
        layers.push_back(pair.first);
    }
    return layers;
}
/**
 * @brief Retrieves all active layers currently in the system.
 * @return A vector containing the names of active layers.
 */
std::vector<std::string> LayerManager::GetActiveLayers() const {
    std::vector<std::string> activeLayers;
    for (const auto& pair : layerStates) {
        if (pair.second) {
            activeLayers.push_back(pair.first);
        }
    }
    return activeLayers;
}
/**
 * @brief Assigns a specific layer to a game object based on its ID.
 * @param gameObjectID The unique ID of the game object.
 * @param layerName The name of the layer to assign.
 */
void LayerManager::SetLayerForGameObject(GameObject* gameObjectID, const std::string& layerName) {
    if (!IsLayerValid(layerName)) {
        std::cout << "Invalid layer: " << layerName << ". Failed to assign.\n";
        return;
    }
    
    if (gameObjectLayers.find(gameObjectID->GetId()) != gameObjectLayers.end())
    {
        gameObjectLayerTest[gameObjectLayers[gameObjectID->GetId()]].erase(std::find(gameObjectLayerTest[gameObjectLayers[gameObjectID->GetId()]].begin(), gameObjectLayerTest[gameObjectLayers[gameObjectID->GetId()]].end(), gameObjectID));
    }
    gameObjectLayers[gameObjectID->GetId()] = layerName;
    gameObjectLayerTest[layerName].push_back(gameObjectID);
    //std::cout << "Layer " << layerName << " assigned successfully to GameObject ID: " << gameObjectID << std::endl;
}
/**
 * @brief Retrieves the layer assigned to a specific game object.
 * @param gameObjectID The unique ID of the game object.
 * @return A constant reference to the assigned layer name.
 */
const std::string& LayerManager::GetLayerForGameObject(int gameObjectID) const {
    auto it = gameObjectLayers.find(gameObjectID);
    if (it != gameObjectLayers.end()) {
        return it->second;
    }

    static const std::string defaultLayer = "Default";
    return defaultLayer;
}
