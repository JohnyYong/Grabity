//By Johny, the layer manager difference from tag manager within the engine is such that turning off the
//the layers would deactivate any physics or rendering applied to it
/*!****************************************************************
\file: LayerManager.h
\author: Johny Yong Jun Siang, j.yong, 2301301 
\co-author: Teng Shi Heng, shiheng.teng, 2301269

\brief: Declares the LayerManager class, which manages game object
		layers within the engine. The LayerManager allows for
		adding, removing, and validating layers, as well as
		controlling the active state of each layer. Layers are
		associated with game objects through unique IDs, enabling
		functionalities such as toggling rendering and physics
		operations based on layer activity. It uses a singleton
		pattern to ensure only one LayerManager instance exists.

		Johny implemented all the layer features where it store
		the data of the activeness of the created layers and each
		gameobject's layer that it belongs to. (90%)

		Shi Heng add on gameObjectLayerTest variable to
		store each game object references to each specified
		layers so that its the physics and collision and rendering
		is updated based on the activeness of that layer(10%).

Copyright (C) 2024 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents
without the prior written consent of DigiPen Institute of
Technology is prohibited.
*******************************************************************!*/
#pragma once
#include <unordered_set>
#include <unordered_map>
#include <string>
#include <vector>
#include <iostream>
#include <set>

class GameObject;

class LayerManager {
	public:
	/**
	* @brief Retrieves the singleton instance of the LayerManager class.
	* @return A reference to the singleton instance.
	*/
	static LayerManager& GetInstance() { static LayerManager instance; return instance;}
	/**
	* @brief Preloads default layers into the layer manager.
	* @return True if successful, false otherwise.
	*/
	bool PreloadLayers();
	/**
	* @brief Adds a new layer to the layer manager.
	* @param newLayer The name of the new layer to add.
	* @return True if the layer is added successfully, false if it already exists.
	*/
	bool AddLayer(const std::string& newLayer);
	/**
	* @brief Removes an existing layer from the layer manager.
	* @param layerName The name of the layer to remove.
	* @return True if the layer is removed successfully, false if it does not exist.
	*/
	bool RemoveLayer(const std::string& layerName);
	/**
	* @brief Checks if a layer exists within the layer manager.
	* @param layerName The name of the layer to validate.
	* @return True if the layer exists, false otherwise.
	*/
	bool IsLayerValid(const std::string& layerName) const;
	/**
	* @brief Sets the active state of a specific layer.
	* @param layerName The name of the layer.
	* @param isActive True to activate the layer, false to deactivate it.
	*/
	void SetLayerActive(const std::string& layerName, bool isActive);
	/**
	* @brief Checks if a specific layer is currently active.
	* @param layerName The name of the layer.
	* @return True if the layer is active, false otherwise.
	*/
	bool IsLayerActive(const std::string& layerName) const;
	/**
	* @brief Retrieves a list of all layers currently managed.
	* @return A vector containing the names of all layers.
	*/
	std::vector<std::string> GetAllLayers() const;
	/**
	* @brief Retrieves a list of all currently active layers.
	* @return A vector containing the names of active layers.
	*/
	std::vector<std::string> GetActiveLayers() const;
	/**
	* @brief Associates a game object with a specific layer.
	* @param gameObjectID The unique ID of the game object.
	* @param layerName The name of the layer to assign.
	*/
	void SetLayerForGameObject(GameObject* gameObjectID, const std::string& layerName);
	/**
	* @brief Retrieves the layer associated with a specific game object.
	* @param gameObjectID The unique ID of the game object.
	* @return A constant reference to the layer name.
	*/
	const std::string& GetLayerForGameObject(int gameObjectID) const;

	/**
	 * @brief Retrieves a list of GameObjects from a specified layer.
	 *
	 * @param layer The name of the layer to retrieve GameObjects from.
	 * @return A reference to a vector of GameObjects in the specified layer.
	 */
	inline const std::vector<GameObject*>& GetSpecifiedLayer(const std::string& layer) {
		static const std::vector<GameObject*> emptySet;
		auto it = gameObjectLayerTest.find(layer);
		if (it != gameObjectLayerTest.end()) {
			return it->second;
		}
		else {
			return emptySet;
		}
	}

	/**
	 * @brief Clears all stored GameObject layer data.
	 */
	void ClearGameObjectLayerData()
	{
		gameObjectLayers.clear();
		std::vector<std::string> allLayer = GetAllLayers();
		for (std::string str : allLayer)
			gameObjectLayerTest[str].clear();
		gameObjectLayerTest.clear();
	}
private:

	//Default existing layers for now
	LayerManager() {
		AddLayer("Default");
		AddLayer("UI");
		AddLayer("Enemies");
		AddLayer("Background");
	}

	/**
	* @brief A map that holds the active state (true/false) of each layer by name.
	*/
	std::unordered_map<std::string, bool> layerStates;

	//We get the gameobjects via their ID instead
	/**
	* @brief A map that associates game objects (by their IDs) with their respective layers.
	*/
	std::unordered_map<int, std::string> gameObjectLayers;

	std::unordered_map<std::string, std::vector<GameObject*>> gameObjectLayerTest;
};